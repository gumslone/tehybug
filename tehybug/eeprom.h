#pragma once
#ifndef _TeHyBugEeprom_
#define _TeHyBugEeprom_
#include <EepromFS.h>

class TeHyBugEeprom{
  public : 
  TeHyBugEeprom(): EFS(0x50, 32768) {}
  void setup(){
      int s;
      if (s=EFS.begin()) {
        D_println("EEPROM filessystem mounted");
        D_print(s); 
        D_print(" slots in fs");
      }
      readdir();
    }
    void readdir() {
    EFS.dirp=0;
    while (uint8_t f=EFS.readdir()) {
            D_print("Slot "); 
            D_print(f);
            D_print(": ");
            D_print(EFS.filename(f)); 
            D_print(" "); 
            D_println(EFS.filesize(f));
     } 
  
     D_print("Error status: "); D_println(EFS.ferror);
  }
  void read(char *n) {
    uint8_t f;
    f=EFS.fopen(n, "r");
  
    D_print("Bytes in file: ");  
    D_println(EFS.available(f));
  
    long t=millis();
    if (f) for(int i=0; i<1006 && ! EFS.eof(f); i++) Serial.write(EFS.fgetc(f));
    else D_println("Read error");
    D_println();
    t=millis()-t;
    D_print("Read baud rate: "); D_println(11*8/t);
    
    EFS.fclose(f);
  
    D_print("Error status: "); D_println(EFS.ferror);
  }
  void write(char* n, char* s) {
    uint8_t f;
    f=EFS.fopen(n, "w");
    if (f) for(int i=0; i<1006 && s[i]!=0; i++) EFS.fputc(s[i], f);
    else D_println("Write error");
  
    EFS.fflush(f);
  
    D_print("Filesize now: ");
    D_println(EFS.filesize(f));
  
    D_print("Bytes left in slot:");
    D_println(EFS.available(f));
    
    EFS.fclose(f);
  
    D_print("Error status: "); D_println(EFS.ferror);
  }
  void format() {
    D_println("Formating 32 slots");
    long t=millis();
    EFS.format(32);
    t=millis()-t;
    D_print("Format time = "); D_println(t);
    D_print(16*32+6); D_println(" bytes updated");
    D_print("Update baudrate = "); D_print((16*32+6)*8/t); D_println("000");
  
   D_print("Error status: "); D_println(EFS.ferror);
  }
  private:
  EepromFS EFS;
};
#endif
