#pragma once
#ifndef _TeHyBugEeprom_
#define _TeHyBugEeprom_
#include <EepromFS.h>
class TeHyBugEeprom{
  public : 
  TeHyBugEeprom(RtcTime & time): m_time(time), m_efs(0x50, 32768) {}
  void setup(){
      int s;
      if (s=m_efs.begin()) {
        D_println("EEPROM filessystem mounted");
        D_print(s); 
        D_print(" slots in fs");
      }
      readdir();
    }
    void readdir() {
    m_efs.dirp=0;
    while (uint8_t f=m_efs.readdir()) {
            D_print("Slot "); 
            D_print(f);
            D_print(": ");
            D_print(m_efs.filename(f)); 
            D_print(" "); 
            D_println(m_efs.filesize(f));
     } 
  
     D_print("Error status: "); D_println(m_efs.ferror);
  }
  String read(char *n) {
    uint8_t f;
    String data;
    f=m_efs.fopen(n, "r");
  
    D_print("Bytes in file: ");  
    D_println(m_efs.available(f));
  
    long t=millis();
    if (f) for(int i=0; i<1006 && ! m_efs.eof(f); i++) data += m_efs.fgetc(f);
    else D_println("Read error");
    D_println();
    t=millis()-t;
    D_print("Read baud rate: "); D_println(11*8/t);
    
    m_efs.fclose(f);
  
    D_print("Error status: "); D_println(m_efs.ferror);
    return data;
  }
  void write(const char* n, char* s) {
    uint8_t f;
    f=m_efs.fopen(n, "w");
    if (f) for(int i=0; i<1006 && s[i]!=0; i++) m_efs.fputc(s[i], f);
    else D_println("Write error");
  
    m_efs.fflush(f);
  
    D_print("Filesize now: ");
    D_println(m_efs.filesize(f));
  
    D_print("Bytes left in slot:");
    D_println(m_efs.available(f));
    
    m_efs.fclose(f);
  
    D_print("Error status: "); D_println(m_efs.ferror);
  }
  void format() {
    D_println("Formating 32 slots");
    long t=millis();
    m_efs.format(32);
    t=millis()-t;
    D_print("Format time = "); D_println(t);
    D_print(16*32+6); D_println(" bytes updated");
    D_print("Update baudrate = "); D_print((16*32+6)*8/t); D_println("000");
  
   D_print("Error status: "); D_println(m_efs.ferror);
  }

  String getFileName()
  {
    return m_time.getMonthDay() + ".txt";
  }
  void writeData(String & data)
  {
    const char* fileName = getFileName().c_str();
    write(fileName, "testData");
  }
  private:
  EepromFS m_efs;
  RtcTime & m_time;

};
#endif
