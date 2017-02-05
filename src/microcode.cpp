#include "microcode.h"
#include <memory.h>
#include <malloc.h>
#include <assert.h>

#define REG_A    0x07
#define REG_M    0x06

#define RP_BC    0x00
#define RP_DE    0x01
#define RP_HL    0x02
#define RP_SP    0x03
#define RP_PSW   0x03 // Used only by PUSH/POP

#define NOP      0x00
#define LXI_B    0x01
#define STAX_B   0x02
#define INX_B    0x03
#define INR_B    0x04
#define DCR_B    0x05
#define MVI_B    0x06
#define RLC      0x07
//#define NOP      0x08
#define DAD_B    0x09
#define LDAX_B   0x0A
#define DCX_B    0x0B
#define INR_C    0x0C
#define DCR_C    0x0D
#define MVI_C    0x0E
#define RRC      0x0F
//#define NOP      0x10
#define LXI_D    0x11
#define STAX_D   0x12
#define INX_D    0x13
#define INR_D    0x14
#define DCR_D    0x15
#define MVI_D    0x16
#define RAL      0x17
//#define NOP      0x18
#define DAD_D    0x19
#define LDAX_D   0x1A
#define DCX_D    0x1B
#define INR_E    0x1C
#define DCR_E    0x1D
#define MVI_E    0x1E
#define RAR      0x1F
//#define NOP      0x20
#define LXI_H    0x21
#define SHLD     0x22
#define INX_H    0x23
#define INR_H    0x24
#define DCR_H    0x25
#define MVI_H    0x26
#define DAA      0x27
//#define NOP      0x28
#define DAD_H    0x29
#define LHLD     0x2A
#define DCX_H    0x2B
#define INR_L    0x2C
#define DCR_L    0x2D
#define MVI_L    0x2E
#define CMA      0x2F
//#define NOP      0x30
#define LXI_SP   0x31
#define STA      0x32
#define INX_SP   0x33
#define INR_M    0x34
#define DCR_M    0x35
#define MVI_M    0x36
#define STC      0x37
//#define NOP      0x38
#define DAD_SP   0x39
#define LDA      0x3A
#define DCX_SP   0x3B
#define INR_A    0x3C
#define DCR_A    0x3D
#define MVI_A    0x3E
#define CMC      0x3F
#define MOV_BB   0x40
#define MOV_BC   0x41
#define MOV_BD   0x42
#define MOV_BE   0x43
#define MOV_BH   0x44
#define MOV_BL   0x45
#define MOV_BM   0x46
#define MOV_BA   0x47
#define MOV_CB   0x48
#define MOV_CC   0x49
#define MOV_CD   0x4A
#define MOV_CE   0x4B
#define MOV_CH   0x4C
#define MOV_CL   0x4D
#define MOV_CM   0x4E
#define MOV_CA   0x4F
#define MOV_DB   0x50
#define MOV_DC   0x51
#define MOV_DD   0x52
#define MOV_DE   0x53
#define MOV_DH   0x54
#define MOV_DL   0x55
#define MOV_DM   0x56
#define MOV_DA   0x57
#define MOV_EB   0x58
#define MOV_EC   0x59
#define MOV_ED   0x5A
#define MOV_EE   0x5B
#define MOV_EH   0x5C
#define MOV_EL   0x5D
#define MOV_EM   0x5E
#define MOV_EA   0x5F
#define MOV_HB   0x60
#define MOV_HC   0x61
#define MOV_HD   0x62
#define MOV_HE   0x63
#define MOV_HH   0x64
#define MOV_HL   0x65
#define MOV_HM   0x66
#define MOV_HA   0x67
#define MOV_LB   0x68
#define MOV_LC   0x69
#define MOV_LD   0x6A
#define MOV_LE   0x6B
#define MOV_LH   0x6C
#define MOV_LL   0x6D
#define MOV_LM   0x6E
#define MOV_LA   0x6F
#define MOV_MB   0x70
#define MOV_MC   0x71
#define MOV_MD   0x72
#define MOV_ME   0x73
#define MOV_MH   0x74
#define MOV_ML   0x75
#define HLT      0x76
#define MOV_MA   0x77
#define MOV_AB   0x78
#define MOV_AC   0x79
#define MOV_AD   0x7A
#define MOV_AE   0x7B
#define MOV_AH   0x7C
#define MOV_AL   0x7D
#define MOV_AM   0x7E
#define MOV_AA   0x7F
#define ADD_B    0x80
#define ADD_C    0x81
#define ADD_D    0x82
#define ADD_E    0x83
#define ADD_H    0x84
#define ADD_L    0x85
#define ADD_M    0x86
#define ADD_A    0x87
#define ADC_B    0x88
#define ADC_C    0x89
#define ADC_D    0x8A
#define ADC_E    0x8B
#define ADC_H    0x8C
#define ADC_L    0x8D
#define ADC_M    0x8E
#define ADC_A    0x8F
#define SUB_B    0x90
#define SUB_C    0x91
#define SUB_D    0x92
#define SUB_E    0x93
#define SUB_H    0x94
#define SUB_L    0x95
#define SUB_M    0x96
#define SUB_A    0x97
#define SBB_B    0x98
#define SBB_C    0x99
#define SBB_D    0x9A
#define SBB_E    0x9B
#define SBB_H    0x9C
#define SBB_L    0x9D
#define SBB_M    0x9E
#define SBB_A    0x9F
#define ANA_B    0xA0
#define ANA_C    0xA1
#define ANA_D    0xA2
#define ANA_E    0xA3
#define ANA_H    0xA4
#define ANA_L    0xA5
#define ANA_M    0xA6
#define ANA_A    0xA7
#define XRA_B    0xA8
#define XRA_C    0xA9
#define XRA_D    0xAA
#define XRA_E    0xAB
#define XRA_H    0xAC
#define XRA_L    0xAD
#define XRA_M    0xAE
#define XRA_A    0xAF
#define ORA_B    0xB0
#define ORA_C    0xB1
#define ORA_D    0xB2
#define ORA_E    0xB3
#define ORA_H    0xB4
#define ORA_L    0xB5
#define ORA_M    0xB6
#define ORA_A    0xB7
#define CMP_B    0xB8
#define CMP_C    0xB9
#define CMP_D    0xBA
#define CMP_E    0xBB
#define CMP_H    0xBC
#define CMP_L    0xBD
#define CMP_M    0xBE
#define CMP_A    0xBF
#define RNZ      0xC0
#define POP_B    0xC1
#define JNZ      0xC2
#define JMP      0xC3
#define CNZ      0xC4
#define PUSH_B   0xC5
#define ADI      0xC6
#define RST_0    0xC7
#define RZ       0xC8
#define RET      0xC9
#define JZ       0xCA
//#define JMP      0xCB
#define CZ       0xCC
#define CALL     0xCD
#define ACI      0xCE
#define RST_1    0xCF
#define RNC      0xD0
#define POP_D    0xD1
#define JNC      0xD2
#define OUT      0xD3
#define CNC      0xD4
#define PUSH_D   0xD5
#define SUI      0xD6
#define RST_2    0xD7
#define RC       0xD8
//#define RET      0xD9
#define JC       0xDA
#define IN       0xDB
#define CC       0xDC
//#define CALL     0xDD
#define SBI      0xDE
#define RST_3    0xDF
#define RPO      0xE0
#define POP_H    0xE1
#define JPO      0xE2
#define XTHL     0xE3
#define CPO      0xE4
#define PUSH_H   0xE5
#define ANI      0xE6
#define RST_4    0xE7
#define RPE      0xE8
#define PCHL     0xE9
#define JPE      0xEA
#define XCHG     0xEB
#define CPE      0xEC
//#define CALL     0xED
#define XRI      0xEE
#define RST_5    0xEF
#define RP       0xF0
#define POP_PSW  0xF1
#define JP       0xF2
#define DI       0xF3
#define CP       0xF4
#define PUSH_PSW 0xF5
#define ORI      0xF6
#define RST_6    0xF7
#define RM       0xF8
#define SPHL     0xF9
#define JM       0xFA
#define EI       0xFB
#define CM       0xFC
//#define CALL     0xFD
#define CPI      0xFE
#define RST_7    0xFF

void romInit(DecodeROM* rom)
{
	memset(rom->m_AddressROM, 0, sizeof(uint16_t) * 256);
	memset(rom->m_States, 0, sizeof(MicroInstruction) * 2048);
	rom->m_NumStates = 0;
	rom->m_InterruptSequenceAddr = 0;
	rom->m_InterruptHaltSequenceAddr = 0;
}

void romNewInstruction(DecodeROM* rom, uint32_t opcode)
{
	assert(opcode < 256);
	rom->m_AddressROM[opcode] = rom->m_NumStates;
	rom->m_CurOpCode = opcode;
}

MicroInstruction* romAllocStates(DecodeROM* rom, uint32_t numStates)
{
	assert(rom->m_NumStates + numStates <= 2048);
	MicroInstruction* ui = &rom->m_States[rom->m_NumStates];
	rom->m_NumStates += numStates;
	return ui;
}

uint16_t romGetStateAddr(DecodeROM* rom, uint16_t relStateOffset)
{
	return rom->m_AddressROM[rom->m_CurOpCode] + relStateOffset;
}

void insertNOP(DecodeROM* rom)
{
	InstructionBuilder ib(1, romAllocStates(rom, 4), 4);

	ib.nextCycle(MachineCycleType::InstructionFetch, 4, true)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertLXI(DecodeROM* rom, uint32_t rp)
{
	RegisterFileDstReg::Enum dstRegLow = RegisterFileDstReg::fromRegisterPair_low(rp);
	RegisterFileDstReg::Enum dstRegHigh = RegisterFileDstReg::fromRegisterPair_high(rp);

	InstructionBuilder ib(4, romAllocStates(rom, 10), 10);
	
	ib.nextCycle(MachineCycleType::InstructionFetch, 1, false)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end()
	.nextCycle(MachineCycleType::MemoryRead, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, dstRegLow, RegisterFileSrcRegPair::PC, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Inc, false, false, false) // prepare destination register; addr = PC; prepare next PC
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, dstRegLow, RegisterFileSrcRegPair::PC, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Inc, false, false, true) // PC = PC + 1
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_writeReg(dstRegLow)
			.misc_DataIn()
			.flow_continue()
		.end()
	.end()
	.nextCycle(MachineCycleType::MemoryRead, 3, true)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, dstRegHigh, RegisterFileSrcRegPair::PC, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Inc, false, false, false) // prepare destination register; addr = PC; prepare next PC
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, dstRegHigh, RegisterFileSrcRegPair::PC, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Inc, false, false, true) // PC = PC + 1
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_writeReg(dstRegHigh)
			.misc_DataIn()
			.flow_continue()
		.end()
	.end()
	.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertSTAX(DecodeROM* rom, uint32_t rp)
{
	assert(rp == RP_BC || rp == RP_DE);

	InstructionBuilder ib(3, romAllocStates(rom, 7), 7);

	ib.nextCycle(MachineCycleType::InstructionFetch, 1, false)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryWrite, 3, true)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::W, RegisterFileDstReg::W, RegisterFileSrcRegPair::fromInstruction(rp), RegisterFileDstRegPair::fromInstruction(rp), RegisterFileRegPairOp::Zero, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataOut(InternalDataBusSrc::ACC)
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn() // NOTE: On T3 data has already been written to memory. 
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();	

	ib.end();
}

void insertLDAX(DecodeROM* rom, uint32_t rp)
{
	assert(rp == RP_BC || rp == RP_DE);

	InstructionBuilder ib(3, romAllocStates(rom, 7), 7);

	ib.nextCycle(MachineCycleType::InstructionFetch, 1, false)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryRead, 3, true)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::W, RegisterFileDstReg::W, RegisterFileSrcRegPair::fromInstruction(rp), RegisterFileDstRegPair::fromInstruction(rp), RegisterFileRegPairOp::Zero, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters(ACCSrc::InternalDataBus, true, ACTSrc::ACC, false, false)
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();	

	ib.end();
}

