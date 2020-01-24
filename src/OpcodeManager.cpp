#include "OpcodeManager.h"
#include "CPU.h"
#include "ELFile.h"
#include "API.h"


// + 3 for api-array after sram-array !!!!
#ifdef DEBUG
#define sram_warning if (sram_pos > sizeof(cpu->sram) + 3){	Serial.print(F(":  sram error !! > ")); Serial.print(sram_pos);	Serial.print(F(" > "));	Serial.print(sizeof(cpu->sram);} void(0)
#else
#define sram_warning void(0)
#endif

#define DEBUG

#ifdef DEBUG
  #define DEBUG_HEAD(func_name) Serial.println(F(#func_name))
#else
 #define DEBUG_HEAD(func_name) void(0)
#endif




OpcodeManager::OpcodeManager(VirtualMachine* _parentVM, CPU* _cpu, File* _file)
{
	parentVM = _parentVM;
	cpu = _cpu;
	file = _file;
}




ubyte2 OpcodeManager::unsignedBitToSignedBit(ubyte2 value, ubyte2 bit)
{
	if (!(value & (1 << (bit - 1))))
		return value;

	// 2's complement
	ubyte2 temp = (1 << bit) - 1;
	return -1 * ((~value & temp) + 1);
}



void OpcodeManager::opcodeSEI() // SEI was raped, executes api function
{
  DEBUG_HEAD(SEI);
  
	//eventController.fireEvent(ardunix::events::etAPI, (void*)cpu); opcode manager needed, for stack psuh/pop in API class

	// calls in virtual machine

	cpu->incPC();
}

void OpcodeManager::opcodeRJMP(ubyte2 k)
{
  DEBUG_HEAD(RJMP);
#warning k*2 ! anywhere else needed?
	cpu->PC = cpu->PC + (k*2) + 2; // +2 because opcode = short, not byte !
}

void OpcodeManager::opcodeEOR(ubyte2 Rd, ubyte2 Rr)
{
  DEBUG_HEAD(EOR);
  
	ubyte2 oldRd = cpu->gpwr[Rd];
	ubyte2 oldRr = cpu->gpwr[Rr];

	ubyte2 xord = oldRd ^ oldRr;

	ubyte2 sreg = cpu->sreg;
  
  ubyte2 V = 0;
  ubyte2 N = ((xord >> 7) & 0x1);
  ubyte2 S = (N ^ V);
  ubyte2 Z = ((xord & 0xff) == 0);

	sreg = setBitInSreg(V);
	sreg = setBitInSreg(N);
	sreg = setBitInSreg(S);
	sreg = setBitInSreg(Z);

	cpu->sreg = sreg;
	cpu->gpwr[Rd] = xord;
	cpu->incPC();
}

void OpcodeManager::opcodeIN(ubyte2 Rd, ubyte2 A)
{
  DEBUG_HEAD(IN);
  
	ubyte2 sram_pos = A+IO_REG_ADDR_BEGIN;

	sram_warning;

	cpu->gpwr[Rd] = cpu->sram[sram_pos];
	cpu->incPC();
}

void OpcodeManager::opcodeLDI(ubyte1 Rd /* register to save to (e.g. "r24") */, ubyte1 K /* value to safe to that register */)
{
  DEBUG_HEAD(LDI);
  
	cpu->gpwr[Rd] = K;

#ifdef DEBUG
	Serial.print(": R"); Serial.print(Rd); Serial.print(" = "); Serial.print(K);
	Serial.print(F(":  Rd (")); Serial.print((int)Rd); Serial.print(F(")  K (")); Serial.print((int)K); Serial.print(F(")  "));
#endif

	cpu->incPC();
}

void OpcodeManager::STACK_PUSH(ubyte2 dataSize, ubyte2 value)
{
  DEBUG_HEAD(STACK_PUSH);
  
	converter con;

	//con.UBYTE2 = value; // to use smaller data types, we put the value ubyte2o the con verter

#ifdef DEBUG
	//Serial.print(F(":  Pushed value ")); Serial.print(con.UBYTE1[1]); Serial.print(F(" to stptr ")); Serial.print(cpu->stptr+0);
#endif

	// Stack Pointer @ r28 & r29
	// initialized with: r28 = 0xFF | r29 = 0x08
	// reihenfolge: r29 & r28
	// result: stack pointer = 2303
	// means: max ram size = 2303 - stack is currently at last position of ram, from then on downwards counting!

	// decrement stack pointer
	con.UBYTE1[0] = cpu->gpwr[29]; // 0x08
	con.UBYTE1[1] = cpu->gpwr[28]; // 0XFF
	// result: con => 0x08FF => 2303

	ubyte2 stackPointer = con.UBYTE2;
	con.UBYTE2 - 1; // decrement stack pointer after writing the variable, because at "stackPointer" position shall be written

	cpu->gpwr[29] = con.UBYTE1[0];
	cpu->gpwr[28] = con.UBYTE1[1];


	// save value to stack
	con.UBYTE2 = value;

#warning reihenfolge okay?
	cpu->sram[stackPointer-1] = con.UBYTE1[0];
	cpu->sram[stackPointer-0] = con.UBYTE1[1];



	/*cpu->stack[cpu->stptr+0] = con.UBYTE1[0]; // push low ubyte2 first(little endian, eh)
	if(dataSize == 2) // do we need to push a existing high value ?
		cpu->stack[cpu->stptr+1] = con.UBYTE1[1]; // push 2 values(value-high & value-low)

	cpu->stptr += dataSize;*/
}

bool OpcodeManager::STACK_CHECK(ubyte2 dataSize) // check if stptr - dataSize points at a valid position (at least bigger than 0, TODO: add more checks)
{
  DEBUG_HEAD(STACK_CHECK);
  
#warning unused
	return false; // don't care, stack is full, no program exit


	/*if ((cpu->stptr - dataSize) > 0)
		return true;
	else
		return false;

	return false; // for security reasons*/
}

converter OpcodeManager::STACK_POP(ubyte2 dataSize) // do not return, put result ubyte2 in "converter" variable
{
  DEBUG_HEAD(STACK_POP);
  
	converter con;




	// increment stack pointer
	con.UBYTE1[0] = cpu->gpwr[29]; // 0x08
	con.UBYTE1[1] = cpu->gpwr[28]; // 0XFF
	// result: con => 0x08FF => 2303

	ubyte2 stackPointer = con.UBYTE2;
	con.UBYTE2 + 1; // increment stack pointer

	cpu->gpwr[29] = con.UBYTE1[0];
	cpu->gpwr[28] = con.UBYTE1[1];


	// get value from stack
#warning reihenfolge okay?
	con.UBYTE1[0] = cpu->sram[stackPointer-1];
	con.UBYTE1[1] = cpu->sram[stackPointer-0];

	return con;







	/*con.UBYTE1[1] = cpu->stack[cpu->stptr-dataSize]; // WARNING! substrate before using this AND reading it BACKWARDS

	if(dataSize == 2)
		con.UBYTE1[0] = cpu->stack[cpu->stptr-dataSize+1];

	cpu->stptr -= dataSize; // decrement. do NOT use -- on the both readings above..

	return con;*/
}

