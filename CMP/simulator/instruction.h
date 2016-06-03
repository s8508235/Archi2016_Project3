#ifndef INSTRUCTION_H_INCLUDED
#define INSTRUCTION_H_INCLUDED
#define R 0
#define I 1
#define J 2
#define T 3
/*define R-type */
#define add 0x20
#define addu 0x21
#define sub 0x22
#define AND 0x24
#define OR 0x25
#define XOR 0x26
#define NOR 0x27
#define NAND 0x28
#define slt 0x2A
#define sll 0x00
#define srl 0x02
#define sra 0x03
#define jr 0x08
/*define I-type*/
#define addi 0x08
#define addiu 0x09
#define lw 0x23
#define lh 0x21
#define lhu 0x25
#define lb 0x020
#define lbu 0x24
#define sw 0x2B
#define sh 0x29
#define sb 0x28
#define lui 0x0F
#define andi 0x0C
#define ori 0x0D
#define nori 0x0E
#define slti 0x0A
#define beq 0x04
#define bne 0x05
#define bgtz 0x07
/*define J-type*/
#define j 0x02
#define jal 0x03
#define halt 0x3F
#define $sp 29
int instruction_operation(int *opcode,unsigned *reg,int c, int *pc,unsigned *Dmem,unsigned conpc);
void instruction_R(int *code,unsigned *reg, int *pc);
void instruction_J(int *code,int ct,unsigned *reg, int *pc);
void instruction_I(int *code,int ct,unsigned *reg, int *pc,unsigned *Dmem);
void getrsrtrd(unsigned *rs,unsigned *rt,unsigned *rd,int *code);
void getrsrtimm(unsigned *rs,unsigned *rt,int *code,unsigned *imm);
int cycle;
#endif // INSTRUCTION_H_INCLUDED
