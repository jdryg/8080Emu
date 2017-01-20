#ifndef I8080_H
#define I8080_H

#include <stdint.h>
#include <assert.h>
#include "microcode.h"

struct i8080
{
	// Inputs
	uint8_t iDin;
	uint8_t iPhi1 : 1;
	uint8_t iPhi2 : 1;
	uint8_t iReset : 1;
	uint8_t iInt : 1;
	uint8_t iHold : 1;
	uint8_t iReady : 1;

	// Outputs
	uint8_t oDout;
	uint8_t oAddrLow;
	uint8_t oAddrHigh;
	uint8_t oWRb : 1;
	uint8_t oDBIn : 1;
	uint8_t oINTE : 1;
	uint8_t oHoldAck : 1;
	uint8_t oWait : 1;
	uint8_t oSync : 1;

	i8080() : m_uInstr(nullptr), m_PrevPhi1(0), m_PrevPhi2(0), iPhi1(0), iPhi2(0)
	{}

	bool init()
	{
		buildDecodeROM(&m_DecodeROM);
		iPhi1 = 0;
		iPhi2 = 0;
		m_PrevPhi1 = 0;
		m_PrevPhi2 = 0;

		return true;
	}

	void tick()
	{
		// Check async inputs first
		if (iReset) {
			m_DecodeROMNextStateID = 0;
			m_uInstr = &m_DecodeROM.m_States[0];
			m_SwapHLDE = 0;

			// NOTE: The following code doesn't appear in the circuit. In DLS those signals will be UNDEFINED and (hopefully) 
			// everything will work as expected without needing to reset those registers.
			m_StateRun = 1;
			m_StateTW = 0;
			m_StateTWH = 0;
			m_StateHOLD = 0;
			m_InternalHOLD = 0;
			m_InternalINT = 0;
		} else {
			if (m_PrevPhi1 == 0 && iPhi1 == 1) {
				onPhi1_risingEdge();
			}

			if (m_PrevPhi1 == 1 && iPhi1 == 0) {
				onPhi1_fallingEdge();
			}

			if (m_PrevPhi2 == 0 && iPhi2 == 1) {
				onPhi2_risingEdge();
			}

			if (m_PrevPhi2 == 1 && iPhi2 == 0) {
				onPhi2_fallingEdge();
			}
		}

		// Update the clock state...
		m_PrevPhi1 = iPhi1;
		m_PrevPhi2 = iPhi2;
	}

#if EXERCISER
	void setPC(uint16_t addr)
	{
		writeRegisterPair(RegisterFileDstRegPair::PC, addr);
	}

	void setSP(uint16_t addr)
	{
		writeRegisterPair(RegisterFileDstRegPair::SP, addr);
	}

	uint16_t getSP()
	{
		return getRegisterPairValue(RegisterFileSrcRegPair::SP);
	}

	void cpm_bdos(uint8_t* ram)
	{
		if (m_Registers[RegisterFileSrcReg::C] == 2) {
			// output a character
			putchar(m_Registers[m_SwapHLDE ? RegisterFileSrcReg::L : RegisterFileSrcReg::E]);
		} else if (m_Registers[RegisterFileSrcReg::C] == 9) {
			uint8_t c;
			uint16_t addr = getRegisterPairValue(RegisterFileSrcRegPair::DE);
			while ((c = ram[addr++]) != '$') {
				putchar(c);
			}
		} else {
			printf("Unknown CP/M call %d!\n", m_Registers[RegisterFileSrcReg::C]);
		}
	}
#endif

private:
	DecodeROM m_DecodeROM;
	uint16_t m_DecodeROMNextStateID;
	MicroInstruction* m_uInstr;
	uint8_t m_IR;
	uint8_t m_ACC;
	uint8_t m_TMP;
	uint8_t m_ACT;
	uint8_t m_Registers[12]; // Includes PC and SP
	uint8_t m_PrevPhi1 : 1;
	uint8_t m_PrevPhi2 : 1;
	uint8_t m_FlagsC : 1;
	uint8_t m_FlagsP : 1;
	uint8_t m_FlagsAC : 1;
	uint8_t m_FlagsZ : 1;
	uint8_t m_FlagsS : 1;
	uint8_t m_StateTW : 1;
	uint8_t m_StateTWH : 1;
	uint8_t m_StateHOLD : 1;
	uint8_t m_StateRun : 1;
	uint8_t m_InternalHOLD : 1;
	uint8_t m_InternalINT : 1;
	uint8_t m_HLTA : 1;
	uint8_t m_IntEn : 1;
	uint8_t m_SwapHLDE : 1;