void OpcodeManager::opcodeLPM_Z_inc(ubyte2 Rd) // wtf we only need to read 1 byte from flash!
{
	ubyte2 Z = (cpu->gpwr[31] << 8) + cpu->gpwr[30];
	ubyte1 data = file->getByte(Z); //getWord(Z); // Z /2 !!!

#ifdef DEBUG
	Serial.print(F(": flash_pos = ")); Serial.print(Z); Serial.print(F("  data = ")); Serial.print(data);
#endif

  cpu->gpwr[Rd] = data; //(data & 0xFF);
	
	Z += 1; // z ptr increase
	cpu->gpwr[30] = (Z & 0xFF);
	cpu->gpwr[31] = (Z >> 8);

	cpu->incPC();
}

void OpcodeManager::opcodeRCALL(ubyte2 k)
{
  DEBUG_HEAD(RCALL);
  
	ubyte2 pc = cpu->PC;

	STACK_PUSH(2, pc + 2);

	cpu->PC = pc + k + 2;
}

void OpcodeManager::opcodePUSH(ubyte2 Rd)
{
  DEBUG_HEAD(PUSH);
  
#ifdef DEBUG
  if (Rd > 31)
  {
    Serial.print(F("Rd > 31 !"));
  }
  
	Serial.print(F(":  Pushing Rd "));
	Serial.print(Rd);
	Serial.print(" (");
	Serial.print(cpu->gpwr[Rd]);
	Serial.print(")");
#endif

	STACK_PUSH(1, cpu->gpwr[Rd]);

	cpu->incPC();
}

void OpcodeManager::opcodeSTD_Y(ubyte2 Rd, ubyte2 q)
{
  DEBUG_HEAD(STD_Y);
  
	ubyte2 Y = (cpu->gpwr[29] << 8) + cpu->gpwr[28];

#ifdef DEBUG
	Serial.print(F(": Y = "));
	Serial.print((int)Y);
	Serial.print(F("   q = "));
	Serial.print((int)q);
#endif

	ubyte2 sram_pos = Y + q;

	sram_warning;

	cpu->sram[sram_pos] = cpu->gpwr[Rd];
	cpu->incPC();
}

void OpcodeManager::opcodeSTD_Z (ubyte2 q, ubyte2 Rd)
{
  DEBUG_HEAD(STD_Z);
  
	ubyte2 Z = (cpu->gpwr[31] << 8) + cpu->gpwr[30];

	ubyte2 sram_pos = Z + q;

	sram_warning;

#ifdef DEBUG
	Serial.print(F(": Storing  sram["));
	Serial.print(sram_pos);
	Serial.print(F("] = Rd"));
	Serial.print(Rd);
	Serial.print(" (");
	Serial.print(cpu->gpwr[Rd]);
	Serial.print(")");
#endif

	cpu->sram[sram_pos] = cpu->gpwr[Rd];
	cpu->incPC();
}

void OpcodeManager::opcodeST_X(ubyte2 Rd)
{
  DEBUG_HEAD(ST_X);
  
	ubyte2 X = ((cpu->gpwr[27] << 8) + cpu->gpwr[26]); // ptr

	cpu->sram[X] = cpu->gpwr[Rd];
	cpu->incPC();
}

void OpcodeManager::opcodeST_X_inc(ubyte2 Rd)
{
  DEBUG_HEAD(STD_X_inc);
  
	ubyte2 X = (cpu->gpwr[27] << 8) + cpu->gpwr[26]; // ptr

	cpu->sram[X] = cpu->gpwr[Rd];

#ifdef DEBUG
	Serial.print(F(": sram["));
	Serial.print(X);
	Serial.print(F("] = Rd"));
	Serial.print(Rd);
	Serial.print(" (");
	Serial.print(cpu->gpwr[Rd]);
	Serial.print(")");
#endif

	X += 1; // x ptr increase

	cpu->gpwr[26] = (X & 0xFF);
	cpu->gpwr[27] = (X >> 8);
	cpu->incPC();
}

void OpcodeManager::opcodePOP(ubyte2 Rd)
{
  DEBUG_HEAD(POP);
  
	cpu->gpwr[Rd] = STACK_POP(1).UBYTE1[1]; // warning, this function alters the "converter" variable // converter.UBYTE1[1]; // now use value from "converter" variable
  cpu->incPC();
}

void OpcodeManager::opcodeRET() // has the power to call system call "exit" (etExitVM) if the stack is empty
{
  DEBUG_HEAD(RET);
  
	if (STACK_CHECK(2)) // check if stack pointer is valid, if not, it was probably a RET without the intent to go back to another function, but to close the program (last RET)
	{
		eventController.fireEvent(ardunix::events::etExitVM,(void*)parentVM);
		return; // TODO: exit vm
	}

	ubyte2 pc = STACK_POP(2).UBYTE2;
	cpu->PC = pc;
}

void OpcodeManager::opcodeNOP()
{
  DEBUG_HEAD(NOP);
  
  // do nothing, only increase pc
	cpu->incPC();
}

void OpcodeManager::opcodeADD(ubyte2 Rd, ubyte2 Rr)
{
  DEBUG_HEAD(ADD);
  
	ubyte2 RdVal = cpu->gpwr[Rd];
	ubyte2 RrVal = cpu->gpwr[Rr];

	ubyte2 added = RdVal + RrVal;

	ubyte2 sreg = cpu->sreg;
  
  ubyte2 H = addCarry(added, RdVal, RrVal, 3);
  ubyte2 V = addOverflow(added, RdVal, RrVal);
  ubyte2 N = ((added >> 7) & 0x1);
  ubyte2 S = (N ^ V);
  ubyte2 Z = ((added & 0xff) == 0);
  ubyte2 C = addCarry(added, RdVal, RrVal, 7);

	sreg = setBitInSreg(H);
	sreg = setBitInSreg(V); 
	sreg = setBitInSreg(N); 
	sreg = setBitInSreg(S);
	sreg = setBitInSreg(Z);
	sreg = setBitInSreg(C);

	cpu->sreg = sreg;
	cpu->gpwr[Rd] = added;
	cpu->incPC();
}

