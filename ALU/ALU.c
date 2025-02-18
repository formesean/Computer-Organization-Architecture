#include <stdio.h>

// Global flags and ACC
unsigned char C = 0;
unsigned char Z = 0;
unsigned char OF = 0;
unsigned char SF = 0;
unsigned char ACC = 0;

// Function prototypes
int ALU(unsigned char operand1, unsigned char operand2, unsigned char control_signals);
unsigned char twosComp(unsigned char data);
unsigned char setFlags(unsigned int ACC);
void printBin(int data, unsigned char data_width);
void printBoothsTable(unsigned char A, unsigned char Q, unsigned char Qneg1, unsigned char M);
unsigned char getLSB_8bit(unsigned char data);
unsigned char getMSB_8bit(unsigned char data);

// Arithmetic functions
unsigned char add(unsigned char operand1, unsigned char operand2);
unsigned char subtract(unsigned char operand1, unsigned char operand2);
unsigned char multiply(unsigned char operand1, unsigned char operand2);

// Logic functions
unsigned char AND(unsigned char operand1, unsigned char operand2);
unsigned char OR(unsigned char operand1, unsigned char operand2);
unsigned char NOT(unsigned char operand1);
unsigned char XOR(unsigned char operand1, unsigned char operand2);
unsigned char shiftRight(unsigned char operand1);
unsigned char shiftLeft(unsigned char operand1);

void main()
{
    // ALU(0x03, 0x05, 0x02); // 00000011 - 00000101 (Subtract)
    // ALU(0x88, 0x85, 0x01); // 10001000 + 10000101 (Add)
    // ALU(0xC0, 0x0A, 0x03); // 11000000 * 00000010 (Multiply)
    // ALU(0x46, 0x02, 0x03); // 01000110 * 00000010 (Multiply)

    ALU(0x04,0x02,0x01); // Addition
	ALU(0x04,0x02,0x02); // Subtraction
	ALU(0x09,0x03,0x03); // Multiplication
	ALU(0x04,0x02,0x04); // AND
	ALU(0x04,0x02,0x05); // OR
	ALU(0x04,0x02,0x06); // NOT
	ALU(0x04,0x02,0x07); // XOR
	ALU(0xFF,0x03,0x08); // Shift Right
	ALU(0xFF,0x06,0x09); // Shift Left
}

