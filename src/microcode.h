#ifndef MICROCODE_H
#define MICROCODE_H

#include <stdint.h>
#include <assert.h>

struct MachineCycleType
{
	enum Enum : uint8_t
	{
		InstructionFetch = 0,
		MemoryRead = 1,
		MemoryWrite = 2,
		StackRead = 3,
		StackWrite = 4,
		InputRead = 5,
		OutputWrite = 6,
		InterruptAck = 7,
		HaltAck = 8,
		InterruptAckWhileHalt = 9,
		Internal = 10 // NOTE: Used by DAD instruction and the reset sequence.
	};
};

struct ALUASrc
{
	enum Enum : uint8_t
	{
		ACT = 0,
		TMP = 1
	};
};

struct ALUBSrc
{
	enum Enum : uint8_t 
	{
		TMP = 0,
		Zero = 1
	};
};

struct ALUCSrc
{
	enum Enum : uint8_t
	{
		Zero = 0,
		One = 1,
		Flags = 2,
		ACC_0 = 3,
		ACC_7 = 4,
	};
};

struct ALUOp
{
	enum Enum : uint8_t
	{
		AND = 0,
		OR = 1,
		XOR = 2,
		Add = 3,
		Sub = 4,
		RotateLeft = 5,
		RotateRight = 6,
		DecimalAdjust = 7
	};
};

struct ACCSrc
{
	enum Enum : uint8_t
	{
		NOT_Self = 0,
		InternalDataBus = 1,
	};
};

struct ACTSrc
{
	enum Enum : uint8_t
	{
		ACC = 0,
		InternalDataBus = 1,
	};
};

struct FlagsCSrc
{
	enum Enum : uint8_t
	{
		ALU = 0,
		One = 1,
		NOT_Self = 2,
		InternalDataBus = 3,
	};
};

struct FlagsPSrc
{
	enum Enum : uint8_t
	{
		ALU = 0,
		InternalDataBus = 1,
	};
};

struct FlagsACSrc
{
	enum Enum : uint8_t
	{
		ALU = 0,
		InternalDataBus = 1,
	};
};

struct FlagsZSrc
{
	enum Enum : uint8_t
	{
		ALU = 0,
		InternalDataBus = 1,
	};
};

struct FlagsSSrc
{
	enum Enum : uint8_t
	{
		ALU = 0,
		InternalDataBus = 1,
	};
};

struct InternalDataBusSrc
{
	enum Enum : uint8_t
	{
		ACC = 0,
		TMP = 1,
		Flags = 2,
		ALU = 3,
		RegisterFile = 4,
		DataIn = 5,
		StatusWord = 6,
	};
};

struct RegisterFileSrcReg
{
	enum Enum : uint8_t
	{
		B = 0,
		C = 1, 
		D = 2,
		E = 3,
		H = 4,
		L = 5,
		W = 6,
		Z = 7,
		SPH = 8,
		SPL = 9,
		PCH = 10,
		PCL = 11
	};

	static Enum fromInstruction(uint32_t regID)
	{
		assert(regID < 6); // A (111b = 7) and M (110b = 6) aren't part of the register file and should be handled separately.
		return (Enum)regID;
	}

	static Enum fromRegisterPair_low(uint32_t rp)
	{
		if (rp == 0) {
			return C;
		} else if (rp == 1) {
			return E;
		} else if (rp == 2) {
			return L;
		} else if (rp == 3) {
			return SPL;
		}

		assert(false);
		return Z;
	}

	static Enum fromRegisterPair_high(uint32_t rp)
	{
		if (rp == 0) {
			return B;
		} else if (rp == 1) {
			return D;
		} else if (rp == 2) {
			return H;
		} else if (rp == 3) {
			return SPH;
		}

		assert(false);
		return W;
	}
};

struct RegisterFileDstReg
{
	enum Enum : uint8_t
	{
		B = 0,
		C = 1,
		D = 2,
		E = 3,
		H = 4,
		L = 5,
		W = 6,
		Z = 7,
		SPH = 8,
		SPL = 9,
		PCH = 10,
		PCL = 11,
		W_and_Z = 12
	};

	static Enum fromInstruction(uint32_t regID)
	{
		assert(regID < 6); // A (111b = 7) and M (110b = 6) aren't part of the register file and should be handled separately.
		return (Enum)regID;
	}

	static Enum fromRegisterPair_low(uint32_t rp)
	{
		if (rp == 0) {
			return C;
		} else if (rp == 1) {
			return E;
		} else if (rp == 2) {
			return L;
		} else if (rp == 3) {
			return SPL;
		}

		assert(false);
		return Z;
	}