void OpcodeManager::opcodeADC(ubyte2 Rd, ubyte2 Rr)
{
  DEBUG_HEAD(ADC);
  
	ubyte2 RdVal = cpu->gpwr[Rd];
	ubyte2 RrVal = cpu->gpwr[Rr];

	ubyte2 added = RdVal + RrVal + sregGetBit(SREG_C);

	ubyte2 sreg = cpu->sreg;

  ubyte2 H = addCarry(added, RdVal, RrVal, 3);
  ubyte2 V = addOverflow(added, RdVal, RrVal);
  ubyte2 N = ((added >> 7) & 0x1);
  ubyte2 S = (N ^ V);
  ubyte2 Z = ((added & 0xff) == 0);
  ubyte2 C = addCarry(added, RdVal, RrVal, 7);

  sreg = setBitInSreg(H);
	sreg = setBitInSreg(V); 
	sreg = setBitInSreg(N); 
	sreg = setBitInSreg(S);
	sreg = setBitInSreg(Z);
	sreg = setBitInSreg(C);

	cpu->sreg = sreg;
	cpu->gpwr[Rd] = added;
	cpu->incPC();
}

void OpcodeManager::opcodeADIW(ubyte2 Rd, ubyte2 K) //Rücksprache Mike: Funktion funktioniert wie vorgesehen?
{
  DEBUG_HEAD(ADIW);
  
	ubyte2 RdValLow  = cpu->gpwr[Rd];
	ubyte2 RdValHigh = cpu->gpwr[Rd + 1];

	ubyte2 RdVal = (RdValHigh << 8) + RdValLow;
	ubyte2 added = RdVal + K;

	ubyte2 sreg = cpu->sreg;

  ubyte2 V = (~(RdValHigh >> 7 & 0x1) &(added >> 15 & 0x1));
  ubyte2 N = ((added >> 15) & 0x1);
  ubyte2 S = (N ^ V);
  ubyte2 Z = ((added & 0xffff) == 0);
  ubyte2 C = (~(added >> 15 & 0x1) &(RdValHigh >> 7 & 0x1));
  
	sreg = setBitInSreg(V);
	sreg = setBitInSreg(N);
	sreg = setBitInSreg(S);
	sreg = setBitInSreg(Z);
	sreg = setBitInSreg(C);

	cpu->sreg = sreg;
  
  // save Rd value back
	cpu->gpwr[Rd] = added & 0xff;
	cpu->gpwr[Rd + 1] = added >> 8;
	cpu->incPC();
}

void OpcodeManager::opcodeAND(ubyte2 Rd, ubyte2 Rr)
{
  DEBUG_HEAD(AND);
  
	ubyte2 RdVal = cpu->gpwr[Rd];
	ubyte2 RrVal = cpu->gpwr[Rr];
	ubyte2 anded = RdVal & RrVal;

	ubyte2 sreg = cpu->sreg;

  ubyte2 V = 0;
  ubyte2 N = ((anded >> 7) & 0x1);
  ubyte2 S = (N ^ V);
  ubyte2 Z = ((anded & 0xff) == 0);
  
	sreg = setBitInSreg(V);
  sreg = setBitInSreg(N);
  sreg = setBitInSreg(S);
  sreg = setBitInSreg(Z);
  
	cpu->sreg = sreg;
	cpu->gpwr[Rd] = anded;
	cpu->incPC();
}

void OpcodeManager::opcodeANDI(ubyte2 Rd, ubyte2 K) //Rücksprache Mike
{
  DEBUG_HEAD(ANDI);
  
  ubyte2 RdVal = cpu->gpwr[Rd];
	ubyte2 anded = RdVal & K;

	ubyte2 sreg = cpu->sreg;

  ubyte2 V = 0;
  ubyte2 N = ((anded >> 7) & 0x1);
  ubyte2 S = (N ^ V);
  ubyte2 Z = ((anded & 0xff) == 0);
  
	sreg = setBitInSreg(V);
  sreg = setBitInSreg(N);
  sreg = setBitInSreg(S);
  sreg = setBitInSreg(Z);
	
	cpu->sreg = sreg;
	cpu->gpwr[Rd] = anded;
	cpu->incPC();
}

void OpcodeManager::opcodeASR(ubyte2 Rd)
{
  DEBUG_HEAD(ASR);
  
	ubyte2 RdVal = cpu->gpwr[Rd];
	ubyte2 added = (RdVal >> 1) + (RdVal & 0x80);

	ubyte2 sreg = cpu->sreg;
  
  ubyte2 N = ((added >> 7) & 0x1);
  ubyte2 C = (RdVal & 0x1);
  ubyte2 V = (N ^ C);
  ubyte2 S = (N ^ V);
  ubyte2 Z = ((added & 0xff) == 0);
      
	sreg = setBitInSreg(N);
  sreg = setBitInSreg(C);
  sreg = setBitInSreg(V);
  sreg = setBitInSreg(S);
  sreg = setBitInSreg(Z);

	cpu->sreg = sreg;
	cpu->gpwr[Rd] = added;
	cpu->incPC();
}

void OpcodeManager::opcodeBCLR(ubyte2 Rd)
{
  DEBUG_HEAD(BCLR);
  
	sregUnsetBit(Rd);
	cpu->incPC();
}

void OpcodeManager::opcodeBLD(ubyte2 Rd, ubyte2 K)
{
  DEBUG_HEAD(BLD);
  
	ubyte2 RdVal = cpu->gpwr[Rd];
	ubyte2 T = sregGetBit(SREG_T);
	ubyte2 result;

	if(T == 0)
		result = RdVal & (~(1 << K));
	else
		result = RdVal | (1 << K);

	cpu->gpwr[Rd] = result;
	cpu->incPC();
}

void OpcodeManager::opcodeBSET(ubyte2 K)
{
  DEBUG_HEAD(BSET);
  
	sregSetBit(K);
	cpu->incPC();
}

void OpcodeManager::opcodeBST(ubyte2 Rd, ubyte2 K)
{
  DEBUG_HEAD(BST);
  
	ubyte2 RdVal = cpu->gpwr[Rd]; // ubyte2 rd = cpu->gpwrRd];

	if(((RdVal >> K) & 0x1) == 1)
		sregSetBit(SREG_T);
	else
		sregUnsetBit(SREG_T);

	cpu->incPC();
}

void OpcodeManager::opcodeCOM(ubyte2 Rd)
{
  DEBUG_HEAD(COM);
  
	ubyte2 RdVal = cpu->gpwr[Rd];
	ubyte2 sub = 0xff - RdVal;

	ubyte2 sreg = cpu->sreg;

  ubyte2 N = ((sub >> 7) & 0x1);
  ubyte2 C = 1;
  ubyte2 V = 0;
  ubyte2 S = (N ^ V);
  ubyte2 Z = ((sub & 0xff) == 0);
  
	sreg = setBitInSreg(N);
  sreg = setBitInSreg(C);
  sreg = setBitInSreg(V);
  sreg = setBitInSreg(S);
  sreg = setBitInSreg(Z);

	cpu->sreg = sreg;
	cpu->gpwr[Rd] = sub;
	cpu->incPC();
}

