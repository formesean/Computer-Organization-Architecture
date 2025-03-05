#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

// Instructio code
#define WM 0x01
#define RM 0x02
#define BR 0x03
#define RIO 0x04
#define WIO 0x05
#define WB 0x06
#define WIB 0x07
#define EOP 0x1F

// Function prototypes
int CU();
void mainMemory();
void IOMemory();
void initMemory();

// Global variables
unsigned char dataMemory[2048];
unsigned char ioBuffer[32];

// External control signals
unsigned char CONTROL = 0x00;
int IOM = 0;
int RW = 0;
int OE = 0;

// Buses
unsigned int ADDR = 0x000;
unsigned char BUS = 0x00;

void main(void)
{
  initMemory(); // this function populates the memory

  if (CU() == 1) // check the return value
    printf("Program run successfully!");
  else
    printf("Error encountered, program terminated!");
}

int CU(void)
{
  unsigned int PC = 0x000;
  unsigned int IR = 0x0000;
  unsigned int MAR = 0x000;
  unsigned char MBR = 0x00;
  unsigned int IOAR = 0x000;
  unsigned char IOBR = 0x00;

  unsigned char inst_code = 0x0000;
  unsigned int operand = 0x000;
  int exitCode = 0;

  // Internal control signals
  int _FETCH = 0;
  int _IO = 0;
  int _MEMORY = 0;

  while (!exitCode)
  {
    printf("****************************\n");
    printf("PC\t\t\t: 0x%03X\n", PC);
    printf("Fetching instruction...\n");

    // Setting control signals, main memory access, read operation, allow data movement
    CONTROL = inst_code;
    IOM = 1;
    RW = 0;
    OE = 1;

    // set internal control signal to signify fetch operation
    _FETCH = 1;
    _IO = 0;
    _MEMORY = 0;

    // fetch upper byte
    ADDR = PC;
    IOM = 1;
    RW = 0;
    OE = 1;
    mainMemory();
    IR = (unsigned int)BUS << 8;
    PC++;

    // fetch lower byte
    ADDR = PC;
    mainMemory();
    IR |= BUS;
    PC++;

    printf("IR\t\t\t: 0x%04X\n", IR);

    // Decode instruction
    inst_code = (IR >> 11) & 0x1F;
    operand = IR & 0x07FF;

    printf("Instruction Code\t: 0x%02X\n", inst_code);
    printf("Operand\t\t\t: 0x%03X\n", operand);

    printf("Instruction\t\t: ");

    if (inst_code == WM)
    {
      printf("WM\n");
      MAR = operand;

      // set internal control signals, accessing memory
      _FETCH = 0;
      _MEMORY = 1;
      _IO = 0;

      // set external control signals, accessing main memory, write operation, allow data movement
      CONTROL = inst_code;
      IOM = 1;
      RW = 1;
      OE = 1;

      ADDR = MAR;

      if (_MEMORY)
        BUS = MBR;

      mainMemory();
      printf("Writing data to memory...\n");
    }
    else if (inst_code == RM)
    {
      printf("RM\n");
      MAR = operand;

      // set internal control signals, accessing memory
      _FETCH = 0;
      _MEMORY = 1;
      _IO = 0;

      // set external control signals, accessing main memory, read operation, allow data movement
      CONTROL = inst_code;
      IOM = 1;
      RW = 0;
      OE = 1;

      printf("Reading data to memory...\n");
      ADDR = MAR;
      mainMemory();

      if (_MEMORY)
        MBR = BUS;

      printf("MBR\t\t\t: 0x%02X\n", MBR);
    }
    else if (inst_code == BR)
    {
      printf("BR\n");
      PC = operand;

      // set external control signals
      CONTROL = inst_code;
      OE = 1;
      printf("Branch to 0x%03X on next cycle.\n", PC);
    }
    else if (inst_code == RIO)
    {
      printf("RIO\n");
      IOAR = operand;

      // set internal control signals, accessing I/O
      _FETCH = 0;
      _MEMORY = 0;
      _IO = 1;

      // set external control signals, accessing I/O, read operation, allow data movement
      CONTROL = inst_code;
      IOM = 0;
      RW = 0;
      OE = 1;

      printf("Reading to IO buffer...\n");
      ADDR = IOAR;
      IOMemory();

      if (_IO)
        IOBR = BUS;

      printf("IOBR\t\t\t: 0x%02X\n", IOBR);
    }
    else if (inst_code == WIO)
    {
      printf("WIO\n");
      IOAR = operand;

      // set internal control signals, accessing I/O
      _FETCH = 0;
      _MEMORY = 0;
      _IO = 1;

      // set external control signals, accessing I/O, write operation, allow data movement
      CONTROL = inst_code;
      IOM = 0;
      RW = 1;
      OE = 1;

      printf("Writing to IO buffer...\n");
      ADDR = IOAR;

      if (_IO)
        BUS = IOBR;

      IOMemory();
      printf("IOBR\t\t\t: 0x%02X\n", IOBR);
    }
    else if (inst_code == WB)
    {
      printf("WB\n");

      // set internal control signals, accessing memory
      _FETCH = 0;
      _MEMORY = 1;
      _IO = 0;

      // set external control signals, accessing main memory, write operation, allow data movement
      CONTROL = inst_code;
      IOM = 1;
      RW = 1;
      OE = 1;

      printf("Loading data to MBR...\n");
      ADDR = PC - 1;
      mainMemory();

      if (_MEMORY)
        MBR = BUS;

      printf("MBR\t\t\t: 0x%02X\n", MBR);
    }
    else if (inst_code == WIB)
    {
      printf("WIB\n");

      // set internal control signals, accessing I/O
      _FETCH = 0;
      _MEMORY = 0;
      _IO = 1;

      // set external control signals, accessing I/O, write operation, allow data movement
      CONTROL = inst_code;
      IOM = 0;
      RW = 1;
      OE = 1;

      printf("Loading data to IOBR...\n");
      ADDR = PC - 1;

      if (_IO)
        IOBR = BUS;

      IOMemory();
      printf("IOBR\t\t\t: 0x%02X\n", IOBR);
    }
    else if (inst_code == EOP)
    {
      printf("EOP\n");
      exitCode = 1;
      printf("End of program.\n\n");
      return 1;
    }
    else
    {
      printf("Unknown instruction\n");
      return 0;
    }

    printf("BUS\t\t: 0x%02X\n", BUS);
    printf("ADDR\t\t: 0x%03X\n", ADDR);
    printf("PC\t\t: 0x%03X\n", PC);
    printf("MAR\t\t: 0x%02X\n", MAR);
    printf("IOAR\t\t: 0x%02X\n", IOAR);
    printf("IOBR\t\t: 0x%02X\n", IOBR);
    printf("CONTROL\t\t: 0x%02X\n", CONTROL);
    getch();
  }

  return 1;
}