	static Enum fromRegisterPair_high(uint32_t rp)
	{
		if (rp == 0) {
			return B;
		} else if (rp == 1) {
			return D;
		} else if (rp == 2) {
			return H;
		} else if (rp == 3) {
			return SPH;
		}

		assert(false);
		return W;
	}
};

struct RegisterFileSrcRegPair
{
	enum Enum : uint8_t
	{
		BC = 0,
		DE = 1,
		HL = 2,
		WZ = 3,
		SP = 4,
		PC = 5,
	};

	static Enum fromInstruction(uint32_t rp)
	{
		assert(rp <= 3);
		if (rp == 0) {
			return BC;
		} else if (rp == 1) {
			return DE;
		} else if (rp == 2) {
			return HL;
		} else if (rp == 3) {
			return SP;
		}

		return WZ; // Unreachable code
	}
};

struct RegisterFileDstRegPair
{
	enum Enum : uint8_t
	{
		BC = 0,
		DE = 1,
		HL = 2,
		WZ = 3,
		SP = 4,
		PC = 5,
	};

	static Enum fromInstruction(uint32_t rp)
	{
		assert(rp <= 3);
		if (rp == 0) {
			return BC;
		} else if (rp == 1) {
			return DE;
		} else if (rp == 2) {
			return HL;
		} else if (rp == 3) {
			return SP;
		}

		return WZ; // Unreachable code
	}
};

struct RegisterFileRegPairOp
{
	enum Enum : uint8_t
	{
		Zero = 0,      // +0
		Inc = 1,       // +1
//		MinusZero = 2, // -0
		Dec = 3,       // -1
	};
};

struct ConditionCode
{
	enum Enum : uint8_t
	{
		NZ = 0,
		Z = 1,
		NC = 2,
		C = 3,
		PO = 4,
		PE = 5,
		P = 6,
		M = 7
	};

	static Enum fromInstruction(uint32_t cc)
	{
		assert(cc < 8);
		return (Enum)cc;
	}

	static Enum fromInstructionInverse(uint32_t cc)
	{
		assert(cc < 8);
		return (Enum)(cc ^ 1);
	}
};

struct NextAddressSelect
{
	enum Enum : uint8_t
	{
		Continue = 0,
		Branch = 1
	};
};

#pragma pack(push, 1)
struct MicroInstruction
{
	// Flow control
	union
	{
		struct
		{
			uint16_t m_ConditionCode : 3;
			uint16_t m_NextAddressSelect : 1; // 0 = continue, 1 = branch address
			uint16_t m_BranchAddr : 11; // Max 2048 addresses in decode ROM.
		};
		uint16_t m_FlowControlBitfield;
	};

	union
	{
		struct
		{
			// State identification
			uint64_t m_IsT1 : 1;
			uint64_t m_IsT2 : 1;
			uint64_t m_IsT3 : 1;
			uint64_t m_IsLastT : 1;
			uint64_t m_Sync : 1;

			// Machine cycle identification
			uint64_t m_IsLastMachineCycle : 1;
			uint64_t m_MachineCycleType : 4;

			// ALU
			uint64_t m_ALUASrc : 1;
			uint64_t m_ALUBSrc : 1;
			uint64_t m_ALUCSrc : 3;
			uint64_t m_ALUOp : 3;

			// Internal registers
			uint64_t m_ACCSrc : 1;
			uint64_t m_ACTSrc : 1;
			uint64_t m_WriteEnableACC : 1;
			uint64_t m_WriteEnableTMP : 1;
			uint64_t m_WriteEnableACT : 1;

			// Flags
			uint64_t m_FlagsCSrc : 2;
			uint64_t m_FlagsPSrc : 1;
			uint64_t m_FlagsACSrc : 1;
			uint64_t m_FlagsZSrc : 1;
			uint64_t m_FlagsSSrc : 1;
			uint64_t m_WriteEnableFlagsC : 1;
			uint64_t m_WriteEnableFlagsP : 1;
			uint64_t m_WriteEnableFlagsAC : 1;
			uint64_t m_WriteEnableFlagsZ : 1;
			uint64_t m_WriteEnableFlagsS : 1;

			// Register file
			uint64_t m_RFSrcReg : 4;
			uint64_t m_RFDstReg : 4;
			uint64_t m_RFSrcRegPair : 3;
			uint64_t m_RFDstRegPair : 3;
			uint64_t m_RFRegPairOp : 2;
			uint64_t m_RFSwapHLDE : 1;
			uint64_t m_WriteEnableRFReg : 1;
			uint64_t m_WriteEnableRFRegPair : 1;