	void onPhi1_risingEdge();
	void onPhi1_fallingEdge();
	void onPhi2_risingEdge();
	void onPhi2_fallingEdge();

	uint8_t getInternalDataBusValue();
	uint8_t getFlags();
	uint8_t getALUResult(uint8_t& cOut, uint8_t& ac);
	uint8_t getALU_S(uint8_t alu);
	uint8_t getALU_Z(uint8_t alu);
	uint8_t getALU_P(uint8_t alu);
	uint8_t getRegisterFileOutput();
	uint8_t getRegisterPairHigh(RegisterFileSrcRegPair::Enum rp);
	uint8_t getRegisterPairLow(RegisterFileSrcRegPair::Enum rp);
	uint16_t getRegisterPairValue(RegisterFileSrcRegPair::Enum rp);
	void writeRegisterPair(RegisterFileDstRegPair::Enum rp, uint16_t value);
	void writeRegister(RegisterFileDstReg::Enum reg, uint8_t value);
};

#ifdef I8080_IMPLEMENTATION
static const uint8_t g_StatusWords[] = {
	0xA2, // Instruction Fetch
	0x82, // Memory Read
	0x00, // Memory Write
	0x86, // Stack Read
	0x04, // Stack Write
	0x42, // Input Read
	0x10, // Output Write
	0x23, // Interrupt Ack
	0x8A, // Halt Ack
	0x2B, // Interrupt Ack while Halt
	0x02, // Internal (= memory read w/o the MR bit set; this word will never get out of the CPU; required for DAD)
};

