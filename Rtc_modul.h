/*
    File name: Rtc_modul.h
    Date: 3.08.2020
    Created by: Martin P.

    Variables and function decalarations for i2c communication with Rtc modul
*/

#ifndef _RTC_MODUL_H_
#define _RTC_MODUL_H_

#include <avr/io.h>



  //-----------------Real time clock module parameters-------------//

  //RTC module address
  #define rtc_addr 0x68
  
  //I2C macros
  #define I2C_SendStart()    (TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)|(1<<TWIE))
  #define I2C_TWISendStop()   (TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN)|(1<<TWIE)) 
  #define I2C_TWISendTransmit() (TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)) 
  #define I2C_TWISendACK()    (TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA)) 
  #define I2C_TWISendNACK()   (TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE))
  
  //I2C state codes
  #define I2C_START_SENT 0x08
  #define I2C_R_START_SENT 0x10
  #define I2C_SLAW_SENT 0x18
  
 // Send buffer array
 volatile uint8_t RtcSendBuffer[3];
 
 // Read buffer array
 volatile uint8_t RtcReadBuffer[3];


  enum I2C_Mode
  {
    Ready,
    MasterTransmitter,
    MasterReciever
  };


  enum Rtc_Mode
  {
    Idle,
    WritingTime,
    ReadingTime
  };

  struct Rtc_State_Struct
  {
    uint8_t error_code;
    enum I2C_Mode comm_mode;
    enum Rtc_Mode rtc_mode;
    
    uint8_t rtc_pointer_set_flag;
    int Rtc_read_seq_state;
    int Rtc_write_seq_state;
    int write_counter;
    int read_counter;
   
  };


  ///Function definitions///
  void Rtc_ini(void);
  int RtcReadTime(uint8_t *ss, uint8_t *mm, uint8_t *hh);
  int RtcSendTime(uint8_t send_seconds, uint8_t send_minutes, uint8_t send_hours);
  uint8_t dec2bcd(uint8_t val);
  uint8_t dec2bcd(uint8_t val);

#endif
