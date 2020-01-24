#ifndef OPCODEMANAGER_H
#define OPCODEMANAGER_H

#include "ardunix.h"
#include "CPU.h"
#include "File.h"
#include "AbstractEventClass.h"

class VirtualMachine; // predefine, only to make system calls with its pointer to the kernel

/* this class manages all commands that are read from ELF executable files
 * in addition there is no need to increment clocks ('CKS'), because there
 * is no debugger (which uses clocks to calculate time between opcode execution
 * the biggest part of this class contains alternated source-code of the
 * open-source project 'simulAVR' */
class OpcodeManager
{
	public:
		enum OpcodeOperands
		{
       // source & destination
			oo_Rd2b = 0x0030, // 2 bit - R24, R26, R28, R30
			oo_Rd3b = 0x0070, // 3 bit - R16 - R23
			oo_Rd4b = 0x00f0, // 4 bit - R16 - R31
			oo_Rd5b = 0x01f0, // 5 bit - R00 - R31

       // source only
			oo_Rr3b = 0x0007, // 3 bit - R16 - R23
			oo_Rr4b = 0x000f, // 4 bit - R16 - R31
			oo_Rr5b = 0x020f, // 5 bit - R00 - R31

       // constant data
			oo_K8b = 0x0F0F, // 8 bit
			oo_K6b = 0x00CF, // 6 bit

       // constant address
			oo_k7b  = 0x03F8, // 7 bit
			oo_k12b = 0x0FFF, // 12 bit
			oo_k22b = 0x01F1, // 22 bit

			// specials
			oo_regBit  = 0x0007, // 1 bit of a register
			oo_sregBit = 0x0070, // 1 bit in status register
			oo_q6b     = 0x2C07, // 6 bit - displacement for direct address

			oo_A5b = 0x00F8, // 5 bit - IO Registers R00 - R31
			oo_A6b = 0x060F, // 6 bit - IO Registers (Port ID?)
		};

		CPU* cpu;
		File* file;
		VirtualMachine* parentVM;

		#define IO_REG_ADDR_BEGIN 128 //0x20 // start sddress of IO register in sram(@ ubyte1 32)
		#define IO_REG_ADDR_END   IO_REG_ADDR_BEGIN + 64  //0x60 // 20h - 60h = 64 BYTES!!
    
    #define k12b(opcode)    (opcode & OpcodeManager::oo_k12b)
    #define Rd5b(opcode)    (((opcode & OpcodeManager::oo_Rd5b) >> 4) & 0x1f)
    #define Rr5b(opcode)    (((opcode & OpcodeManager::oo_Rr5b) & 0xf) + (((opcode & OpcodeManager::oo_Rr5b) >> 5) & 0x10))
    #define A6b(opcode)     ((((opcode & OpcodeManager::oo_A6b) >> 5) & 0x0030) + ((opcode & OpcodeManager::oo_A6b) & 0xf))
    #define Rd4b(opcode)    ((((opcode & OpcodeManager::oo_Rd4b) >> 4) & 0xf ) + 16)
    #define K8b(opcode)     ((((opcode & OpcodeManager::oo_K8b ) >> 4) & 0xf0) + ((opcode & OpcodeManager::oo_K8b) & 0xf)) // oder 0xf ausgeklammert??
    #define Rd2b(opcode)    (((((opcode & OpcodeManager::oo_Rd2b) >> 4) & 0x3) * 2) + 24)
    #define Rd3b(opcode)    ((((opcode & OpcodeManager::oo_Rd3b) >> 4) & 0x7) + 16)
    #define Rr3b(opcode)    ((opcode & OpcodeManager::oo_Rr3b) + 16)
    #define Rr4b(opcode)    ((opcode & OpcodeManager::oo_Rr4b) + 16)
    #define K6b(opcode)     ((((opcode & OpcodeManager::oo_K6b) >> 2) & 0x0030) + ((opcode & OpcodeManager::oo_K6b) & 0xf))
    #define K7b(opcode)     ((((opcode & OpcodeManager::oo_k7b) >> 3) & 0x7f))
    #define K22b(opcode)    ((((opcode & OpcodeManager::oo_k22b) >> 3) & 0x003e) + ((opcode & OpcodeManager::oo_k22b) & 0x1))
    #define A5b(opcode)     ((opcode & OpcodeManager::oo_A5b) >> 3)
    #define q6b(opcode)     (((((((opcode & OpcodeManager::oo_q6b) >> 1) & 0x1000) + ((opcode & OpcodeManager::oo_q6b) & 0x0c00)) >> 7) & 0x0038) + ((opcode & OpcodeManager::oo_q6b) & 0x7)) // TODO: reuse some vals
    #define sregBit(opcode) ((opcode & OpcodeManager::oo_sregBit) >> 4)
    
    
    #warning IF WORKING WITH VAR "opcode" THEN USE UBYTE2 WHEN CALCULATING!
    