void i8080::onPhi1_risingEdge()
{
	assert(m_uInstr != nullptr);

	// Cache all used state to make the code below order independent.
	// NOTE: Values are assumed to have propagated to the flip-flop/register inputs at this point.
	const uint8_t stateTW = m_StateTW;
	const uint8_t stateTWH = m_StateTWH;
	const uint8_t stateRun = m_StateRun;
	const uint8_t stateHold = m_StateHOLD;
	const uint8_t isT1 = m_uInstr->m_IsT1;
	const uint8_t isT2 = m_uInstr->m_IsT2;
	const uint8_t isT3 = m_uInstr->m_IsT3;
	const uint8_t isLastT = m_uInstr->m_IsLastT;
	const uint8_t isLastCycle = m_uInstr->m_IsLastMachineCycle;
	const uint8_t cycleType = m_uInstr->m_MachineCycleType;
	const uint8_t nextAddrSelect = m_uInstr->m_NextAddressSelect;
	const uint8_t conditionCode = m_uInstr->m_ConditionCode;
	const uint8_t hlta = m_HLTA;
	const uint8_t inte = m_IntEn;
	const uint8_t internalHold = m_InternalHOLD;

	// TW flip flop
	if ((stateTW && !iReady) ||
		(isT2 && stateRun && ((!iReady || hlta) && !hlta))) {
#if TRACE_STATE
		if (m_StateTW == 0) {
			printf("Entering TW\n");
		}
#endif
		m_StateTW = 1;
	} else {
#if TRACE_STATE
		if (m_StateTW == 1) {
			printf("Exiting TW\n");
		}
#endif
		m_StateTW = 0;
	}
	oWait = m_StateTW;

	// TWH flip flop
	if ((stateTWH && !(iHold && iInt)) ||
		(isT2 && stateRun && hlta)) {
#if TRACE_STATE
		if (m_StateTWH == 0) {
			printf("Entering TWH\n");
		}
#endif
		m_StateTWH = 1;
	} else {
#if TRACE_STATE
		if (m_StateTW == 1) {
			printf("Exiting TWH\n");
		}
#endif
		m_StateTWH = 0;
	}

	// Run state flip flop
	if ((stateRun && !isT1 && !isT2) ||
		(!isLastT && internalHold) ||
		(stateTW && iReady) ||
		(isT2 && iReady && !hlta) ||
		isT1 ||
		iReset ||
		(stateTWH && iInt && inte && !internalHold)) {
#if TRACE_STATE
		if (m_StateRun == 0) {
			printf("Entering normal state\n");
		}
#endif
		m_StateRun = 1;
	} else {
#if TRACE_STATE
		if (m_StateRun == 0) {
			printf("Exiting normal state\n");
		}
#endif
		m_StateRun = 0;
	}

	// HOLD mode flip flop
	if ((stateHold && iHold) ||
		(isLastT && m_InternalHOLD) ||
		(stateTWH && iHold)) {
#if TRACE_STATE
		if (m_StateHOLD == 0) {
			printf("Entering HOLD mode\n");
		}
#endif
		m_StateHOLD = 1;
	} else {
#if TRACE_STATE
		if (m_StateHOLD == 1) {
			printf("Exiting HOLD mode\n");
		}
#endif
		m_StateHOLD = 0;
	}

	// Internal HOLD register
	if ((stateHold && !iHold) ||
		(stateRun && isT2 && iHold) ||
		(stateTWH && iHold)) {
		m_InternalHOLD = iHold;
	}
	oHoldAck = iHold;

	// Internal INT register
	if ((stateRun && isLastT && isLastCycle && iInt && inte) ||
		(stateTWH && iInt && inte) ||
		(isT2 && inte)) {
		m_InternalINT = !isT2;
	}

	// WR output flip flop
	// The 8080 CPU generates a WR output for the synchronization of external transfers, during those machine
	// cycles in which the processor outputs data.These include MEMORY WRITE, STACK WRITE, and OUTPUT. The
	// negative - going leading edge of WR is referenced to the rising edge of the first phi1 clock pulse 
	// following T2, and occurs within a brief delay(tDC) of that event. WR remains low until retriggered by 
	// the leading edge of phi1 during the state following T3. Note that any TW states intervening between T2 
	// and T3 of the output machine cycle will necessarily extend WR, in much the same way that DBIN is affected
	// during data input operations.
	if ((cycleType == MachineCycleType::MemoryWrite || cycleType == MachineCycleType::StackWrite || cycleType == MachineCycleType::OutputWrite) &&
		(stateTW || stateTWH || (stateRun && !isT2 && !isT1))) {
		oWRb = 0;
	} else {
		oWRb = 1;
	}

	if (stateRun && !stateHold) {
		if (cycleType == MachineCycleType::InstructionFetch && isT3) {
			// New instruction just fetched. Load ROM counter with the new address.
			m_DecodeROMNextStateID = m_DecodeROM.m_AddressROM[m_IR];
			m_uInstr = &m_DecodeROM.m_States[m_DecodeROMNextStateID];
		} else {
			// Find out the next microinstruction.
			if (nextAddrSelect == NextAddressSelect::Continue) {
				m_DecodeROMNextStateID++;
			} else if (nextAddrSelect == NextAddressSelect::Branch) {
				// Check if condition is true.
				uint8_t cc = (conditionCode & 0x06) >> 1;
				uint8_t polarity = conditionCode & 0x01;

				uint8_t condition = 0;
				if (cc == 0) {
					condition = m_FlagsZ;
				} else if (cc == 1) {
					condition = m_FlagsC;
				} else if (cc == 2) {
					condition = m_FlagsP;
				} else if (cc == 3) {
					condition = m_FlagsS;
				} else {
					// Shouldn't land here
					assert(false);
				}

				// NOTE: This is the same as condition == polarity
				condition ^= polarity;
				if (!condition) {
					m_DecodeROMNextStateID = m_uInstr->m_BranchAddr;
				} else {
					m_DecodeROMNextStateID++;
				}
			} else {
				assert(false);
			}

			m_uInstr = &m_DecodeROM.m_States[m_DecodeROMNextStateID];
		}
	}
}

void i8080::onPhi1_fallingEdge()
{
	// Nothing to do here...
}

