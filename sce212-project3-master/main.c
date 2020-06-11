#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h> // for getopt()

#define BYTES_PER_WORD 4
#define READ           1
#define WRITE          0
#define TRUE           1
#define FALSE          0
#define INF            100000000

#define VALBIT(CACHE, IDX, I) (CACHE)->sets[IDX].lines[I].valid
#define MODBIT(CACHE, IDX, I) (CACHE)->sets[IDX].lines[I].modified
#define USECNT(CACHE, IDX, I) (CACHE)->sets[IDX].lines[I].use_cnt
#define TAG(CACHE, IDX, I)    (CACHE)->sets[IDX].lines[I].tag
#define CNT(CACHE, IDX)       (CACHE)->sets[IDX].cnt
// #define DEBUG

/*
 * Cache structures
 */
int time = 0;
typedef struct {
    int use_cnt;  // for LRU
    int valid;    // valid bit
    int modified; // dirty bit
    uint32_t tag;
} cline;

typedef struct {
    int cnt;
    cline *lines;
} cset;

typedef struct {
    int s; // index bits
    int E; // way
    int b; // block offset bits
    cset *sets; // set의 개수만큼을 가져야함. -> blocksize, way에 따라서 달라질듯?
} cache;

// 받은게 몇 bit 인지 전달하는 함수
int index_bit(int n) {
    int cnt = 0;

    while (n) {
        cnt++;
        n = n >> 1;
    }

    return cnt - 1;
}

/***************************************************************/
/*                                                             */
/* Procedure : build_cache                                     */
/*                                                             */
/* Purpose   : Initialize cache structure                      */
/*                                                             */
/* Parameters:                                                 */
/*     int S: The set of cache                                 */
/*     int E: The associativity way of cache                   */
/*     int b: The blocksize of cache                           */
/*                                                             */
/***************************************************************/
cache build_cache(int S, int E, int b) {
    /* Implement this function */
    cache ret;
    ret.s = index_bit(S); // index bit
    ret.E = E;            // way 의 개수
    ret.b = index_bit(b); // block offset bit
    ret.sets = (cset *) malloc(sizeof(cset) * S); // set의 개수는 S개

    // line들의 개수는 way개
    for (int i = 0; i < S; i++) {
        ret.sets[i].lines = (cline *) malloc(sizeof(cline) * E);
        ret.sets[i].cnt = 0;

        for (int j = 0; j < E; j++) {
            ret.sets[i].lines[j].use_cnt = 0;
            ret.sets[i].lines[j].valid = FALSE;
            ret.sets[i].lines[j].modified = FALSE;
            ret.sets[i].lines[j].tag = 0;
        }
    }

    return ret;
}

/***************************************************************/
/*                                                             */
/* Procedure : access_cache                                    */
/*                                                             */
/* Purpose   : Update cache stat and content                   */
/*                                                             */
/* Parameters:                                                 */
/*     cache *L: An accessed cache                             */
/*     int op: Read/Write operation                            */
/*     uint32_t addr: The address of memory access             */
/*     int *hit: The number of cache hit                       */
/*     int *miss: The number of cache miss                     */
/*     int *wb: The number of write-back                       */
/*                                                             */
/***************************************************************/
void access_cache(cache *L, int op, uint32_t addr, int *hit, int *miss, int *wb) {
    int offsetBit = L->b; // block offset bit
    int indexBit = L->s;  // index bit
    int way = L->E;       // way 개수

    uint32_t tag = addr >> (indexBit + offsetBit);
    uint32_t off = addr - ((addr >> offsetBit) << offsetBit);
    uint32_t idx = (addr >> offsetBit) - (tag << (indexBit));

    /************* HIT *************/
    /* idx 번째의 tag 값들을 비교하면서 일치하는 애가 있는 지 검사한다. */
    for (int i = 0; i < way; i++) {
        if (VALBIT(L, idx, i) && TAG(L, idx, i) == tag) {
            (*hit)++;
            if (op == WRITE) MODBIT(L, idx, i) = TRUE;
            USECNT(L, idx, i) = CNT(L, idx)++;
            return;
        }
    }

    /************* MISS *************/

    /*
     * 1. cache 에 기록할 공간이 남아있는 지 확인한다. -> valid bit 가 FALSE 인 애를 찾아서 기록한다.
     * 2. idx 번째 set 이 꽉 차서 더 이상 쓸 수 없는 경우 -> LRU 를 찾아서 걔를 방출하고 걔로 update 한다.
     */

    (*miss)++;
    int minCnt = INF, minIdx;

    for (int i = 0; i < way; i++) {
        if (!VALBIT(L, idx, i)) { // 비어있는 way 를 찾은 경우 여기다가 넣어주고 끝내면 됨.
            VALBIT(L, idx, i) = TRUE;
            TAG(L, idx, i) = tag;
            MODBIT(L, idx, i) = ((op == WRITE) ? TRUE : FALSE);
            USECNT(L, idx, i) = CNT(L, idx)++;
            return;
        }

        // 비어있는 칸이 없는 경우, LRU를 갈아끼워줘야하기 때문에 LRU를 찾는다.
        int curCnt = USECNT(L, idx, i);
        if (minCnt > curCnt) {
            minCnt = curCnt;
            minIdx = i;
        }
    }

    if (MODBIT(L, idx, minIdx)) (*wb)++;
    TAG(L, idx, minIdx) = tag;
    MODBIT(L, idx, minIdx) = ((op == WRITE) ? TRUE : FALSE);
    USECNT(L, idx, minIdx) = CNT(L, idx)++;
}