void insertRegisterPairOp(DecodeROM* rom, uint32_t rp, RegisterFileRegPairOp::Enum op)
{
	InstructionBuilder ib(1, romAllocStates(rom, 5), 5);

	ib.nextCycle(MachineCycleType::InstructionFetch, 5, true)
		.nextState()
			.state(4, false, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::W, RegisterFileDstReg::W, RegisterFileSrcRegPair::fromInstruction(rp), RegisterFileDstRegPair::fromInstruction(rp), op, false, false, false) // Prepare result
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state(5, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::W, RegisterFileDstReg::W, RegisterFileSrcRegPair::fromInstruction(rp), RegisterFileDstRegPair::fromInstruction(rp), op, false, false, true) // Write result
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertINX(DecodeROM* rom, uint32_t rp)
{
	insertRegisterPairOp(rom, rp, RegisterFileRegPairOp::Inc);
}

void insertDCX(DecodeROM* rom, uint32_t rp)
{
	insertRegisterPairOp(rom, rp, RegisterFileRegPairOp::Dec);
}

void insertDAD(DecodeROM* rom, uint32_t rp)
{
	InstructionBuilder ib(4, romAllocStates(rom, 10), 10);

	ib.nextCycle(MachineCycleType::InstructionFetch, 1, false)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::Internal, 3, false)
		.nextState()
			.state(2, false, 0) // 
			.alu_disabled()
			.flags_none()
			.internalRegisters(ACCSrc::InternalDataBus, false, ACTSrc::InternalDataBus, true, false)
			.registerFile(RegisterFileSrcReg::fromRegisterPair_low(rp), RegisterFileDstReg::B, RegisterFileSrcRegPair::PC, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Zero, false, false, false)
			.misc(InternalDataBusSrc::RegisterFile, 0, false, 0, false, false)
			.flow_continue()
		.end()
		.nextState()
			.state(2, false, 0)
			.alu(ALUASrc::ACT, ALUBSrc::TMP, ALUCSrc::Zero, ALUOp::Add)
			.flags_none()
			.internalRegisters(ACCSrc::InternalDataBus, false, ACTSrc::ACC, false, true)
			.registerFile(RegisterFileSrcReg::L, RegisterFileDstReg::L, RegisterFileSrcRegPair::PC, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Zero, false, false, false)
			.misc(InternalDataBusSrc::RegisterFile, 0, false, 0, false, false)
			.flow_continue()
		.end()
		.nextState()
			.state(3, true, 0)
			.alu(ALUASrc::ACT, ALUBSrc::TMP, ALUCSrc::Zero, ALUOp::Add)
			.flags(FlagsCSrc::ALU, FlagsPSrc::ALU, FlagsACSrc::ALU, FlagsZSrc::ALU, FlagsSSrc::ALU, true, false, false, false, false)
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::L, RegisterFileDstReg::L, RegisterFileSrcRegPair::PC, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Zero, false, true, false)
			.misc(InternalDataBusSrc::ALU, 0, false, 0, false, false)
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::Internal, 3, true)
		.nextState()
			.state(2, false, 0) // Sync signal is not generated during M2 and M3
			.alu_disabled()
			.flags_none()
			.internalRegisters(ACCSrc::InternalDataBus, false, ACTSrc::InternalDataBus, true, false) // ACT = IDB
			.registerFile(RegisterFileSrcReg::fromRegisterPair_high(rp), RegisterFileDstReg::B, RegisterFileSrcRegPair::PC, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Zero, false, false, false)
			.misc(InternalDataBusSrc::RegisterFile, 0, false, 0, false, false) // IDB = RF = rh
			.flow_continue()
		.end()
		.nextState()
			.state(2, false, 0)
			.alu(ALUASrc::ACT, ALUBSrc::TMP, ALUCSrc::Flags, ALUOp::Add) // ALU = ACT + TMP + CY
			.flags_none()
			.internalRegisters(ACCSrc::InternalDataBus, false, ACTSrc::ACC, false, true) // TMP = IDB
			.registerFile(RegisterFileSrcReg::H, RegisterFileDstReg::H, RegisterFileSrcRegPair::PC, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Zero, false, false, false)
			.misc(InternalDataBusSrc::RegisterFile, 0, false, 0, false, false) // IDB = H
			.flow_continue()
		.end()
		.nextState()
			.state(3, true, 0)
			.alu(ALUASrc::ACT, ALUBSrc::TMP, ALUCSrc::Flags, ALUOp::Add) // ALU = ACT + TMP + CY
			.flags(FlagsCSrc::ALU, FlagsPSrc::ALU, FlagsACSrc::ALU, FlagsZSrc::ALU, FlagsSSrc::ALU, true, false, false, false, false) // CY = ALU_C
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::H, RegisterFileDstReg::H, RegisterFileSrcRegPair::PC, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Zero, false, true, false) // H = IDB == ALU
			.misc(InternalDataBusSrc::ALU, 0, false, 0, false, false) // IDB = ALU
			.flow_continue()
		.end()
	.end();

	// Fetch next instruction...
	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();	

	ib.end();
}

void insertIncDec_reg(DecodeROM* rom, uint32_t reg, int dir)
{
	InstructionBuilder ib(1, romAllocStates(rom, 5), 5);

	MachineCycleBuilder& mcb = ib.nextCycle(MachineCycleType::InstructionFetch, 5, true);

	if (reg == REG_A) {
		mcb.nextState()
			.state(4, false, 0)
			.alu(ALUASrc::TMP, ALUBSrc::Zero, ALUCSrc::One, dir > 0 ? ALUOp::Add : ALUOp::Sub)
			.flags_none()
			.internalRegisters(ACCSrc::InternalDataBus, false, ACTSrc::ACC, false, true) // TMP = IDB
			.registerFile_disabled()
			.misc(InternalDataBusSrc::ACC, 0, false, 0, false, false) // IDB = ACC
			.flow_continue()
		.end()
		.nextState()
			.state(5, true, 0)
			.alu(ALUASrc::TMP, ALUBSrc::Zero, ALUCSrc::One, dir > 0 ? ALUOp::Add : ALUOp::Sub)
			.flags(FlagsCSrc::ALU, FlagsPSrc::ALU, FlagsACSrc::ALU, FlagsZSrc::ALU, FlagsSSrc::ALU, false, true, true, true, true)
			.internalRegisters(ACCSrc::InternalDataBus, true, ACTSrc::ACC, false, false) // ACC = IDB
			.registerFile_disabled()
			.misc(InternalDataBusSrc::ALU, 0, false, 0, false, false) // IDB = ALU
			.flow_continue()
		.end();
	} else {
		mcb.nextState()
			.state(4, false, 0)
			.alu(ALUASrc::TMP, ALUBSrc::Zero, ALUCSrc::One, dir > 0 ? ALUOp::Add : ALUOp::Sub)
			.flags_none()
			.internalRegisters(ACCSrc::InternalDataBus, false, ACTSrc::ACC, false, true) // TMP = IDB
			.registerFile(RegisterFileSrcReg::fromInstruction(reg), RegisterFileDstReg::fromInstruction(reg), RegisterFileSrcRegPair::PC, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Zero, false, false, false)
			.misc(InternalDataBusSrc::RegisterFile, 0, false, 0, false, false) // IDB = RF
			.flow_continue()
		.end()
		.nextState()
			.state(5, true, 0)
			.alu(ALUASrc::TMP, ALUBSrc::Zero, ALUCSrc::One, dir > 0 ? ALUOp::Add : ALUOp::Sub)
			.flags(FlagsCSrc::ALU, FlagsPSrc::ALU, FlagsACSrc::ALU, FlagsZSrc::ALU, FlagsSSrc::ALU, false, true, true, true, true)
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::fromInstruction(reg), RegisterFileDstReg::fromInstruction(reg), RegisterFileSrcRegPair::PC, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Zero, false, true, false) // DDD = IDB
			.misc(InternalDataBusSrc::ALU, 0, false, 0, false, false) // IDB = ALU
			.flow_continue()
		.end();
	}

	mcb.nextState()
		.state_T1()
		.alu_disabled()
		.flags_none()
		.internalRegisters_disabled()
		.registerFile_addrPC()
		.misc_StatusWordOut()
		.flow_continue()
	.end()
	.nextState()
		.state_T2()
		.alu_disabled()
		.flags_none()
		.internalRegisters_disabled()
		.registerFile_addrPC()
		.misc_DataIn()
		.flow_continue()
	.end()
	.nextState()
		.state_T3(false)
		.alu_disabled()
		.flags_none()
		.internalRegisters_disabled()
		.registerFile_disabled()
		.misc_DataIn()
		.flow_continue()
	.end()
	.end();

	ib.end();
}

void insertIncDec_mem(DecodeROM* rom, int dir)
{
	InstructionBuilder ib(4, romAllocStates(rom, 10), 10);

	ib.nextCycle(MachineCycleType::InstructionFetch, 1, false)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryRead, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrHL()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu(ALUASrc::TMP, ALUBSrc::Zero, ALUCSrc::One, dir > 0 ? ALUOp::Add : ALUOp::Sub)
			.flags_none()
			.internalRegisters_disabled() // Dont write TMP just yet
			.registerFile_addrHL()
			.misc_DataIn() // Prepare IDB = DataIn
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu(ALUASrc::TMP, ALUBSrc::Zero, ALUCSrc::One, dir > 0 ? ALUOp::Add : ALUOp::Sub)
			.flags_none()
			.internalRegisters(ACCSrc::InternalDataBus, false, ACTSrc::ACC, false, true) // TMP = IDB
			.registerFile_disabled()
			.misc_DataIn() // IDB = Data in
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryWrite, 3, true)
		.nextState()
			.state_T1()
			.alu(ALUASrc::TMP, ALUBSrc::Zero, ALUCSrc::One, dir > 0 ? ALUOp::Add : ALUOp::Sub)
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrHL()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu(ALUASrc::TMP, ALUBSrc::Zero, ALUCSrc::One, dir > 0 ? ALUOp::Add : ALUOp::Sub)
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataOut(InternalDataBusSrc::ALU)
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu(ALUASrc::TMP, ALUBSrc::Zero, ALUCSrc::One, dir > 0 ? ALUOp::Add : ALUOp::Sub)
			.flags(FlagsCSrc::ALU, FlagsPSrc::ALU, FlagsACSrc::ALU, FlagsZSrc::ALU, FlagsSSrc::ALU, false, true, true, true, true)
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataOut(InternalDataBusSrc::ALU)
			.flow_continue()
		.end()
	.end();

	// Fetch next instruction
	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertINR(DecodeROM* rom, uint32_t reg)
{
	if (reg == REG_M) {
		insertIncDec_mem(rom, 1);
	} else {
		insertIncDec_reg(rom, reg, 1);
	}
}

void insertDCR(DecodeROM* rom, uint32_t reg)
{
	if (reg == REG_M) {
		insertIncDec_mem(rom, -1);
	} else {
		insertIncDec_reg(rom, reg, -1);
	}
}

void insertMOV_imm_to_reg(DecodeROM* rom, uint32_t reg)
{
	InstructionBuilder ib(3, romAllocStates(rom, 7), 7);

	ib.nextCycle(MachineCycleType::InstructionFetch, 1, false)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	MachineCycleBuilder& mcb = ib.nextCycle(MachineCycleType::MemoryRead, 3, true);
	mcb.nextState()
		.state_T1()
		.alu_disabled()
		.flags_none()
		.internalRegisters_disabled()
		.registerFile_addrPC()
		.misc_StatusWordOut()
		.flow_continue()
	.end();

	if (reg == REG_A) {
		mcb.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters(ACCSrc::InternalDataBus, false, ACTSrc::ACC, false, false) // Prepare ACC to read data from IDB
			.registerFile_addrPC()
			.misc_DataIn() // IDB = Data In 
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters(ACCSrc::InternalDataBus, true, ACTSrc::ACC, false, false) // ACC = IDB
			.registerFile_disabled()
			.misc_DataIn() // IDB = Data In
			.flow_continue()
		.end();
	} else {
		mcb.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::fromInstruction(reg), RegisterFileSrcRegPair::PC, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Zero, false, true, false)
			.misc_DataIn()
			.flow_continue()
		.end();
	}
	mcb.end();

	// Read next instruction
	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertMOV_imm_to_mem(DecodeROM* rom)
{
	InstructionBuilder ib(4, romAllocStates(rom, 10), 10);

	ib.nextCycle(MachineCycleType::InstructionFetch, 1, false)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryRead, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters(ACCSrc::InternalDataBus, false, ACTSrc::ACC, false, true) // TMP = IDB
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryWrite, 3, true)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrHL()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataOut(InternalDataBusSrc::TMP)
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataOut(InternalDataBusSrc::TMP)
			.flow_continue()
		.end()
	.end();

	// Read next instruction
	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertMVI(DecodeROM* rom, uint32_t reg)
{
	if (reg == REG_M) {
		insertMOV_imm_to_mem(rom);
	} else {
		insertMOV_imm_to_reg(rom, reg);
	}
}