void OpcodeManager::opcodeCP(ubyte2 Rd, ubyte2 Rr)
{
  DEBUG_HEAD(CP);
  
	ubyte2 RdVal = cpu->gpwr[Rd];
	ubyte2 RrVal = cpu->gpwr[Rr];
	ubyte2 sub = RdVal - RrVal;

	ubyte2 sreg = cpu->sreg;

  ubyte2 H = compareCarry(sub, RdVal, RrVal, 3);
  ubyte2 V = compareOverflow(sub, RdVal, RrVal);
  ubyte2 N = ((sub >> 7) & 0x1);
  ubyte2 S = (N ^ V);
  ubyte2 Z = ((sub & 0xff) == 0);
  ubyte2 C = compareCarry(sub, RdVal, RrVal, 7);
  
  sreg = setBitInSreg(H);
  sreg = setBitInSreg(V);
  sreg = setBitInSreg(N);
  sreg = setBitInSreg(S);
  sreg = setBitInSreg(Z);
  sreg = setBitInSreg(C);

	cpu->sreg = sreg;
	cpu->incPC();
}

void OpcodeManager::opcodeCPC(ubyte2 Rd, ubyte2 Rr)
{
  DEBUG_HEAD(CPC);
  
	ubyte2 RdVal = cpu->gpwr[Rd];
	ubyte2 RrVal = cpu->gpwr[Rr];
	ubyte2 sub = RdVal - RrVal - sregGetBit(SREG_C);

	ubyte2 sreg = cpu->sreg;

  ubyte2 H = compareCarry(sub, RdVal, RrVal, 3);
  ubyte2 V = compareOverflow(sub, RdVal, RrVal);
  ubyte2 N = ((sub >> 7) & 0x1);
  ubyte2 S = (N ^ V);
  ubyte2 C = compareCarry(sub, RdVal, RrVal, 7);
  
	sreg = setBitInSreg(H);
  sreg = setBitInSreg(V);
  sreg = setBitInSreg(N);
  sreg = setBitInSreg(S);
  sreg = setBitInSreg(C);

  // does not work, i promise.......
  ubyte2 Z = ((sub & 0xff) == 0); // does not work, i promise.......
	ubyte2 oldZ = sregGetBit(SREG_Z); // does not work, i promise.......
  
  if (Z) // does not work, i promise.......
    sreg = setBitInSreg(Z); // does not work, i promise.......
  
  cpu->sreg = sreg;
	cpu->incPC();
}

void OpcodeManager::opcodeCPI(ubyte2 Rd, ubyte2 K)
{
  DEBUG_HEAD(CPI);
  
	ubyte2 RdVal = cpu->gpwr[Rd];
	ubyte2 sub   = Rd - K;

	ubyte2 sreg = cpu->sreg;

  ubyte2 H = compareCarry(sub, RdVal, K, 3);
  ubyte2 V = compareOverflow(sub, RdVal, K);
  ubyte2 N = ((sub >> 7) & 0x1);
  ubyte2 S = (N ^ V);
  ubyte2 Z = ((sub & 0xff) == 0);
  ubyte2 C = compareCarry(sub, RdVal, K, 7);
  
  sreg = setBitInSreg(H);
  sreg = setBitInSreg(V);
  sreg = setBitInSreg(N);
  sreg = setBitInSreg(S);
  sreg = setBitInSreg(Z);
  sreg = setBitInSreg(C);
  
	cpu->sreg = sreg;
	cpu->incPC();
}

void OpcodeManager::opcodeDEC(ubyte2 Rd)
{
  DEBUG_HEAD(DEC);
  
	ubyte2 RdVal = cpu->gpwr[Rd];
	ubyte2 sub = RdVal - 1;

	ubyte2 sreg = cpu->sreg;

  ubyte2 N = ((sub >> 7) & 0x1);
  ubyte2 V = (RdVal == 0x80);
  ubyte2 S = (N ^ V);
  ubyte2 Z = ((sub & 0xff) == 0);
  
	sreg = setBitInSreg(N);
  sreg = setBitInSreg(V);
  sreg = setBitInSreg(S);
  sreg = setBitInSreg(Z);

	cpu->sreg = sreg;
	cpu->gpwr[Rd] = sub;
	cpu->incPC();
}

void OpcodeManager::opcodeFMUL(ubyte2 Rd, ubyte2 Rr)//Rücksprache Mike: Funktionsweise
{
  DEBUG_HEAD(FMUL);
  
  ubyte2 RdVal = cpu->gpwr[Rd];
	ubyte2 RrVal = cpu->gpwr[Rr];

	ubyte2 mult = RdVal * RrVal;
	ubyte2 shift = mult << 1;

	ubyte2 sreg = cpu->sreg;

  ubyte2 Z = ((shift & 0xffff) == 0);
  ubyte2 C = ((mult >> 15) & 0x1);
  
  sreg = setBitInSreg(Z);
  sreg = setBitInSreg(C);
  
	cpu->sreg = sreg;
	cpu->gpwr[1] = shift >> 8;
	cpu->gpwr[0] = shift & 0xff;
	cpu->incPC();
}

void OpcodeManager::opcodeFMULS(ubyte2 Rd, ubyte2 Rr)//Rückprache Mike
{
  DEBUG_HEAD(FMULS);
  
	ubyte8 RdVal = cpu->gpwr[Rd];
	ubyte8 RrVal = cpu->gpwr[Rr];

	ubyte2 mult = RdVal * RrVal;  // resp
	ubyte2 shift = mult << 1; // res

	ubyte2 sreg = cpu->sreg;

  ubyte2 Z = ((shift & 0xffff) == 0);
  ubyte2 C = ((mult >> 15) & 0x1);
  
  sreg = setBitInSreg(Z);
  sreg = setBitInSreg(C);
  
	cpu->sreg = sreg;
	cpu->gpwr[1] = shift >> 8;
	cpu->gpwr[0] = shift & 0xff;
	cpu->incPC();
}

void OpcodeManager::opcodeFMULSU(ubyte2 Rd, ubyte2 Rr)//Rücksprache Mike
{
  DEBUG_HEAD(FMULSU);
  
	ubyte2 RdVal = cpu->gpwr[Rd];
	ubyte2 RrVal = cpu->gpwr[Rr];

	ubyte2 mult = RdVal * RrVal;
	ubyte2 shift = mult << 1;

	ubyte2 sreg = cpu->sreg;

  ubyte2 Z = (shift & 0xffff) == 0;
  ubyte2 C = (mult >> 15) & 0x1;
  
  sreg = setBitInSreg(Z);
  sreg = setBitInSreg(C);
  
	cpu->sreg = sreg;

	cpu->gpwr[1] = shift >> 8;
	cpu->gpwr[0] = shift & 0xff;
	cpu->incPC();
}

void OpcodeManager::opcodeINC(ubyte2 Rd)
{
  DEBUG_HEAD(INC);
  
	ubyte2 RdVal = cpu->gpwr[Rd];
	ubyte2 added = RdVal + 1;

	ubyte2 sreg = cpu->sreg;

  ubyte2 N = (added >> 7) & 0x1;
	ubyte2 V = RdVal == 0x7f;
	ubyte2 S = N ^ V;
	ubyte2 Z = (added & 0xff) == 0;
  
  sreg = setBitInSreg(N);
  sreg = setBitInSreg(V);
  sreg = setBitInSreg(S);
  sreg = setBitInSreg(Z);

	cpu->sreg = sreg;
	cpu->gpwr[Rd] = added;
	cpu->incPC();
}

