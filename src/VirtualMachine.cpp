#include "VirtualMachine.h"
#include "OpcodeManager.h"
#include "File.h"
#include "EventController.h"


#define DEBUG


VirtualMachine::VirtualMachine()
{
  
}

bool VirtualMachine::init(File* file, char* arga, int args)//, bool isBinary)
{
  eventController.fireEvent(ardunix::events::etStdOutFlashLine, (void*)F("ardunix virtual machine\n"));
  
  
  this->file = file;
  
  if (!this->file)
    return false;
  
  /*if (isBinary)
  {
    this->file = file;
  }
  else // is a elf file
  {
    this->file = new ELFile(file);
  }*/
  
  /*if (!this->file->openFile(file->file))
    return false;*/ // FILE IS ALREADY OPENED BY INTERNAL FUNCTIONS -> STARTVM
  
  cpu = new CPU;
  memset(cpu, 0, sizeof(CPU));
  
  /*if (isBinary)
  {*/
  this->opcodeManager = new OpcodeManager(this, cpu, file); /*
  }
  else
  {
    this->opcodeManager = new OpcodeManager(this->cpu, this->elfile);
  }*/
  
  return true;
}

ubyte2 VirtualMachine::readWord(int pos) // TODO: short to int ?!
{
  return this->file->getWord(pos);
  
  //this->cpu->PC += 2; // ???? because we are working with 8 bit we have to increment PC ??????????? !!!!!!!!!!!!!!!!!!!!!!!!!
  
  //return conv.UBYTE2;
}

//#define call(x) Serial.print(F("avr_op_x")); opcodeManager.avr_op_x(operand1, operand2);

