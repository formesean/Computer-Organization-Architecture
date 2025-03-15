#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

// Arithmetic & Logical (Instruction Code)
#define ADD 0x1E
#define SUB 0x1D
#define MUL 0x1B
#define AND 0x1A
#define OR 0x19
#define NOT 0x18
#define XOR 0x17
#define SHL 0x16
#define SHR 0x15

// Data Movement (Instruction code)
#define WM 0x01
#define RM 0x02
#define RIO 0x04
#define WIO 0x05
#define WB 0x06
#define WIB 0x07
#define WACC 0x09
#define RACC 0x0B
#define SWAP 0x0E

// Program Control (Instruction Code)
#define BR 0x03
#define BRE 0x14
#define BRNE 0x13
#define BRGT 0x12
#define BRLT 0x11
#define EOP 0x1F

// Global variables

// Variables so that we can print the operand1 and acc globally
unsigned int global_OPERAND1 = 0x000;
unsigned int global_ACC = 0x000;

// Flags
unsigned char FLAGS = 0x00;

// Memory Arrays
unsigned char dataMemory[2048];
unsigned char ioBuffer[32];

// External Control Signal
unsigned char CONTROL = 0x00;
int IOM = 0;
int RW = 0;
int OE = 0;

// Buses
unsigned int ADDR = 0x000;
unsigned char BUS = 0x00;

// Function prototypes

// Arithmetic & Logic Unit
int ALU();
unsigned char twosComp(unsigned char data);
unsigned char setFlags(unsigned int ACC);

// Arithmetic functions
unsigned char arithmeticAddition(unsigned char operand1, unsigned char operand2);
unsigned char arithmeticSubtraction(unsigned char operand1, unsigned char operand2);
unsigned char arithmeticMultiplication(unsigned char operand1, unsigned char operand2);

// Logic functions
unsigned char logicalAND(unsigned char operand1, unsigned char operand2);
unsigned char logicalOR(unsigned char operand1, unsigned char operand2);
unsigned char logicalNOT(unsigned char operand1);
unsigned char logicalXOR(unsigned char operand1, unsigned char operand2);
unsigned char logicalShiftRight(unsigned char operand1, int numberOfBits);
unsigned char logicalShiftLeft(unsigned char operand1, int numberOfBits);

unsigned char getLSB_8bit(unsigned char data);
unsigned char getMSB_8bit(unsigned char data);
void printBin(int data, unsigned char data_width);
void printBoothsTable(unsigned char A, unsigned char Q, unsigned char Qneg1, unsigned char M);
void printACC(unsigned int ACC, int numberOfBits);

// Control Unit functions
int CU();
void mainMemory();
void IOMemory();
void initMemory();

/*============================================================================================================*/
int main()
{
  initMemory(); // populate memory

  if (CU() == 1)
  {
  }
  else
    printf("Error encountered, program terminated!");

  return 0;
}

/*==========================================Arithmetic & Logic Unit==========================================*/
int ALU()
{
  static int ACC = 0x000;
  global_OPERAND1 = ACC;

  switch (CONTROL)
  {
  case ADD: // Addition
    ACC = arithmeticAddition(ACC, BUS);
    global_ACC = ACC;
    break;
  case SUB: // Subtraction
    ACC = arithmeticSubtraction(ACC, BUS);
    global_ACC = ACC;
    break;
  case MUL: // Multiplication
    ACC = arithmeticMultiplication(ACC, BUS);
    global_ACC = ACC;
    break;
  case AND: // AND
    ACC = logicalAND(ACC, BUS);
    global_ACC = ACC;
    break;
  case OR: // OR
    ACC = logicalOR(ACC, BUS);
    global_ACC = ACC;
    break;
  case NOT: // NOT
    ACC = logicalNOT(ACC);
    global_ACC = ACC;
    break;
  case XOR: // XOR
    ACC = logicalXOR(ACC, BUS);
    global_ACC = ACC;
    break;
  case SHR: // Shift Right
    ACC = logicalShiftRight(ACC, BUS);
    global_ACC = ACC;
    break;
  case SHL: // Shift Left
    ACC = logicalShiftLeft(ACC, BUS);
    global_ACC = ACC;
    break;
  case WACC:
    ACC = (unsigned char)BUS;
    global_ACC = ACC;
    break;
  case RACC:
    BUS = ACC;
    global_ACC = ACC;
    break;
  case BRE:
    ACC = (ACC + twosComp(BUS)) & 0x00FF;
    global_ACC = ACC;
    break;
  case BRNE:
    ACC = (ACC + twosComp(BUS)) & 0x00FF;
    global_ACC = ACC;
    break;
  case BRGT:
    ACC = (ACC + twosComp(BUS)) & 0x00FF;
    global_ACC = ACC;
    break;
  case BRLT:
    ACC = (ACC + twosComp(BUS)) & 0x00FF;
    global_ACC = ACC;
    break;
  default:
    printf("Out of range!\n");
  }

  setFlags(ACC);
}