void insertMOV_reg_to_reg(DecodeROM* rom, uint32_t encodedDstReg, uint32_t encodedSrcReg)
{
	InstructionBuilder ib(1, romAllocStates(rom, 5), 5);

	StateBuilder& sb = ib.nextCycle(MachineCycleType::InstructionFetch, 5, true).nextState();

	if (encodedSrcReg == REG_A) {
		sb.state(4, false, 0) // M1.T4
			.alu_disabled()
			.flags_none()
			.internalRegisters(ACCSrc::InternalDataBus, false, ACTSrc::ACC, false, true) // TMP = IDB
			.registerFile_disabled()
			.misc(InternalDataBusSrc::ACC, 0, false, 0, false, false) // IDB = ACC
			.flow_continue();
	} else {
		RegisterFileSrcReg::Enum srcReg = RegisterFileSrcReg::fromInstruction(encodedSrcReg);
		sb.state(4, false, 0) // M1.T4
			.alu_disabled()
			.flags_none()
			.internalRegisters(ACCSrc::InternalDataBus, false, ACTSrc::ACC, false, true) // TMP = IDB
			.registerFile(srcReg, RegisterFileDstReg::B, RegisterFileSrcRegPair::PC, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Zero, false, false, false) // read SSS
			.misc(InternalDataBusSrc::RegisterFile, 0, false, 0, false, false) // IDB = RF
			.flow_continue();
	}

	sb = sb.end().nextState();

	if (encodedDstReg == REG_A) {
		sb.state(5, true, 0) // M1.T5
			.alu_disabled()
			.flags_none()
			.internalRegisters(ACCSrc::InternalDataBus, true, ACTSrc::ACC, false, false) // ACC = IDB
			.registerFile_disabled()
			.misc(InternalDataBusSrc::TMP, 0, false, 0, false, false) // IDB = TMP
			.flow_continue();
	} else {
		RegisterFileDstReg::Enum dstReg = RegisterFileDstReg::fromInstruction(encodedDstReg);

		sb.state(5, true, 0) // M1.T5
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_writeReg(dstReg)
			.misc(InternalDataBusSrc::TMP, 0, false, 0, false, false)
			.flow_continue();
	}

	// Read next instruction
	sb.end()
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertMOV_reg_to_M(DecodeROM* rom, uint32_t encodedSrcReg)
{
	InstructionBuilder ib(3, romAllocStates(rom, 7), 7);

	StateBuilder& sb = ib.nextCycle(MachineCycleType::InstructionFetch, 1, false).nextState();

	if (encodedSrcReg == REG_A) {
		sb.state(4, true, 0) // M1.T4
			.alu_disabled()
			.flags_none()
			.internalRegisters(ACCSrc::InternalDataBus, false, ACTSrc::ACC, false, true) // TMP = IDB
			.registerFile_disabled()
			.misc(InternalDataBusSrc::ACC, 0, false, 0, false, false) // IDB = ACC
			.flow_continue();
	} else {
		RegisterFileSrcReg::Enum srcReg = RegisterFileSrcReg::fromInstruction(encodedSrcReg);
		sb.state(4, true, 0) // M1.T4
			.alu_disabled()
			.flags_none()
			.internalRegisters(ACCSrc::InternalDataBus, false, ACTSrc::ACC, false, true) // TMP = IDB
			.registerFile(srcReg, RegisterFileDstReg::B, RegisterFileSrcRegPair::PC, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Zero, false, false, false) // read SSS
			.misc(InternalDataBusSrc::RegisterFile, 0, false, 0, false, false) // IDB = RF
			.flow_continue();
	}

	sb.end().end() // end machine cycle
	.nextCycle(MachineCycleType::MemoryWrite, 3, true)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrHL()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataOut(InternalDataBusSrc::TMP)
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataOut(InternalDataBusSrc::TMP)
			.flow_continue()
		.end()
	.end()
	.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertMOV_M_to_reg(DecodeROM* rom, uint32_t encodedDstReg)
{
	InstructionBuilder ib(3, romAllocStates(rom, 7), 7);

	MachineCycleBuilder& mcb = ib.nextCycle(MachineCycleType::InstructionFetch, 1, false);
	mcb.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	mcb = ib.nextCycle(MachineCycleType::MemoryRead, 3, true);
	{
		StateBuilder& sb = mcb.nextState();
		sb
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrHL()
			.misc_StatusWordOut()
			.flow_continue()
		.end();

		sb = mcb.nextState();

		sb
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end();

		sb = mcb.nextState();

		if (encodedDstReg == REG_A) {
			sb.state_T3(true)
				.alu_disabled()
				.flags_none()
				.internalRegisters(ACCSrc::InternalDataBus, true, ACTSrc::ACC, false, false) // ACC = IDB
				.registerFile_disabled()
				.misc_DataIn()
				.flow_continue()
			.end();
		} else {
			RegisterFileDstReg::Enum dstReg = RegisterFileDstReg::fromInstruction(encodedDstReg);

			sb
				.state_T3(true)
				.alu_disabled()
				.flags_none()
				.internalRegisters_disabled()
				.registerFile_writeReg(dstReg)
				.misc_DataIn()
				.flow_continue()
			.end();
		}
	}
	mcb.end();
	
	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertMOV(DecodeROM* rom, uint32_t dstReg, uint32_t srcReg)
{
	if (dstReg == REG_M) {
		assert(srcReg != REG_M);
		insertMOV_reg_to_M(rom, srcReg);
	} else {
		if (srcReg == REG_M) {
			insertMOV_M_to_reg(rom, dstReg);
		} else {
			insertMOV_reg_to_reg(rom, dstReg, srcReg);
		}
	}
}

void insertALUOp_reg(DecodeROM* rom, uint32_t reg, ALUOp::Enum op, bool withCarry, bool weACC)
{
	assert(op != ALUOp::RotateLeft && op != ALUOp::RotateRight);

	InstructionBuilder ib(2, romAllocStates(rom, 4), 4);

	if (reg == REG_A) {
		ib.nextCycle(MachineCycleType::InstructionFetch, 1, true)
			.nextState()
				.state(4, true, 0)
				.alu(ALUASrc::ACT, ALUBSrc::TMP, withCarry ? ALUCSrc::Flags : ALUCSrc::Zero, op)
				.flags_none() // Prepare the MUX inputs to read all values from the ALU
				.internalRegisters(ACCSrc::InternalDataBus, false, ACTSrc::ACC, true, true) // TMP = IDB; ACT = ACC
				.registerFile_disabled()
				.misc(InternalDataBusSrc::ACC, 0, false, 0, false, false) // IDB = ACC
				.flow_continue()
			.end()
		.end();
	} else {
		ib.nextCycle(MachineCycleType::InstructionFetch, 1, true)
			.nextState()
				.state(4, true, 0)
				.alu(ALUASrc::ACT, ALUBSrc::TMP, withCarry ? ALUCSrc::Flags : ALUCSrc::Zero, op)
				.flags_none() // Prepare the MUX inputs to read all values from the ALU
				.internalRegisters(ACCSrc::InternalDataBus, false, ACTSrc::ACC, true, true) // TMP = IDB; ACT = ACC
				.registerFile(RegisterFileSrcReg::fromInstruction(reg), RegisterFileDstReg::B, RegisterFileSrcRegPair::PC, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Zero, false, false, false)
				.misc(InternalDataBusSrc::RegisterFile, 0, false, 0, false, false) // IDB = RF
				.flow_continue()
			.end()
		.end();
	}

	// NOTE: This is the fetch cycle of the next instruction with interleaved ALU operations.
	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu(ALUASrc::ACT, ALUBSrc::TMP, withCarry ? ALUCSrc::Flags : ALUCSrc::Zero, op)
			.flags_none() // Don't write the result just yet
			.internalRegisters_disabled()
			.registerFile_addrPC() // Prepare PC + 1
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu(ALUASrc::ACT, ALUBSrc::TMP, withCarry ? ALUCSrc::Flags : ALUCSrc::Zero, op)
			.flags(FlagsCSrc::ALU, FlagsPSrc::ALU, FlagsACSrc::ALU, FlagsZSrc::ALU, FlagsSSrc::ALU, true, true, true, true, true)
			.internalRegisters(ACCSrc::InternalDataBus, weACC, ACTSrc::ACC, false, false) // ACC = IDB
			.registerFile_addrPC() // PC = PC + 1
			.misc(InternalDataBusSrc::ALU, 0, false, 0, false, false) // IDB = ALU
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn() // NOTE: IDB should point to DataIn because on T3 DataIn holds the next instruction.
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertALUOp_mem(DecodeROM* rom, ALUOp::Enum op, bool withCarry, bool weACC)
{
	assert(op != ALUOp::RotateLeft && op != ALUOp::RotateRight);

	InstructionBuilder ib(3, romAllocStates(rom, 7), 7);

	ib.nextCycle(MachineCycleType::InstructionFetch, 1, false)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters(ACCSrc::InternalDataBus, false, ACTSrc::ACC, true, false) // ACT = ACC
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryRead, 3, true)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrHL()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn() // IDB = Data in
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters(ACCSrc::InternalDataBus, false, ACTSrc::ACC, false, true) // TMP = IDB
			.registerFile_disabled()
			.misc_DataIn() // IDB = Data In
			.flow_continue()
		.end()
	.end();

	// NOTE: This is the fetch cycle of the next instruction with interleaved ALU operations.
	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu(ALUASrc::ACT, ALUBSrc::TMP, withCarry ? ALUCSrc::Flags : ALUCSrc::Zero, op)
			.flags_none() // Don't write the result just yet
			.internalRegisters_disabled() // TMP and ACT already prepared
			.registerFile_addrPC() // Prepare PC + 1
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu(ALUASrc::ACT, ALUBSrc::TMP, withCarry ? ALUCSrc::Flags : ALUCSrc::Zero, op)
			.flags(FlagsCSrc::ALU, FlagsPSrc::ALU, FlagsACSrc::ALU, FlagsZSrc::ALU, FlagsSSrc::ALU, true, true, true, true, true)
			.internalRegisters(ACCSrc::InternalDataBus, weACC, ACTSrc::ACC, false, false) // ACC = IDB
			.registerFile_addrPC() // PC = PC + 1
			.misc(InternalDataBusSrc::ALU, 0, false, 0, false, false) // IDB = ALU
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn() // NOTE: IDB should point to DataIn because on T3 DataIn holds the next instruction.
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertADD(DecodeROM* rom, uint32_t reg)
{
	if (reg == REG_M) {
		insertALUOp_mem(rom, ALUOp::Add, false, true);
	} else {
		insertALUOp_reg(rom, reg, ALUOp::Add, false, true);
	}
}

void insertADC(DecodeROM* rom, uint32_t reg)
{
	if (reg == REG_M) {
		insertALUOp_mem(rom, ALUOp::Add, true, true);
	} else {
		insertALUOp_reg(rom, reg, ALUOp::Add, true, true);
	}
}

void insertSUB(DecodeROM* rom, uint32_t reg)
{
	if (reg == REG_M) {
		insertALUOp_mem(rom, ALUOp::Sub, false, true);
	} else {
		insertALUOp_reg(rom, reg, ALUOp::Sub, false, true);
	}
}

void insertSBB(DecodeROM* rom, uint32_t reg)
{
	if (reg == REG_M) {
		insertALUOp_mem(rom, ALUOp::Sub, true, true);
	} else {
		insertALUOp_reg(rom, reg, ALUOp::Sub, true, true);
	}
}

void insertANA(DecodeROM* rom, uint32_t reg)
{
	if (reg == REG_M) {
		insertALUOp_mem(rom, ALUOp::AND, false, true);
	} else {
		insertALUOp_reg(rom, reg, ALUOp::AND, false, true);
	}
}

void insertXRA(DecodeROM* rom, uint32_t reg)
{
	if (reg == REG_M) {
		insertALUOp_mem(rom, ALUOp::XOR, false, true);
	} else {
		insertALUOp_reg(rom, reg, ALUOp::XOR, false, true);
	}
}

void insertORA(DecodeROM* rom, uint32_t reg)
{
	if (reg == REG_M) {
		insertALUOp_mem(rom, ALUOp::OR, false, true);
	} else {
		insertALUOp_reg(rom, reg, ALUOp::OR, false, true);
	}
}

void insertCMP(DecodeROM* rom, uint32_t reg)
{
	if (reg == REG_M) {
		insertALUOp_mem(rom, ALUOp::Sub, false, false);
	} else {
		insertALUOp_reg(rom, reg, ALUOp::Sub, false, false);
	}
}

void insertRotate(DecodeROM* rom, ALUOp::Enum op, bool throughCarry)
{
	assert(op == ALUOp::RotateLeft || op == ALUOp::RotateRight);

	InstructionBuilder ib(2, romAllocStates(rom, 4), 4);

	ib.nextCycle(MachineCycleType::InstructionFetch, 1, true)
		.nextState()
			.state(4, true, 0)
			.alu(ALUASrc::ACT, ALUBSrc::TMP, throughCarry ? ALUCSrc::Flags : (op == ALUOp::RotateLeft ? ALUCSrc::ACC_7 : ALUCSrc::ACC_0), op)
			.flags_none() // Prepare the MUX inputs to read all values from the ALU
			.internalRegisters(ACCSrc::InternalDataBus, false, ACTSrc::ACC, true, false) // ACT = ACC
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	// NOTE: This is the fetch cycle of the next instruction with interleaved ALU operations.
	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu(ALUASrc::ACT, ALUBSrc::TMP, throughCarry ? ALUCSrc::Flags : (op == ALUOp::RotateLeft ? ALUCSrc::ACC_7 : ALUCSrc::ACC_0), op)
			.flags_none() // Don't write the result just yet
			.internalRegisters_disabled()
			.registerFile_addrPC() // Prepare PC + 1
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu(ALUASrc::ACT, ALUBSrc::TMP, throughCarry ? ALUCSrc::Flags : (op == ALUOp::RotateLeft ? ALUCSrc::ACC_7 : ALUCSrc::ACC_0), op)
			.flags(FlagsCSrc::ALU, FlagsPSrc::ALU, FlagsACSrc::ALU, FlagsZSrc::ALU, FlagsSSrc::ALU, true, false, false, false, false) // Only C is affected
			.internalRegisters(ACCSrc::InternalDataBus, true, ACTSrc::ACC, false, false) // ACC = IDB
			.registerFile_addrPC() // PC = PC + 1
			.misc(InternalDataBusSrc::ALU, 0, false, 0, false, false) // IDB = ALU
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn() // NOTE: IDB should point to DataIn because on T3 DataIn holds the next instruction.
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertRLC(DecodeROM* rom)
{
	insertRotate(rom, ALUOp::RotateLeft, false);
}

void insertRRC(DecodeROM* rom)
{
	insertRotate(rom, ALUOp::RotateRight, false);
}

void insertRAL(DecodeROM* rom)
{
	insertRotate(rom, ALUOp::RotateLeft, true);
}

void insertRAR(DecodeROM* rom)
{
	insertRotate(rom, ALUOp::RotateRight, true);
}

void insertCMA(DecodeROM* rom)
{
	InstructionBuilder ib(1, romAllocStates(rom, 4), 4);

	ib.nextCycle(MachineCycleType::InstructionFetch, 4, true)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters(ACCSrc::NOT_Self, true, ACTSrc::ACC, false, false)
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertCMC(DecodeROM* rom)
{
	InstructionBuilder ib(1, romAllocStates(rom, 4), 4);

	ib.nextCycle(MachineCycleType::InstructionFetch, 4, true)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags(FlagsCSrc::NOT_Self, FlagsPSrc::ALU, FlagsACSrc::ALU, FlagsZSrc::ALU, FlagsSSrc::ALU, true, false, false, false, false)
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertSTC(DecodeROM* rom)
{
	InstructionBuilder ib(1, romAllocStates(rom, 4), 4);

	ib.nextCycle(MachineCycleType::InstructionFetch, 4, true)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags(FlagsCSrc::One, FlagsPSrc::ALU, FlagsACSrc::ALU, FlagsZSrc::ALU, FlagsSSrc::ALU, true, false, false, false, false)
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertHLT(DecodeROM* rom)
{
	InstructionBuilder ib(3, romAllocStates(rom, 7), 7);

	ib.nextCycle(MachineCycleType::InstructionFetch, 1, false)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end()
	.nextCycle(MachineCycleType::HaltAck, 3, true)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state(2, false, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled() // NOTE: Don't call registerFile_addrPC() because this is T2 and it will increment the PC.
			.misc(InternalDataBusSrc::DataIn, 1, true, 0, false, false)
			.flow_continue()
		.end()
		.nextState()
			.state(3, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc(InternalDataBusSrc::DataIn, 0, false, 0, false, false) // Reset weHLTA to false because otherwise it's impossible to get out of HALT state even with an interrupt.
			.flow_continue()
		.end()
	.end()
	.nextCycle(MachineCycleType::InstructionFetch, 3, false) // NOTE: This isn't required here because after an HLT is executed the only way to continue operating is via RESET or INT.
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertALUOp_imm(DecodeROM* rom, ALUOp::Enum op, bool withCarry, bool weACC)
{
	assert(op != ALUOp::RotateLeft && op != ALUOp::RotateRight);

	InstructionBuilder ib(3, romAllocStates(rom, 7), 7);

	ib.nextCycle(MachineCycleType::InstructionFetch, 1, false)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters(ACCSrc::InternalDataBus, false, ACTSrc::ACC, true, false) // ACT = ACC
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryRead, 3, true)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn() // IDB = Data in
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters(ACCSrc::InternalDataBus, false, ACTSrc::ACC, false, true) // TMP = IDB
			.registerFile_disabled()
			.misc_DataIn() // IDB = Data In
			.flow_continue()
		.end()
	.end();

	// NOTE: This is the fetch cycle of the next instruction with interleaved ALU operations.
	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu(ALUASrc::ACT, ALUBSrc::TMP, withCarry ? ALUCSrc::Flags : ALUCSrc::Zero, op)
			.flags_none() // Don't write the result just yet
			.internalRegisters_disabled() // TMP and ACT already prepared
			.registerFile_addrPC() // Prepare PC + 1
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu(ALUASrc::ACT, ALUBSrc::TMP, withCarry ? ALUCSrc::Flags : ALUCSrc::Zero, op)
			.flags(FlagsCSrc::ALU, FlagsPSrc::ALU, FlagsACSrc::ALU, FlagsZSrc::ALU, FlagsSSrc::ALU, true, true, true, true, true)
			.internalRegisters(ACCSrc::InternalDataBus, weACC, ACTSrc::ACC, false, false) // ACC = IDB
			.registerFile_addrPC() // PC = PC + 1
			.misc(InternalDataBusSrc::ALU, 0, false, 0, false, false) // IDB = ALU
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn() // NOTE: IDB should point to DataIn because on T3 DataIn holds the next instruction.
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertADI(DecodeROM* rom)
{
	insertALUOp_imm(rom, ALUOp::Add, false, true);
}

void insertACI(DecodeROM* rom)
{
	insertALUOp_imm(rom, ALUOp::Add, true, true);
}

void insertSUI(DecodeROM* rom)
{
	insertALUOp_imm(rom, ALUOp::Sub, false, true);
}

void insertSBI(DecodeROM* rom)
{
	insertALUOp_imm(rom, ALUOp::Sub, true, true);
}

void insertANI(DecodeROM* rom)
{
	insertALUOp_imm(rom, ALUOp::AND, false, true);
}

void insertXRI(DecodeROM* rom)
{
	insertALUOp_imm(rom, ALUOp::XOR, false, true);
}

void insertORI(DecodeROM* rom)
{
	insertALUOp_imm(rom, ALUOp::OR, false, true);
}

void insertCPI(DecodeROM* rom)
{
	insertALUOp_imm(rom, ALUOp::Sub, false, false);
}

void insertPCHL(DecodeROM* rom)
{
	InstructionBuilder ib(1, romAllocStates(rom, 5), 5);

	ib.nextCycle(MachineCycleType::InstructionFetch, 5, true)
		.nextState()
			.state(4, false, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::HL, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Zero, false, false, false) // Prepare HL + 0
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state(5, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::HL, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Zero, false, false, true) // Write HL + 0 to PC
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertSPHL(DecodeROM* rom)
{
	InstructionBuilder ib(1, romAllocStates(rom, 5), 5);

	ib.nextCycle(MachineCycleType::InstructionFetch, 5, true)
		.nextState()
			.state(4, false, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::HL, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Zero, false, false, false) // Prepare HL + 0
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state(5, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::HL, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Zero, false, false, true) // Write HL + 0 to SP
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertXCHG(DecodeROM* rom)
{
	InstructionBuilder ib(1, romAllocStates(rom, 4), 4);

	ib.nextCycle(MachineCycleType::InstructionFetch, 4, true)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::PC, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Zero, true, false, false)
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertDI(DecodeROM* rom)
{
	InstructionBuilder ib(1, romAllocStates(rom, 4), 4);

	ib.nextCycle(MachineCycleType::InstructionFetch, 4, true)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc(InternalDataBusSrc::DataIn, 0, false, 0, true, false)
			.flow_continue()
		.end()
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertEI(DecodeROM* rom)
{
	InstructionBuilder ib(1, romAllocStates(rom, 4), 4);

	ib.nextCycle(MachineCycleType::InstructionFetch, 4, true)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc(InternalDataBusSrc::DataIn, 0, false, 1, true, false)
			.flow_continue()
		.end()
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertPOP_rp(DecodeROM* rom, uint32_t rp)
{
	InstructionBuilder ib(4, romAllocStates(rom, 10), 10);

	ib.nextCycle(MachineCycleType::InstructionFetch, 1, false)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::StackRead, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::fromRegisterPair_low(rp), RegisterFileDstReg::fromRegisterPair_low(rp), RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Inc, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::fromRegisterPair_low(rp), RegisterFileDstReg::fromRegisterPair_low(rp), RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Inc, false, false, true)
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::fromRegisterPair_low(rp), RegisterFileDstReg::fromRegisterPair_low(rp), RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Inc, false, true, false)
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::StackRead, 3, true)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::fromRegisterPair_high(rp), RegisterFileDstReg::fromRegisterPair_high(rp), RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Inc, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::fromRegisterPair_high(rp), RegisterFileDstReg::fromRegisterPair_high(rp), RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Inc, false, false, true)
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::fromRegisterPair_high(rp), RegisterFileDstReg::fromRegisterPair_high(rp), RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Inc, false, true, false)
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertPOP_PSW(DecodeROM* rom)
{
	InstructionBuilder ib(4, romAllocStates(rom, 10), 10);

	ib.nextCycle(MachineCycleType::InstructionFetch, 1, false)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::StackRead, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Inc, false, false, false) // Addr = SP;Prepare SP + 1
			.misc_StatusWordOut() // IDB = Status
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Inc, false, false, true) // SP = SP + 1
			.misc_DataIn() // IDB = DataIn
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags(FlagsCSrc::InternalDataBus, FlagsPSrc::InternalDataBus, FlagsACSrc::InternalDataBus, FlagsZSrc::InternalDataBus, FlagsSSrc::InternalDataBus, true, true, true, true, true)
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::StackRead, 3, true)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Inc, false, false, false) // Addr = SP;Prepare SP + 1
			.misc_StatusWordOut() // IDB = Status
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Inc, false, false, true) // SP = SP + 1
			.misc_DataIn() // IDB = DataIn
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters(ACCSrc::InternalDataBus, true, ACTSrc::ACC, false, false) // ACC = IDB
			.registerFile_disabled()
			.misc_DataIn() // IDB = DataIn
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertPOP(DecodeROM* rom, uint32_t rp)
{
	if (rp == RP_PSW) {
		insertPOP_PSW(rom);
	} else {
		insertPOP_rp(rom, rp);
	}
}

void insertPUSH_rp(DecodeROM* rom, uint32_t rp)
{
	InstructionBuilder ib(4, romAllocStates(rom, 11), 11);

	ib.nextCycle(MachineCycleType::InstructionFetch, 2, false)
		.nextState()
			.state(4, false, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, false, false) // Prepare SP - 1
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state(5, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, false, true) // SP = SP - 1
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::StackWrite, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::fromRegisterPair_high(rp), RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, false, false) // Addr = SP;Prepare SP - 1
			.misc_StatusWordOut() // IDB = Status
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::fromRegisterPair_high(rp), RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, false, true) // SP = SP - 1
			.misc_DataOut(InternalDataBusSrc::RegisterFile) // IDB = rh
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::fromRegisterPair_high(rp), RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, false, false) // RF = rh
			.misc_DataOut(InternalDataBusSrc::RegisterFile) // IDB = rh
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::StackWrite, 3, true)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::fromRegisterPair_low(rp), RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Zero, false, false, false) // Addr = SP
			.misc_StatusWordOut() // IDB = Status
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::fromRegisterPair_low(rp), RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_DataOut(InternalDataBusSrc::RegisterFile) // IDB = rl
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::fromRegisterPair_low(rp), RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Zero, false, false, false) // RF = rl
			.misc_DataOut(InternalDataBusSrc::RegisterFile) // IDB = rl
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertPUSH_PSW(DecodeROM* rom)
{
	InstructionBuilder ib(4, romAllocStates(rom, 11), 11);

	ib.nextCycle(MachineCycleType::InstructionFetch, 2, false)
		.nextState()
			.state(4, false, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, false, false) // Prepare SP - 1
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state(5, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, false, true) // SP = SP - 1
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::StackWrite, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, false, false) // Addr = SP;Prepare SP - 1
			.misc_StatusWordOut() // IDB = Status
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, false, true) // SP = SP - 1
			.misc_DataOut(InternalDataBusSrc::ACC) // IDB = ACC
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataOut(InternalDataBusSrc::ACC) // IDB = ACC
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::StackWrite, 3, true)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Zero, false, false, false) // Addr = SP
			.misc_StatusWordOut() // IDB = Status
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_DataOut(InternalDataBusSrc::Flags) // IDB = Flags
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataOut(InternalDataBusSrc::Flags) // IDB = Flags
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertPUSH(DecodeROM* rom, uint32_t rp)
{
	if (rp == RP_PSW) {
		insertPUSH_PSW(rom);
	} else {
		insertPUSH_rp(rom, rp);
	}
}