void i8080::onPhi2_risingEdge()
{
	assert(m_uInstr != nullptr);

	// Cache used state
	const uint8_t cycleType = m_uInstr->m_MachineCycleType;
	const uint8_t isT1 = m_uInstr->m_IsT1;
	const uint8_t isT2 = m_uInstr->m_IsT2;
	const uint8_t isT3 = m_uInstr->m_IsT3;
	const uint8_t dstReg = m_uInstr->m_RFDstReg;
	const uint8_t idb = getInternalDataBusValue();
	const uint8_t regPairLo = getRegisterPairLow((RegisterFileSrcRegPair::Enum)m_uInstr->m_RFSrcRegPair);
	const uint8_t regPairHi = getRegisterPairHigh((RegisterFileSrcRegPair::Enum)m_uInstr->m_RFSrcRegPair);
	const uint8_t acc = m_ACC;
	const uint8_t inte = m_IntEn;
	uint8_t aluC, aluAC;
	const uint8_t alu = getALUResult(aluC, aluAC);

	// Update IR
	if (cycleType == MachineCycleType::InstructionFetch && isT3) {
		m_IR = idb;
#if TRACE_INSTRUCTIONS
		printf("%02X%02X: %-15s (A:%02X,BC:%04X,DE=%04X,HL=%04X,SP=%04X,CZSPAC=%d%d%d%d%d)\n"
			   , oAddrHigh
			   , oAddrLow
			   , mnemonicFromOpcode(idb)
			   , m_ACC
			   , getRegisterPairValue(RegisterFileSrcRegPair::BC)
			   , getRegisterPairValue(RegisterFileSrcRegPair::DE)
			   , getRegisterPairValue(RegisterFileSrcRegPair::HL)
			   , getRegisterPairValue(RegisterFileSrcRegPair::SP)
			   , m_FlagsC, m_FlagsZ, m_FlagsS, m_FlagsP, m_FlagsAC);
#endif
	}

	// Update Address bus
	if (isT1) {
		oAddrHigh = regPairHi;
		oAddrLow = regPairLo;
	}

	// Update sync
	oSync = m_uInstr->m_Sync;

	// Write output data
	if (m_uInstr->m_WriteEnableDataOut) {
		oDout = idb;
	}

	// During the input of data to the processor, the 8080 
	// generates a DBIN signal which should be used externally to 
	// enable the transfer.Machine cycles in which DB IN is available 
	// include: FETCH, MEMORY READ, STACK READ, and INTERRUPT.
	// DBIN is initiated by the rising edge of phi2 during state T2 
	// and terminated by the corresponding edge of phi2 during T3.
	if ((cycleType == MachineCycleType::InstructionFetch ||
		cycleType == MachineCycleType::MemoryRead ||
		cycleType == MachineCycleType::StackRead ||
		cycleType == MachineCycleType::InterruptAck ||
		cycleType == MachineCycleType::InterruptAckWhileHalt) &&
		(m_StateTW || (isT2 && m_StateRun))) {
		oDBIn = 1;
	} else {
		oDBIn = 0;
	}

	// HLTA register
	if (m_uInstr->m_WriteEnableHLTA || (m_StateTWH && iInt && inte)) {
		m_HLTA = m_uInstr->m_WriteEnableHLTA ? m_uInstr->m_HLTA : 0;
	}

	// INTE register
	if (m_uInstr->m_WriteEnableINTE || (m_InternalINT && isT1)) {
		m_IntEn = m_uInstr->m_WriteEnableINTE ? m_uInstr->m_INTE : 0;
	}

	// S flag
	if (m_uInstr->m_WriteEnableFlagsS) {
		m_FlagsS = m_uInstr->m_FlagsSSrc == FlagsSSrc::ALU ? getALU_S(alu) : ((idb & 0x80) >> 7);
	}

	// Z flag
	if (m_uInstr->m_WriteEnableFlagsZ) {
		m_FlagsZ = m_uInstr->m_FlagsZSrc == FlagsZSrc::ALU ? getALU_Z(alu) : ((idb & 0x40) >> 6);
	}

	// AC flag
	if (m_uInstr->m_WriteEnableFlagsAC) {
		m_FlagsAC = m_uInstr->m_FlagsACSrc == FlagsACSrc::ALU ? aluAC : ((idb & 0x10) >> 4);
	}

	// P flag
	if (m_uInstr->m_WriteEnableFlagsP) {
		m_FlagsP = m_uInstr->m_FlagsPSrc == FlagsPSrc::ALU ? getALU_P(alu) : ((idb & 0x04) >> 2);
	}

	// C flag
	if (m_uInstr->m_WriteEnableFlagsC) {
		m_FlagsC = m_uInstr->m_FlagsCSrc == FlagsCSrc::ALU ? aluC :
			(m_uInstr->m_FlagsCSrc == FlagsCSrc::One ? 1 :
			(m_uInstr->m_FlagsCSrc == FlagsCSrc::NOT_Self ? ~m_FlagsC : (idb & 0x01)));
	}

	// ACC
	if (m_uInstr->m_WriteEnableACC) {
		m_ACC = m_uInstr->m_ACCSrc == ACCSrc::NOT_Self ? ~acc : idb;
	}

	// ACT
	if (m_uInstr->m_WriteEnableACT) {
		m_ACT = m_uInstr->m_ACTSrc == ACTSrc::ACC ? acc : idb;
	}

	// TMP
	if (m_uInstr->m_WriteEnableTMP) {
		m_TMP = idb;
	}

	// Register file
	if (m_uInstr->m_WriteEnableRFReg) {
		writeRegister((RegisterFileDstReg::Enum)dstReg, idb);
	}

	if (m_uInstr->m_WriteEnableRFRegPair) {
		uint16_t rp = ((uint16_t)regPairLo) | (uint16_t)(regPairHi << 8);
		if (m_uInstr->m_RFRegPairOp == RegisterFileRegPairOp::Inc) {
			rp++;
		} else if (m_uInstr->m_RFRegPairOp == RegisterFileRegPairOp::Dec) {
			rp--;
		}

		writeRegisterPair((RegisterFileDstRegPair::Enum)m_uInstr->m_RFDstRegPair, rp);
	}

	if (m_uInstr->m_RFSwapHLDE) {
		m_SwapHLDE ^= 1;
	}
}