void OpcodeManager::opcodeBRBC(ubyte2 sregBit, ubyte2 k)
{
  DEBUG_HEAD(BRBC);
  
#ifdef DEBUG
	Serial.print(F(": k = "));
	Serial.println((int)k);
#endif

	if (sregGetBit(sregBit) == 0)
	{
		cpu->PC = cpu->PC + (k*2) + 2; // remember that the vm reads bytes, not words (so *2)
	}
	else
	{
		cpu->incPC();
	}
}

void OpcodeManager::opcodeELPM_Z_inc(ubyte2 Rd) // ELPM & LPM
{
  DEBUG_HEAD(ELPM_Z_inc);
  
  ubyte2 Z = ((cpu->rampz & 0x3F) << 16) + // why 3F?
		 (cpu->gpwr[31] << 8) + cpu->gpwr[30];

#warning / 2? or * 2? or take z as is ?
	ubyte2 address = Z / 2;

#warning GET SHORT OR GET BYTE ?
	ubyte2 value = file->getWord(address);

	if (Z & 0x1) // hi byte
		cpu->gpwr[Rd] = (value >> 8);
	else
		cpu->gpwr[Rd] = (value & 0xFF);

	Z += 1;
	cpu->gpwr[30] = Z & 0xFF;
	cpu->gpwr[31] = Z >> 8;
	cpu->rampz = (Z >> 16) & 0x3F;
	cpu->incPC();
}

void OpcodeManager::opcodeBRBS(ubyte2 bit, ubyte2 k)
{
  DEBUG_HEAD(BRBS);
  
  if (sregGetBit(bit))
	{
#warning k*2 + 2 ?
		cpu->PC = cpu->PC + k + 1;
	}
	else
	{
		cpu->incPC();
	}
}

void OpcodeManager::opcodeBRCC(ubyte2 k)
{
  DEBUG_HEAD(BRCC);
  
	opcodeBRBC(SREG_C, k);
}

void OpcodeManager::opcodeBRCS(ubyte2 k)
{
  DEBUG_HEAD(BRCS);
  
	opcodeBRBS(SREG_C, k);
}

void OpcodeManager::opcodeBRHC(ubyte2 k)
{
  DEBUG_HEAD(BRHC);
  
	opcodeBRBC(SREG_H, k);
}

void OpcodeManager::opcodeBRHS(ubyte2 k)
{
  DEBUG_HEAD(BRHS);
  
	opcodeBRBS(SREG_H, k);
}

void OpcodeManager::opcodeBRTC(ubyte2 k)
{
  DEBUG_HEAD(BRTC);
  
	opcodeBRBC(SREG_T, k);
}

void OpcodeManager::opcodeBRTS(ubyte2 k)
{
  DEBUG_HEAD(BRTS);
  
	opcodeBRBS(SREG_T, k);
}

void OpcodeManager::opcodeBRVC(ubyte2 k)
{
  DEBUG_HEAD(BRVC);
  
	opcodeBRBC(SREG_V, k);
}

void OpcodeManager::opcodeBRVS(ubyte2 k)
{
  DEBUG_HEAD(BRVS);
  
	opcodeBRBS(SREG_V, k);
}

void OpcodeManager::opcodeBREQ(ubyte2 k)
{
  DEBUG_HEAD(BREQ);
  
	if (sregGetBit(SREG_Z))
	{
#warning +2 ? / *2 ?
		cpu->PC = cpu->PC + k + 1;
	}
	else
	{
		cpu->incPC();
	}
}

void OpcodeManager::opcodeBRGE(ubyte2 k)
{
  DEBUG_HEAD(BRGE);
  
	if (sregGetBit(SREG_S))
	{
#warning +2 ? / *2 ?    
		cpu->PC = cpu->PC + k + 1;
	}
	else
	{
		cpu->incPC();
	}
}

#warning there are no interrupts in ardunix vm
void OpcodeManager::opcodeBRID(ubyte2 k)
{
  DEBUG_HEAD(BRID);
  
	if (!sregGetBit(SREG_I))
	{
#warning +2 ? / *2 ?    
		cpu->PC = cpu->PC + k + 1;
	}
	else
	{
		cpu->incPC();
	}
}

#warning there are no interrupts in ardunix vm
void OpcodeManager::opcodeBRIE(ubyte2 k)
{
  DEBUG_HEAD(BRIE);
  
	if (sregGetBit(SREG_I))
	{
#warning +2 ? / *2 ?    
		cpu->PC = cpu->PC + k + 1;
	}
	else
	{
		cpu->incPC();
	}
}

void OpcodeManager::opcodeBRLO(ubyte2 k)
{
  DEBUG_HEAD(BRLO);
  
	if (sregGetBit(SREG_C))
	{
#warning +2 ? / *2 ?    
		cpu->PC = cpu->PC + k + 1;
	}
	else
	{
		cpu->incPC();
	}
}

void OpcodeManager::opcodeBRLT(ubyte2 k)
{
  DEBUG_HEAD(BRLT);
  
	if (sregGetBit(SREG_S))
	{
#warning +2 ? / *2 ?    
		cpu->PC = cpu->PC + k + 1;
	}
	else
	{
		cpu->incPC();
	}
}

void OpcodeManager::opcodeBRMI(ubyte2 k)
{
  DEBUG_HEAD(BRMI);
  
	if (sregGetBit(SREG_N))
	{
#warning +2 ? / *2 ?    
		cpu->PC = cpu->PC + k + 1;
	}
	else
	{
		cpu->incPC();
	}
}

void OpcodeManager::opcodeBRNE(ubyte2 k)
{
  DEBUG_HEAD(BRNE);
  
	if (!sregGetBit(SREG_Z))
	{
#warning +2 ? / *2 ?    
		cpu->PC = cpu->PC + k + 1;
	}
	else
	{
		cpu->incPC();
	}
}

void OpcodeManager::opcodeBRPL(ubyte2 k)
{
  DEBUG_HEAD(BRPL);
  
	if (!sregGetBit(SREG_N))
	{
#warning +2 ? / *2 ?    
		cpu->PC = cpu->PC + k + 1;
	}
	else
	{
		cpu->incPC();
	}
}

void OpcodeManager::opcodeBRSH(ubyte2 k)
{
  DEBUG_HEAD(BRSH);
  
	if (!sregGetBit(SREG_C))
	{
#warning +2 ? / *2 ?    
		cpu->PC = cpu->PC + k + 1;
	}
	else
	{
		cpu->incPC();
	}
}