void insertSHLD(DecodeROM* rom)
{
	InstructionBuilder ib(6, romAllocStates(rom, 16), 16);

	ib.nextCycle(MachineCycleType::InstructionFetch, 1, false)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryRead, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_writeReg(RegisterFileDstReg::Z)
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryRead, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_writeReg(RegisterFileDstReg::W)
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryWrite, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::L, RegisterFileDstReg::L, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::WZ, RegisterFileRegPairOp::Inc, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::L, RegisterFileDstReg::L, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::WZ, RegisterFileRegPairOp::Inc, false, false, true)
			.misc_DataOut(InternalDataBusSrc::RegisterFile)
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::L, RegisterFileDstReg::L, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::WZ, RegisterFileRegPairOp::Inc, false, false, false)
			.misc_DataOut(InternalDataBusSrc::RegisterFile)
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryWrite, 3, true)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::H, RegisterFileDstReg::H, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::WZ, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::H, RegisterFileDstReg::H, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::WZ, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_DataOut(InternalDataBusSrc::RegisterFile)
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::H, RegisterFileDstReg::H, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::WZ, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_DataOut(InternalDataBusSrc::RegisterFile)
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertLHLD(DecodeROM* rom)
{
	InstructionBuilder ib(6, romAllocStates(rom, 16), 16);

	ib.nextCycle(MachineCycleType::InstructionFetch, 1, false)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryRead, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_writeReg(RegisterFileDstReg::Z)
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryRead, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_writeReg(RegisterFileDstReg::W)
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryRead, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::L, RegisterFileDstReg::L, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::WZ, RegisterFileRegPairOp::Inc, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::L, RegisterFileDstReg::L, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::WZ, RegisterFileRegPairOp::Inc, false, false, true)
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::L, RegisterFileDstReg::L, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::WZ, RegisterFileRegPairOp::Inc, false, true, false)
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryRead, 3, true)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::H, RegisterFileDstReg::H, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::WZ, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::H, RegisterFileDstReg::H, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::WZ, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::H, RegisterFileDstReg::H, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::WZ, RegisterFileRegPairOp::Zero, false, true, false)
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertSTA(DecodeROM* rom)
{
	InstructionBuilder ib(5, romAllocStates(rom, 13), 13);

	ib.nextCycle(MachineCycleType::InstructionFetch, 1, false)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryRead, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_writeReg(RegisterFileDstReg::Z)
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryRead, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_writeReg(RegisterFileDstReg::W)
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryWrite, 3, true)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::WZ, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::WZ, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_DataOut(InternalDataBusSrc::ACC)
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::WZ, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_DataOut(InternalDataBusSrc::ACC)
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertLDA(DecodeROM* rom)
{
	InstructionBuilder ib(5, romAllocStates(rom, 13), 13);

	ib.nextCycle(MachineCycleType::InstructionFetch, 1, false)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryRead, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_writeReg(RegisterFileDstReg::Z)
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryRead, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_writeReg(RegisterFileDstReg::W)
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryRead, 3, true)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::WZ, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters(ACCSrc::InternalDataBus, false, ACTSrc::ACC, false, false)
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::WZ, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters(ACCSrc::InternalDataBus, true, ACTSrc::ACC, false, false)
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::WZ, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertIN(DecodeROM* rom)
{
	// NOTE: This is the same as LDA except that it has only 1 MemoryRead cycle for building the WZ register (both W and Z get the same value)
	InstructionBuilder ib(4, romAllocStates(rom, 10), 10);

	ib.nextCycle(MachineCycleType::InstructionFetch, 1, false)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryRead, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_writeReg(RegisterFileDstReg::W_and_Z)
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryRead, 3, true)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::WZ, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters(ACCSrc::InternalDataBus, false, ACTSrc::ACC, false, false)
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::WZ, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters(ACCSrc::InternalDataBus, true, ACTSrc::ACC, false, false)
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::WZ, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertOUT(DecodeROM* rom)
{
	// NOTE: This is the same as STA except that it has only 1 MemoryRead cycle for building the WZ register (both W and Z get the same value)
	InstructionBuilder ib(4, romAllocStates(rom, 10), 10);

	ib.nextCycle(MachineCycleType::InstructionFetch, 1, false)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryRead, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_writeReg(RegisterFileDstReg::W_and_Z)
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryWrite, 3, true)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::WZ, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::WZ, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_DataOut(InternalDataBusSrc::ACC)
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::WZ, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_DataOut(InternalDataBusSrc::ACC)
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertJMP(DecodeROM* rom)
{
	InstructionBuilder ib(4, romAllocStates(rom, 10), 10);

	ib.nextCycle(MachineCycleType::InstructionFetch, 1, false)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryRead, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn() // IDB = Data in
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_writeReg(RegisterFileDstReg::Z) // Z = IDB
			.misc_DataIn() // IDB = Data In
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryRead, 3, true)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn() // IDB = Data in
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_writeReg(RegisterFileDstReg::W) // W = IDB
			.misc_DataIn() // IDB = Data In
			.flow_continue()
		.end()
	.end();

	// Fetch next instruction and point PC to the WZ address.
	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Inc, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Inc, false, false, true)
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertRET(DecodeROM* rom)
{
	InstructionBuilder ib(4, romAllocStates(rom, 10), 10);

	ib.nextCycle(MachineCycleType::InstructionFetch, 1, false)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::StackRead, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Inc, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Inc, false, false, true)
			.misc_DataIn() // IDB = Data in
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_writeReg(RegisterFileDstReg::Z) // Z = IDB
			.misc_DataIn() // IDB = Data In
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::StackRead, 3, true)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Inc, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Inc, false, false, true)
			.misc_DataIn() // IDB = Data in
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_writeReg(RegisterFileDstReg::W) // Z = IDB
			.misc_DataIn() // IDB = Data In
			.flow_continue()
		.end()
	.end();

	// Fetch next instruction and point PC to the WZ address.
	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Inc, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Inc, false, false, true)
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertCALL(DecodeROM* rom)
{
	InstructionBuilder ib(6, romAllocStates(rom, 17), 17);

	ib.nextCycle(MachineCycleType::InstructionFetch, 2, false)
		.nextState()
			.state(4, false, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, false, false)
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state(5, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, false, true)
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryRead, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_writeReg(RegisterFileDstReg::Z)
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryRead, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_writeReg(RegisterFileDstReg::W)
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::StackWrite, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::PCH, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, false, true)
			.misc_DataOut(InternalDataBusSrc::RegisterFile)
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::PCH, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, false, false)
			.misc_DataOut(InternalDataBusSrc::RegisterFile)
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::StackWrite, 3, true)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::PCL, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_DataOut(InternalDataBusSrc::RegisterFile)
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::PCL, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_DataOut(InternalDataBusSrc::RegisterFile)
			.flow_continue()
		.end()
	.end();

	// Fetch next instruction and point PC to the WZ address.
	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Inc, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Inc, false, false, true)
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertJcc(DecodeROM* rom, uint32_t cc)
{
	InstructionBuilder ib(4, romAllocStates(rom, 14), 14);

	ib.nextCycle(MachineCycleType::InstructionFetch, 2, false) // Judge condition (NOP)
		.nextState()
			.state(4, false, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state(5, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryRead, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn() // IDB = Data in
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_writeReg(RegisterFileDstReg::Z) // Z = IDB
			.misc_DataIn() // IDB = Data In
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryRead, 3, true)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn() // IDB = Data in
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_writeReg(RegisterFileDstReg::W) // W = IDB
			.misc_DataIn() // IDB = Data In
			.flow_branch(ConditionCode::fromInstruction(cc), romGetStateAddr(rom, 11)) // If condition is true jump to WZ out instruction fetch.
		.end()
	.end();

	// Fetch next instruction and point PC to the WZ address.
	ib.nextCycle(MachineCycleType::InstructionFetch, 6, false)
		// Selected condition is false. Continue to the next instruction pointed by PC.
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue() // NOTE: At this point the Instruction Register will be overwritten and the ROM address will change to point to new instruction just fetched. It won't continue to the next state below.
		.end()
		// Selected condition is true. Continue to the instruction pointed by WZ and set PC = WZ + 1
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Inc, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Inc, false, false, true)
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertRcc(DecodeROM* rom, uint32_t cc)
{
	InstructionBuilder ib(4, romAllocStates(rom, 14), 14);

	ib.nextCycle(MachineCycleType::InstructionFetch, 2, false)
		.nextState()
			.state(4, false, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state(5, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_branch(ConditionCode::fromInstructionInverse(cc), romGetStateAddr(rom, 11))
		.end()
	.end();

	ib.nextCycle(MachineCycleType::StackRead, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Inc, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Inc, false, false, true)
			.misc_DataIn() // IDB = Data in
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_writeReg(RegisterFileDstReg::Z) // Z = IDB
			.misc_DataIn() // IDB = Data In
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::StackRead, 3, true)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Inc, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Inc, false, false, true)
			.misc_DataIn() // IDB = Data in
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_writeReg(RegisterFileDstReg::W) // Z = IDB
			.misc_DataIn() // IDB = Data In
			.flow_continue()
		.end()
	.end();

	// Fetch next instruction and point PC to the WZ address.
	ib.nextCycle(MachineCycleType::InstructionFetch, 6, false)
		// Condition was true. Fetch instruction pointed by WZ and set PC = WZ + 1
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Inc, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Inc, false, false, true)
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()

		// Condition was false. Fetch instruction pointed by PC.
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertCcc(DecodeROM* rom, uint32_t cc)
{
	InstructionBuilder ib(6, romAllocStates(rom, 21), 21);

	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state(4, false, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, false, false)
			.misc_DataIn()
			.flow_branch(ConditionCode::fromInstruction(cc), romGetStateAddr(rom, 2)) // If CC == true jump over next state
		.end()
		.nextState()
			.state(5, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, false, false) // CC = false, SP = SP
			.misc_DataIn()
			.flow_branch(ConditionCode::fromInstructionInverse(cc), romGetStateAddr(rom, 3)) // If inv(CC) == true jump over next state
		.end()
		.nextState()
			.state(5, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, false, true) // CC = true, SP = SP - 1
			.misc_DataIn()
			.flow_continue() // Continue to next state
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryRead, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_writeReg(RegisterFileDstReg::Z)
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::MemoryRead, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_writeReg(RegisterFileDstReg::W)
			.misc_DataIn()
			.flow_branch(ConditionCode::fromInstructionInverse(cc), romGetStateAddr(rom, 18)) // If CC == false sub-cycles M4 and M5 are skipped. 
		.end()
	.end();

	// Condition is true. M4
	ib.nextCycle(MachineCycleType::StackWrite, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::PCH, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, false, true)
			.misc_DataOut(InternalDataBusSrc::RegisterFile)
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::PCH, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, false, false)
			.misc_DataOut(InternalDataBusSrc::RegisterFile)
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::StackWrite, 3, true)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::PCL, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_DataOut(InternalDataBusSrc::RegisterFile)
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::PCL, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_DataOut(InternalDataBusSrc::RegisterFile)
			.flow_continue()
		.end()
	.end();

	// Fetch next instruction and point PC to the WZ address.
	ib.nextCycle(MachineCycleType::InstructionFetch, 6, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Inc, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Inc, false, false, true)
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue() // NOTE: At this point the controller will jump to the first microinstruction of the new instruction just fetched.
		.end()

		// Condition was false. Fetch next instruction from PC
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertXTHL(DecodeROM* rom)
{
	InstructionBuilder ib(6, romAllocStates(rom, 18), 18);

	ib.nextCycle(MachineCycleType::InstructionFetch, 1, false)
		.nextState()
			.state(4, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::StackRead, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::Z, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Inc, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::Z, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Inc, false, false, true)
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::Z, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Inc, false, true, false)
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::StackRead, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::W, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::W, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::W, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Zero, false, true, false)
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::StackWrite, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::H, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::H, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, false, true)
			.misc_DataOut(InternalDataBusSrc::RegisterFile)
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::H, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, false, false)
			.misc_DataOut(InternalDataBusSrc::RegisterFile)
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::StackWrite, 5, true)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::L, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::L, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_DataOut(InternalDataBusSrc::RegisterFile)
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::L, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_DataOut(InternalDataBusSrc::RegisterFile)
			.flow_continue()
		.end()
		.nextState()
			.state(4, false, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::HL, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state(5, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::HL, RegisterFileRegPairOp::Zero, false, false, true)
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertRST(DecodeROM* rom, uint32_t n)
{
	InstructionBuilder ib(4, romAllocStates(rom, 11), 11);

	ib.nextCycle(MachineCycleType::InstructionFetch, 2, false)
		.nextState()
			.state(4, false, 0)
			.alu(ALUASrc::ACT, ALUBSrc::Zero, ALUCSrc::Zero, ALUOp::AND) // Generate a zero at the ALU output to write it to the W register
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::W, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, false, false)
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state(5, true, 0)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::W, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, true, true)
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::StackWrite, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::PCH, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, false, true)
			.misc_DataOut(InternalDataBusSrc::RegisterFile)
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::PCH, RegisterFileDstReg::B, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Dec, false, false, false)
			.misc_DataOut(InternalDataBusSrc::RegisterFile)
			.flow_continue()
		.end()
	.end();

	ib.nextCycle(MachineCycleType::StackWrite, 3, true)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::Z, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::PCL, RegisterFileDstReg::Z, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_DataOut(InternalDataBusSrc::RegisterFile)
			.flow_continue()
		.end()
		.nextState()
			.state_T3(true)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::PCL, RegisterFileDstReg::Z, RegisterFileSrcRegPair::SP, RegisterFileDstRegPair::SP, RegisterFileRegPairOp::Zero, false, true, false) // Z = Reset address
			.misc(InternalDataBusSrc::ResetAddress, 0, false, 0, false, false)
			.flow_continue()
		.end()
	.end();

	// Fetch next instruction from WZ and point PC to the WZ + 1 address.
	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Inc, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Inc, false, false, true)
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertDAA(DecodeROM* rom)
{
	InstructionBuilder ib(2, romAllocStates(rom, 4), 4);

	ib.nextCycle(MachineCycleType::InstructionFetch, 1, true)
		.nextState()
			.state(4, true, 0)
			.alu(ALUASrc::ACT, ALUBSrc::Zero, ALUCSrc::Flags, ALUOp::DecimalAdjust)
			.flags_none() // Prepare the MUX inputs to read all values from the ALU
			.internalRegisters(ACCSrc::InternalDataBus, false, ACTSrc::ACC, true, false) // ACT = ACC
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	// NOTE: This is the fetch cycle of the next instruction with interleaved ALU operations.
	ib.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		.nextState()
			.state_T1()
			.alu(ALUASrc::ACT, ALUBSrc::Zero, ALUCSrc::Flags, ALUOp::DecimalAdjust)
			.flags_none() // Don't write the result just yet
			.internalRegisters_disabled()
			.registerFile_addrPC() // Prepare PC + 1
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu(ALUASrc::ACT, ALUBSrc::Zero, ALUCSrc::Flags, ALUOp::DecimalAdjust)
			.flags(FlagsCSrc::ALU, FlagsPSrc::ALU, FlagsACSrc::ALU, FlagsZSrc::ALU, FlagsSSrc::ALU, true, true, true, true, true)
			.internalRegisters(ACCSrc::InternalDataBus, true, ACTSrc::ACC, false, false) // ACC = IDB
			.registerFile_addrPC() // PC = PC + 1
			.misc(InternalDataBusSrc::ALU, 0, false, 0, false, false) // IDB = ALU
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn() // NOTE: IDB should point to DataIn because on T3 DataIn holds the next instruction.
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertResetSequence(DecodeROM* rom)
{
	InstructionBuilder ib(2, romAllocStates(rom, 6), 6);

	ib.nextCycle(MachineCycleType::Internal, 3, true)
		.nextState() // NOP
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc(InternalDataBusSrc::StatusWord, 0, true, 0, true, true)
			.flow_continue()
		.end()
		.nextState() // PCL = 0
			.state_T2()
			.alu(ALUASrc::ACT, ALUBSrc::Zero, ALUCSrc::Zero, ALUOp::AND)
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::PCL, RegisterFileDstReg::PCL, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::WZ, RegisterFileRegPairOp::Zero, false, true, false)
			.misc(InternalDataBusSrc::ALU, 0, false, 0, false, false)
			.flow_continue()
		.end()
		.nextState() // PCH = 0
			.state(3, true, 0)
			.alu(ALUASrc::ACT, ALUBSrc::Zero, ALUCSrc::Zero, ALUOp::AND)
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::PCH, RegisterFileDstReg::PCH, RegisterFileSrcRegPair::WZ, RegisterFileDstRegPair::WZ, RegisterFileRegPairOp::Zero, false, true, false)
			.misc(InternalDataBusSrc::ALU, 0, false, 0, false, false)
			.flow_continue()
		.end()
	.end()
	.nextCycle(MachineCycleType::InstructionFetch, 3, false)
		// Fetch next instruction from PC
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_addrPC()
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void insertInterruptSequence(DecodeROM* rom, MachineCycleType::Enum mcType)
{
	assert(mcType == MachineCycleType::InterruptAck || mcType == MachineCycleType::InterruptAckWhileHalt);

	InstructionBuilder ib(1, romAllocStates(rom, 3), 3);
	ib.nextCycle(mcType, 3, false)
		// Fetch next instruction from the peripheral (usually an RST n)
		.nextState()
			.state_T1()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::PC, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_StatusWordOut()
			.flow_continue()
		.end()
		.nextState()
			.state_T2()
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::PC, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Zero, false, false, false)
			.misc_DataIn()
			.flow_continue()
		.end()
		.nextState()
			.state_T3(false)
			.alu_disabled()
			.flags_none()
			.internalRegisters_disabled()
			.registerFile_disabled()
			.misc_DataIn()
			.flow_continue()
		.end()
	.end();

	ib.end();
}