void i8080::onPhi2_fallingEdge()
{
}

void i8080::writeRegister(RegisterFileDstReg::Enum reg, uint8_t value)
{
	if (m_SwapHLDE) {
		if (reg == RegisterFileDstReg::D) {
			reg = RegisterFileDstReg::H;
		} else if (reg == RegisterFileDstReg::E) {
			reg = RegisterFileDstReg::L;
		} else if (reg == RegisterFileDstReg::H) {
			reg = RegisterFileDstReg::D;
		} else if (reg == RegisterFileDstReg::L) {
			reg = RegisterFileDstReg::E;
		}
	}

	if (reg == RegisterFileDstReg::W_and_Z) {
		m_Registers[RegisterFileDstReg::W] = value;
		m_Registers[RegisterFileDstReg::Z] = value;
	} else {
		m_Registers[reg] = value;
	}
}

void i8080::writeRegisterPair(RegisterFileDstRegPair::Enum rp, uint16_t value)
{
	if (m_SwapHLDE) {
		if (rp == RegisterFileDstRegPair::DE) {
			rp = RegisterFileDstRegPair::HL;
		} else if (rp == RegisterFileDstRegPair::HL) {
			rp = RegisterFileDstRegPair::DE;
		}
	}

	if (rp == RegisterFileDstRegPair::BC) {
		m_Registers[RegisterFileDstReg::B] = (value & 0xFF00) >> 8;
		m_Registers[RegisterFileDstReg::C] = (value & 0x00FF);
	} else if (rp == RegisterFileDstRegPair::DE) {
		m_Registers[RegisterFileDstReg::D] = (value & 0xFF00) >> 8;
		m_Registers[RegisterFileDstReg::E] = (value & 0x00FF);
	} else if (rp == RegisterFileDstRegPair::HL) {
		m_Registers[RegisterFileDstReg::H] = (value & 0xFF00) >> 8;
		m_Registers[RegisterFileDstReg::L] = (value & 0x00FF);
	} else if (rp == RegisterFileDstRegPair::WZ) {
		m_Registers[RegisterFileDstReg::W] = (value & 0xFF00) >> 8;
		m_Registers[RegisterFileDstReg::Z] = (value & 0x00FF);
	} else if (rp == RegisterFileDstRegPair::PC) {
		m_Registers[RegisterFileDstReg::PCH] = (value & 0xFF00) >> 8;
		m_Registers[RegisterFileDstReg::PCL] = (value & 0x00FF);
	} else if (rp == RegisterFileDstRegPair::SP) {
		m_Registers[RegisterFileDstReg::SPH] = (value & 0xFF00) >> 8;
		m_Registers[RegisterFileDstReg::SPL] = (value & 0x00FF);
	}
}

uint8_t i8080::getFlags()
{
	return 0x02 | m_FlagsC | (m_FlagsP << 2) | (m_FlagsAC << 4) | (m_FlagsZ << 6) | (m_FlagsS << 7);
}

uint8_t i8080::getALU_S(uint8_t alu)
{
	return (alu & 0x80) >> 7;
}

