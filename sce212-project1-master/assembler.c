#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>

typedef struct Inst {
    char *name;
    char *op;
    char type;
    char *funct;
} Inst;

typedef struct Data {
    char *name;
    int value;
    unsigned int address;
} Data;

typedef struct Text {
    int op_idx;
    char type;
    char *opcode;
    char *rs;
    char *rt;
    char *rd;
    char *shamt;
    char *funct;
    char *immediate;
    char *address;
} Text;

typedef struct Func {
    char *name;
    unsigned int address;
} Func;

struct Inst inst[20] = {
        {"addiu", "001001", 'I', ""}, // inst name, opcode, type, funct
        {"addu",  "000000", 'R', "100001"},
        {"and",   "000000", 'R', "100100"},
        {"andi",  "001100", 'I', ""},
        {"beq",   "000100", 'I', ""},
        {"bne",   "000101", 'I', ""},
        {"j",     "000010", 'J', ""},
        {"jal",   "000011", 'J', ""},
        {"jr",    "000000", 'R', "001000"},
        {"lui",   "001111", 'I', ""},
        {"lw",    "100011", 'I', ""},
        {"nor",   "000000", 'R', "100111"},
        {"or",    "000000", 'R', "100101"},
        {"ori",   "001101", 'I', ""},
        {"sltiu", "001011", 'I', ""},
        {"sltu",  "000000", 'R', "101011"},
        {"sll",   "000000", 'R', "000000"},
        {"srl",   "000000", 'R', "000010"},
        {"sw",    "101011", 'I', ""},
        {"subu",  "000000", 'R', "100011"}
};

Data data[100];
int data_idx = 0;

Text text[100];
int text_idx = 0;

Func func[100];
int func_idx = 0;

int datasize, textsize;


char *change_file_ext(char *str);

void read_asm();

void print_bits();

int str_to_int(char *str);

char *get_token(char *str);

char *num_to_bits(int num, int len);

void free_();