    #warning test with ubyte2
    
    #define addCarry(res, rd, rr, b) (((rd >> b & 0x1) & (rr >> b & 0x1)) |((rr >> b & 0x1) & ~(res >> b & 0x1)) |(~(res >> b & 0x1) & (rd >> b & 0x1)))
    #define addOverflow(res, rd, rr) (((rd >> 7 & 0x1) & (rr >> 7 & 0x1) & ~(res >> 7 & 0x1)) | (~(rd >> 7 & 0x1) & ~(rr >> 7 & 0x1) & (res >> 7 & 0x1)))
    #define compareCarry(res, rd, rr, b) ((~(rd >> b & 0x1) & (rr >> b & 0x1)) | ((rr >> b & 0x1) & (res >> b & 0x1)) | ((res >> b & 0x1) & ~(rd >> b & 0x1)))
    #define compareOverflow(res, rd, rr) (((rd >> 7 & 0x1) & ~(rr >> 7 & 0x1) & ~(res >> 7 & 0x1)) | (~(rd >> 7 & 0x1) & (rr >> 7 & 0x1) & (res >> 7 & 0x1)))
    #define subCarry(res, rd, rr, b) ((~(rd >> b & 0x1) & (rr >> b & 0x1)) | ((rr >> b & 0x1) & (res >> b & 0x1)) | ((res >> b & 0x1) & ~(rd >> b & 0x1)))
    #define subOverflow(res, rd, rr) (((rd >> 7 & 0x1) & ~(rr >> 7 & 0x1) & ~(res >> 7 & 0x1)) | (~(rd >> 7 & 0x1) & (rr >> 7 & 0x1) & (res >> 7 & 0x1)))
    
    
    
    
    #define setBitInSreg(x)   ((sreg &~ (1 << SREG_##x)) | ((x != 0) << SREG_##x))
    #define regBit(opcode)    (opcode & OpcodeManager::oo_regBit)
    #define sregGetBit(bit)   ((cpu->sreg & (1 << bit)) != 0) // FIXME: worx?
    #define sregSetBit(bit)   (this->cpu->sreg |= 1 << bit) // eigentlich "sregGetFlag"
    #define sregUnsetBit(bit) (this->cpu->sreg &= ~(1 << bit))

		OpcodeManager(VirtualMachine* _parentVM, CPU* _cpu, File* _file);

    ubyte2 unsignedBitToSignedBit(ubyte2 value, ubyte2 bit);
    
		void opcodeSEI();