void buildDecodeROM(DecodeROM* rom)
{
	romInit(rom);
	insertResetSequence(rom);

	rom->m_InterruptSequenceAddr = rom->m_NumStates;
	insertInterruptSequence(rom, MachineCycleType::InterruptAck);

	rom->m_InterruptHaltSequenceAddr = rom->m_NumStates;
	insertInterruptSequence(rom, MachineCycleType::InterruptAckWhileHalt);

	// Loop over all possible instructions...
	for (uint32_t instr = 0; instr < 256; ++instr) {
		romNewInstruction(rom, instr);

		switch (instr) {
		case LXI_B:
		case LXI_D:
		case LXI_H:
		case LXI_SP:
			insertLXI(rom, (instr & 0x30) >> 4);
			break;
		case INX_B:
		case INX_D:
		case INX_H:
		case INX_SP:
			insertINX(rom, (instr & 0x30) >> 4);
			break;
		case DCX_B:
		case DCX_D:
		case DCX_H:
		case DCX_SP:
			insertDCX(rom, (instr & 0x30) >> 4);
			break;
		case DAD_B:
		case DAD_D:
		case DAD_H:
		case DAD_SP:
			insertDAD(rom, (instr & 0x30) >> 4);
			break;
		case POP_B:
		case POP_D:
		case POP_H:
		case POP_PSW:
			insertPOP(rom, (instr & 0x30) >> 4);
			break;
		case PUSH_B:
		case PUSH_D:
		case PUSH_H:
		case PUSH_PSW:
			insertPUSH(rom, (instr & 0x30) >> 4);
			break;
		case STAX_B:
		case STAX_D:
			insertSTAX(rom, (instr & 0x30) >> 4);
			break;
		case LDAX_B:
		case LDAX_D:
			insertLDAX(rom, (instr & 0x30) >> 4);
			break;
		case INR_B:
		case INR_C:
		case INR_D:
		case INR_E:
		case INR_H:
		case INR_L:
		case INR_M:
		case INR_A:
			insertINR(rom, (instr & 0x38) >> 3);
			break;
		case DCR_B:
		case DCR_C:
		case DCR_D:
		case DCR_E:
		case DCR_H:
		case DCR_L:
		case DCR_M:
		case DCR_A:
			insertDCR(rom, (instr & 0x38) >> 3);
			break;
		case MVI_B:
		case MVI_C:
		case MVI_D:
		case MVI_E:
		case MVI_H:
		case MVI_L:
		case MVI_M:
		case MVI_A:
			insertMVI(rom, (instr & 0x38) >> 3);
			break;
		case ADD_B:
		case ADD_C:
		case ADD_D:
		case ADD_E:
		case ADD_H:
		case ADD_L:
		case ADD_M:
		case ADD_A:
			insertADD(rom, (instr & 0x07));
			break;
		case ADC_B:
		case ADC_C:
		case ADC_D:
		case ADC_E:
		case ADC_H:
		case ADC_L:
		case ADC_M:
		case ADC_A:
			insertADC(rom, (instr & 0x07));
			break;
		case SUB_B:
		case SUB_C:
		case SUB_D:
		case SUB_E:
		case SUB_H:
		case SUB_L:
		case SUB_M:
		case SUB_A:
			insertSUB(rom, (instr & 0x07));
			break;
		case SBB_B:
		case SBB_C:
		case SBB_D:
		case SBB_E:
		case SBB_H:
		case SBB_L:
		case SBB_M:
		case SBB_A:
			insertSBB(rom, (instr & 0x07));
			break;
		case ANA_B:
		case ANA_C:
		case ANA_D:
		case ANA_E:
		case ANA_H:
		case ANA_L:
		case ANA_M:
		case ANA_A:
			insertANA(rom, (instr & 0x07));
			break;
		case XRA_B:
		case XRA_C:
		case XRA_D:
		case XRA_E:
		case XRA_H:
		case XRA_L:
		case XRA_M:
		case XRA_A:
			insertXRA(rom, (instr & 0x07));
			break;
		case ORA_B:
		case ORA_C:
		case ORA_D:
		case ORA_E:
		case ORA_H:
		case ORA_L:
		case ORA_M:
		case ORA_A:
			insertORA(rom, (instr & 0x07));
			break;
		case CMP_B:
		case CMP_C:
		case CMP_D:
		case CMP_E:
		case CMP_H:
		case CMP_L:
		case CMP_M:
		case CMP_A:
			insertCMP(rom, (instr & 0x07));
			break;
		case MOV_BB:
		case MOV_BC:
		case MOV_BD:
		case MOV_BE:
		case MOV_BH:
		case MOV_BL:
		case MOV_BM:
		case MOV_BA:
		case MOV_CB:
		case MOV_CC:
		case MOV_CD:
		case MOV_CE:
		case MOV_CH:
		case MOV_CL:
		case MOV_CM:
		case MOV_CA:
		case MOV_DB:
		case MOV_DC:
		case MOV_DD:
		case MOV_DE:
		case MOV_DH:
		case MOV_DL:
		case MOV_DM:
		case MOV_DA:
		case MOV_EB:
		case MOV_EC:
		case MOV_ED:
		case MOV_EE:
		case MOV_EH:
		case MOV_EL:
		case MOV_EM:
		case MOV_EA:
		case MOV_HB:
		case MOV_HC:
		case MOV_HD:
		case MOV_HE:
		case MOV_HH:
		case MOV_HL:
		case MOV_HM:
		case MOV_HA:
		case MOV_LB:
		case MOV_LC:
		case MOV_LD:
		case MOV_LE:
		case MOV_LH:
		case MOV_LL:
		case MOV_LM:
		case MOV_LA:
		case MOV_MB:
		case MOV_MC:
		case MOV_MD:
		case MOV_ME:
		case MOV_MH:
		case MOV_ML:
		case MOV_MA:
		case MOV_AB:
		case MOV_AC:
		case MOV_AD:
		case MOV_AE:
		case MOV_AH:
		case MOV_AL:
		case MOV_AM:
		case MOV_AA:
			insertMOV(rom, (instr & 0x38) >> 3, (instr & 0x07));
			break;
		case RLC:
			insertRLC(rom);
			break;
		case RRC:
			insertRRC(rom);
			break;
		case RAL:
			insertRAL(rom);
			break;
		case RAR:
			insertRAR(rom);
			break;
		case SHLD:
			insertSHLD(rom);
			break;
		case LHLD:
			insertLHLD(rom);
			break;
		case DAA:
			insertDAA(rom);
			break;
		case CMA:
			insertCMA(rom);
			break;
		case CMC:
			insertCMC(rom);
			break;
		case STA:
			insertSTA(rom);
			break;
		case STC:
			insertSTC(rom);
			break;
		case LDA:
			insertLDA(rom);
			break;
		case HLT:
			insertHLT(rom);
			break;
		case RET:
			insertRET(rom);
			break;
		case RNZ:
		case RZ:
		case RNC:
		case RC:
		case RPO:
		case RPE:
		case RP:
		case RM:
			insertRcc(rom, (instr & 0x38) >> 3);
			break;
		case JMP:
			insertJMP(rom);
			break;
		case JNZ:
		case JZ:
		case JNC:
		case JC:
		case JPO:
		case JPE:
		case JP:
		case JM:
			insertJcc(rom, (instr & 0x38) >> 3);
			break;
		case CALL:
			insertCALL(rom);
			break;
		case CNZ:
		case CZ:
		case CNC:
		case CC:
		case CPO:
		case CPE:
		case CP:
		case CM:
			insertCcc(rom, (instr & 0x38) >> 3);
			break;
		case RST_0:
		case RST_1:
		case RST_2:
		case RST_3:
		case RST_4:
		case RST_5:
		case RST_6:
		case RST_7:
			insertRST(rom, (instr & 0x38) >> 3);
			break;
		case ADI:
			insertADI(rom);
			break;
		case ACI:
			insertACI(rom);
			break;
		case SUI:
			insertSUI(rom);
			break;
		case SBI:
			insertSBI(rom);
			break;
		case ANI:
			insertANI(rom);
			break;
		case XRI:
			insertXRI(rom);
			break;
		case ORI:
			insertORI(rom);
			break;
		case CPI:
			insertCPI(rom);
			break;
		case OUT:
			insertOUT(rom);
			break;
		case IN:
			insertIN(rom);
			break;
		case XTHL:
			insertXTHL(rom);
			break;
		case PCHL:
			insertPCHL(rom);
			break;
		case SPHL:
			insertSPHL(rom);
			break;
		case XCHG:
			insertXCHG(rom);
			break;
		case DI:
			insertDI(rom);
			break;
		case EI:
			insertEI(rom);
			break;
		case NOP:
		default:
			insertNOP(rom);
			break;
		}
	}
}

