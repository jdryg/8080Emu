#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <memory.h>

#define TRACE_STATE 0
#define TRACE_INSTRUCTIONS 0
#define EXERCISER 1
#define I8080_IMPLEMENTATION
#include "i8080.h"

#define TRACE_RAM_READS 0
#define TRACE_RAM_WRITES 0
#define REAL_CLOCK 0

uint8_t* loadBinaryFile(const char* filename, uint32_t& fileSize);

int main()
{
	printf("Initializing i8080...\n");
	i8080 cpu;
	if (!cpu.init()) {
		printf("(x) Failed to initialize CPU\n");
		return -1;
	}

	printf("Allocating 64k RAM...\n");
	uint8_t* ram = (uint8_t*)malloc(65536);
	if (!ram) {
		printf("(x) Failed to allocate 64k RAM buffer\n");
		return -1;
	}

#if EXERCISER
	printf("Loading 8080 Exerciser at address 0x0100...\n");
	uint32_t bootROMSize = 0;
	uint8_t* bootROM = loadBinaryFile("8080EXER.COM", bootROMSize);
//	uint8_t* bootROM = loadBinaryFile("cpudiag.bin", bootROMSize);

	if (!bootROM) {
		printf("(x) Failed to load test program.\n");
		return -1;
	}

	memset(ram, 0, 65536);

	// Copy the program at address 0x0100
	memcpy(&ram[0x0100], bootROM, bootROMSize);

	// Set 16-bit word at address 0x0006 to 0xF000. This is used as the stack pointer (see instr @ addr 0x0113 and 0x0116)
	ram[0x0006] = 0x00;
	ram[0x0007] = 0xF0;
#endif

	printf("Entering main loop...\n");

	cpu.iReset = 1;
	cpu.iDin = 0;
	cpu.iHold = 0;
	cpu.iInt = 0;
	cpu.iReady = 1;

	uint8_t prevSync = 0;

#if REAL_CLOCK
	uint8_t mod9Counter = 0xFF;
#endif

#if EXERCISER
	uint8_t statusWord;
	bool firstInstructionFetch = true;
	bool fixPC = false;
#endif

	while (true) {
#if REAL_CLOCK
		mod9Counter = ((uint8_t)(mod9Counter + 1)) % 9;
		if (mod9Counter == 0 || mod9Counter == 1) {
			cpu.iPhi1 = 1;
			cpu.iPhi2 = 0;
		} else if (mod9Counter >= 2 && mod9Counter < 7) {
			cpu.iPhi1 = 0;
			cpu.iPhi2 = 1;
		} else {
			cpu.iPhi1 = 0;
			cpu.iPhi2 = 0;
		}
#else
		if (cpu.iPhi1 == 0 && cpu.iPhi2 == 0) {
			cpu.iPhi1 = 1;
		} else if (cpu.iPhi1 == 1 && cpu.iPhi2 == 0) {
			cpu.iPhi1 = 0;
			cpu.iPhi2 = 1;
		} else if (cpu.iPhi1 == 0 && cpu.iPhi2 == 1) {
			cpu.iPhi2 = 0;
		}
		assert(cpu.iPhi1 == 0 || cpu.iPhi2 != cpu.iPhi1);
#endif

		cpu.tick();

		if (prevSync == 0 && cpu.oSync == 1) {
			// Machine cycle starts
#if EXERCISER
			statusWord = cpu.oDout;

			if (firstInstructionFetch) {
				// If this is the first instruction to be fetched set PC to 0x0100
				cpu.setPC(0x0100);
				cpu.setSP(0xF000);
				cpu.oAddrLow = 0x00;
				cpu.oAddrHigh = 0x01;

				firstInstructionFetch = false;
			}

			// Sync == 1 => T1.phi2 rising edge.
			if (statusWord == g_StatusWords[MachineCycleType::InstructionFetch]) {
				// Instruction Fetch T1.phi2_re
				if (cpu.oAddrLow == 0x05 && cpu.oAddrHigh == 0x00) {
					// Fetch the first byte of the function at address 0x0005 (CALL 5)
					cpu.cpm_bdos(ram);

					fixPC = true;
				} else if (cpu.oAddrLow == 0x00 && cpu.oAddrHigh == 0x00) {
					// Done.
					break;
				}
			}
#endif
		}
		prevSync = cpu.oSync;

		if (cpu.oDBIn) {
#if EXERCISER
			if (fixPC) {
				uint16_t sp = cpu.getSP();
				uint8_t pcl = ram[sp];
				uint8_t pch = ram[sp + 1];

				uint16_t newPC = (uint16_t)pcl | ((uint16_t)pch << 8);

				// NOTE: Set PC to return address + 1 because at this point we are already in T2 
				// and the PC has been increased (it's 1 greater than the address on the address bus).
				cpu.setPC(newPC + 1);
				cpu.setSP(sp + 2);
				cpu.oAddrLow = pcl;
				cpu.oAddrHigh = pch;

				fixPC = false;
			}
#endif

			// Memory read
			uint16_t addr = cpu.oAddrLow | (cpu.oAddrHigh << 8);
			cpu.iDin = ram[addr];
			cpu.iReady = 1;

#if TRACE_RAM_READS
			if (cpu.iPhi1 == 0 && cpu.iPhi2 == 0) {
				printf("Read RAM[%04X] = 0x%02X\n", addr, cpu.iDin);
			}
#endif
		}

		if (!cpu.iReset && !cpu.oWRb && !cpu.oDBIn && !cpu.oSync) {
			uint16_t addr = cpu.oAddrLow | (cpu.oAddrHigh << 8);
			ram[addr] = cpu.oDout;
			cpu.iReady = 1;

#if TRACE_RAM_WRITES
			printf("Write RAM[%04X] = 0x%02X\n", addr, cpu.oDout);
#endif
		}

		if (cpu.iReset && cpu.iPhi1 == 0 && cpu.iPhi2 == 0) {
			cpu.iReset = 0;
			printf("iReset = 0\n");
		}
	}

	printf("\nDone\n");

	free(ram);

	return 0;
}

uint8_t* loadBinaryFile(const char* filename, uint32_t& fileSize)
{
	FILE* f = fopen(filename, "rb");
	if (!f) {
		return nullptr;
	}

	fseek(f, 0, SEEK_END);
	fileSize = (uint32_t)ftell(f);
	fseek(f, 0, SEEK_SET);

	uint8_t* buffer = (uint8_t*)malloc(sizeof(uint8_t) * fileSize);

	fread(buffer, fileSize, 1, f);

	fclose(f);

	return buffer;
}
