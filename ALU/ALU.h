#ifndef ALU_H
#define ALU_H

#include <stdio.h>

// Global flags and ACC
extern unsigned char C, Z, OF, SF, ACC;

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

#endif