uint8_t i8080::getALU_Z(uint8_t alu)
{
	return alu == 0 ? 1 : 0;
}

uint8_t i8080::getALU_P(uint8_t alu)
{
	uint32_t cnt = 0;
	for (uint8_t i = 0; i < 8; ++i) {
		cnt += (alu & (1 << i)) ? 1 : 0;
	}

	return 1 - (cnt & 1);
}

uint8_t i8080::getALUResult(uint8_t& cOut, uint8_t& ac)
{
	uint8_t A = m_uInstr->m_ALUASrc == ALUASrc::ACT ? m_ACT : m_TMP;
	uint8_t B = m_uInstr->m_ALUBSrc == ALUBSrc::TMP ? m_TMP : 0;
	uint8_t Cin = m_uInstr->m_ALUCSrc == ALUCSrc::Zero ? 0 :
		(m_uInstr->m_ALUCSrc == ALUCSrc::One ? 1 :
		(m_uInstr->m_ALUCSrc == ALUCSrc::Flags ? m_FlagsC :
		(m_uInstr->m_ALUCSrc == ALUCSrc::ACC_0 ? (m_ACC & 1) : ((m_ACC >> 7) & 1))));

	switch (m_uInstr->m_ALUOp) {
	case ALUOp::AND:
		cOut = 0;
		ac = 0;
		return A & B;
	case ALUOp::OR:
		cOut = 0;
		ac = 0;
		return A | B;
	case ALUOp::XOR:
		cOut = 0;
		ac = 0;
		return A ^ B;
	case ALUOp::Add:
	{
		uint32_t res = A + B + Cin;
		cOut = (res & 0x100) >> 8;

		// https://github.com/thibaultimbert/Intel8080/blob/master/8080.js#L924
		ac = ((A ^ B ^ res) & 0x10) >> 4;

		return (uint8_t)res;
	}
	case ALUOp::Sub:
	{
		uint32_t res = A - B - Cin;
		cOut = (res & 0x100) >> 8;

		// https://github.com/thibaultimbert/Intel8080/blob/master/8080.js#L924
		ac = ~(((A ^ B ^ res) & 0x10) >> 4) & 1;

		return (uint8_t)res;
	}
	case ALUOp::RotateLeft:
		cOut = (A & 0x80) >> 7;
		ac = (A & 0x08) >> 3;
		return (Cin | (A << 1));
	case ALUOp::RotateRight:
		cOut = (A & 0x01);
		ac = (A & 0x08) >> 3;
		return (Cin << 7) | ((A >> 1) & 0x7F);
	case ALUOp::DecimalAdjust:
		uint32_t res = A;
		if ((res & 0x0F) > 9 || m_FlagsAC == 1) {
			res += 6;
			ac = 1;
		} else {
			ac = 0;
		}

		if (res > 0x9F || Cin == 1) {
			res += 0x60;
			cOut = 1;
		} else {
			cOut = 0;
		}

		return (uint8_t)res;
	}

	// Shouldn't land here
	assert(false);

	return 0;
}

uint16_t i8080::getRegisterPairValue(RegisterFileSrcRegPair::Enum rp)
{
	if (m_SwapHLDE) {
		if (rp == RegisterFileSrcRegPair::DE) {
			rp = RegisterFileSrcRegPair::HL;
		} else if (rp == RegisterFileSrcRegPair::HL) {
			rp = RegisterFileSrcRegPair::DE;
		}
	}

	switch (rp) {
	case RegisterFileSrcRegPair::BC:
		return (uint16_t)m_Registers[RegisterFileSrcReg::C] | (((uint16_t)m_Registers[RegisterFileSrcReg::B]) << 8);
	case RegisterFileSrcRegPair::DE:
		return (uint16_t)m_Registers[RegisterFileSrcReg::E] | (((uint16_t)m_Registers[RegisterFileSrcReg::D]) << 8);
	case RegisterFileSrcRegPair::HL:
		return (uint16_t)m_Registers[RegisterFileSrcReg::L] | (((uint16_t)m_Registers[RegisterFileSrcReg::H]) << 8);
	case RegisterFileSrcRegPair::WZ:
		return (uint16_t)m_Registers[RegisterFileSrcReg::Z] | (((uint16_t)m_Registers[RegisterFileSrcReg::W]) << 8);
	case RegisterFileSrcRegPair::SP:
		return (uint16_t)m_Registers[RegisterFileSrcReg::SPL] | (((uint16_t)m_Registers[RegisterFileSrcReg::SPH]) << 8);
	case RegisterFileSrcRegPair::PC:
		return (uint16_t)m_Registers[RegisterFileSrcReg::PCL] | (((uint16_t)m_Registers[RegisterFileSrcReg::PCH]) << 8);
	}

	assert(false);

	return 0;
}