void OpcodeManager::opcodeCLC()
{
  DEBUG_HEAD(CLC);
  
  sregUnsetBit(SREG_C);
}

void OpcodeManager::opcodeCLH()
{
  DEBUG_HEAD(CLH);
  
  sregUnsetBit(SREG_H);
}

#warning no i's
void OpcodeManager::opcodeCLI()
{
  DEBUG_HEAD(CLI);
  
	sregUnsetBit(SREG_I);
}

void OpcodeManager::opcodeCLN()
{
  DEBUG_HEAD(CLN);
  
	sregUnsetBit(SREG_N);
}

void OpcodeManager::opcodeCLS()
{
  DEBUG_HEAD(CLS);
  
	sregUnsetBit(SREG_S);
}

void OpcodeManager::opcodeCLT()
{
  DEBUG_HEAD(CLT);
  
	sregUnsetBit(SREG_T);
}

void OpcodeManager::opcodeCLV()
{
  DEBUG_HEAD(CLV);
  
	sregUnsetBit(SREG_V);
}

void OpcodeManager::opcodeCLZ()
{
  DEBUG_HEAD(CLZ);
  
	sregUnsetBit(SREG_Z);
}

void OpcodeManager::opcodeSEC()
{
  DEBUG_HEAD(SEC);
  
	sregSetBit(SREG_C);
}

void OpcodeManager::opcodeSEH()
{
  DEBUG_HEAD(SEH);
  
	sregSetBit(SREG_H);
}

void OpcodeManager::opcodeSEN()
{
  DEBUG_HEAD(SEN);
  
	sregSetBit(SREG_N);
}

void OpcodeManager::opcodeSES()
{
  DEBUG_HEAD(SES);
  
	sregSetBit(SREG_S);
}

void OpcodeManager::opcodeSET()
{
  DEBUG_HEAD(SET);
  
	sregSetBit(SREG_T);
}

void OpcodeManager::opcodeSEV()
{
  DEBUG_HEAD(SEV);
  
	sregSetBit(SREG_V);
}

void OpcodeManager::opcodeSEZ()
{
  DEBUG_HEAD(SEZ);
  
	sregSetBit(SREG_Z);
}

void OpcodeManager::opcodeST_Z_dec(ubyte2 Rd)
{
  DEBUG_HEAD(ST_Z_dec);
  
	ubyte2 Z = (cpu->gpwr[31] << 8) + cpu->gpwr[30] - 1; // and dec Z !

  cpu->gpwr[30] = Z & 0xff;
	cpu->gpwr[31] = Z >> 8;

	ubyte2 sram_pos = Z;

	sram_warning;

	cpu->sram[sram_pos] = cpu->gpwr[Rd];

	cpu->incPC();
}

void OpcodeManager::opcodeST_Z_inc(ubyte2 Rd)
{
  DEBUG_HEAD(ST_Z_inc);
  
	ubyte2 Z = (cpu->gpwr[31] << 8) + cpu->gpwr[30] + 1; // and inc Z !

	cpu->gpwr[30] = Z & 0xff;
	cpu->gpwr[31] = Z >> 8;

	ubyte2 sram_pos = Z;

	sram_warning;

	cpu->sram[sram_pos] = cpu->gpwr[Rd];

	cpu->incPC();
}

void OpcodeManager::opcodeCALL(ubyte2 k)
{
  DEBUG_HEAD(CALL);
  
	ubyte2 pc = cpu->PC;

	ubyte2 kHi = k;
	ubyte2 kLo = file->getWord(pc + 2) * 2;

	STACK_PUSH(2, pc + 4);

	cpu->PC = (kHi << 16) + kLo;
}

void OpcodeManager::opcodeICALL()
{
  DEBUG_HEAD(ICALL);
  
	STACK_PUSH(2, cpu->PC + 2);
	cpu->PC = (cpu->gpwr[31] << 8) + cpu->gpwr[30];
}

void OpcodeManager::opcodeIJMP()
{
  DEBUG_HEAD(IJMP);
  
	cpu->PC = (cpu->gpwr[31] << 8) + cpu->gpwr[30];
}

void OpcodeManager::opcodeJMP(ubyte2 k) // Rücksprache Mike [ 32 BIT !! ]
{
  DEBUG_HEAD(JMP);
  
	ubyte2 kHi = k;
	ubyte2 kLo = file->getWord(cpu->PC+2) * 2; // *2 because of alternating bus width

#warning k is only 1 byte ? i hope so, see next line's warning
	cpu->PC = (kHi << 16) + kLo;
}

void OpcodeManager::opcodeLDD_Y(ubyte2 Rd, ubyte2 q)
{
  DEBUG_HEAD(LDD_Y);
  
	ubyte2 Y = (cpu->gpwr[29] << 8) + cpu->gpwr[28];

  cpu->gpwr[Rd] = cpu->sram[Y + q];
	cpu->incPC();
}

void OpcodeManager::opcodeLDD_Z(ubyte2 Rd, ubyte2 q)
{
  DEBUG_HEAD(LDD_Z);
  
  ubyte2 Z = (cpu->gpwr[31] << 8) + cpu->gpwr[30];

	cpu->gpwr[Rd] = cpu->sram[Z + q];
	cpu->incPC();
}

void OpcodeManager::opcodeLDS(ubyte2 Rd)
{
  DEBUG_HEAD(LDS);
  
	ubyte2 k = file->getWord(cpu->PC + 2);

	ubyte2 sram_pos = k;

	sram_warning;

	cpu->gpwr[Rd] = cpu->sram[sram_pos];

//#ifdef DEBUG
//	Serial.print(": Rd");
//	Serial.print(Rd);
//	Serial.print(F(" = sram["));
//	Serial.print(sram_pos);
//	Serial.print("] (");
//	Serial.print(cpu->sram[sram_pos]);
//	Serial.print(")");
//#endif

	cpu->incPC();
	cpu->incPC(); // 2x inc
}

void OpcodeManager::opcodeLSR(ubyte2 Rd)
{
  DEBUG_HEAD(LSR);
  
	ubyte2 RdVal = cpu->gpwr[Rd];

	ubyte2 anded = (RdVal >> 1) & 0x7f;

	ubyte2 sreg = cpu->sreg;

	ubyte2 C = RdVal & 0x1;
	ubyte2 N = 0;
	ubyte2 V = N ^ C;
	ubyte2 S = N ^ V;
	ubyte2 Z = (anded & 0xff) == 0;
  
  sreg = setBitInSreg(C);
  sreg = setBitInSreg(N);
  sreg = setBitInSreg(V);
  sreg = setBitInSreg(S);
  sreg = setBitInSreg(Z);

	cpu->sreg = sreg;
	cpu->gpwr[Rd] = anded;
	cpu->incPC();
}

void OpcodeManager::opcodeMOV(ubyte2 Rd, ubyte2 Rr)
{
  DEBUG_HEAD(MOV);
  
	cpu->gpwr[Rd] = cpu->gpwr[Rr];
	cpu->incPC();
}

