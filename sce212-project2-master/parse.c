/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   SCE212 Ajou University                                    */
/*   parse.c                                                   */
/*   Adapted from CS311@KAIST                                  */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include "util.h"
#include "parse.h"

int text_size;
int data_size;

instruction parsing_instr(const char *buffer, const int index)
{
    // buffer : binary instruction, index 를 받아서 각 instruction 별로 분류해서 instr 값에 넣어주면 됨.
    // instr value는 시작주소+index
    // memory에 write도 시켜줘야함!!!!
    instruction instr;
    char *opcode, *rs, *rt, *rd, *shamt, *imm, *add, *func;

    char *temp = (char *)malloc(sizeof(char) * 33);
    strcpy(temp, buffer);

    opcode = (char *)malloc(sizeof(char) * 7);
    for (int i = 0; i < 6; i++) opcode[i] = temp[i];
    instr.opcode = fromBinary(opcode);

    switch(instr.opcode) {
        //Type I
        case 0x9:		//(0x001001)ADDIU
        case 0xc:		//(0x001100)ANDI
        case 0xf:		//(0x001111)LUI
        case 0xd:		//(0x001101)ORI
        case 0xb:		//(0x001011)SLTIU
        case 0x23:		//(0x100011)LW
        case 0x2b:		//(0x101011)SW
        case 0x4:		//(0x000100)BEQ
        case 0x5:		//(0x000101)BNE
            rs = (char *)malloc(sizeof(char) * 6);
            rt = (char *)malloc(sizeof(char) * 6);
            imm = (char *)malloc(sizeof(char) * 17);

            for (int i = 0; i < 5; i++) {
                rs[i] = temp[i + 6];
                rt[i] = temp[i + 11];
            }
            for (int i = 0; i < 16; i++) imm[i] = temp[i + 16];

            instr.r_t.r_i.rs = fromBinary(rs);
            instr.r_t.r_i.rt = fromBinary(rt);
            instr.r_t.r_i.r_i.imm = fromBinary(imm);
            break;

            //TYPE R
        case 0x0:		//(0x000000)ADDU, AND, NOR, OR, SLTU, SLL, SRL, SUBU  if JR
            rs = (char *)malloc(sizeof(char) * 6);
            rt = (char *)malloc(sizeof(char) * 6);
            rd = (char *)malloc(sizeof(char) * 6);
            shamt = (char *)malloc(sizeof(char) * 6);
            func = (char *)malloc(sizeof(char) * 7);

            for (int i = 0; i < 5; i++) {
                rs[i] = temp[i + 6];
                rt[i] = temp[i + 11];
                rd[i] = temp[i + 16];
                shamt[i] = temp[i + 21];
            }
            for (int i = 0; i < 6; i++) func[i] = temp[i + 26];

            instr.r_t.r_i.rs = fromBinary(rs);
            instr.r_t.r_i.rt = fromBinary(rt);
            instr.r_t.r_i.r_i.r.rd = fromBinary(rd);
            instr.r_t.r_i.r_i.r.shamt = fromBinary(shamt);
            instr.func_code = fromBinary(func);

            break;

            //TYPE J
        case 0x2:		//(0x000010)J
        case 0x3:		//(0x000011)JAL
            add = (char *)malloc(sizeof(char) * 27);

            for (int i = 0; i < 26; i++) add[i] = temp[i + 6];

            instr.r_t.target = fromBinary(add);

            break;

        default:
            break;
    }

    instr.value = 0x400000 + index;
    mem_write_32(0x400000 + index, fromBinary(temp)); // instruction이 memory에 올라감

    return instr;
}

void parsing_data(const char *buffer, const int index)
{ //data 영역에서 가져와서 memory에 write 해주면 될듯.
    char *temp = (char *)malloc(sizeof(char) * 33);
    strcpy(temp, buffer);

    mem_write_32(0x10000000 + index, fromBinary(temp));
}

void print_parse_result()
{
    int i;
    printf("Instruction Information\n");

    for(i = 0; i < text_size/4; i++)
    {
        printf("INST_INFO[%d].value : %x\n",i, INST_INFO[i].value);
        printf("INST_INFO[%d].opcode : %d\n",i, INST_INFO[i].opcode);

	    switch(INST_INFO[i].opcode)
        {
            //Type I
            case 0x9:		//(0x001001)ADDIU
            case 0xc:		//(0x001100)ANDI
            case 0xf:		//(0x001111)LUI	
            case 0xd:		//(0x001101)ORI
            case 0xb:		//(0x001011)SLTIU
            case 0x23:		//(0x100011)LW
            case 0x2b:		//(0x101011)SW
            case 0x4:		//(0x000100)BEQ
            case 0x5:		//(0x000101)BNE
                printf("INST_INFO[%d].rs : %d\n",i, INST_INFO[i].r_t.r_i.rs);
                printf("INST_INFO[%d].rt : %d\n",i, INST_INFO[i].r_t.r_i.rt);
                printf("INST_INFO[%d].imm : %d\n",i, INST_INFO[i].r_t.r_i.r_i.imm);
                break;

            //TYPE R
            case 0x0:		//(0x000000)ADDU, AND, NOR, OR, SLTU, SLL, SRL, SUBU  if JR
                printf("INST_INFO[%d].func_code : %d\n",i, INST_INFO[i].func_code);
                printf("INST_INFO[%d].rs : %d\n",i, INST_INFO[i].r_t.r_i.rs);
                printf("INST_INFO[%d].rt : %d\n",i, INST_INFO[i].r_t.r_i.rt);
                printf("INST_INFO[%d].rd : %d\n",i, INST_INFO[i].r_t.r_i.r_i.r.rd);
                printf("INST_INFO[%d].shamt : %d\n",i, INST_INFO[i].r_t.r_i.r_i.r.shamt);
                break;

            //TYPE J
            case 0x2:		//(0x000010)J
            case 0x3:		//(0x000011)JAL
                printf("INST_INFO[%d].target : %d\n",i, INST_INFO[i].r_t.target);
                break;

            default:
                printf("Not available instruction\n");
                assert(0);
        }
    }

    printf("Memory Dump - Text Segment\n");
    for(i = 0; i < text_size; i+=4)
        printf("text_seg[%d] : %x\n", i, mem_read_32(MEM_TEXT_START + i));
    for(i = 0; i < data_size; i+=4)
        printf("data_seg[%d] : %x\n", i, mem_read_32(MEM_DATA_START + i));
    printf("Current PC: %x\n", CURRENT_STATE.PC);
}
