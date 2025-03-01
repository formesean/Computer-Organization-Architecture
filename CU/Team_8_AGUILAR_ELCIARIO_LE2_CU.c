#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#define WM 0x01
#define RM 0x02
#define BR 0x03
#define RIO 0x04
#define WIO 0x05
#define WB 0x06
#define WIB 0x07
#define EOP 0x1F

unsigned char ADDR = 0x00;
unsigned char BUS = 0x00;
unsigned char dataMemory[2048];
unsigned char ioBuffer[32];
int IOM = 0;
int RW = 0; // if 1, write, otherwise read
int OE = 0; // if 1, can output, otherwise cant output

int CU(void);
void mainMemory();
void IOMemory();
void initMemory();

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
  // starting address Program Memory (instruction) is 00016
  // Data Memory starts at 40016
  // I/O BUffer = 00016
  unsigned int PC = 0x000;
  unsigned int IR = 0x0000;
  unsigned int MAR = 0x000;
  unsigned int MBR = 0x00;
  unsigned int IOAR = 0x000;
  unsigned int IOBR = 0x00;

  unsigned char inst_code = 0x0000;
  unsigned int operand = 0x000;

  int FETCH = 0;
  int exitCode = 0;

  while (!exitCode)
  {
    printf("****************************\n");
    printf("PC\t\t\t: 0x%03X\n", PC);
    printf("Fetching instruction...\n");

    FETCH = 1;
    IOM = 1;
    RW = 0;
    OE = 1;

    ADDR = PC;
    mainMemory();
    if (FETCH == 1)
    {
      // fetching upper byte
      IR = (int) BUS;
      IR = IR << 8;
      PC++;
      ADDR = PC;
    }

    mainMemory();
    if (FETCH == 1)
    {
      // fetching lower byte
      IR = IR | BUS;
      PC++;
    }

    printf("IR\t\t\t: 0x%04X\n", IR);

    // decoding instruction
    inst_code = IR >> 11;
    operand = IR & 0x07FF;

    printf("Instrucion Code\t\t: 0x%02X\n", inst_code);
    printf("Operand\t\t\t: 0x%03X\n", operand);

    printf("Instruction\t\t: ");

    if (inst_code == WM)
    {
      printf("WM\n");
      MAR = operand;
      BUS = MBR;

      FETCH = 0;
      IOM = 1;
      RW = 1;
      OE = 0;
      mainMemory();

      printf("Writing data to memory...\n");
    }

    else if (inst_code == RM)
    {
      printf("RM\n");
      MAR = operand;

      FETCH = 0;
      IOM = 1;
      RW = 0;
      OE = 1;
      mainMemory();

      printf("Reading data to memory...\n");
      printf("MBR\t\t: 0x%02X\n", MBR);
    }

    else if (inst_code == BR)
    {
      printf("BR\n");
      PC = operand;
      OE = 1;
      printf("Branch to 0x%03X on next cycle.\n", PC);
      getch();
    }

    else if (inst_code == RIO)
    {
      printf("RIO\n");
      IOAR = operand;
      IOBR = BUS;
      printf("Reading to IO buffer...\n");
    }

    else if (inst_code == WIO)
    {
      printf("WIO\n");
      IOAR = operand;
      ioBuffer[IOAR] = IOBR;
      printf("Writing to IO buffer...\n");
    }

    else if (inst_code == WB)
    {
      printf("WB\n");
      FETCH = 0;
      IOM = 1;
      RW = 0;
      OE = 1;
      mainMemory();

      MBR = BUS;

      printf("Loading data to MBR...\n");
      printf("MBR\t\t: 0x%02X\n", MBR);
    }

    else if (inst_code == WIB)
    {
      printf("WIB\n");
      FETCH = 0;
      IOM = 0;
      RW = 1;
      OE = 0;

      IOBR = BUS;
      IOMemory();
      printf("Loading data to IOBR...\n");
      printf("IOBR\t\t: 0x%02X\n", IOBR);
    }

    else if (inst_code == EOP)
    {
      printf("EOP\n");
      exitCode = 1;
      printf("End of program.\n");
      getch();
      return exitCode;
    }

    else
    {
      return 0;
    }

  }
}

void mainMemory(void)
{
  if (IOM == 1)
  {
    if (RW == 0 && OE == 1)
      BUS = dataMemory[ADDR]; // memory read
    else if (RW == 1 && OE == 0)
      dataMemory[ADDR] = BUS; // memory write
  }
}

void IOMemory(void)
{
  if (IOM == 0)
  {
    if (RW == 0 && OE == 1)
      BUS = ioBuffer[ADDR]; // I/O read
    else if (RW == 1 && OE == 0)
      ioBuffer[ADDR] = BUS; // I/O write
  }
}

void initMemory(void)
{
  printf("Initializing Main Memory...\n\n");

  dataMemory[0x000] = 0x30;
  dataMemory[0x001] = 0xFF;
  dataMemory[0x002] = 0x0C;
  dataMemory[0x003] = 0x00; // 0xFF
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