// 0x01 - Addition
// 0x02 - Subtraction
// 0x03 - Multiplication
// 0x04 - AND
// 0x05 - OR
// 0x06 - NOT
// 0x07 - XOR
// 0x08 - Shift Right (logical)
// 0x09 - Shift Left (logical)
int ALU(unsigned char operand1, unsigned char operand2, unsigned char control_signals)
{
    printf("\n*****************************************");
    printf("\nFetching operands...");
    printf("\nOP1 = ");
    printBin(operand1, 8);
    printf("\nOP2 = ");
    printBin(operand2, 8);

    unsigned int temp_ACC = 0;

    switch (control_signals)
    {
    case 0x01: // Addition
        printf("\nOperation = ADD");
        printf("\nAdding OP1 & OP2");
        temp_ACC = add(operand1, operand2);
        break;
    case 0x02: // Subtraction
        printf("\nOperation = SUB");
        printf("\n2's complement OP2");
        printf("\nAdding OP1 & OP2");
        temp_ACC = subtract(operand1, operand2);
        break;
    case 0x03: // Multiplication
        printf("\nOperation = MUL");
        temp_ACC = multiply(operand1, operand2);
        break;
    case 0x04: // AND
        printf("\nOperation = AND");
        temp_ACC = AND(operand1, operand2);
        break;
    case 0x05: // OR
        printf("\nOperation = OR");
        temp_ACC = OR(operand1, operand2);
        break;
    case 0x06: // NOT
        printf("\nOperation = NOT");
        temp_ACC = NOT(operand1);
        break;
    case 0x07: // XOR
        printf("\nOperation = XOR");
        temp_ACC = XOR(operand1, operand2);
        break;
    case 0x08: // Shift Right
        printf("\nOperation = Shift Right");
        temp_ACC = shiftRight(operand1);
        break;
    case 0x09: // Shift Left
        printf("\nOperation = Shift Left");
        temp_ACC = shiftLeft(operand1);
        break;
    default:
        ACC = 0;
        C = Z = OF = SF = 0;
        break;
    }

    printf("\nACC = ");
    printBin(temp_ACC, 16);
    printf("\nZF=%d, CF=%d, SF=%d, OF=%d\n", Z, C, SF, OF);

    return ACC;
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

// Function to take the 2’s complement of a number
unsigned char twosComp(unsigned char data)
{
    return ~data + 1;
}

// Function to set the zero, overflow, sign and carry flags
unsigned char setFlags(unsigned int ACC)
{
    C = (ACC > 0xFF) ? 1 : 0;
    Z = ((ACC & 0xFF) == 0) ? 1 : 0;
    SF = (ACC & 0x80) ? 1 : 0;

    unsigned char upper = (ACC >> 8) & 0xFF;
    unsigned char lower = ACC & 0xFF;
    unsigned char expected_upper = (lower & 0x80) ? 0xFF : 0x00;

    OF = (upper != expected_upper) ? 1 : 0;
}

// Print binary characters of data
void printBin(int data, unsigned char data_width)
{
    for (int i = data_width - 1; i >= 0; i--)
    {
        putchar((data & (1 << i)) ? '1' : '0');
    }
}

// Arithmetic functions
unsigned char add(unsigned char operand1, unsigned char operand2)
{
    unsigned int temp_ACC = operand1 + operand2;

    ACC = temp_ACC & 0xFF;
    setFlags(temp_ACC);
    OF = (((operand1 & 0x80) == (operand2 & 0x80)) && ((ACC & 0x80) != (operand1 & 0x80)));

    return ACC;
}

unsigned char subtract(unsigned char operand1, unsigned char operand2)
{
    unsigned char comp_op2 = twosComp(operand2);
    unsigned int temp_ACC = operand1 + comp_op2;

    ACC = temp_ACC & 0xFF;
    setFlags(temp_ACC);
    OF = (((operand1 ^ comp_op2) & 0x80) == 0) && (((operand1 ^ ACC) & 0x80) != 0);

    return ACC;
}

unsigned char multiply(unsigned char operand1, unsigned char operand2)
{
    unsigned char A_LSB = 0x00;
    unsigned char A_MSB = 0x00;
    unsigned char ACC_local = 0x0000;

    unsigned char A = 0x00;
    unsigned char Q = operand2;
    unsigned char Qneg1 = 0x00;
    unsigned char Qsub0 = getLSB_8bit(Q);
    unsigned char M = operand1;

    printf("\n    A        Q     Q_-1     M\n");
    printBoothsTable(A, Q, Qneg1, M);

    for (int i = 0; i < 8; i++)
    {
        if ((int)Qsub0 == 1 && (int)Qneg1 == 0)
        {
            A = subtract(A, M);
            printBoothsTable(A, Q, Qneg1, M);
        }
        else if ((int)Qsub0 == 0 && (int)Qneg1 == 1)
        {
            A = add(A, M);
            printBoothsTable(A, Q, Qneg1, M);
        }
        A_MSB = getMSB_8bit(A);
        A_LSB = getLSB_8bit(A);
        Qneg1 = getLSB_8bit(Q);

        // Arithmetic Shift Right
        A = OR((A >> 1), (A_MSB << 7));
        Q = OR((Q >> 1), (A_LSB << 7));
        Qsub0 = getLSB_8bit(Q);

        printBoothsTable(A, Q, Qneg1, M);
    }

    ACC_local = A;
    ACC_local = ACC_local << 8;
    return OR(ACC_local, Q);
}

// Logic functions
unsigned char AND(unsigned char operand1, unsigned char operand2)
{
    ACC = operand1 & operand2;
    setFlags(ACC);
    OF = 0;
    return ACC;
}

unsigned char OR(unsigned char operand1, unsigned char operand2)
{
    ACC = operand1 | operand2;
    setFlags(ACC);
    OF = 0;
    return ACC;
}

unsigned char NOT(unsigned char operand1)
{
    ACC = ~operand1;
    setFlags(ACC);
    OF = 0;
    return ACC;
}

unsigned char XOR(unsigned char operand1, unsigned char operand2)
{
    ACC = operand1 ^ operand2;
    setFlags(ACC);
    OF = 0;
    return ACC;
}

unsigned char shiftRight(unsigned char operand1)
{
    C = operand1 & 0x01;
    ACC = operand1 >> 1;
    setFlags(ACC);
    OF = 0;
    return ACC;
}

unsigned char shiftLeft(unsigned char operand1)
{
    C = (operand1 & 0x80) >> 7;
    ACC = operand1 << 1;
    setFlags(ACC);
    OF = 0;
    return ACC;
}