void OpcodeManager::opcodeMOVW(ubyte2 Rd, ubyte2 Rr)
{
  DEBUG_HEAD(MOVW);
  
#warning problem with opcode mask returning values only > than 16
	Rd = (Rd - 16) * 2;
	Rr = (Rr - 16) * 2;

	cpu->gpwr[Rd] = cpu->gpwr[Rr];
	cpu->gpwr[Rd + 1] = cpu->gpwr[Rr + 1];
	cpu->incPC();
}

void OpcodeManager::opcodeMUL(ubyte2 Rd, ubyte2 Rr)
{
  DEBUG_HEAD(MUL);
  
	ubyte2 RdVal = cpu->gpwr[Rd];
	ubyte2 RrVal = cpu->gpwr[Rr];

	ubyte2 mult = RdVal * RrVal;

	ubyte2 sreg = cpu->sreg;

	ubyte2 Z = (mult & 0xffff) == 0;
	ubyte2 C = (mult >> 15) & 0x1;
  
  sreg = setBitInSreg(Z);
  sreg = setBitInSreg(C);
  
	cpu->sreg = sreg;

	cpu->gpwr[1] = mult >> 8;
	cpu->gpwr[0] = mult & 0xff;
	cpu->incPC();
}

void OpcodeManager::opcodeMULS(ubyte2 Rd, ubyte2 Rr)
{
  DEBUG_HEAD(MULS);
  
	ubyte2 RdVal = cpu->gpwr[Rd];
	ubyte2 RrVal = cpu->gpwr[Rr];
	ubyte2 mult = RdVal * RrVal;

	ubyte2 sreg = cpu->sreg;

  ubyte2 Z = (mult & 0xffff) == 0;
	ubyte2 C = (mult >> 15) & 0x1;

  sreg = setBitInSreg(Z);
  sreg = setBitInSreg(C);
  
	cpu->sreg = sreg;

	cpu->gpwr[1] = mult >> 8;
	cpu->gpwr[0] = mult & 0xff;
	cpu->incPC();
}

void OpcodeManager::opcodeMULSU(ubyte2 Rd, ubyte2 Rr)
{
  DEBUG_HEAD(MULSU);
  
	ubyte2 RdVal = cpu->gpwr[Rd];
	ubyte2 RrVal = cpu->gpwr[Rr];

	ubyte2 mult = RdVal * RrVal;

	ubyte2 sreg = cpu->sreg;

	ubyte2 Z = (mult & 0xffff) == 0;
	ubyte2 C = (mult >> 15) & 0x1;

  sreg = setBitInSreg(Z);
  sreg = setBitInSreg(C);

	cpu->sreg = sreg;

	cpu->gpwr[1] = mult >> 8;
	cpu->gpwr[0] = mult & 0xff;
	cpu->incPC();
}

void OpcodeManager::opcodeNEG(ubyte2 Rd)
{
  DEBUG_HEAD(NEG);
  
	ubyte2 RdVal = cpu->gpwr[Rd];
	ubyte2 anded = RdVal & 0xff;

	ubyte2 sreg = cpu->sreg;

  ubyte2 H = ((anded >> 3) | (RdVal >> 3)) & 0x1;
	ubyte2 V = anded == 0x80;
	ubyte2 N = (anded >> 7) & 0x1;
	ubyte2 S = N ^ V;
	ubyte2 Z = anded == 0x0;
	ubyte2 C = anded != 0x0;
  
  sreg = setBitInSreg(H);
  sreg = setBitInSreg(V);
  sreg = setBitInSreg(N);
  sreg = setBitInSreg(S);
  sreg = setBitInSreg(Z);
  sreg = setBitInSreg(C);

	cpu->sreg = sreg;
	cpu->gpwr[Rd] = anded;
	cpu->incPC();
}

void OpcodeManager::opcodeOR(ubyte2 Rd, ubyte2 Rr)
{
  DEBUG_HEAD(OR);
  
	ubyte2 ord = cpu->gpwr[Rd] | cpu->gpwr[Rr];

	ubyte2 sreg = cpu->sreg;

	ubyte2 V = 0;
	ubyte2 N = (ord >> 7) & 0x1;
	ubyte2 S = N ^ V;
	ubyte2 Z = ord == 0x0;
  
  sreg = setBitInSreg(V);
  sreg = setBitInSreg(N);
  sreg = setBitInSreg(S);
  sreg = setBitInSreg(Z);

	cpu->sreg = sreg;
	cpu->gpwr[Rd] = ord;
	cpu->incPC();
}

void OpcodeManager::opcodeORI(ubyte2 Rd, ubyte2 k)
{
  DEBUG_HEAD(ORI);
  
	ubyte2 ord = cpu->gpwr[Rd] | k;

	ubyte2 sreg = cpu->sreg;

	ubyte2 V = 0;
	ubyte2 N = (ord >> 7) & 0x1;
	ubyte2 S = N ^ V;
	ubyte2 Z = ord == 0x0;
  
  sreg = setBitInSreg(V);
  sreg = setBitInSreg(N);
  sreg = setBitInSreg(S);
  sreg = setBitInSreg(Z);

	cpu->sreg = sreg;
	cpu->gpwr[Rd] = ord;
	cpu->incPC();
}

void OpcodeManager::opcodeOUT(ubyte2 Rd, ubyte2 A)
{
  DEBUG_HEAD(OUT);
  
	ubyte2 sram_pos = A + IO_REG_ADDR_BEGIN;

	sram_warning;

	cpu->sram[sram_pos] = cpu->gpwr[Rd];
	cpu->incPC();
}

#warning iwo war ne funktion da war das genauso, aber habe k als data interpretiert... aender das !
void OpcodeManager::opcodeSTS(ubyte2 Rd)
{
  DEBUG_HEAD(STS);
  
	ubyte2 k = file->getWord(cpu->PC + 2);

	ubyte2 sram_pos = k;

	sram_warning;

//#ifdef DEBUG
//	Serial.print(F(": k_pc = "));
//	Serial.print(k_pc);
//	Serial.print(F("  sram["));
//	Serial.print(sram_pos);
//	Serial.print(F("] = Rd"));
//	Serial.print(Rd);
//	Serial.print(" (");
//	Serial.print(cpu->gpwr[Rd]);
//	Serial.print(")");
//#endif

	cpu->sram[sram_pos] = cpu->gpwr[Rd]; // value from register[k] into sram[Rd]

	cpu->incPC();
	cpu->incPC();
}


#warning will never be used...
void OpcodeManager::opcodeRETI()
{
  DEBUG_HEAD(RETI);
  
  cpu->PC = STACK_POP(2).UBYTE2;
	sregSetBit(SREG_I);
}

