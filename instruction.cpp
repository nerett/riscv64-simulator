#include "instruction.hpp"
#include "hart.hpp"

void decode_instruction_R(Instruction &ins, instT code) {
    DEB("decs R");
    RS1_INIT RS2_INIT RD_INIT
}

void decode_instruction_I(Instruction &ins, instT code) {
    DEB("decs I");
    RS1_INIT RD_INIT SIGN_INIT
    ins.imm = ((code&INSN_FIELD_IMM12) >> 20) | (sign * SIGNN(11));
}

void decode_instruction_S(Instruction &ins, instT code) {
    DEB("decs S");
    RS1_INIT RS2_INIT SIGN_INIT
    ins.imm = ((code&INSN_FIELD_IMM12HI) >> 20)
        | ((code&INSN_FIELD_IMM12LO) >> 7)
        | (sign * SIGNN(12));
}

void decode_instruction_B(Instruction &ins, instT code) {
    DEB("decs B");
    RS1_INIT RS2_INIT SIGN_INIT
    ins.imm = ((code & (1<<7)) << 4)
        | ((code & 0xfe000000) >> 20)
        | ((code & 0xf00) >> 7)
        | (sign * SIGNN(12));
}

void decode_instruction_U(Instruction &ins, instT code) {
    DEB("decs U");
    RD_INIT SIGN_INIT
    ins.imm = (code & INSN_FIELD_IMM20) | (sign * SIGNN(31));
}

void decode_instruction_J(Instruction &ins, instT code) {
    DEB("decs J");
    SIGN_INIT RD_INIT
    ins.imm = ((code & 0x7ff00000) >> 20) & ~1ULL
        | ((code & (1<<20)) >> 9)
        | ((code & (0xff000)))
        | (sign * SIGNN(20));
}

namespace Executors {

// for basic blocks
void empty_executor(Hart *hart, const Instruction &instr) {
    return;
}

#define EXEC_RET_true (&instr+1)->execute(heart, *(&instr+1))
#define EXEC_RET_false 

#define _INSTR_(name, type, code, linear) \
__attribute__((noinline)) void exec_##name(Hart *heart, const Instruction &instr) { \
    DEB("exec "#name); \
    {int new_pc = heart->pc+4; code; heart->pc = new_pc;} \
    EXEC_RET_##linear; }

#include "instrs.h"
#undef _INSTR_

};