			// Misc states
			uint64_t m_HLTA : 1;
			uint64_t m_WriteEnableHLTA : 1;
			uint64_t m_INTE : 1;
			uint64_t m_WriteEnableINTE : 1;
			uint64_t m_WriteEnableDataOut : 1;
			uint64_t m_InternalDataBusSrc : 3;
		};

		uint64_t m_MicroInstructionBitfield;
	};
};
#pragma pack(pop)

struct DecodeROM
{
	MicroInstruction m_States[2048];
	uint16_t m_AddressROM[256];
	uint32_t m_NumStates;
	uint8_t m_CurOpCode;
};

void buildDecodeROM(DecodeROM* rom);

// Helper functions...
const char* mnemonicFromOpcode(uint8_t opcode);

// Procedular interface for build the microcode
struct MachineCycleBuilder;
struct InstructionBuilder;

struct StateBuilder
{
	enum Fields
	{
		MachineCycle        = 0x00000001,
		State               = 0x00000002,
		ALU                 = 0x00000004,
		InternalRegisters   = 0x00000008,
		Flags               = 0x00000010,
		RegisterFile        = 0x00000020,
		Misc                = 0x00000040,
		Flow                = 0x00000080,

		All = MachineCycle | State | ALU | InternalRegisters | Flags | RegisterFile | Misc | Flow
	};

	StateBuilder() : m_Parent(nullptr), m_Instr(nullptr), m_FieldsCompleted(0), m_HasEnded(true)
	{}

	StateBuilder& reset(MachineCycleBuilder* parent, MicroInstruction* uinstr);
	MachineCycleBuilder& end();
	StateBuilder& flow(ConditionCode::Enum cc, NextAddressSelect::Enum nas, uint16_t branchAddr);	
	StateBuilder& machineCycle(MachineCycleType::Enum type, bool isLastMachineCycle);
	StateBuilder& state(uint32_t stateNumber, bool isLastState, uint8_t sync);
	StateBuilder& alu(ALUASrc::Enum srcA, ALUBSrc::Enum srcB, ALUCSrc::Enum srcC, ALUOp::Enum op);
	StateBuilder& internalRegisters(ACCSrc::Enum accSrc, bool weACC, ACTSrc::Enum actSrc, bool weACT, bool weTMP);
	StateBuilder& flags(FlagsCSrc::Enum cSrc, FlagsPSrc::Enum pSrc, FlagsACSrc::Enum acSrc, FlagsZSrc::Enum zSrc, FlagsSSrc::Enum sSrc, bool weC, bool weP, bool weAC, bool weZ, bool weS);
	StateBuilder& registerFile(RegisterFileSrcReg::Enum srcReg, RegisterFileDstReg::Enum dstReg, RegisterFileSrcRegPair::Enum srcPair, RegisterFileDstRegPair::Enum dstPair, RegisterFileRegPairOp::Enum rpOp, bool swapHLDE, bool weReg, bool wePair);
	StateBuilder& misc(InternalDataBusSrc::Enum dbSrc, uint8_t hlta, bool weHLTA, uint8_t inte, bool weINTE, bool weDataOut);

	bool hasEnded() const
	{
		return m_HasEnded;
	}

	// Additional helpers for common operations...
	StateBuilder& state_T1()
	{
		return state(1, false, 1);
	}

	StateBuilder& state_T2()
	{
		return state(2, false, 0);
	}

	StateBuilder& state_T3(bool isLast)
	{
		return state(3, isLast, 0);
	}

	StateBuilder& alu_disabled()
	{
		return alu(ALUASrc::ACT, ALUBSrc::Zero, ALUCSrc::Zero, ALUOp::OR);
	}

	StateBuilder& flags_none()
	{
		return flags(FlagsCSrc::ALU, FlagsPSrc::ALU, FlagsACSrc::ALU, FlagsZSrc::ALU, FlagsSSrc::ALU, false, false, false, false, false);
	}

