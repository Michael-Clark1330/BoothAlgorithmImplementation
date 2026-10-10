/* Lab 1: Booth Algorithm Implementation
 * CMPE220 Computer Organization
 * Programmer: Michael Clark
 * Professor: Dr. Belkhouche
 * File Created: October 05, 2026
 * File Updated: October 09, 2026
 */

#include <stdio.h>
#include <string.h>

/*
 * Booth Algorithm - Binary Multiplication
 *   
 *   How my program works:
 *   1. Takes a number of bits desired by user (either 4 or 8)
 *   2. Takes two binary numbers seperately as input variables
 *   3. Displays the inputs in binary, then converts them to decimal (also shows them in decimal)
 *   4. Displays each step of the Booth Algorithm
 *   5. Displays the final answer in binary and decimal
 *
 *   Variable Names:
 *   A = Accumulator
 *   M = Multiplicand
 *   Q = Multiplier
 *   P = Previous Q bit (Q-1)
 *   C = Counter
 *   Z = Product
 */

// Print number using specified number of bits
void printBinary(int value, int bits)
{
    int i;
    for (i = bits - 1; i >= 0; i--)
    {
        printf("%d", (value >> i) & 1);

        // Add space between nibbles for readability
        if (i >> 0 && i % 4 == 0)
            printf(" ");
    }
}

// Unsigned bit --> 2's C int
int signedValue(int value, int bits)
{
    int signedBit = 1 << (bits - 1);

    if (value & signedBit)
        return value - (1 << bits);

    return value;
}

/*
 * Keep only the specified number of bits.
 *
 * For example, with 4 bits:
 *   1001 0101 -> only the lowest 4 bits are kept
 */
int keepBits(int value, int bits)
{
    int mask = (1 << bits) - 1;
    return value & mask;
}

/*
 * Perform ARS on the A, Q, & P registers
 *
 * Before:
 *       A        Q       P
 *   [ A bits ][ Q bits ][P]
 * After:
 *       A        Q       P
 *   [signed bit][A***][Q***][Q0]
 *
 * A's signed bit stays
 */
void arithmeticRightShift(int *A, int *Q, int *P, int bits)
{
    int oldQ0;
    int signedBit;
    int newA;

    oldQ0 = *Q & 1;

    // Save A's signed bit
    signedBit = *A & (1 << (bits - 1));

    /*
     * Shift A right by one
     * Signed bit must be brought back into the leftmost position
     */
    newA = *A >> 1;

    if (signedBit)
        newA |= (1 << (bits - 1));

    // A's LSB --> Q's MSB
    if (*A & 1)
        *Q |= (1 << (bits - 1));
    else
        *Q &= ~(1 << (bits - 1));

    *A = keepBits(newA, bits);

    // Q0 --> P
    *P = oldQ0;
}

// Print current state of the Booth registers
void printState(int A, int Q, int P, int M, int bits)
{
    printf("A = ");
    printBinary(A, bits);

    printf("    Q = ");
    printBinary(Q, bits);

    printf("    P = %d    M = ", P);
    printBinary(M, bits);

    printf("\n");
}

/*
 * Booth Algorithm (for multiplication)
 *
 * M = multiplicand
 * Q = multiplier
 * number of bits = 4 or 8
 */
int multiply(int M, int Q, int bits)
{
    int A = 0;
    int P = 0;
    int C = bits;
    int originalM = M;
    int originalQ = Q;

    // Convert M and Q into their bit representations
    M = keepBits(M, bits);
    Q = keepBits(Q, bits);

    printf("\n");
    printf("=================================\n");
    printf("Booth Algorithm for %d x %d:\n",
           signedValue(originalM, bits),
           signedValue(originalQ, bits));
    printf("=================================\n");


    printf("\nInitial values:\n");
    printf("A = ");
    printBinary(A, bits);

    printf("    Q = ");
    printBinary(Q, bits);

    printf("    P = %d    M = ", P);
    printBinary(M, bits);

    printf("\n\n");

    // Booth algorithm
    while (C > 0)
    {
        int q0 = Q & 1;

        printf("Cycle %d:\n", bits - C + 1);

        /* Case 1: P Q0 = 00 or P Q0 = 11
         * ARS only
         */
        if ((P == 0 && q0 == 0) ||
            (P == 1 && q0 == 1))
        {
            printf("  P,Q0 = %d%d -> ARS\n", P, q0);


            arithmeticRightShift(&A, &Q, &P, bits);
        }

        /* Case 2: P Q0 = 10
         * A = A + M
         */
        else if (P == 1 && q0 == 0)
        {
            printf("  P,Q0 = 10 -> A = A + M\n");

            A = A + M;
            A = keepBits(A, bits);

            printf("  After A + M: A = ");
            printBinary(A, bits);
            printf("\n");

            arithmeticRightShift(&A, &Q, &P, bits);
        }

        /* Case 3: P Q0 = 01
         * A = A - M (Equivalent to A = A + 2's C(M))
         */
        else if (P == 0 && q0 == 1)
        {
            printf("  P,Q0 = 01 -> A = A - M\n");

            A = A - M;
            A = keepBits(A, bits);

            printf("  After A - M: A = ");
            printBinary(A, bits);
            printf("\n");

            arithmeticRightShift(&A, &Q, &P, bits);
        }

        printf("  After ARS:   ");
        printState(A, Q, P, M, bits);
        
        // post-decrement step counter
        C--;
        printf("  C = %d\n\n", C);
    }

        // Combine A and Q to create the final (2 * bits) product
    {
        int result = (A << bits) | Q;
        int resultBits = bits * 2;

        printf("Final result: Z = ");
        printBinary(A, bits);
        printf(" ");
        printBinary(Q, bits);

        printf("\n");

        printf("Decimal result = %d\n",
               signedValue(result, resultBits));

        printf("Binary result  = ");
        printBinary(result, resultBits);
        printf("\n");

        return signedValue(result, resultBits);
    }
}

int main()
{
    int bits;
    char M_input[20];
    char Q_input[20];

    int M = 0;
    int Q = 0;
    int i;

    // User chooses between 4-bit and 8-bit options
    printf("Enter number of bits (4 or 8): ");
    scanf("%d", &bits);

    // if user enters any other number of bits besides 4 or 8, print error msg
    if (bits != 4 && bits != 8)
    {
        printf("Error: Number of bits must be 4 or 8.\n");
        return 1;
    }

    // prompt user for multiplicand then take input
    printf("\nEnter the multiplicand M (%d-bit binary): ", bits);
    scanf("%s", M_input);

    // prompt user for multiplier then take input
    printf("Enter the multiplier Q (%d-bit binary): ", bits);
    scanf("%s", Q_input);

    // Binary --> decimal int
    for (i = 0; i < bits; i++)
    {
        M = M << 1;

        if (M_input[i] == '1')
            M = M | 1;

        Q = Q << 1;

        if (Q_input[i] == '1')
            Q = Q | 1;
    }

    // Display input values in decimal (as shown on page 3)
    printf("\nMultiplying M x Q = %d x %d\n", M, Q);

    printf("M = ");
    printBinary(M, bits);
    printf(" = %d\n", signedValue(M, bits));

    printf("Q = ");
    printBinary(Q, bits);
    printf(" = %d\n", signedValue(Q, bits));

    // Perform Booth multiplication
    multiply(M, Q, bits);

    return 0;
}