		void opcodeSTS(ubyte2 Rd);
		void opcodeLDS(ubyte2 Rd);
		void opcodeRJMP(ubyte2 k);
		void opcodeEOR(ubyte2 Rd, ubyte2 Rr);
		void opcodeIN(ubyte2 Rd, ubyte2 A);
		void opcodeOUT(ubyte2 Rd, ubyte2 A);
		void opcodeLDI(ubyte1 Rd, ubyte1 K);
		void STACK_PUSH(ubyte2 dataSize, ubyte2 value);
		converter STACK_POP(ubyte2 dataSize);
		bool STACK_CHECK(ubyte2 dataSize);
		void opcodeRCALL(ubyte2 k);
		void opcodePUSH(ubyte2 Rd);
		void opcodePOP(ubyte2 Rd);
		void opcodeRET();
		void opcodeNOP();
		void opcodeADD(ubyte2 Rd, ubyte2 Rr);
		void opcodeADC(ubyte2 Rd, ubyte2 Rr);
		void opcodeADIW(ubyte2 Rd, ubyte2 K);
		void opcodeAND(ubyte2 Rd, ubyte2 Rr);
		void opcodeANDI(ubyte2 Rd, ubyte2 K);
		void opcodeASR(ubyte2 Rd);
		void opcodeBCLR(ubyte2 Rd);
		void opcodeBLD(ubyte2 Rd, ubyte2 K);
		void opcodeBSET(ubyte2 K);
		void opcodeBST(ubyte2 Rd, ubyte2 K);
		void opcodeCOM(ubyte2 Rd);
		void opcodeCP(ubyte2 Rd, ubyte2 Rr);
		void opcodeCPC(ubyte2 Rd, ubyte2 Rr);
		void opcodeCPI(ubyte2 Rd, ubyte2 K);
		void opcodeDEC(ubyte2 Rd);
		void opcodeFMUL(ubyte2 Rd, ubyte2 Rr);
		void opcodeFMULS(ubyte2 Rd, ubyte2 Rr);
		void opcodeFMULSU(ubyte2 Rd, ubyte2 Rr);
		void opcodeINC(ubyte2 Rd);
		void opcodeBRBC(ubyte2 sregBit, ubyte2 k);
		void opcodeBRBS(ubyte2 bit, ubyte2 k);
		void opcodeBRCC(ubyte2 k);
		void opcodeBRCS(ubyte2 k);
		void opcodeBRHC(ubyte2 k);
		void opcodeBRHS(ubyte2 k);
		void opcodeBRTC(ubyte2 k);
		void opcodeBRTS(ubyte2 k);
		void opcodeBRVC(ubyte2 k);
		void opcodeBRVS(ubyte2 k);
		void opcodeBREQ(ubyte2 k);
		void opcodeBRGE(ubyte2 k);
		void opcodeBRID(ubyte2 k);
		void opcodeBRIE(ubyte2 k);
		void opcodeBRLO(ubyte2 k);
		void opcodeBRLT(ubyte2 k);
		void opcodeBRMI(ubyte2 k);
		void opcodeBRNE(ubyte2 k);
		void opcodeBRPL(ubyte2 k);
		void opcodeBRSH(ubyte2 k);
		void opcodeCLC();
		void opcodeCLH();
		void opcodeCLI();
		void opcodeCLN();
		void opcodeCLS();
		void opcodeCLT();
		void opcodeCLV();
		void opcodeCLZ();
		void opcodeSEC();
		void opcodeSEH();
		void opcodeSEN();
		void opcodeSES();
		void opcodeSET();
		void opcodeSEV();
		void opcodeSEZ();
		void opcodeELPM_Z_inc(ubyte2 arg1);
		void opcodeICALL();
		void opcodeIJMP();
		void opcodeLDD_Y(ubyte2 Rd, ubyte2 q);
		void opcodeLDD_Z(ubyte2 Rd, ubyte2 q);
		void opcodeLSR(ubyte2 Rd);
		void opcodeMOV(ubyte2 Rd, ubyte2 Rr);
		void opcodeMOVW(ubyte2 Rd, ubyte2 Rr);
		void opcodeMUL(ubyte2 Rd, ubyte2 Rr);
		void opcodeMULS(ubyte2 Rd, ubyte2 Rr);
		void opcodeMULSU(ubyte2 Rd, ubyte2 Rr);
		void opcodeNEG(ubyte2 Rd);
		void opcodeOR(ubyte2 Rd, ubyte2 Rr);
		void opcodeORI(ubyte2 Rd, ubyte2 k);
		void opcodeRETI();
		void opcodeROR(ubyte2 arg1);
		void opcodeSBC(ubyte2 Rd, ubyte2 Rr);
		void opcodeSBCI(ubyte2 Rd, ubyte2 k);
		void opcodeSBIW(ubyte2 Rd, ubyte2 k);
		void opcodeSUB(ubyte2 Rd, ubyte2 Rr);
		void opcodeSUBI(ubyte2 Rd, ubyte2 k);
		void opcodeSWAP(ubyte2 Rd);
		void opcodeCALL(ubyte2 k);
		void opcodeJMP(ubyte2 arg1); // UBYTE 2 !!
		void opcodeLPM_Z_inc(ubyte2 Rd);

		void opcodeSTD_X(ubyte2 arg1, ubyte2 arg2);
		void opcodeSTD_Y(ubyte2 Rd, ubyte2 q);
		void opcodeSTD_Z(ubyte2 q, ubyte2 Rd);

		void opcodeST_X(ubyte2 Rd);

		void opcodeST_X_dec(ubyte2 arg1);
		void opcodeST_X_inc(ubyte2 Rd);

		void opcodeST_Z_dec(ubyte2 Rd);
		void opcodeST_Z_inc(ubyte2 Rd);
};

#endif // OPCODEMANAGER_H