uint8_t i8080::getRegisterPairHigh(RegisterFileSrcRegPair::Enum rp)
{
	if (m_SwapHLDE) {
		if (rp == RegisterFileSrcRegPair::DE) {
			rp = RegisterFileSrcRegPair::HL;
		} else if (rp == RegisterFileSrcRegPair::HL) {
			rp = RegisterFileSrcRegPair::DE;
		}
	}

	if (rp == RegisterFileSrcRegPair::BC) {
		return m_Registers[RegisterFileSrcReg::B];
	} else if (rp == RegisterFileSrcRegPair::DE) {
		return m_Registers[RegisterFileSrcReg::D];
	} else if (rp == RegisterFileSrcRegPair::HL) {
		return m_Registers[RegisterFileSrcReg::H];
	} else if (rp == RegisterFileSrcRegPair::WZ) {
		return m_Registers[RegisterFileSrcReg::W];
	} else if (rp == RegisterFileSrcRegPair::PC) {
		return m_Registers[RegisterFileSrcReg::PCH];
	} else if (rp == RegisterFileSrcRegPair::SP) {
		return m_Registers[RegisterFileSrcReg::SPH];
	}

	assert(false);
	return 0;
}

uint8_t i8080::getRegisterPairLow(RegisterFileSrcRegPair::Enum rp)
{
	if (m_SwapHLDE) {
		if (rp == RegisterFileSrcRegPair::DE) {
			rp = RegisterFileSrcRegPair::HL;
		} else if (rp == RegisterFileSrcRegPair::HL) {
			rp = RegisterFileSrcRegPair::DE;
		}
	}

	if (rp == RegisterFileSrcRegPair::BC) {
		return m_Registers[RegisterFileSrcReg::C];
	} else if (rp == RegisterFileSrcRegPair::DE) {
		return m_Registers[RegisterFileSrcReg::E];
	} else if (rp == RegisterFileSrcRegPair::HL) {
		return m_Registers[RegisterFileSrcReg::L];
	} else if (rp == RegisterFileSrcRegPair::WZ) {
		return m_Registers[RegisterFileSrcReg::Z];
	} else if (rp == RegisterFileSrcRegPair::PC) {
		return m_Registers[RegisterFileSrcReg::PCL];
	} else if (rp == RegisterFileSrcRegPair::SP) {
		return m_Registers[RegisterFileSrcReg::SPL];
	}

	assert(false);
	return 0;
}

uint8_t i8080::getRegisterFileOutput()
{
	RegisterFileSrcReg::Enum reg = (RegisterFileSrcReg::Enum)m_uInstr->m_RFSrcReg;
	if (m_SwapHLDE) {
		if (reg == RegisterFileSrcReg::D) {
			reg = RegisterFileSrcReg::H;
		} else if (reg == RegisterFileSrcReg::E) {
			reg = RegisterFileSrcReg::L;
		} else if (reg == RegisterFileSrcReg::H) {
			reg = RegisterFileSrcReg::D;
		} else if (reg == RegisterFileSrcReg::L) {
			reg = RegisterFileSrcReg::E;
		}
	}

	return m_Registers[reg];
}

uint8_t i8080::getInternalDataBusValue()
{
	switch (m_uInstr->m_InternalDataBusSrc) {
	case InternalDataBusSrc::ACC:
		return m_ACC;
	case InternalDataBusSrc::TMP:
		return m_TMP;
	case InternalDataBusSrc::Flags:
		return getFlags();
	case InternalDataBusSrc::ALU:
	{
		uint8_t c, ac;
		return getALUResult(c, ac);
	}
	case InternalDataBusSrc::RegisterFile:
		return getRegisterFileOutput();
	case InternalDataBusSrc::DataIn:
		return iDin;
	case InternalDataBusSrc::StatusWord:
		return g_StatusWords[m_uInstr->m_MachineCycleType];
	}

	// Shouldn't land here.
	assert(false);

	return 0;
}
#endif // I8080_IMPLEMENTATION

#endif