	StateBuilder& registerFile_disabled()
	{
		return registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::PC, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Zero, false, false, false);
	}

	StateBuilder& registerFile_addrPC()
	{
		const bool we = m_Instr->m_IsT2 == 1;
		return registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::PC, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Inc, false, false, we);
	}

	StateBuilder& registerFile_addrHL()
	{
		return registerFile(RegisterFileSrcReg::B, RegisterFileDstReg::B, RegisterFileSrcRegPair::HL, RegisterFileDstRegPair::HL, RegisterFileRegPairOp::Zero, false, false, false);
	}

	StateBuilder& registerFile_writeReg(RegisterFileDstReg::Enum r)
	{
		return registerFile(RegisterFileSrcReg::B, r, RegisterFileSrcRegPair::PC, RegisterFileDstRegPair::PC, RegisterFileRegPairOp::Zero, false, true, false);
	}

	StateBuilder& internalRegisters_disabled()
	{
		return internalRegisters(ACCSrc::InternalDataBus, false, ACTSrc::ACC, false, false);
	}

	StateBuilder& misc_StatusWordOut()
	{
		assert(m_Instr->m_IsT1 == 1);
		return misc(InternalDataBusSrc::StatusWord, 0, false, 0, false, true);
	}

	StateBuilder& misc_DataIn()
	{
		assert(m_Instr->m_IsT1 == 0);
		return misc(InternalDataBusSrc::DataIn, 0, false, 0, false, false);
	}

	StateBuilder& misc_DataOut(InternalDataBusSrc::Enum src)
	{
		return misc(src, 0, false, 0, false, true);
	}

	StateBuilder& flow_continue()
	{
		return flow(ConditionCode::NZ, NextAddressSelect::Continue, 0);
	}

	StateBuilder& flow_branch(ConditionCode::Enum cc, uint16_t branchAddr)
	{
		return flow(cc, NextAddressSelect::Branch, branchAddr);
	}

private:
	MicroInstruction* m_Instr;
	MachineCycleBuilder* m_Parent;
	uint32_t m_FieldsCompleted;
	bool m_HasEnded;
};

struct MachineCycleBuilder
{
	MachineCycleBuilder() : m_States(nullptr), m_NumStates(0), m_NextStateID(0), m_Parent(nullptr), m_HasEnded(true)
	{}

	MachineCycleBuilder& reset(InstructionBuilder* parent, MachineCycleType::Enum type, bool isLast, MicroInstruction* states, uint32_t numStates)
	{
		m_Parent = parent;
		m_States = states;
		m_NumStates = numStates;
		m_NextStateID = 0;
		m_Type = type;
		m_IsLast = isLast;
		m_HasEnded = false;

		return *this;
	}

	StateBuilder& nextState()
	{
		assert(m_StateBuilder.hasEnded());
		assert(m_NextStateID < m_NumStates);
		MicroInstruction* s = &m_States[m_NextStateID++];
		return m_StateBuilder
			.reset(this, s)
			.machineCycle(m_Type, m_IsLast);
	}

	InstructionBuilder& end()
	{
		assert(m_Parent);
		assert(m_NextStateID == m_NumStates);
		assert(m_StateBuilder.hasEnded());
		m_HasEnded = true;
		return *m_Parent;
	}

	bool hasEnded() const
	{
		return m_HasEnded;
	}

private:
	StateBuilder m_StateBuilder;
	InstructionBuilder* m_Parent;
	MicroInstruction* m_States;
	uint32_t m_NumStates;
	uint32_t m_NextStateID;
	MachineCycleType::Enum m_Type;
	bool m_IsLast;
	bool m_HasEnded;
};

struct InstructionBuilder
{
	InstructionBuilder(uint32_t numMachineCycles, MicroInstruction* states, uint32_t numStates) : 
		m_States(states), 
		m_NumMachineCycles(numMachineCycles), 
		m_NumStates(numStates), 
		m_HasEnded(false),
		m_NextStateID(0)
	{
		m_NumMachineCycles = numMachineCycles;
		m_NumStates = numStates;
		m_States = states;
	}

	~InstructionBuilder()
	{
		assert(m_HasEnded);
	}

	MachineCycleBuilder& nextCycle(MachineCycleType::Enum type, uint32_t numStates, bool isLast)
	{
		assert(m_MachineCycleBuilder.hasEnded());
		assert(m_NextStateID + numStates <= m_NumStates);

		MicroInstruction* s = &m_States[m_NextStateID];
		m_NextStateID += numStates;

		return m_MachineCycleBuilder.reset(this, type, isLast, s, numStates);
	}

	void end()
	{
		assert(m_NextStateID == m_NumStates);
		assert(m_MachineCycleBuilder.hasEnded());
		m_HasEnded = true;
	}

private:
	MicroInstruction* m_States;
	MachineCycleBuilder m_MachineCycleBuilder;
	uint32_t m_NumMachineCycles;
	uint32_t m_NumStates;
	uint32_t m_NextStateID;
	bool m_HasEnded;
};

#endif