const char* mnemonicFromOpcode(uint8_t instr)
{
	switch (instr) {
	case LXI_B:
		return "LXI B";
	case LXI_D:
		return "LXI D";
	case LXI_H:
		return "LXI H";
	case LXI_SP:
		return "LXI SP";
	case INX_B:
		return "INX B";
	case INX_D:
		return "INX D";
	case INX_H:
		return "INX H";
	case INX_SP:
		return "INX SP";
	case DCX_B:
		return "DCX B";
	case DCX_D:
		return "DCX D";
	case DCX_H:
		return "DCX H";
	case DCX_SP:
		return "DCX SP";
	case DAD_B:
		return "DAD B";
	case DAD_D:
		return "DAD D";
	case DAD_H:
		return "DAD H";
	case DAD_SP:
		return "DAD SP";
	case POP_B:
		return "POP B";
	case POP_D:
		return "POP D";
	case POP_H:
		return "POP H";
	case POP_PSW:
		return "POP PSW";
	case PUSH_B:
		return "PUSH B";
	case PUSH_D:
		return "PUSH D";
	case PUSH_H:
		return "PUSH H";
	case PUSH_PSW:
		return "PUSH PSW";
	case STAX_B:
		return "STAX B";
	case STAX_D:
		return "STAX D";
	case LDAX_B:
		return "LDAX B";
	case LDAX_D:
		return "LDAX D";
	case INR_B:
		return "INR B";
	case INR_C:
		return "INR C";
	case INR_D:
		return "INR D";
	case INR_E:
		return "INR E";
	case INR_H:
		return "INR H";
	case INR_L:
		return "INR L";
	case INR_M:
		return "INR M";
	case INR_A:
		return "INR A";
	case DCR_B:
		return "DCR B";
	case DCR_C:
		return "DCR C";
	case DCR_D:
		return "DCR D";
	case DCR_E:
		return "DCR E";
	case DCR_H:
		return "DCR H";
	case DCR_L:
		return "DCR L";
	case DCR_M:
		return "DCR M";
	case DCR_A:
		return "DCR A";
	case MVI_B:
		return "MVI B";
	case MVI_C:
		return "MVI C";
	case MVI_D:
		return "MVI D";
	case MVI_E:
		return "MVI E";
	case MVI_H:
		return "MVI H";
	case MVI_L:
		return "MVI L";
	case MVI_M:
		return "MVI M";
	case MVI_A:
		return "MVI A";
	case ADD_B:
		return "ADD B";
	case ADD_C:
		return "ADD C";
	case ADD_D:
		return "ADD D";
	case ADD_E:
		return "ADD E";
	case ADD_H:
		return "ADD H";
	case ADD_L:
		return "ADD L";
	case ADD_M:
		return "ADD M";
	case ADD_A:
		return "ADD A";
	case ADC_B:
		return "ADC B";
	case ADC_C:
		return "ADC C";
	case ADC_D:
		return "ADC D";
	case ADC_E:
		return "ADC E";
	case ADC_H:
		return "ADC H";
	case ADC_L:
		return "ADC L";
	case ADC_M:
		return "ADC M";
	case ADC_A:
		return "ADC A";
	case SUB_B:
		return "SUB B";
	case SUB_C:
		return "SUB C";
	case SUB_D:
		return "SUB D";
	case SUB_E:
		return "SUB E";
	case SUB_H:
		return "SUB H";
	case SUB_L:
		return "SUB L";
	case SUB_M:
		return "SUB M";
	case SUB_A:
		return "SUB A";
	case SBB_B:
		return "SBB B";
	case SBB_C:
		return "SBB C";
	case SBB_D:
		return "SBB D";
	case SBB_E:
		return "SBB E";
	case SBB_H:
		return "SBB H";
	case SBB_L:
		return "SBB L";
	case SBB_M:
		return "SBB M";
	case SBB_A:
		return "SBB A";
	case ANA_B:
		return "ANA B";
	case ANA_C:
		return "ANA C";
	case ANA_D:
		return "ANA D";
	case ANA_E:
		return "ANA E";
	case ANA_H:
		return "ANA H";
	case ANA_L:
		return "ANA L";
	case ANA_M:
		return "ANA M";
	case ANA_A:
		return "ANA A";
	case XRA_B:
		return "XRA B";
	case XRA_C:
		return "XRA C";
	case XRA_D:
		return "XRA D";
	case XRA_E:
		return "XRA E";
	case XRA_H:
		return "XRA H";
	case XRA_L:
		return "XRA L";
	case XRA_M:
		return "XRA M";
	case XRA_A:
		return "XRA A";
	case ORA_B:
		return "ORA B";
	case ORA_C:
		return "ORA C";
	case ORA_D:
		return "ORA D";
	case ORA_E:
		return "ORA E";
	case ORA_H:
		return "ORA H";
	case ORA_L:
		return "ORA L";
	case ORA_M:
		return "ORA M";
	case ORA_A:
		return "ORA A";
	case CMP_B:
		return "CMP B";
	case CMP_C:
		return "CMP C";
	case CMP_D:
		return "CMP D";
	case CMP_E:
		return "CMP E";
	case CMP_H:
		return "CMP H";
	case CMP_L:
		return "CMP L";
	case CMP_M:
		return "CMP M";
	case CMP_A:
		return "CMP A";
	case MOV_BB:
		return "MOV B, B";
	case MOV_BC:
		return "MOV B, C";
	case MOV_BD:
		return "MOV B, D";
	case MOV_BE:
		return "MOV B, E";
	case MOV_BH:
		return "MOV B, H";
	case MOV_BL:
		return "MOV B, L";
	case MOV_BM:
		return "MOV B, M";
	case MOV_BA:
		return "MOV B, A";
	case MOV_CB:
		return "MOV C, B";
	case MOV_CC:
		return "MOV C, C";
	case MOV_CD:
		return "MOV C, D";
	case MOV_CE:
		return "MOV C, E";
	case MOV_CH:
		return "MOV C, H";
	case MOV_CL:
		return "MOV C, L";
	case MOV_CM:
		return "MOV C, M";
	case MOV_CA:
		return "MOV C, A";
	case MOV_DB:
		return "MOV D, B";
	case MOV_DC:
		return "MOV D, C";
	case MOV_DD:
		return "MOV D, D";
	case MOV_DE:
		return "MOV D, E";
	case MOV_DH:
		return "MOV D, H";
	case MOV_DL:
		return "MOV D, L";
	case MOV_DM:
		return "MOV D, M";
	case MOV_DA:
		return "MOV D, A";
	case MOV_EB:
		return "MOV E, B";
	case MOV_EC:
		return "MOV E, C";
	case MOV_ED:
		return "MOV E, D";
	case MOV_EE:
		return "MOV E, E";
	case MOV_EH:
		return "MOV E, H";
	case MOV_EL:
		return "MOV E, L";
	case MOV_EM:
		return "MOV E, M";
	case MOV_EA:
		return "MOV E, A";
	case MOV_HB:
		return "MOV H, B";
	case MOV_HC:
		return "MOV H, C";
	case MOV_HD:
		return "MOV H, D";
	case MOV_HE:
		return "MOV H, E";
	case MOV_HH:
		return "MOV H, H";
	case MOV_HL:
		return "MOV H, L";
	case MOV_HM:
		return "MOV H, M";
	case MOV_HA:
		return "MOV H, A";
	case MOV_LB:
		return "MOV L, B";
	case MOV_LC:
		return "MOV L, C";
	case MOV_LD:
		return "MOV L, D";
	case MOV_LE:
		return "MOV L, E";
	case MOV_LH:
		return "MOV L, H";
	case MOV_LL:
		return "MOV L, L";
	case MOV_LM:
		return "MOV L, M";
	case MOV_LA:
		return "MOV L, A";
	case MOV_MB:
		return "MOV M, B";
	case MOV_MC:
		return "MOV M, C";
	case MOV_MD:
		return "MOV M, D";
	case MOV_ME:
		return "MOV M, E";
	case MOV_MH:
		return "MOV M, H";
	case MOV_ML:
		return "MOV M, L";
	case MOV_MA:
		return "MOV M, A";
	case MOV_AB:
		return "MOV A, B";
	case MOV_AC:
		return "MOV A, C";
	case MOV_AD:
		return "MOV A, D";
	case MOV_AE:
		return "MOV A, E";
	case MOV_AH:
		return "MOV A, H";
	case MOV_AL:
		return "MOV A, L";
	case MOV_AM:
		return "MOV A, M";
	case MOV_AA:
		return "MOV A, A";
	case RLC:
		return "RLC";
	case RRC:
		return "RRC";
	case RAL:
		return "RAL";
	case RAR:
		return "RAR";
	case SHLD:
		return "SHLD";
	case LHLD:
		return "LHLD";
	case DAA:
		return "DAA";
	case CMA:
		return "CMA";
	case CMC:
		return "CMC";
	case STA:
		return "STA";
	case STC:
		return "STC";
	case LDA:
		return "LDA";
	case HLT:
		return "HLT";
	case RET:
		return "RET";
	case RNZ:
		return "RNZ";
	case RZ:
		return "RZ";
	case RNC:
		return "RNC";
	case RC:
		return "RC";
	case RPO:
		return "RPO";
	case RPE:
		return "RPE";
	case RP:
		return "RP";
	case RM:
		return "RM";
	case JMP:
		return "JMP";
	case JNZ:
		return "JNZ";
	case JZ:
		return "JZ";
	case JNC:
		return "JNC";
	case JC:
		return "JC";
	case JPO:
		return "JPO";
	case JPE:
		return "JPE";
	case JP:
		return "JP";
	case JM:
		return "JM";
	case CALL:
		return "CALL";
	case CNZ:
		return "CNZ";
	case CZ:
		return "CZ";
	case CNC:
		return "CNC";
	case CC:
		return "CC";
	case CPO:
		return "CPO";
	case CPE:
		return "CPE";
	case CP:
		return "CP";
	case CM:
		return "CM";
	case RST_0:
		return "RST 0";
	case RST_1:
		return "RST 1";
	case RST_2:
		return "RST 2";
	case RST_3:
		return "RST 3";
	case RST_4:
		return "RST 4";
	case RST_5:
		return "RST 5";
	case RST_6:
		return "RST 6";
	case RST_7:
		return "RST 7";
	case ADI:
		return "ADI";
	case ACI:
		return "ACI";
	case SUI:
		return "SUI";
	case SBI:
		return "SBI";
	case ANI:
		return "ANI";
	case XRI:
		return "XRI";
	case ORI:
		return "ORI";
	case CPI:
		return "CPI";
	case OUT:
		return "OUT";
	case IN:
		return "IN";
	case XTHL:
		return "XTHL";
	case PCHL:
		return "PCHL";
	case SPHL:
		return "SPHL";
	case XCHG:
		return "XCHG";
	case DI:
		return "DI";
	case EI:
		return "EI";
	case NOP:
		return "NOP";
	}

	return "Invalid opcode";
}