/*============================================================================================================*/
// Function to set the zero, overflow, sign and carry flags
unsigned char setFlags(unsigned int ACC)
{
  FLAGS ^= FLAGS;

  if (CONTROL == 0x17 || CONTROL == 0x18 || CONTROL == 0x19 || CONTROL == 0x1A)
    FLAGS &= 0x01;

  if (CONTROL == 0x15 || CONTROL == 0x16)
    FLAGS &= 0x02;

  if (ACC == 0x0000)
    FLAGS |= 0x01;

  if (ACC > 0x7F)
    FLAGS |= 0x80;

  if ((ACC & 0x0080) == 0x0080)
    FLAGS |= 0x04;

  if (ACC > 0xFF)
    FLAGS |= 0x02;
}

// Function to take the 2’s complement of a number
unsigned char twosComp(unsigned char data)
{
  return ~data + 1;
}

// Function to print ACC in binary characters of data
void printACC(unsigned int ACC, int numberOfBits)
{
  printf("\nACC = ");
  printBin(ACC, numberOfBits);
  printf("\n");
}

// Function to print the Booth's multiplication table
void printBoothsTable(unsigned char A, unsigned char Q, unsigned char Qneg1, unsigned char M)
{
  printBin(A, 8);
  printf(" ");
  printBin(Q, 8);
  printf("   %d", (int)Qneg1);
  printf("   ");
  printBin(M, 8);
  printf("\n");
}

// Function to print binary characters of data
void printBin(int data, unsigned char data_width)
{
  for (int i = data_width - 1; i >= 0; i--)
  {
    putchar((data & (1 << i)) ? '1' : '0');
  }
}

// Function to get the LSB of an 8-bit number
unsigned char getLSB_8bit(unsigned char data)
{
  return data & 0x01;
}

// Function to get the MSB of an 8-bit number
unsigned char getMSB_8bit(unsigned char data)
{
  return (data >> 7) & 0x01;
}

/*============================================================================================================*/
// Arithmetic functions
unsigned char arithmeticAddition(unsigned char operand1, unsigned char operand2)
{
  return operand1 + operand2;
}

unsigned char arithmeticSubtraction(unsigned char operand1, unsigned char operand2)
{
  operand2 = twosComp(operand2);
  return arithmeticAddition(operand1, operand2);
}

unsigned char arithmeticMultiplication(unsigned char operand1, unsigned char operand2)
{
  // uses Booth's Algorithm

  unsigned char A_LSB = 0x00;
  unsigned char A_MSB = 0x00;
  unsigned char ACC_local = 0x0000;

  unsigned char A = 0x00;
  unsigned char Q = operand2;
  unsigned char Qneg1 = 0x00;
  unsigned char Qsub0 = getLSB_8bit(Q);
  unsigned char M = operand1;

  for (int i = 0; i < 8; i++)
  {
    if ((int)Qsub0 == 1 && (int)Qneg1 == 0)
    {
      A = arithmeticSubtraction(A, M);
    }
    else if ((int)Qsub0 == 0 && (int)Qneg1 == 1)
    {
      A = arithmeticAddition(A, M);
    }
    A_MSB = getMSB_8bit(A);
    A_LSB = getLSB_8bit(A);
    Qneg1 = getLSB_8bit(Q);

    // Arithmetic Shift Right
    A = logicalOR((A >> 1), (A_MSB << 7));
    Q = logicalOR((Q >> 1), (A_LSB << 7));
    Qsub0 = getLSB_8bit(Q);
  }

  ACC_local = A;
  ACC_local = ACC_local << 8;
  return logicalOR(ACC_local, Q);
}

