/* **************************************
 * Module: top design of rv32i pipeline processor
 *
 * Author:
 *
 * **************************************
 */

// headers
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


// defines
#define REG_WIDTH 32
#define IMEM_DEPTH 1024
#define DMEM_DEPTH 1024

// configs
#define CLK_NUM 30

//configs for ALU(modified by Taewoon)
#define AND 0b0000
#define OR 0b0001
#define ADD 0b0010
#define SUB 0b0110
#define XOR 0b0011
#define SLL 0b1000
#define SRL 0b1001
#define SRA 0b1010 //Shift right arithmetic
#define SLT 0b1100 //set less than
#define SLU 0b1101 //set less than unsigned

// structures
typedef struct imem_input_t {
	uint32_t addr;
}imem_input_t;

typedef struct imem_output_t {
	uint32_t dout;
}imem_output_t;;

typedef struct dmem_input_t {
	uint32_t addr;
	int32_t din;
	uint8_t rd_en;
	uint8_t wr_en;
	uint8_t sz;
	uint8_t sign;
}dmem_input_t;

typedef struct dmem_output_t{
	int32_t dout;
}dmem_output_t;

typedef struct regfile_input_t{
	int32_t rd_din;
	uint8_t rs1;
	uint8_t rs2;
	uint8_t rd;
	uint8_t reg_write;
}regfile_input_t;

typedef struct regfile_output_t{
	int32_t rs1_dout;
	int32_t rs2_dout;
}regfile_output_t;

typedef struct alu_input_t{
	int32_t in1;
	int32_t in2;
	int8_t alu_control;
}alu_input_t;

typedef struct alu_output_t{
	int32_t result;
	uint8_t zero;
	uint8_t sign;
}alu_output_t;

typedef struct control_input_t{
	uint8_t funct3;
	uint8_t opcode;
}control_input_t;

typedef struct control_output_t{
	uint8_t branch0;
	uint8_t branch1;
	uint8_t branch2;
	uint8_t branch3;
	uint8_t branch4;
	uint8_t mem_read;
	uint8_t mem_write;
	uint8_t mem_to_reg;
	uint8_t reg_write;
	uint8_t alu_src;
	uint8_t alu_op;
	uint8_t jal_check; //added by taewoon
}control_output_t;

typedef struct alu_control_input_t{
	uint8_t opcode;
	uint8_t alu_op;
	uint8_t funct3;
	uint8_t funct7;
}alu_control_input_t;

typedef struct alu_control_output_t{
	uint8_t alu_control;
}alu_control_output_t;

//pipe regL ID/EX
typedef struct pipe_if_id_t {
    uint32_t	pc;
    uint32_t    inst;
} pipe_if_id_t;

//pipe reg: ID/EX
typedef struct pipe_id_ex_t {
    uint32_t	pc;
    uint32_t    rs1_dout;
    uint32_t    rs2_dout;
    uint32_t    imm32;
    uint32_t    imm32_branch; //added by tw
	uint8_t     funct3;
    uint8_t     funct7;
    uint8_t     branch0;     // branch[0] = beq, branch[1] = bne, branch[2] = blt, branch[3] = bge
    uint8_t     branch1;     // branch[0] = beq, branch[1] = bne, branch[2] = blt, branch[3] = bge
    uint8_t     branch2;     // branch[0] = beq, branch[1] = bne, branch[2] = blt, branch[3] = bge
    uint8_t     branch3;     // branch[0] = beq, branch[1] = bne, branch[2] = blt, branch[3] = bge
	uint8_t		branch4;
    uint32_t    alu_src;
    uint8_t     alu_op;
    uint8_t     mem_read;
    uint8_t     mem_write;
    uint8_t     rs1;
    uint8_t     rs2;
    uint8_t     rd;         // rd for regfile
    uint8_t     reg_write;
    uint8_t     mem_to_reg;
    uint8_t     jal_check; //added by tw
    uint8_t     opcode; //added by tw
} pipe_id_ex_t;

// Pipe reg: EX/MEM
typedef struct pipe_ex_mem_t {
    uint32_t    alu_result; // for address
    uint32_t    rs2_dout;   // for store & tw : forwarding
    uint8_t		mem_write;
    uint8_t		mem_read;
    uint8_t		rd;
    uint8_t		reg_write;
    uint8_t		mem_to_reg;
    uint32_t    pc; //added by tw
    uint8_t		jal_check; //added by tw
    uint32_t    jal_rd_din; //added by tw
    uint32_t	rs1_dout; //added by tw
    uint8_t    	funct3; //added by tw
} pipe_ex_mem_t;

// Pipe reg: MEM/WB
typedef struct pipe_mem_wb_t {
    uint32_t    alu_result;
    uint32_t    dmem_dout;
    uint8_t   	rd;
    uint8_t     reg_write;
    uint8_t     mem_to_reg;
    uint32_t    pc; //added by tw
    uint8_t     jal_check; //added by tw
    uint32_t	jal_rd_din; //added by tw
    uint32_t    rs1_dout; //added by tw
    uint32_t    rs2_dout; //added bt tw
} pipe_mem_wb_t;