int main(int argc, char *argv[]) {

    FILE *input, *output;
    char *filename;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <*.s>\n", argv[0]);
        fprintf(stderr, "Example: %s sample_input/example?.s\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // reading the input file
    input = freopen(argv[1], "r", stdin);
    if (input == NULL) {
        perror("ERROR");
        exit(EXIT_FAILURE);
    }

    // creating the output file (*.o)
    filename = strdup(argv[1]); // strdup() is not a standard C library but fairy used a lot.
    if (change_file_ext(filename) == NULL) {
        fprintf(stderr, "'%s' file is not an assembly file.\n", filename);
        exit(EXIT_FAILURE);
    }

    output = freopen(filename, "w", stdout);
    if (output == NULL) {
        perror("ERROR");
        exit(EXIT_FAILURE);
    }

    read_asm();
    print_bits();
    free_();
    free(filename);

    fclose(input);
    fclose(output);
    exit(EXIT_SUCCESS);
}

char *change_file_ext(char *str) {
    char *dot = strrchr(str, '.');

    if (!dot || dot == str || (strcmp(dot, ".s") != 0))
        return NULL;

    str[strlen(str) - 1] = 'o';
    return "";
}

// num to bits with sign
char *num_to_bits(int num, int len) {
    bool sign = false;

    if (num < 0) {
        num = -num;
        num--;
        sign = true;
    }

    char *bits = (char *) malloc(len + 1);
    int idx = len - 1, i, j = 0;

    while (num > 0 && idx >= 0) {
        if (num % 2 == 1) {
            bits[idx--] = '1';
        } else {
            bits[idx--] = '0';
        }
        num /= 2;
        j++;
    }

    for (i = idx; i >= 0; i--) {
        bits[i] = '0';
    }

    if (sign) {
        for (i = 0; i <= j + idx; i++) {
            if (bits[i] == '1') bits[i] = '0';
            else bits[i] = '1';
        }
    }

    return bits;
}

char *get_token(char *str) {
    int len = strlen(str);

    char *ret = (char *) malloc(sizeof(char) * 4);
    int idx = 0;

    for (int i = 0; i < len; i++) {
        if (str[i] == '(' || str[i] == ')' || str[i] == '$' || str[i] == ',')
            continue;
        ret[idx++] = str[i];
    }

    return ret;
}

void read_asm() {
    int i;
    unsigned int address;
    char temp[0x1000] = {0};

    address = 0x10000000;

    char *cur_data_name = NULL;

    for (i = 0; scanf("%s", temp) == 1;) {
        if (strcmp(temp, ".text") == 0) break;

        if (temp[strlen(temp) - 1] == ':') {
            temp[strlen(temp) - 1] = NULL;
            cur_data_name = malloc(sizeof(strlen(temp) + 1));
            strcpy(cur_data_name, temp);
        }

        if (strcmp(temp, ".word") == 0) {
            scanf("%s", temp);
            int value = str_to_int(temp);

            data[data_idx].name = malloc(sizeof(strlen(cur_data_name) + 1));
            strcpy(data[data_idx].name, cur_data_name);
            data[data_idx].value = value;
            data[data_idx].address = address;

            address += 4;
            data_idx++;
        }
    }

    datasize = address - 0x10000000;

    address = 0x400000;
    int k = 0;


    for (k = 0; scanf("%s", temp) == 1;) {
        if (temp[strlen(temp) - 1] == ':') {
            temp[strlen(temp) - 1] = NULL;

            func[func_idx].name = (char *) malloc(sizeof(char) * strlen(temp) + 1);
            strcpy(func[func_idx].name, temp);
            func[func_idx].address = address;

            func_idx++;
        }
        else {
            if (strcmp(temp, "la") == 0) {
                char *load_reg = (char *) malloc(sizeof(char) * 5);
                scanf("%s", temp);
                strcpy(load_reg, temp);

                scanf("%s", temp);
                unsigned int load_address = 0;
                for (int i = 0; i < data_idx; i++) {
                    if (strcmp(temp, data[i].name) == 0) {
                        load_address = data[i].address;
                        break;
                    }
                }

                if (load_address > 0x10000000) {
                    unsigned int upper_bit = load_address >> 16;
                    unsigned int lower_bit = load_address - ((load_address >> 16) << 16);

                    text[text_idx].op_idx = 9;
                    text[text_idx].type = 'I';

                    text[text_idx].opcode = (char *) malloc(sizeof(char) * 7);
                    text[text_idx].rs = (char *) malloc(sizeof(char) * 6);
                    text[text_idx].rt = (char *) malloc(sizeof(char) * 6);
                    text[text_idx].immediate = (char *) malloc(sizeof(char) * 17);

                    strcpy(text[text_idx].opcode, inst[9].op);
                    strcpy(text[text_idx].rs, "00000");
                    strcpy(text[text_idx].rt, num_to_bits(str_to_int(get_token(load_reg)), 5));
                    strcpy(text[text_idx].immediate, num_to_bits(upper_bit, 16));

                    text_idx++;
                    address += 4;

                    text[text_idx].op_idx = 13;
                    text[text_idx].type = 'I';

                    text[text_idx].opcode = (char *) malloc(sizeof(char) * 7);
                    text[text_idx].rs = (char *) malloc(sizeof(char) * 6);
                    text[text_idx].rt = (char *) malloc(sizeof(char) * 6);
                    text[text_idx].immediate = (char *) malloc(sizeof(char) * 17);

                    strcpy(text[text_idx].opcode, inst[13].op);
                    strcpy(text[text_idx].rs, num_to_bits(str_to_int(get_token(load_reg)), 5));
                    strcpy(text[text_idx].rt, num_to_bits(str_to_int(get_token(load_reg)), 5));
                    strcpy(text[text_idx].immediate, num_to_bits(lower_bit, 16));

                    text_idx++;
                    address += 4;
                } else {
                    unsigned int upper_bit = load_address >> 16;

                    text[text_idx].op_idx = 9;
                    text[text_idx].type = 'I';

                    text[text_idx].opcode = (char *) malloc(sizeof(char) * 7);
                    text[text_idx].rs = (char *) malloc(sizeof(char) * 6);
                    text[text_idx].rt = (char *) malloc(sizeof(char) * 6);
                    text[text_idx].immediate = (char *) malloc(sizeof(char) * 17);

                    strcpy(text[text_idx].opcode, inst[9].op);
                    strcpy(text[text_idx].rs, "00000");
                    strcpy(text[text_idx].rt, num_to_bits(str_to_int(get_token(load_reg)), 5));
                    strcpy(text[text_idx].immediate, num_to_bits(upper_bit, 16));

                    text_idx++;
                    address += 4;
                }
            } else {
                for (int i = 0; i < 20; i++) {
                    if (strcmp(temp, inst[i].name) == 0) {
                        text[text_idx].op_idx = i;

                        if (inst[i].type == 'R') {
                            text[text_idx].type = 'R';
                            text[text_idx].opcode = (char *) malloc(sizeof(char) * 7);
                            text[text_idx].rs = (char *) malloc(sizeof(char) * 6);
                            text[text_idx].rt = (char *) malloc(sizeof(char) * 6);
                            text[text_idx].rd = (char *) malloc(sizeof(char) * 6);
                            text[text_idx].shamt = (char *) malloc(sizeof(char) * 6);
                            text[text_idx].funct = (char *) malloc(sizeof(char) * 7);

                            if (strcmp(temp, "sll") == 0 || strcmp(temp, "srl") == 0) {
                                char *rd = (char *) malloc(sizeof(char) * 6);
                                char *rt = (char *) malloc(sizeof(char) * 6);
                                char *shamt = (char *) malloc(sizeof(char) * 6);

                                scanf("%s", temp);
                                strcpy(rd, temp);
                                scanf("%s", temp);
                                strcpy(rt, temp);
                                scanf("%s", temp);
                                strcpy(shamt, temp);

                                strcpy(text[text_idx].opcode, inst[i].op);
                                strcpy(text[text_idx].rs, "00000");
                                strcpy(text[text_idx].rt, num_to_bits(str_to_int(get_token(rt)), 5));
                                strcpy(text[text_idx].rd, num_to_bits(str_to_int(get_token(rd)), 5));
                                strcpy(text[text_idx].shamt, num_to_bits(str_to_int(get_token(shamt)), 5));
                                strcpy(text[text_idx].funct, inst[i].funct);
                            } else if (strcmp(temp, "jr") == 0) {
                                char *rs = (char *) malloc(sizeof(char) * 6);

                                scanf("%s", temp);
                                strcpy(rs, temp);

                                strcpy(text[text_idx].opcode, inst[i].op);
                                strcpy(text[text_idx].rs, num_to_bits(str_to_int(get_token(rs)), 5));
                                strcpy(text[text_idx].rt, "00000");
                                strcpy(text[text_idx].rd, "00000");
                                strcpy(text[text_idx].shamt, "00000");
                                strcpy(text[text_idx].funct, inst[i].funct);
                            } else {
                                char *rd = (char *) malloc(sizeof(char) * 6);
                                char *rs = (char *) malloc(sizeof(char) * 6);
                                char *rt = (char *) malloc(sizeof(char) * 6);

                                scanf("%s", temp);
                                strcpy(rd, temp);
                                scanf("%s", temp);
                                strcpy(rs, temp);
                                scanf("%s", temp);
                                strcpy(rt, temp);

                                strcpy(text[text_idx].opcode, inst[i].op);
                                strcpy(text[text_idx].rs, num_to_bits(str_to_int(get_token(rs)), 5));
                                strcpy(text[text_idx].rt, num_to_bits(str_to_int(get_token(rt)), 5));
                                strcpy(text[text_idx].rd, num_to_bits(str_to_int(get_token(rd)), 5));
                                strcpy(text[text_idx].shamt, "00000");
                                strcpy(text[text_idx].funct, inst[i].funct);
                            }

                            text_idx++;
                            address += 4;
                        } else if (inst[i].type == 'I') {
                            text[text_idx].type = 'I';
                            text[text_idx].opcode = (char *) malloc(sizeof(char) * 7);
                            text[text_idx].rs = (char *) malloc(sizeof(char) * 6);
                            text[text_idx].rt = (char *) malloc(sizeof(char) * 6);
                            text[text_idx].immediate = (char *) malloc(sizeof(char) * 17);

                            if (strcmp(temp, "beq") == 0 || strcmp(temp, "bne") == 0) {
                                char *rs = (char *) malloc(sizeof(char) * 6);
                                char *rt = (char *) malloc(sizeof(char) * 6);

                                scanf("%s", temp);
                                strcpy(rs, temp);
                                scanf("%s", temp);
                                strcpy(rt, temp);
                                scanf("%s", temp);
                                strcpy(text[text_idx].immediate, temp);

                                strcpy(text[text_idx].opcode, inst[i].op);
                                strcpy(text[text_idx].rs, num_to_bits(str_to_int(get_token(rs)), 5));
                                strcpy(text[text_idx].rt, num_to_bits(str_to_int(get_token(rt)), 5));
                            } else if (strcmp(temp, "sw") == 0 || strcmp(temp, "lw") == 0) {
                                char *rt = (char *) malloc(sizeof(char) * 6);
                                char *immediate = (char *) malloc(sizeof(char) * 17);
                                char *rs = (char *) malloc(sizeof(char) * 6);

                                char *token1 = (char *) malloc(sizeof(char) * 10);
                                char *token2 = (char *) malloc(sizeof(char) * 10);
                                int token_idx = 0;

                                scanf("%s", temp);
                                strcpy(rt, temp);
                                scanf("%s", temp);

                                int break_idx = 0;

                                for (int j = 0; j < strlen(temp); j++) {
                                    if (temp[j] == '(') {
                                        break_idx = j;
                                        break;
                                    }
                                    token1[token_idx++] = temp[j];
                                }

                                token_idx = 0;
                                for (int j = break_idx + 2; j < strlen(temp); j++) {
                                    if (temp[j] == ')') break;
                                    token2[token_idx++] = temp[j];
                                }
                                strcpy(immediate, token1);
                                strcpy(rs, token2);

                                strcpy(text[text_idx].opcode, inst[i].op);
                                strcpy(text[text_idx].rs, num_to_bits(str_to_int(rs), 5));
                                strcpy(text[text_idx].rt, num_to_bits(str_to_int(get_token(rt)), 5));
                                strcpy(text[text_idx].immediate, num_to_bits(str_to_int(immediate), 16));
                            } else if (strcmp(temp, "lui") == 0) {
                                char *rt = (char *) malloc(sizeof(char) * 6);
                                char *immediate = (char *) malloc(sizeof(char) * 17);

                                scanf("%s", temp);
                                strcpy(rt, temp);
                                scanf("%s", temp);
                                strcpy(immediate, temp);

                                strcpy(text[text_idx].opcode, inst[i].op);
                                strcpy(text[text_idx].rt, num_to_bits(str_to_int(get_token(rt)), 5));
                                strcpy(text[text_idx].rs, "00000");
                                strcpy(text[text_idx].immediate, num_to_bits(str_to_int(immediate), 16));
                            } else {
                                char *rt = (char *) malloc(sizeof(char) * 6);
                                char *rs = (char *) malloc(sizeof(char) * 6);
                                char *immediate = (char *) malloc(sizeof(char) * 17);

                                scanf("%s", temp);
                                strcpy(rt, temp);
                                scanf("%s", temp);
                                strcpy(rs, temp);
                                scanf("%s", temp);
                                strcpy(immediate, temp);

                                strcpy(text[text_idx].opcode, inst[i].op);
                                strcpy(text[text_idx].rt, num_to_bits(str_to_int(get_token(rt)), 5));
                                strcpy(text[text_idx].rs, num_to_bits(str_to_int(get_token(rs)), 5));
                                strcpy(text[text_idx].immediate, num_to_bits(str_to_int(immediate), 16));
                            }

                            text_idx++;
                            address += 4;
                        } else if (inst[i].type == 'J') {
                            text[text_idx].type = 'J';
                            text[text_idx].opcode = (char *) malloc(sizeof(char) * 7);
                            text[text_idx].address = (char *) malloc(sizeof(char) * 27);

                            scanf("%s", temp);

                            strcpy(text[text_idx].opcode, inst[i].op);
                            strcpy(text[text_idx].address, temp);

                            text_idx++;
                            address += 4;
                        }
                        break;
                    }
                }
            }
        }
    }

    textsize = address - 0x400000;
}

void print_bits() {
    printf("%s", num_to_bits(textsize, 32));
    printf("%s", num_to_bits(datasize, 32));


    for (int i = 0; i < text_idx; i++) {
        if (text[i].type == 'R') {
            printf("%s", text[i].opcode);
            printf("%s", text[i].rs);
            printf("%s", text[i].rt);
            printf("%s", text[i].rd);
            printf("%s", text[i].shamt);
            printf("%s", text[i].funct);
        } else if (text[i].type == 'I') {
            if (text[i].op_idx == 4 || text[i].op_idx == 5) {
                printf("%s", text[i].opcode);
                printf("%s", text[i].rs);
                printf("%s", text[i].rt);

                for (int j = 0; j < func_idx; j++) {
                    if (strcmp(text[i].immediate, func[j].name) == 0) {
                        printf("%s", num_to_bits((func[j].address - (0x40000 + ((i + 1) << 2))) >> 2, 16));
                        break;
                    }
                }
            } else {
                printf("%s", text[i].opcode);
                printf("%s", text[i].rs);
                printf("%s", text[i].rt);
                printf("%s", text[i].immediate);
            }
        } else {
            printf("%s", text[i].opcode);

            for (int j = 0; j < func_idx; j++) {
                if (strcmp(text[i].address, func[j].name) == 0) {
                    printf("%s", num_to_bits(func[j].address >> 2, 26));
                    break;
                }
            }
        }
    }

    for (int i = 0; i < data_idx; i++) {
        printf("%s", num_to_bits(data[i].value, 32));
    }

    printf("\n");
}

int str_to_int(char *str) {
    int ret = 0;
    int position = 1;
    int len = strlen(str);

    if (str[1] == 'x') {
        for (int i = len - 1; i > 1; i--) {
            char ch = str[i];

            if (ch >= 48 && ch <= 57) ret += (ch - 48) * position;

            else if (ch >= 65 && ch <= 70) ret += (ch - (65 - 10)) * position;

            else if (ch >= 97 && ch <= 102) ret += (ch - (97 - 10)) * position;

            position *= 16;
        }
    } else {
        if (str[0] == '-') {
            for (int i = len - 1; i >= 1; i--) {
                char ch = str[i];
                ret += (ch - '0') * position;
                position *= 10;
            }
            ret *= -1;
        } else {
            for (int i = len - 1; i >= 0; i--) {
                char ch = str[i];
                ret += (ch - '0') * position;
                position *= 10;
            }
        }
    }
    return ret;
}

void free_(){
    for (int i = 0; i < data_idx; i++) {
        free(data[i].name);
    }

    for (int i = 0; i < text_idx; i++) {
        free(text[i].opcode);
        free(text[i].rs);
        free(text[i].rt);
        free(text[i].rd);
        free(text[i].shamt);
        free(text[i].funct);
        free(text[i].immediate);
        free(text[i].address);
    }

    for (int i = 0; i < func_idx; i++) {
        free(func[i].name);
    }
}