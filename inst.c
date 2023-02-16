//for print wich instruction is excuting
#include "inst.h"

uint8_t print_inst(uint32_t inst)// for print the meaning of instruction
{
	uint32_t inst_count = 0;
	uint8_t opcode = (inst & 0x7F); //make inst[6:0]
	uint8_t rd = (inst >>7) & 0x1F; //make inst[11:7]
	uint8_t imm_low = (inst >>7) & 0x1F; //make inst[11:7]
	uint8_t funct3 = (inst >>12) & 7; //make inst[14:12]
	uint8_t rs1 = (inst >>15) & 0x1F; //make inst[19:15] 
	uint8_t rs2 = (inst >>20) & 0x1F; //make inst[24:20]
	uint8_t funct7 = (inst >>25) & 0x7F; //make inst[31:25] 
	uint16_t imm12 = (inst >>20) & 0xFFF; //make inst[31:20]
	uint8_t imm_high = (inst >>25) & 0x7F; //make inst[31:25]
	int32_t imm32, imm32_branch;

	//for check the error or nothing to execute(imem execute finished) //just for debug
	uint8_t err = 0;

	if(opcode == 0b0000011 || opcode == 0b0010011) //lw, imm arithmetic
			imm32 = imm12;//imm32 = {{20{inst[31]}},inst[31:20]};
	else if (opcode == 0b0100011) //store
			imm32 = (imm_high<<5)+imm_low;//imm32 = {{20{inst[31]}}, inst[31:25], inst[11:7]};
	else if (opcode == 0b0110111 || opcode == 0b0010111) //LUI or AUIP
			imm32 = inst & 0x000;//imm32 = {inst[31:12], 12'h000 };
	else if (opcode == 0b1100111)
			imm32 = imm12;//imm32 = {{20{inst[31]}},inst[31:20]};
	else
			imm32 = 0;//imm32 = 32'h000;
	
	if(opcode == 0b0010111) // AUIPC
		imm32_branch = inst & 0xFFFFF000;//imm32 = {inst[31:12], 12'h000 };
	else if(opcode == 0b1100011) //SB-type
		imm32_branch = ((((inst>>7)&1)<<11) + (((inst>>25)&0x3F)<<5) + (((inst>>8)&0xF)<<1));
		//imm32_branch = {{20{inst[31]}}, inst[7], inst[30:25], inst[11:8], 1'b0};
	else if(opcode == 0b1101111 || opcode == 0b1100111) //UJ-type
		imm32_branch = ((((inst>>12)&0xFF)<<12) + (((inst>>20)&1)<<11) + (((inst>>21)&0x3FF)<<1));
		//imm32_branch = {{12{inst[31]}}, inst[19:12], inst[20], inst[30:25], inst[24:21], 1'b0};
	else
		imm32_branch = 0;

	if(opcode == 0b00000011) //I-type Load instructions
	{
		err = 2;
		if(funct3 == 0b000)
			printf("LB r%d, r%d, %d ( %x )",rd,rs1,imm32,imm32);
		else if(funct3 == 0b001)
			printf("LH r%d, r%d, %d ( %x )",rd,rs1,imm32,imm32);
		else if(funct3 == 0b010)
			printf("LW r%d, r%d, %d ( %x )",rd,rs1,imm32,imm32);
		else if(funct3 == 0b100)
			printf("LBU r%d, r%d, %d ( %x )",rd,rs1,imm32,imm32);
		else if(funct3 == 0b101)
			printf("LHU r%d, r%d, %d ( %x )",rd,rs1,imm32,imm32);
	}
	else if(opcode == 0b0100011) //S-type, store instruction
	{
		err = 2;
		if(funct3 == 0b000)
			printf("SB r%d, r%d, %d",rs2,rs1,imm32);
		else if(funct3 == 0b001)
			printf("SH r%d, r%d, %d",rs2,rs1,imm32);
		else if(funct3 == 0b010)
			printf("SW r%d, r%d, %d",rs2,rs1,imm32);

	}
	else if(opcode == 0b0110011) // R type, arithmetic instruction and bitwise operation
	{
		if(funct3 == 0b001)
			printf("SLL r%d, r%d, r%d, Shift Left", rd, rs1, rs2);
		else if(funct3 == 0b101)
		{
			if(funct7 == 0b0100000)
				printf("SRA r%d, r%d, r%d, Shift Right Arithmetic", rd, rs1, rs2);
			else if(funct7 == 0)
				printf("SRL r%d, r%d, r%d, Shift Right", rd, rs1, rs2);
		}
		else if(funct3 == 0b000)
		{
			if(funct7 == 0b0100000)
				printf("SUB r%d, r%d, r%d", rd, rs1, rs2);
			else if(funct7 == 0)
				printf("ADD r%d, r%d, r%d", rd, rs1, rs2);
		}
		else if(funct3 == 0b100)
			printf("XOR r%d, r%d, r%d", rd, rs1, rs2);
		else if(funct3 == 0b110)
			printf("OR r%d, r%d, r%d", rd, rs1, rs2);
		else if(funct3 == 0b111)
			printf("AND r%d, r%d, r%d", rd, rs1, rs2);
		else if(funct3 == 0b010)
			printf("SLT r%d, r%d, r%d", rd, rs1, rs2);
		else if(funct3 == 0b011)
			printf("SLTU r%d, r%d, r%d", rd, rs1, rs2);
	}
	else if(opcode == 0b0010011) //I type, arithmetic and bitwise operation
	{
		err = 2;
		if(funct3 == 0b001)
			printf("SLLI r%d, r%d, %d, Shift Left Immediate", rd, rs1, imm32);
		else if(funct3 == 0b101)
		{
			if(funct7 == 0b0000000)
				printf("SRLI r%d, r%d, %d, Shift Right Immediate", rd, rs1, imm32);
			else if(funct7 == 0b0100000)
				printf("SRAI r%d, r%d, %d, Shift Right Arithmetic Immediate", rd, rs1, imm32);
		}
		else if(funct3 == 0b000)
			printf("ADDI r%d, r%d, %d, ADD Immediate", rd, rs1, imm32);
		else if(funct3 == 0b100)
			printf("XORI r%d, r%d, %d", rd, rs1, imm32);
		else if(funct3 == 0b110)
			printf("ORI r%d, r%d, %d", rd, rs1, imm32);
		else if(funct3 == 0b111)
			printf("ANDI r%d, r%d, %d", rd, rs1, imm32);
		else if(funct3 == 0b010)
			printf("SLTI r%d, r%d, %d", rd, rs1, imm32);
		else if(funct3 == 0b011)
			printf("SLTIU r%d, r%d, %d", rd, rs1, imm32);
	}
	else if(opcode == 0b0110111)
	{
		printf("LUI r%d, %d, Load Upper Immediate",rd,imm32);
		err = 2;
	}
	else if(opcode == 0b0010111)
	{
		err = 3;
		printf("AUIPC r%d, %d, Add Upper Immediate to PC",rd,imm32_branch);
	}
	else if(opcode == 0b1100011)
	{
		if(funct3 == 0b000)
			printf("BEQ r%d, r%d, %d ( %x )", rs1, rs2, imm32_branch, imm32_branch);
		else if(funct3 == 0b001)
			printf("BNE r%d, r%d, %d ( %x )", rs1, rs2, imm32_branch, imm32_branch);
		else if(funct3 == 0b100)
			printf("BLT r%d, r%d, %d ( %x )", rs1, rs2, imm32_branch, imm32_branch);
		else if(funct3 == 0b101)
			printf("BGE r%d, r%d, %d ( %x )", rs1, rs2, imm32_branch, imm32_branch);
		else if(funct3 == 0b110)
			printf("BLTU r%d, r%d, %d ( %x )", rs1, rs2, imm32_branch, imm32_branch);
		else if(funct3 == 0b111)
			printf("BGEU r%d, r%d, %d ( %x )", rs1, rs2, imm32_branch, imm32_branch);
	}
	else if(opcode == 0b1101111)
	{
		err = 2;
		printf("JAL r%d, %d", rd, imm32_branch);
	}
	else if(opcode == 0b1100111)
	{
		printf("JALR r%d, r%d, %d", rd, rs1, imm32_branch);
		err = 2;
	}
	else
	{
		printf("UNDECLARED INSTRUCTION\n");
		err = 1;
	}

	printf("\n");
	return err;
}