void mainMemory(void)
{
  if (IOM == 1)
  {
    if (RW == 0 && OE == 1)
      BUS = dataMemory[ADDR]; // memory read
    else if (RW == 1 && OE == 1)
      dataMemory[ADDR] = BUS; // memory write
  }
}

void IOMemory(void)
{
  if (IOM == 0)
  {
    if (RW == 0 && OE == 1)
      BUS = ioBuffer[ADDR]; // I/O read
    else if (RW == 1 && OE == 1)
      ioBuffer[ADDR] = BUS; // I/O write
  }
}

void initMemory(void)
{
  printf("Initializing Main Memory...\n\n");

  dataMemory[0x000] = 0x30;
  dataMemory[0x001] = 0xFF;
  dataMemory[0x002] = 0x0C;
  dataMemory[0x003] = 0x00;
  dataMemory[0x004] = 0x14;
  dataMemory[0x005] = 0x00;
  dataMemory[0x006] = 0x19;
  dataMemory[0x007] = 0x2A;
  dataMemory[0x12A] = 0x38;
  dataMemory[0x12B] = 0x05;
  dataMemory[0x12C] = 0x28;
  dataMemory[0x12D] = 0x0A;
  dataMemory[0x12E] = 0xF8;
  dataMemory[0x12F] = 0x00;
}