/***************************************************************/
/*                                                             */
/* Procedure : cdump                                           */
/*                                                             */
/* Purpose   : Dump cache configuration                        */
/*                                                             */
/***************************************************************/
void cdump(int capacity, int assoc, int blocksize) {
    printf("Cache Configuration:\n");
    printf("-------------------------------------\n");
    printf("Capacity: %dB\n", capacity);
    printf("Associativity: %dway\n", assoc);
    printf("Block Size: %dB\n", blocksize);
    printf("\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : sdump                                           */
/*                                                             */
/* Purpose   : Dump cache stat		                           */
/*                                                             */
/***************************************************************/
void sdump(int total_reads, int total_writes, int write_backs,
           int reads_hits, int write_hits, int reads_misses, int write_misses) {
    printf("Cache Stat:\n");
    printf("-------------------------------------\n");
    printf("Total reads: %d\n", total_reads);
    printf("Total writes: %d\n", total_writes);
    printf("Write-backs: %d\n", write_backs);
    printf("Read hits: %d\n", reads_hits);
    printf("Write hits: %d\n", write_hits);
    printf("Read misses: %d\n", reads_misses);
    printf("Write misses: %d\n", write_misses);
    printf("\n");
}


/***************************************************************/
/*                                                             */
/* Procedure : xdump                                           */
/*                                                             */
/* Purpose   : Dump current cache state                        */
/*                                                             */
/* Cache Design                                                */
/*                                                             */
/*      cache[set][assoc][word per block]                      */
/*                                                             */
/*                                                             */
/*       ----------------------------------------              */
/*       I        I  way0  I  way1  I  way2  I                 */
/*       ----------------------------------------              */
/*       I        I  word0 I  word0 I  word0 I                 */
/*       I  set0  I  word1 I  word1 I  work1 I                 */
/*       I        I  word2 I  word2 I  word2 I                 */
/*       I        I  word3 I  word3 I  word3 I                 */
/*       ----------------------------------------              */
/*       I        I  word0 I  word0 I  word0 I                 */
/*       I  set1  I  word1 I  word1 I  work1 I                 */
/*       I        I  word2 I  word2 I  word2 I                 */
/*       I        I  word3 I  word3 I  word3 I                 */
/*       ----------------------------------------              */
/*                                                             */
/*                                                             */
/***************************************************************/
void xdump(cache *L) {
    int i, j, k = 0;
    int b = L->b, s = L->s;
    int way = L->E, set = 1 << s;

    uint32_t line;

    printf("Cache Content:\n");
    printf("-------------------------------------\n");

    for (i = 0; i < way; i++) {
        if (i == 0) {
            printf("    ");
        }
        printf("      WAY[%d]", i);
    }
    printf("\n");

    for (i = 0; i < set; i++) {
        printf("SET[%d]:   ", i);

        for (j = 0; j < way; j++) {
            if (k != 0 && j == 0) {
                printf("          ");
            }
            if (L->sets[i].lines[j].valid) {
                line = L->sets[i].lines[j].tag << (s + b);
                line = line | (i << b);
            } else {
                line = 0;
            }
            printf("0x%08x  ", line);
        }
        printf("\n");
    }
    printf("\n");
}


int main(int argc, char *argv[]) {
    int capacity = 1024;
    int way = 8;
    int blocksize = 8;
    int set;

    // Cache
    cache simCache;

    // Counts
    int read = 0, write = 0, writeback = 0;
    int readhit = 0, writehit = 0;
    int readmiss = 0, writemiss = 0;

    // Input option
    int opt = 0;
    char *token;
    int xflag = 0;

    // Parse file
    char *trace_name = (char *) malloc(32);
    FILE *fp;
    char line[16];
    char *op;
    uint32_t addr;

    /* You can define any variables that you want */

    trace_name = argv[argc - 1];
    if (argc < 3) {
        printf("Usage: %s -c cap:assoc:block_size [-x] input_trace \n", argv[0]);
        exit(1);
    }

    while ((opt = getopt(argc, argv, "c:x")) != -1) {
        switch (opt) {
            case 'c':
                token = strtok(optarg, ":");
                capacity = atoi(token);
                token = strtok(NULL, ":");
                way = atoi(token);
                token = strtok(NULL, ":");
                blocksize = atoi(token);
                break;

            case 'x':
                xflag = 1;
                break;

            default:
                printf("Usage: %s -c cap:assoc:block_size [-x] input_trace \n", argv[0]);
                exit(1);

        }
    }

    // Allocate
    set = capacity / way / blocksize; // set calculated!

    /* TODO: Define a cache based on the struct declaration */
    // simCache = build_cache();
    simCache = build_cache(set, way, blocksize);

    // Simulate
    fp = fopen(trace_name, "r"); // read trace file
    if (fp == NULL) {
        printf("\nInvalid trace file: %s\n", trace_name);
        return 1;
    }

    cdump(capacity, way, blocksize);

    /* TODO: Build an access function to load and store data from the file */
    while (fgets(line, sizeof(line), fp) != NULL) {
        op = strtok(line, " ");
        addr = strtoull(strtok(NULL, ","), NULL, 16);

#ifdef DEBUG
        // You can use #define DEBUG above for seeing traces of the file.
        fprintf(stderr, "op: %s\n", op); //op 에는 R or W 가 담겨있음
        fprintf(stderr, "addr: %x\n", addr);
#endif
        // ...
        // access_cache()

        if (!strcmp(op, "R")) {
            read++;
            access_cache(&simCache, READ, addr, &readhit, &readmiss, &writeback);
        } else {
            write++;
            access_cache(&simCache, WRITE, addr, &writehit, &writemiss, &writeback);
        }
    }

    // test example
    sdump(read, write, writeback, readhit, writehit, readmiss, writemiss);
    if (xflag) {
        xdump(&simCache);
    }

    return 0;
}