void OpcodeManager::opcodeROR(ubyte2 Rd)
{
  DEBUG_HEAD(ROR);
  
	ubyte2 RdVal = cpu->gpwr[Rd];

	ubyte2 ord = (RdVal >> 1) | ((sregGetBit(SREG_C) << 7) & 0x80); // TODO: oder die klammer hinten weg ?

	ubyte2 sreg = cpu->sreg;

  ubyte2 C = RdVal & 0x1;
	ubyte2 N = (ord >> 7) & 0x1;
	ubyte2 V = N ^ C;
	ubyte2 S = N ^ V;
	ubyte2 Z = ord == 0;
  
  sreg = setBitInSreg(C);
  sreg = setBitInSreg(N);
  sreg = setBitInSreg(V);
  sreg = setBitInSreg(S);
  sreg = setBitInSreg(Z);

	cpu->sreg = sreg;
	cpu->gpwr[Rd] = ord;
	cpu->incPC();
}

void OpcodeManager::opcodeSBC(ubyte2 Rd, ubyte2 Rr)
{
  DEBUG_HEAD(SBC);
  
	ubyte2 RdVal = cpu->gpwr[Rd];
	ubyte2 RrVal = cpu->gpwr[Rr];

	ubyte2 dec = RdVal - RrVal - sregGetBit(SREG_C);

	ubyte2 sreg = cpu->sreg;

	ubyte2 H = subCarry(dec, RdVal, RrVal, 3);
	ubyte2 V = subOverflow(dec, RdVal, RrVal);
	ubyte2 N = (dec >> 7) & 0x1;
	ubyte2 S = N ^ V;
	ubyte2 C = subCarry(dec, RdVal, RrVal, 7);

  sreg = setBitInSreg(H);
  sreg = setBitInSreg(V);
  sreg = setBitInSreg(N);
  sreg = setBitInSreg(S);
  sreg = setBitInSreg(C);
  
	if (dec & 0xff)
  {
    ubyte2 Z = 0;
    sreg = setBitInSreg(Z);
  }

	cpu->sreg = sreg;
	cpu->gpwr[Rd] = dec;
	cpu->incPC();
}

void OpcodeManager::opcodeSBCI(ubyte2 Rd, ubyte2 k)
{
  DEBUG_HEAD(SBCI);
  
	ubyte2 RdVal = cpu->gpwr[Rd];

	ubyte2 sub = RdVal - k - sregGetBit(SREG_C);

	ubyte2 sreg = cpu->sreg;

	ubyte2 H = subCarry(sub, RdVal, k, 3);
	ubyte2 V = subOverflow(sub, RdVal, k);
	ubyte2 N = (sub >> 7) & 0x1;
  ubyte2 S = N ^ V;
	ubyte2 C = subCarry(sub, RdVal, k, 7);
  
  sreg = sregSetBit(H);
  sreg = sregSetBit(V);
  sreg = sregSetBit(N);
  sreg = sregSetBit(S);
  sreg = sregSetBit(C);

	if (sub & 0xff)
  {
    ubyte2 Z = 0;
		sreg = sregSetBit(Z);
  }

	cpu->sreg = sreg;
	cpu->gpwr[Rd] = sub;
	cpu->incPC();
}

void OpcodeManager::opcodeSBIW(ubyte2 Rd, ubyte2 k)
{
  DEBUG_HEAD(SBIW);
  
	ubyte2 RdLowVal  = cpu->gpwr[Rd];
	ubyte2 RdHighVal = cpu->gpwr[Rd + 1];

	ubyte2 RdVal = (RdHighVal << 8) + RdLowVal;

	ubyte2 sub = RdVal - k;

	ubyte2 sreg = cpu->sreg;
  
  

	ubyte2 V = (RdHighVal >> 7 & 0x1) & ~(sub >> 15 & 0x1);
	ubyte2 N = (sub >> 15) & 0x1;
	ubyte2 S = N ^ V;
	ubyte2 Z = (sub & 0xffff) == 0;
	ubyte2 C = (sub >> 15 & 0x1) & ~(RdHighVal >> 7 & 0x1);
  
  sreg = sregSetBit(V);
  sreg = sregSetBit(N);
  sreg = sregSetBit(S);
  sreg = sregSetBit(Z);
  sreg = sregSetBit(C);

	cpu->sreg = sreg;

	cpu->gpwr[Rd] = sub & 0xff;
	cpu->gpwr[Rd + 1] = sub >> 8;
	cpu->incPC();
}

void OpcodeManager::opcodeSUB(ubyte2 Rd, ubyte2 Rr)
{
  DEBUG_HEAD(SUB);
  
	ubyte2 RdVal = cpu->gpwr[Rd];
	ubyte2 RrVal = cpu->gpwr[Rr];

	ubyte2 sub = RdVal - RrVal;

	ubyte2 sreg = cpu->sreg;

  ubyte2 H = subCarry(sub, RdVal, RrVal, 3);
	ubyte2 V = subOverflow(sub, RdVal, RrVal);
	ubyte2 N = (sub >> 7) & 0x1;
	ubyte2 S = N ^ V;
	ubyte2 Z = (sub & 0xff) == 0;
	ubyte2 C = subCarry(sub, RdVal, RrVal, 7);
  
  sreg = sregSetBit(H);
  sreg = sregSetBit(V);
  sreg = sregSetBit(N);
  sreg = sregSetBit(S);
  sreg = sregSetBit(Z);
  sreg = sregSetBit(C);
  
  cpu->sreg = sreg;

	cpu->gpwr[Rd] = sub & 0xff;
	cpu->gpwr[Rd + 1] = sub >> 8;
	cpu->incPC();
}

void OpcodeManager::opcodeSUBI(ubyte2 Rd, ubyte2 k)
{
  DEBUG_HEAD(SUBI);
  
  ubyte2 RdVal = cpu->gpwr[Rd];

	ubyte2 sub = RdVal - k;

	ubyte2 sreg = cpu->sreg;

  ubyte2 H = subCarry(sub, RdVal, k, 3);
	ubyte2 V = subOverflow(sub, RdVal, k);
	ubyte2 N = (sub >> 7) & 0x1;
	ubyte2 S = N ^ V;
	ubyte2 Z = (sub & 0xff) == 0;
	ubyte2 C = subCarry(sub, RdVal, k, 7);
  
  sreg = sregSetBit(H);
  sreg = sregSetBit(V);
  sreg = sregSetBit(N);
  sreg = sregSetBit(S);
  sreg = sregSetBit(Z);
  sreg = sregSetBit(C);
  
  cpu->sreg = sreg;

	cpu->gpwr[Rd] = sub & 0xff;
	cpu->gpwr[Rd + 1] = sub >> 8;
	cpu->incPC();
}

void OpcodeManager::opcodeSWAP(ubyte2 Rd)
{
  DEBUG_HEAD(SWAP);
  
	ubyte2 RdVal = cpu->gpwr[Rd];

	cpu->gpwr[Rd] = ((RdVal << 4) & 0xf0) | ((RdVal >> 4) & 0x0f);
	cpu->incPC();
}