void VirtualMachine::executeOpcode(ubyte2 opcode) // we do NOT create a opcode loopup table, because of extremly hard sram limitations
{
#ifdef DEBUG
  //Serial.print(F("\r\n"));
  Serial.print(cpu->PC, 16);
  Serial.print(F(": "));
#endif
  
  switch (opcode)
  {
    case 0x9509:
    {
      opcodeManager->opcodeICALL();
      return;
    }        
      
    case 0x9409:
    {
      opcodeManager->opcodeIJMP();
      return;
    }
      
    case 0x0000:
    {
      opcodeManager->opcodeNOP();
      return;
    }
      
    case 0x9508:
    {
      opcodeManager->opcodeRET();
      return;
    }
      
    case 0x9518:
    {
      opcodeManager->opcodeRETI();
      return;
    }
      
    case 0x94F8: // SELF-MADE - bin stolz auf mich hahah
    case 0x9478:
    {
      eventController.fireEvent(ardunix::events::etAPI, (void*)this);
      
      //Serial.print(F("\r\napi[0] = ")); Serial.print((ubyte1)this->cpu->api[0]);
      //Serial.print(F("\r\napi[1] = ")); Serial.print((ubyte1)this->cpu->api[1]);
      //Serial.print(F("\r\napi[2] = ")); Serial.print((ubyte1)this->cpu->api[2]);
      
      /*Serial.print("\r\n2303 = "); Serial.print(this->cpu->sram[2303]);
      Serial.print("\r\n&sram[2303] = "); Serial.print((ubyte2)&this->cpu->sram[2303]);
      Serial.print("\r\nAnd &api[0] = "); Serial.print((ubyte2)&this->cpu->api[0]);*/
      
      //Serial.print("\r\nLooking for 99 !");
      //Serial.print("\r\nCPU size: "); Serial.print((ubyte2)sizeof(CPU));
      
      /*ubyte1* lookFor = (ubyte1*)&this->cpu;
        
      // LOOK FOR 99, WHERE IS THAT FUCKR ?!
      for (ubyte2 i = 0; i < sizeof(CPU); i++)
      {
        if (lookFor[i] == 99)
        {
          Serial.print("\r\nFound 99 @ "); Serial.print((ubyte2)i);
        }
        else
        {
          if (!lookFor[i])
            continue;
          Serial.print("\r\n");
          Serial.print((ubyte2)lookFor[i]);
        }
      }
      Serial.print("\r\nDone.");
*/
      opcodeManager->opcodeSEI(); // TRIGGERS AN ARDUNIX API STACKED ON THE STACK
      return;
    }
  }
  
  
  ubyte2 pureOpcode = opcode & ~(OpcodeManager::oo_Rd5b | OpcodeManager::oo_Rr5b);
  ubyte2 operand1 = Rd5b(opcode);
  ubyte2 operand2 = Rr5b(opcode);
  
  switch (pureOpcode)
  {
    case 0x1C00:
    {
      opcodeManager->opcodeADC(operand1, operand2);
      return;
    }
      
    case 0x0C00:
    {
      opcodeManager->opcodeADD(operand1, operand2);
      return;
    }
      
    case 0x2000:
    {
      opcodeManager->opcodeAND(operand1, operand2);
      return;
    }
      
    case 0x1400:
    {
      opcodeManager->opcodeCP(operand1, operand2);
      return;
    }
      
    case 0x0400:
    {
      opcodeManager->opcodeCPC(operand1, operand2);
      return;
    }
      
    case 0x2400:
    {
      opcodeManager->opcodeEOR(operand1, operand2);
      return;
    }
      
    case 0x2C00:
    {
      opcodeManager->opcodeMOV(operand1, operand2);
      return;
    }
      
    case 0x9C00:
    {
      opcodeManager->opcodeMUL(operand1, operand2);
      return;
    }
      
    case 0x2800:
    {
      opcodeManager->opcodeOR(operand1, operand2);
      return;
    }
      
    case 0x0800:
    {
      opcodeManager->opcodeSBC(operand1, operand2);
      return;
    }
      
    case 0x1800:
    {
      opcodeManager->opcodeSUB(operand1, operand2);
      return;
    }
  }
  
    
  pureOpcode = opcode & ~(OpcodeManager::oo_Rd5b);
  operand1 = Rd5b(opcode);
  
  switch (pureOpcode)
  {
    case 0x9200:
    {
      opcodeManager->opcodeSTS(operand1);
      return;
    }
      
    case 0x9000:
    {
      opcodeManager->opcodeLDS(operand1);
      return;
    }
      
    case 0x9405:
    {
      opcodeManager->opcodeASR(operand1);
      return;
    }
      
    case 0x9400:
    {
      opcodeManager->opcodeCOM(operand1);
      return;
    }
      
    case 0x940A:
    {
      opcodeManager->opcodeDEC(operand1);
      return;
    }
      
    case 0x9007:
    {
      opcodeManager->opcodeELPM_Z_inc(operand1);
      return;
    }
      
    case 0x9005:
    {
      opcodeManager->opcodeLPM_Z_inc(operand1);
      return;
    }
      
    case 0x9403:
    {
      opcodeManager->opcodeINC(operand1);
      return;
    }
      
    case 0x9406:
    {
      opcodeManager->opcodeINC(operand1);
      return;
    }
      
    case 0x9401:
    {
      opcodeManager->opcodeNEG(operand1);
      return;
    }
      
    case 0x900F:
    {
      opcodeManager->opcodePOP(operand1);
      return;
    }
      
    case 0x920F:
    {
      opcodeManager->opcodePUSH(operand1);
      return;
    }
      
    case 0x9407:
    {
      opcodeManager->opcodeROR(operand1);
      return;
    }
      
    case 0x9402:
    {
      opcodeManager->opcodeSWAP(operand1);
      return;
    }
      
    case 0x9202:
    {
      opcodeManager->opcodeST_Z_dec(operand1);
      return;
    }
      
    case 0x9201:
    {
      opcodeManager->opcodeST_Z_inc(operand1);
      return;
    }
      
    case 0x920D:
    {
      opcodeManager->opcodeST_X_inc(operand1);
      return;
    }
  }
  
  
  pureOpcode = opcode & ~(OpcodeManager::oo_Rd4b | OpcodeManager::oo_K8b);
  operand1 = Rd4b(opcode);
  operand2 = K8b(opcode);
  
  switch (pureOpcode)
  {
    case 0x7000:
    {
      opcodeManager->opcodeANDI(operand1, operand2);
      return;
    }
      
    case 0x3000:
    {
      opcodeManager->opcodeCPI(operand1, operand2);
      return;
    }
      
    case 0xE000:
    {
      opcodeManager->opcodeLDI(operand1, operand2);
      return;
    }
      
    case 0x6000:
    {
      opcodeManager->opcodeORI(operand1, operand2);
      return;
    }
      
    case 0x5000:
    {
      opcodeManager->opcodeSUBI(operand1, operand2);
      return;
    }
      
    case 0x4000:
    {
      opcodeManager->opcodeSBCI(operand1, operand2);
      return;
    }
  }
  
  
  pureOpcode = opcode & ~(OpcodeManager::oo_Rd5b | OpcodeManager::oo_regBit);
  operand1 = Rd5b(opcode);
  operand2 = regBit(opcode);
  
  switch (pureOpcode)
  {
    case 0xF800:
    {
      opcodeManager->opcodeBLD(operand1, operand2);
      return;
    }
      
    case 0xFA00:
    {
      opcodeManager->opcodeBST(operand1, operand2);
      return;
    }
  }
  
  
  pureOpcode = opcode & ~(OpcodeManager::oo_k7b | OpcodeManager::oo_regBit);
  operand1 = regBit(opcode);
  operand2 = opcodeManager->unsignedBitToSignedBit(K7b(opcode), 7);
  
  switch (pureOpcode)
  {
    case 0xD800: // the real opcode.. i calc'd it myself..
    case 0xF400: // wrong.. or is not avr:4
    {
      opcodeManager->opcodeBRBC(operand1, operand2);
      return;
    }
      
    case 0xF000:
    {
      opcodeManager->opcodeBRBS(operand1, operand2);
      return;
    }
  }
  
  // opcodes with a 6-bit address displacement (q) and register (Rd) as operands
  pureOpcode = opcode & ~(OpcodeManager::oo_Rd5b | OpcodeManager::oo_q6b);
  operand1 = Rd5b(opcode);
  operand2 = q6b(opcode);
  
  switch (pureOpcode)
  {
    case 0x8008:
    {
      opcodeManager->opcodeLDD_Y(operand1, operand2);
      return;
    }
      
    case 0x8000:
    {
      opcodeManager->opcodeLDD_Z(operand1, operand2);
      return;
    }
      
    case 0x8208:
    {
      opcodeManager->opcodeSTD_Y(operand1, operand2);
      return;
    }
      
    case 0x8200:
    {
      opcodeManager->opcodeSTD_Z(operand1, operand2);
      return;
    }
  }
  
  
  pureOpcode = opcode & ~(OpcodeManager::oo_k22b);
  operand1 = K22b(opcode);
  
  switch (pureOpcode)
  {
    case 0x940E:
    {
      opcodeManager->opcodeCALL(operand1);
      return;
    }
      
    case 0x940C:
    {
      opcodeManager->opcodeJMP(operand1);
      return;
    }
  }
  
  
  pureOpcode = opcode & ~(OpcodeManager::oo_sregBit);
  operand1 = sregBit(opcode);
  
  switch (pureOpcode)
  {
    case 0x9488:
    {
      opcodeManager->opcodeBCLR(operand1);
      return;
    }
      
    case 0x9408:
    {
      opcodeManager->opcodeBSET(operand1);
      return;
    }
  }
  
  
  pureOpcode = opcode & ~(OpcodeManager::oo_K6b | OpcodeManager::oo_Rd2b);
  operand1 = Rd2b(opcode);
  operand2 = K6b(opcode);
  
  switch (pureOpcode)
  {
    case 0x9600:
    {
      opcodeManager->opcodeADIW(operand1, operand2);
      return;
    }
      
    case 0x9700:
    {
      opcodeManager->opcodeSBIW(operand1, operand2);
      return;
    }
  }
  
  
  pureOpcode = opcode & ~(OpcodeManager::oo_A5b | OpcodeManager::oo_sregBit);
  operand1 = A5b(opcode);
  operand2 = sregBit(opcode);
  
  switch (pureOpcode)
  {
    // no opcodes in this category
  }
  
  
  pureOpcode = opcode & ~(OpcodeManager::oo_A6b | OpcodeManager::oo_Rd5b);
  operand1 = Rd5b(opcode);
  operand2 = A6b(opcode);
  
  switch (pureOpcode)
  {
    case 0xB000:
    {
      opcodeManager->opcodeIN(operand1, operand2);
      return;
    }
      
    case 0xB800:
    {
      opcodeManager->opcodeOUT(operand1, operand2);
      return;
    }
  }
  
  
  pureOpcode = opcode & ~(OpcodeManager::oo_k12b);
#warning next line result thingy needed ?
  operand1 = opcodeManager->unsignedBitToSignedBit(k12b(opcode), 12);
  
  switch (pureOpcode)
  {
    case 0xD000:
    {
      opcodeManager->opcodeRCALL(operand1);
      return;
    }
      
    case 0xC000:
    {
      opcodeManager->opcodeRJMP(operand1);
      return;
    }
  }
  
  
  pureOpcode = opcode & ~(OpcodeManager::oo_Rd4b | OpcodeManager::oo_Rr4b);
  operand1 = Rd4b(opcode);
  operand2 = Rr4b(opcode);
  
  switch (pureOpcode)
  {
    case 0x0100:
    {
      opcodeManager->opcodeMOVW(operand1, operand2);
      return;
    }
      
    case 0x0200:
    {
      opcodeManager->opcodeMULS(operand1, operand2);
      return;
    }
  }
  
  // ab hier ALLES selber gemacht hihi :)
  /*pureOpcode = opcode & ~(OpcodeManager::oo_k7b);
  operand1 = K7b(opcode);
  
  switch (pureOpcode)
  {
    case 
  }*/
  // ende
  
  
  pureOpcode = opcode & ~(OpcodeManager::oo_Rd3b | OpcodeManager::oo_Rr3b);
  operand1 = Rd3b(opcode);
  operand2 = Rr3b(opcode);
  
  switch (pureOpcode)
  {
    case 0x0300:
    {
      opcodeManager->opcodeMULSU(operand1, operand2);
      return;
    }
      
    case 0x0308:
    {
      opcodeManager->opcodeFMUL(operand1, operand2);
      return;
    }
      
    case 0x0380:
    {
      opcodeManager->opcodeFMULS(operand1, operand2);
      return;
    }
      
    case 0x0388:
    {
      opcodeManager->opcodeFMULSU(operand1, operand2);
      return;
    }
  }
  
  // OPCODE NOT FOUND!
  //eventController.fireEvent(ardunix::events::etStdOutFlashLine, (void*)F("ardunix_virtual_machine: illegal opcode at pc "));
  //eventController.fireEvent(ardunix::events::etStdOutNumberLine, (void*)this->cpu->PC);
  
  eventController.fireEvent(ardunix::events::etStdOutFlashLine,  (void*)F("vm: illegal opcode "));
  eventController.fireEvent(ardunix::events::etStdOutNumberLine, (void*)this->file->getByte(this->cpu->PC));
  eventController.fireEvent(ardunix::events::etStdOutFlashLine,  (void*)F(" "));
  eventController.fireEvent(ardunix::events::etStdOutFlashLine,  (void*)this->file->getByte(this->cpu->PC+1));
  eventController.fireEvent(ardunix::events::etStdOutFlashLine,  (void*)F(" at pc "));
  eventController.fireEvent(ardunix::events::etStdOutFlashLine,  (void*)this->cpu->PC);
  
  cpu->PC += 2; // step hopefully to next opcode
  return;
}

ubyte2 VirtualMachine::getOpcode()
{
  return readWord(cpu->PC);
}

void VirtualMachine::cpustep()
{
#ifdef DEBUG
  delay(100);
#endif
  
  executeOpcode(getOpcode());
}

void VirtualMachine::addCharToInBuffer(char c)
{
  this->inBuf = c;
}