//////////////////////////////////////////////////////////////////////////
// Helpers
//
StateBuilder& StateBuilder::reset(MachineCycleBuilder* parent, MicroInstruction* uinstr)
{
	assert(parent);

	m_Instr = uinstr;
	m_Parent = parent;
	m_FieldsCompleted = 0;
	m_HasEnded = false;

	return *this;
}

MachineCycleBuilder& StateBuilder::end()
{
	assert(m_FieldsCompleted == All);
	assert(m_Parent);
	m_HasEnded = true;
	return *m_Parent;
}

StateBuilder& StateBuilder::flow(ConditionCode::Enum cc, NextAddressSelect::Enum nas, uint16_t branchAddr)
{
	assert(!(m_FieldsCompleted & Flow));
	m_Instr->m_ConditionCode = (uint8_t)cc;
	m_Instr->m_NextAddressSelect = (uint8_t)nas;
	m_Instr->m_BranchAddr = (branchAddr & 0x07FF);

	m_FieldsCompleted |= Flow;

	return *this;
}

StateBuilder& StateBuilder::machineCycle(MachineCycleType::Enum type, bool isLastMachineCycle)
{
	assert(!(m_FieldsCompleted & MachineCycle));
	m_Instr->m_MachineCycleType = (uint8_t)type;
	m_Instr->m_IsLastMachineCycle = isLastMachineCycle ? 1 : 0;

	m_FieldsCompleted |= MachineCycle;

	return *this;
}