/*============================================================================================================*/
// Logic functions
unsigned char logicalAND(unsigned char operand1, unsigned char operand2)
{
  return operand1 & operand2;
}

unsigned char logicalOR(unsigned char operand1, unsigned char operand2)
{
  return operand1 | operand2;
}

unsigned char logicalNOT(unsigned char operand1)
{
  return ~(operand1);
}

unsigned char logicalXOR(unsigned char operand1, unsigned char operand2)
{
  return operand1 ^ operand2;
}

unsigned char logicalShiftRight(unsigned char operand1, int numberOfBits)
{
  return operand1 >> numberOfBits;
}

unsigned char logicalShiftLeft(unsigned char operand1, int numberOfBits)
{
  return operand1 << numberOfBits;
}

/*===============================================Control Unit===============================================*/
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
    printf("%03X\t", PC);

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

    // Decode instruction
    inst_code = (IR >> 11) & 0x1F;
    operand = IR & 0x07FF;

    if (inst_code == WM)
    {
      printf("WM\t");
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
    }
    else if (inst_code == RM)
    {
      printf("RM\t");
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

      ADDR = MAR;
      mainMemory();

      if (_MEMORY)
        MBR = BUS;
    }
    else if (inst_code == BR)
    {
      printf("BR\t");
      PC = operand;

      // set external control signals
      CONTROL = inst_code;
      OE = 1;
    }
    else if (inst_code == RIO)
    {
      printf("RIO\t");
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

      ADDR = IOAR;
      IOMemory();

      if (_IO)
        IOBR = BUS;
    }
    else if (inst_code == WIO)
    {
      printf("WIO\t");
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

      ADDR = IOAR;

      if (_IO)
        BUS = IOBR;

      IOMemory();
    }
    else if (inst_code == WB)
    {
      printf("WB\t");

      // set internal control signals, accessing memory
      _FETCH = 0;
      _MEMORY = 1;
      _IO = 0;

      // set external control signals, accessing main memory, write operation, allow data movement
      CONTROL = inst_code;
      IOM = 1;
      RW = 1;
      OE = 1;

      ADDR = PC - 1;
      mainMemory();

      if (_MEMORY)
        MBR = BUS;
    }
    else if (inst_code == WIB)
    {
      printf("WIB\t");

      // set internal control signals, accessing I/O
      _FETCH = 0;
      _MEMORY = 0;
      _IO = 1;

      // set external control signals, accessing I/O, write operation, allow data movement
      CONTROL = inst_code;
      IOM = 0;
      RW = 1;
      OE = 1;
      ADDR = PC - 1;

      if (_IO)
        IOBR = BUS;

      IOMemory();
    }
    else if (inst_code == ADD || inst_code == SUB || inst_code == MUL || inst_code == AND || inst_code == OR || inst_code == NOT || inst_code == XOR || inst_code == SHL || inst_code == SHR || inst_code == WACC)
    {
      switch (inst_code)
      {
      case ADD:
        printf("ADD\t");
        break;
      case SUB:
        printf("SUB\t");
        break;
      case MUL:
        printf("MUL\t");
        break;
      case AND:
        printf("AND\t");
        break;
      case OR:
        printf("OR\t");
        break;
      case NOT:
        printf("NOT\t");
        break;
      case XOR:
        printf("XOR\t");
        break;
      case SHL:
        printf("SHL\t");
        break;
      case SHR:
        printf("SHR\t");
        break;
      case WACC:
        printf("WACC\t");
        break;
      }

      // set internal control signals
      _FETCH = 0;
      _MEMORY = 1;
      _IO = 0;

      // set external control signals
      CONTROL = inst_code;
      IOM = 0;
      RW = 0;
      OE = 0;

      if (_MEMORY)
        BUS = MBR;

      ALU();
    }
    else if (inst_code == RACC)
    {
      printf("RACC\t");

      // set internal control signals
      _FETCH = 0;
      _MEMORY = 1;
      _IO = 0;

      // set external control signals
      CONTROL = inst_code;
      IOM = 0;
      RW = 0;
      OE = 0;

      ALU();
      if (_MEMORY)
        MBR = BUS;
    }
    else if (inst_code == SWAP)
    {
      unsigned char temp;
      printf("SWAP\t");

      // set internal control signals
      _FETCH = 0;
      _MEMORY = 0;
      _IO = 0;

      // set external control signals
      CONTROL = inst_code;
      IOM = 0;
      RW = 0;
      OE = 0;

      MBR = MBR ^ IOBR;
      IOBR = MBR ^ IOBR;
      MBR = MBR ^ IOBR;
    }
    else if (inst_code == BRE || inst_code == BRNE || inst_code == BRGT || inst_code == BRLT)
    {
      switch (inst_code)
      {
      case BRE:
        printf("BRE\t");
        break;
      case BRNE:
        printf("BRNE\t");
        break;
      case BRGT:
        printf("BRGT\t");
        break;
      case BRLT:
        printf("BRLT\t");
        break;
      }

      // set internal control signals
      _FETCH = 0;
      _MEMORY = 1;
      _IO = 0;

      // set external control signals
      CONTROL = inst_code;
      IOM = 0;
      RW = 0;
      OE = 0;

      ADDR = operand;

      if (_MEMORY)
        BUS = MBR;

      ALU();

      switch (inst_code)
      {
      case BRE:
        if (FLAGS & 0x01) // Z
        {
          PC = ADDR;
          OE = 1;
        }
        break;
      case BRNE:
        if (!(FLAGS & 0x01)) // !Z
        {
          PC = ADDR;
          OE = 1;
        }
        break;
      case BRGT:
        if (!((FLAGS >> 2) & 0x01)) // !SF
        {
          PC = ADDR;
          OE = 1;
        }
        break;
      case BRLT:
        if ((FLAGS >> 2) & 0x01) // SF
        {
          PC = ADDR;
          OE = 1;
        }
        break;
      }
    }

    if (operand == 0)
      printf("\t\t");
    else
      printf("0x%03X\t\t", operand);

    switch (inst_code)
    {
    case WB:
      printf("; MBR = 0x%02X\n", MBR);
      break;

    case WM:
      printf("; dataMemory[0x%03X] : 0x%02X\n", operand, BUS);
      break;

    case WACC:
      printf("; ACC = 0x%02X\n", global_ACC);
      break;

    case ADD:
      printf("; ACC = (0x%02X) + (0x%02X) = (0x%02X)\t\t", global_OPERAND1, BUS, global_ACC);
      printf("ZF=%d, CF=%d, SF=%d, OF=%d\n", FLAGS & 0x01, (FLAGS >> 1) & 0x01, (FLAGS >> 2) & 0x01, (FLAGS >> 7) & 0x01);
      break;

    case RM:
      printf("; MBR = 0x%02X\n", MBR);
      break;

    case MUL:
      printf("; ACC = (0x%02X) x (0x%02X) = (0x%02X)\t\t", global_OPERAND1, BUS, global_ACC);
      printf("ZF=%d, CF=%d, SF=%d, OF=%d\n", FLAGS & 0x01, (FLAGS >> 1) & 0x01, (FLAGS >> 2) & 0x01, (FLAGS >> 7) & 0x01);
      break;

    case RACC:
      printf("; MBR = 0x%02X\n", MBR);
      break;

    case WIB:
      printf("; IOBR = 0x%02X\n", IOBR);
      break;

    case WIO:
      printf("; ioBuffer[0x%03X] : 0x%02X\n", operand, BUS);
      break;

    case SUB:
      printf("; ACC = (0x%02X) - (0x%02X) = (0x%01X)\t\t", global_OPERAND1, BUS, global_ACC);
      printf("ZF=%d, CF=%d, SF=%d, OF=%d\n", FLAGS & 0x01, (FLAGS >> 1) & 0x01, (FLAGS >> 2) & 0x01, (FLAGS >> 7) & 0x01);
      break;

    case SHL:
      printf("; ACC = (0x%02X) << (0x%02X) = (%d)\t\t", global_OPERAND1, (unsigned int)BUS, global_ACC);
      printf("ZF=%d, CF=%d, SF=%d, OF=%d\n", FLAGS & 0x01, (FLAGS >> 1) & 0x01, (FLAGS >> 2) & 0x01, (FLAGS >> 7) & 0x01);
      break;

    case SHR:
      printf("; ACC = (0x%02X) >> (0x%02X) = (%d)\t\t", global_OPERAND1, (unsigned int)BUS, global_ACC);
      printf("ZF=%d, CF=%d, SF=%d, OF=%d\n", FLAGS & 0x01, (FLAGS >> 1) & 0x01, (FLAGS >> 2) & 0x01, (FLAGS >> 7) & 0x01);
      break;

    case OR:
      printf("; ACC = (0x%02X) OR (0x%02X) = (0x%02X)\t\t", global_OPERAND1, BUS, global_ACC);
      printf("ZF=%d, CF=%d, SF=%d, OF=%d\n", FLAGS & 0x01, (FLAGS >> 1) & 0x01, (FLAGS >> 2) & 0x01, (FLAGS >> 7) & 0x01);
      break;

    case NOT:
      printf("; ACC = NOT (0x%02X) = (0x%02X)\t\t", global_OPERAND1, global_ACC);
      printf("ZF=%d, CF=%d, SF=%d, OF=%d\n", FLAGS & 0x01, (FLAGS >> 1) & 0x01, (FLAGS >> 2) & 0x01, (FLAGS >> 7) & 0x01);
      break;

    case RIO:
      printf("; IOBR = 0x%02X\n", IOBR);
      break;

    case SWAP:
      printf("; MBR = 0x%02X, IOBR = 0x%02X\n", MBR, IOBR);
      break;

    case XOR:
      printf("; ACC = (0x%02X) XOR (0x%02X) = (0x%02X)\t\t", global_OPERAND1, BUS, global_ACC);
      printf("ZF=%d, CF=%d, SF=%d, OF=%d\n", FLAGS & 0x01, (FLAGS >> 1) & 0x01, (FLAGS >> 2) & 0x01, (FLAGS >> 7) & 0x01);
      break;

    case AND:
      printf("; ACC = (0x%02X) AND (0x%02X) = (0x%02X)\t\t", global_OPERAND1, BUS, global_ACC);
      printf("ZF=%d, CF=%d, SF=%d, OF=%d\n", FLAGS & 0x01, (FLAGS >> 1) & 0x01, (FLAGS >> 2) & 0x01, (FLAGS >> 7) & 0x01);
      break;

    case BRE:
      printf("; ACC = (0x%02X) - (0x%02X) = (0x%02X)\t\t", global_OPERAND1, BUS, global_ACC);
      printf("ZF=%d, CF=%d, SF=%d, OF=%d\n", FLAGS & 0x01, (FLAGS >> 1) & 0x01, (FLAGS >> 2) & 0x01, (FLAGS >> 7) & 0x01);
      break;

    case BRGT:
      printf("; ACC = (0x%02X) - (0x%02X) = (0x%02X)\t\t", global_OPERAND1, BUS, global_ACC);
      printf("ZF=%d, CF=%d, SF=%d, OF=%d\n", FLAGS & 0x01, (FLAGS >> 1) & 0x01, (FLAGS >> 2) & 0x01, (FLAGS >> 7) & 0x01);
      break;

    case BRLT:
      printf("; ACC = (0x%02X) - (0x%02X) = (0x%02X)\t\t", global_OPERAND1, BUS, global_ACC);
      printf("ZF=%d, CF=%d, SF=%d, OF=%d\n", FLAGS & 0x01, (FLAGS >> 1) & 0x01, (FLAGS >> 2) & 0x01, (FLAGS >> 7) & 0x01);
      break;

    case BR:
      printf("; PC <- 0x%03X", PC);

    case EOP:
      printf("\n%03X\t", PC+10);
      printf("EOP\n");
      exitCode = 1;
      return 1;
      break;

    default:
      break;
    }

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
  dataMemory[0x001] = 0x15;
  dataMemory[0x002] = 0x0C;
  dataMemory[0x003] = 0x00;
  dataMemory[0x004] = 0x30;
  dataMemory[0x005] = 0x05;
  dataMemory[0x006] = 0x48;
  dataMemory[0x007] = 0x00;
  dataMemory[0x008] = 0x30;
  dataMemory[0x009] = 0x08;
  dataMemory[0x00A] = 0xF0;
  dataMemory[0x00B] = 0x00;
  dataMemory[0x00C] = 0x14;
  dataMemory[0x00D] = 0x00;
  dataMemory[0x00E] = 0xD8;
  dataMemory[0x00F] = 0x00;
  dataMemory[0x010] = 0x58;
  dataMemory[0x011] = 0x00;
  dataMemory[0x012] = 0x0C;
  dataMemory[0x013] = 0x01;
  dataMemory[0x014] = 0x38;
  dataMemory[0x015] = 0x0B;
  dataMemory[0x016] = 0x28;
  dataMemory[0x017] = 0x00;
  dataMemory[0x018] = 0x30;
  dataMemory[0x019] = 0x10;
  dataMemory[0x01A] = 0xE8;
  dataMemory[0x01B] = 0x00;
  dataMemory[0x01C] = 0x58;
  dataMemory[0x01D] = 0x00;
  dataMemory[0x01E] = 0x28;
  dataMemory[0x01F] = 0x01;
  dataMemory[0x020] = 0xB0;
  dataMemory[0x021] = 0x00;
  dataMemory[0x022] = 0xB0;
  dataMemory[0x023] = 0x00;
  dataMemory[0x024] = 0xA8;
  dataMemory[0x025] = 0x00;
  dataMemory[0x026] = 0x14;
  dataMemory[0x027] = 0x01;
  dataMemory[0x028] = 0xC8;
  dataMemory[0x029] = 0x00;
  dataMemory[0x02A] = 0xC0;
  dataMemory[0x02B] = 0x00;
  dataMemory[0x02C] = 0x20;
  dataMemory[0x02D] = 0x01;
  dataMemory[0x02E] = 0x70;
  dataMemory[0x02F] = 0x00;
  dataMemory[0x030] = 0xB8;
  dataMemory[0x031] = 0x00;
  dataMemory[0x032] = 0x30;
  dataMemory[0x033] = 0xFF;
  dataMemory[0x034] = 0xD0;
  dataMemory[0x035] = 0x00;
  dataMemory[0x036] = 0x14;
  dataMemory[0x037] = 0x01;
  dataMemory[0x038] = 0xA0;
  dataMemory[0x039] = 0x3C;
  dataMemory[0x03A] = 0x30;
  dataMemory[0x03B] = 0xF0;
  dataMemory[0x03C] = 0x90;
  dataMemory[0x03D] = 0x40;
  dataMemory[0x03E] = 0x88;
  dataMemory[0x03F] = 0x44;
  dataMemory[0x040] = 0x30;
  dataMemory[0x041] = 0x00;
  dataMemory[0x042] = 0x48;
  dataMemory[0x043] = 0x00;
  dataMemory[0x044] = 0x30;
  dataMemory[0x045] = 0x03;
  dataMemory[0x046] = 0x48;
  dataMemory[0x047] = 0x00;
  dataMemory[0x048] = 0x30;
  dataMemory[0x049] = 0x00;
  dataMemory[0x04A] = 0xA0;
  dataMemory[0x04B] = 0x52;
  dataMemory[0x04C] = 0x30;
  dataMemory[0x04D] = 0x01;
  dataMemory[0x04E] = 0xE8;
  dataMemory[0x04F] = 0x00;
  dataMemory[0x050] = 0x18;
  dataMemory[0x051] = 0x48;
  dataMemory[0x052] = 0xF8;
  dataMemory[0x053] = 0x00;
}
