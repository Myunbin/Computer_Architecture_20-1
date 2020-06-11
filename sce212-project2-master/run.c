/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   SCE212 Ajou University                                    */
/*   run.c                                                     */
/*   Adapted from CS311@KAIST                                  */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdbool.h>
#include "util.h"
#include "run.h"

/***************************************************************/
/*                                                             */
/* Procedure: get_inst_info                                    */
/*                                                             */
/* Purpose: Read insturction information                       */
/*                                                             */
/***************************************************************/
instruction* get_inst_info(uint32_t pc)
{
    return &INST_INFO[(pc - MEM_TEXT_START) >> 2];
}

/***************************************************************/
/*                                                             */
/* Procedure: process_instruction                              */
/*                                                             */
/* Purpose: Process one instrction                             */
/*                                                             */
/***************************************************************/
void process_instruction()
{
    instruction *cur_instr = get_inst_info(CURRENT_STATE.PC); // 현재 instruction 가져옴

    if (cur_instr->value == CURRENT_STATE.PC) {
        CURRENT_STATE.PC += 4; // 다음 PC를 가르키도록 설정해줌

        unsigned char rs = RS(cur_instr);
        unsigned char rt = RT(cur_instr);
        unsigned char rd = RD(cur_instr);
        unsigned char shamt = SHAMT(cur_instr);
        short func = FUNC(cur_instr);
        short imm = IMM(cur_instr);
        short opcode = OPCODE(cur_instr);
        uint32_t target = TARGET(cur_instr);

        if (opcode == 0x00) { // R type
            if (func == 0x21) CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt]; // addiu
            else if (func == 0x24) CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt]; //and
            else if (func == 0x08) CURRENT_STATE.PC = CURRENT_STATE.REGS[rs]; // jr
            else if (func == 0x27) CURRENT_STATE.REGS[rd] = ~((CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]));//nor
            else if (func == 0x25) CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]; //or
            else if (func == 0x2b) CURRENT_STATE.REGS[rd] = (CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt]) ? true : false; //sltu
            else if (func == 0x00) CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << shamt;//sll
            else if (func == 0x02) CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> shamt;//srl
            else if (func == 0x23) CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];//subu
            else return;
        }
            //I type
        else if (opcode == 0x09) CURRENT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + SIGN_EX(imm);//addiu
        else if (opcode == 0x0c) CURRENT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] & imm; //andi
        else if (opcode == 0x04) {//beq
            if (CURRENT_STATE.REGS[rt] == CURRENT_STATE.REGS[rs])
                CURRENT_STATE.PC += imm << 2;
        }
        else if (opcode == 0x05) {//bne
            if (CURRENT_STATE.REGS[rt] != CURRENT_STATE.REGS[rs])
                CURRENT_STATE.PC += imm << 2;
        }
        else if (opcode == 0x0f) CURRENT_STATE.REGS[rt] = imm << 16;//lui
        else if (opcode == 0x23) CURRENT_STATE.REGS[rt] = mem_read_32(CURRENT_STATE.REGS[rs] + SIGN_EX(imm));//lw
        else if (opcode == 0x0d) CURRENT_STATE.REGS[rt] = (CURRENT_STATE.REGS[rs] | imm); //ori
        else if (opcode == 0x0b) CURRENT_STATE.REGS[rt] = (CURRENT_STATE.REGS[rs] < SIGN_EX(imm)) ? true : false;//sltiu
        else if (opcode == 0x2b) mem_write_32(CURRENT_STATE.REGS[rs] + SIGN_EX(imm), CURRENT_STATE.REGS[rt]);//sw
            //J type
        else if (opcode == 0x02) {//j
            JUMP_INST(target << 2);
        }
        else if (opcode == 0x03) {//jal
            CURRENT_STATE.REGS[31]= CURRENT_STATE.PC + 4;
            JUMP_INST(target << 2);
        }
        else return;
    }
    else RUN_BIT = false;
}