StateBuilder& StateBuilder::state(uint32_t stateNumber, bool isLastState, uint8_t sync)
{
	assert(!(m_FieldsCompleted & State));
	//		assert(!isLastState || (isLastState && (stateNumber > 2))); // NOTE: Can happen in HLT so don't check.
	assert(!sync || (sync && (stateNumber == 1))); // Sync can go high only on T1 (but not always on T1 that's why a separate signal is needed (see DAD))
	m_Instr->m_IsT1 = (stateNumber == 1) ? 1 : 0;
	m_Instr->m_IsT2 = (stateNumber == 2) ? 1 : 0;
	m_Instr->m_IsT3 = (stateNumber == 3) ? 1 : 0;
	m_Instr->m_IsLastT = isLastState ? 1 : 0;
	m_Instr->m_Sync = sync & 1;

	m_FieldsCompleted |= State;

	return *this;
}

StateBuilder& StateBuilder::alu(ALUASrc::Enum srcA, ALUBSrc::Enum srcB, ALUCSrc::Enum srcC, ALUOp::Enum op)
{
	assert(!(m_FieldsCompleted & ALU));
	m_Instr->m_ALUASrc = (uint8_t)srcA;
	m_Instr->m_ALUBSrc = (uint8_t)srcB;
	m_Instr->m_ALUCSrc = (uint8_t)srcC;
	m_Instr->m_ALUOp = (uint8_t)op;

	m_FieldsCompleted |= ALU;

	return *this;
}

StateBuilder& StateBuilder::internalRegisters(ACCSrc::Enum accSrc, bool weACC, ACTSrc::Enum actSrc, bool weACT, bool weTMP)
{
	assert(!(m_FieldsCompleted & InternalRegisters));
	m_Instr->m_ACCSrc = (uint8_t)accSrc;
	m_Instr->m_ACTSrc = (uint8_t)actSrc;
	m_Instr->m_WriteEnableACC = weACC ? 1 : 0;
	m_Instr->m_WriteEnableTMP = weTMP ? 1 : 0;
	m_Instr->m_WriteEnableACT = weACT ? 1 : 0;

	m_FieldsCompleted |= InternalRegisters;

	return *this;
}

StateBuilder& StateBuilder::flags(FlagsCSrc::Enum cSrc, FlagsPSrc::Enum pSrc, FlagsACSrc::Enum acSrc, FlagsZSrc::Enum zSrc, FlagsSSrc::Enum sSrc, bool weC, bool weP, bool weAC, bool weZ, bool weS)
{
	assert(!(m_FieldsCompleted & Flags));
	m_Instr->m_FlagsCSrc = (uint8_t)cSrc;
	m_Instr->m_FlagsPSrc = (uint8_t)pSrc;
	m_Instr->m_FlagsACSrc = (uint8_t)acSrc;
	m_Instr->m_FlagsZSrc = (uint8_t)zSrc;
	m_Instr->m_FlagsSSrc = (uint8_t)sSrc;
	m_Instr->m_WriteEnableFlagsC = weC ? 1 : 0;
	m_Instr->m_WriteEnableFlagsP = weP ? 1 : 0;
	m_Instr->m_WriteEnableFlagsAC = weAC ? 1 : 0;
	m_Instr->m_WriteEnableFlagsZ = weZ ? 1 : 0;
	m_Instr->m_WriteEnableFlagsS = weS ? 1 : 0;

	m_FieldsCompleted |= Flags;

	return *this;
}

StateBuilder& StateBuilder::registerFile(RegisterFileSrcReg::Enum srcReg, RegisterFileDstReg::Enum dstReg, RegisterFileSrcRegPair::Enum srcPair, RegisterFileDstRegPair::Enum dstPair, RegisterFileRegPairOp::Enum rpOp, bool swapHLDE, bool weReg, bool wePair)
{
	assert(!(m_FieldsCompleted & RegisterFile));
	assert(!RegisterFileDstRegPair::pairIncludesReg(dstPair, dstReg));

	m_Instr->m_RFSrcReg = (uint8_t)srcReg;
	m_Instr->m_RFDstReg = (uint8_t)dstReg;
	m_Instr->m_RFSrcRegPair = (uint8_t)srcPair;
	m_Instr->m_RFDstRegPair = (uint8_t)dstPair;
	m_Instr->m_RFRegPairOp = (uint8_t)rpOp;
	m_Instr->m_RFSwapHLDE = swapHLDE ? 1 : 0;
	m_Instr->m_WriteEnableRFReg = weReg ? 1 : 0;
	m_Instr->m_WriteEnableRFRegPair = wePair ? 1 : 0;

	m_FieldsCompleted |= RegisterFile;

	return *this;
}

StateBuilder& StateBuilder::misc(InternalDataBusSrc::Enum dbSrc, uint8_t hlta, bool weHLTA, uint8_t inte, bool weINTE, bool weDataOut)
{
	assert(!(m_FieldsCompleted & Misc));

	assert(!m_Instr->m_IsT1 || (m_Instr->m_IsT1 && dbSrc == InternalDataBusSrc::StatusWord));

	m_Instr->m_InternalDataBusSrc = (uint8_t)dbSrc;
	m_Instr->m_HLTA = hlta & 1;
	m_Instr->m_WriteEnableHLTA = weHLTA ? 1 : 0;
	m_Instr->m_INTE = inte & 1;
	m_Instr->m_WriteEnableINTE = weINTE ? 1 : 0;
	m_Instr->m_WriteEnableDataOut = weDataOut ? 1 : 0;

	m_FieldsCompleted |= Misc;

	return *this;
}
