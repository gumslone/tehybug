/***************************************************************************************************************************

	Utilities Library

	Copyright (c) 2021 Josep Comas

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.

***************************************************************************************************************************/


#include "utils.h"


/* Print nibble as hex */
void printNibble(char byte) {
    char value = byte & 0xF;  // Cast to nibble

    if (value >= 10) {
        value += 7;
    }
    Serial.print((char)(48 + value));
}


/* Print a byte as hex */
void printByte(char byte) {
    printNibble(byte >> 4);  // high value
    printNibble(byte);       // low value
}


/* Print several bytes as hex */
void printHex(char *bytes, int size, bool space) {
    for (int i=0; i<size; i++) {
        printByte(bytes[i]);
        if (space)
            Serial.print(" ");
    }

}


/* Print an integer as hexadecimal value */
void printIntToHex(int32_t value, int size) {
    if (size > 3)
        printByte(value >> 24);
    if (size > 2)
        printByte(value >> 16);
    if (size > 1)
        printByte(value >> 8);
    printByte(value);
}


/* Show a number in binary */
void printBinary(int16_t number) {
    int16_t k;

    for (int8_t c = 15; c >= 0; c--)
    {
        k = number >> c;

        if (k & 1) {
            Serial.print("1");
        } else {
            Serial.print("0");
        }

    }
}
