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


#ifndef _JC_UTILS_H
    #define _JC_UTILS_H

    // Boards with a second hardware serial port we don't use sofware serial library
    #if defined ARDUINO_ARCH_SAMD || defined ARDUINO_ARCH_SAM21D || defined ARDUINO_ARCH_ESP32 || defined ARDUINO_SAM_DUE ||  \
        defined ARDUINO_ARCH_APOLLO3 || defined ARDUINO_ARCH_RP2040
        #undef USE_SOFTWARE_SERIAL
    #else
        #define USE_SOFTWARE_SERIAL
    #endif

    #include "Arduino.h"

    #ifdef USE_SOFTWARE_SERIAL
        #include <SoftwareSerial.h>
    #endif


    /*
        Overloading macro
        https://stackoverflow.com/questions/11761703/overloading-macro-on-number-of-arguments
    */

    // get number of arguments with __NARG__
    #define __NARG__(...)  __NARG_I_(__VA_ARGS__,__RSEQ_N())
    #define __NARG_I_(...) __ARG_N(__VA_ARGS__)
    #define __ARG_N( \
        _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
        _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
        _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
        _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
        _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
        _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
        _61,_62,_63,N,...) N
    #define __RSEQ_N() \
        63,62,61,60,                   \
        59,58,57,56,55,54,53,52,51,50, \
        49,48,47,46,45,44,43,42,41,40, \
        39,38,37,36,35,34,33,32,31,30, \
        29,28,27,26,25,24,23,22,21,20, \
        19,18,17,16,15,14,13,12,11,10, \
        9,8,7,6,5,4,3,2,1,0

    // general definition for any function name
    #define _VFUNC_(name, n) name##n
    #define _VFUNC(name, n) _VFUNC_(name, n)
    #define VFUNC(func, ...) _VFUNC(func, __NARG__(__VA_ARGS__)) (__VA_ARGS__)

    /* End Overloading macro */


    /* Debug defines & macros */

    #define LOG_DEBUG_LEVEL_NONE       (0)  // Don't show debug info
    #define LOG_DEBUG_LEVEL_ERROR      (1)  // Show errors
    #define LOG_DEBUG_LEVEL_WARN       (2)  // Show warnings
    #define LOG_DEBUG_LEVEL_INFO       (3)  // Show information
    #define LOG_DEBUG_LEVEL_DEBUG      (4)  // Not used, reserved for compatibility
    #define LOG_DEBUG_LEVEL_VERBOSE    (5)  // Show additional information

    #ifdef CORE_DEBUG_LEVEL
        #define LOG_DEBUG_LEVEL CORE_DEBUG_LEVEL
    #else
        #define LOG_DEBUG_LEVEL LOG_DEBUG_LEVEL_NONE
    #endif


    #if (LOG_DEBUG_LEVEL > LOG_DEBUG_LEVEL_NONE)

        // Using Serial.print(value) instead of printf for automatic type printing
        // Other debug libraries:
        //  https://github.com/bblanchon/ArduinoTrace
        //  https://github.com/hideakitai/DebugLog

        #define _LOG_DEBUG(...) VFUNC(_LOG_DEBUG, __VA_ARGS__)

        #define _LOG_DEBUG1(info)                   { Serial.println(info); }
        #define _LOG_DEBUG2(info, value)            { Serial.print(info); Serial.print(value); Serial.println(""); }
        #define _LOG_DEBUG3(info, value, info2)     { Serial.print(info); Serial.print(value); Serial.println(info2); }

        #define _LOG_DEBUG_SHOW_LEVEL(level)        { Serial.print("[DEBUG-"); Serial.print(level); Serial.print("]: "); }

        #define LOG_DEBUG_ERROR(...)               { _LOG_DEBUG_SHOW_LEVEL("ERROR") _LOG_DEBUG(__VA_ARGS__) }

        #if (LOG_DEBUG_LEVEL >= LOG_DEBUG_LEVEL_WARN)
            #define LOG_DEBUG_WARN(...)            { _LOG_DEBUG_SHOW_LEVEL("WARNING") _LOG_DEBUG(__VA_ARGS__) }
        #else
            #define LOG_DEBUG_WARN(...)
        #endif

        #if (LOG_DEBUG_LEVEL >= LOG_DEBUG_LEVEL_INFO)
            #define LOG_DEBUG_INFO(...)                       { _LOG_DEBUG_SHOW_LEVEL("INFO") _LOG_DEBUG(__VA_ARGS__) }
            #define LOG_DEBUG_INFO_BINARY(info, flags)        { _LOG_DEBUG_SHOW_LEVEL("INFO") Serial.print(info); printBinary(flags); Serial.println(""); }
            #define LOG_DEBUG_INFO_HEX(info, value, size)     { _LOG_DEBUG_SHOW_LEVEL("INFO") Serial.print(info);  printIntToHex(value, size); Serial.println(""); }

        #else
            #define LOG_DEBUG_INFO(...)
            #define LOG_DEBUG_INFO_BINARY(info, flags)
            #define LOG_DEBUG_INFO_HEX(info, value, size)
        #endif

        #if (LOG_DEBUG_LEVEL >= LOG_DEBUG_LEVEL_VERBOSE)
            #define LOG_DEBUG_VERBOSE(...)                      { _LOG_DEBUG_SHOW_LEVEL("VERBOSE") _LOG_DEBUG(__VA_ARGS__) }
            #define LOG_DEBUG_VERBOSE_PACKET(info, buf, size)   { _LOG_DEBUG_SHOW_LEVEL("VERBOSE") Serial.print(info); \
                                                                    printHex(buf, size, true); Serial.print("("); Serial.print(size); Serial.println(" bytes)"); }
        #else
            #define LOG_DEBUG_VERBOSE(...)
            #define LOG_DEBUG_VERBOSE_PACKET(info, buf, size)
        #endif


    #else
        #define LOG_DEBUG_ERROR(...)
        #define LOG_DEBUG_WARN(...)
        #define LOG_DEBUG_INFO(...)
        #define LOG_DEBUG_INFO_BINARY(info, flags)
        #define LOG_DEBUG_INFO_HEX(info, value, size) 
        #define LOG_DEBUG_VERBOSE(...)
        #define LOG_DEBUG_VERBOSE_PACKET(info, buf, size)

    #endif

    /* End Debug defines & macros */


    /* Print nibble as hex */
    void printNibble(char byte);

    /* Print a byte as hex */
    void printByte(char byte);

    /* Print several bytes as hex */
    void printHex(char *bytes, int size, bool space);

    /* Print an integer as hexadecimal value */
    void printIntToHex(int32_t value, int size);

    /* Show a number in binary */
    void printBinary(int16_t number);

#endif
