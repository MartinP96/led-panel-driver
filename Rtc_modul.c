/*
    File name: Rtc_modul.ino
    Date: 3.08.2020
    Created by: Martin P.

    Function definitions and ISR for I2C communication with RTC modul
*/

#include "Rtc_modul.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

struct Rtc_State_Struct Rtc_State = {.error_code = 0, .rtc_pointer_set_flag = 0, .Rtc_read_seq_state = 0, 
									.Rtc_write_seq_state = 0, .write_counter = 0, .read_counter = 0};



/*
 * Function: dec2bcd
 * ----------------------------
 *   Converts decimal value to the bcd format
 *
 *   val: input dec value
 *   
 *
 *   returns: Value in bcd format 
 */
uint8_t dec2bcd(uint8_t val)
    {
      return ( (val / 10 * 16) + (val % 10) );
    }


/*
 * Function: bcd2dec
 * ----------------------------
 *   Converts bcd value to dec value
 *
 *   val: input bcd value
 *   
 *
 *   returns: value in dec format
 */
uint8_t bcd2dec(uint8_t val)
{
  return ( (val / 16 * 10) + (val % 16) );
}


/*
 * Function: Rtc_ini
 * ----------------------------
 *  Initialize Rtc module - initialize i2c registers
 *
 */
void Rtc_ini(void)
{
  //set SCL to 400kHz
  TWSR = 0x00;
  TWBR = 0x0C;
  
  //set SCL to 100kHz
  //TWSR = 0x00;
  //TWBR = 0x72;

  TWCR = (1 << TWIE) | (1 << TWEN);
 
  Rtc_State.rtc_mode = Idle;
  Rtc_State.comm_mode = Ready;
}

/*
 * Function: RtcReadTime
 * ----------------------------
 *  Reads time (seconds, minutes, hours) from RTC module
 *
 *   *ss: pointer to seconds variable
 *   *mm: pointer to minutes variable
 *   *hh: pointer to hours variable
 *
 *
 *   returns: 1
 */
int RtcReadTime(uint8_t *ss, uint8_t *mm, uint8_t *hh)
{
  while(Rtc_State.comm_mode != Ready && Rtc_State.rtc_mode != Idle)
  {
    _delay_us(1);
  }
  Rtc_State.rtc_mode = ReadingTime;
  
  //Send start
  I2C_SendStart();

  while(Rtc_State.comm_mode != Ready && Rtc_State.rtc_mode != Idle)
  {
    _delay_us(1);
  }

  *ss = bcd2dec(RtcReadBuffer[0]);
  *mm = bcd2dec(RtcReadBuffer[1]);
  *hh = bcd2dec(RtcReadBuffer[2]);

  return 1;
}


/*
 * Function: RtcSendTime
 * ----------------------------
 *  Sends time (seconds, minutes, hours) to the  RTC module
 *
 *   send_seconds: value of seconds to be set on rtc module
 *   send_minutes: value of minutes to be set on rtc module
 *   send_hours: value of hours to be set on rtc module
 *
 *   returns: 1
 */
int RtcSendTime(uint8_t send_seconds, uint8_t send_minutes, uint8_t send_hours)
{
  //Write time into buffer
  RtcSendBuffer[0] = dec2bcd(send_seconds);
  RtcSendBuffer[1] = dec2bcd(send_minutes);
  RtcSendBuffer[2] = dec2bcd(send_hours);

  //Wait until I2C read 
  while(Rtc_State.comm_mode != Ready && Rtc_State.rtc_mode != Idle)
  {
    _delay_us(1);
  }
  
  Rtc_State.rtc_mode = WritingTime;
  
  //Zacnemo prenos
  I2C_SendStart();

  while(Rtc_State.comm_mode != Ready && Rtc_State.rtc_mode != Idle)
  {
    _delay_us(1);
  }
  
  return 1;
}


//-------------------------------Interrupt Service Routine-------------------------------//

//Interrupt service routine,i2c communication with rtc module
ISR(TWI_vect)
{
  //read status of I2C from TSWR register, mask out prescaler bits
  uint8_t status = TWSR & 0xFC;
  switch(status)
  {
    case 0x08: //Start sent, in this step we send SLA+W or SLA+R

      //Send SLA+W
      if(Rtc_State.rtc_mode == WritingTime || Rtc_State.rtc_mode == ReadingTime)
      {
        //Load slave addr + W bit 
        TWDR = 0xD0;

      }
      TWCR |= (1<<TWIE);
      TWCR &= ~(1 << TWSTA);
    break;

    case 0x10: //Repeated start sent, send SLA+R
      if (Rtc_State.rtc_mode == ReadingTime)
      {
        //Load slave addr + R bit
        TWDR = 0xD1;
      }
      TWCR &= ~(1 << TWSTO);
      TWCR &= ~(1 << TWSTA);
      TWCR |= (1<<TWIE);
    break;
      
    ///MASTER TRANSMITTER MODE///
    
    case 0x18: //SLA+W sent, in this step we send rtc pointer 0x00
      TWDR = 0x00;
    break;

    case 0x28: //Pointer sent, here we send our data
      if(Rtc_State.rtc_mode == WritingTime && Rtc_State.write_counter < 3)
      {
         TWDR = RtcSendBuffer[Rtc_State.write_counter];
         Rtc_State.write_counter++;
         
         TWCR |= (1<<TWIE);
      }
      else if(Rtc_State.rtc_mode == WritingTime && Rtc_State.write_counter >= 3)
      {

        //Send stop 
        TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWEA) | (1 << TWSTO);
        TWCR &= ~(1 << TWSTA);

         //Put seq in idle mode and reset counter 
         Rtc_State.rtc_mode = Idle;
         Rtc_State.comm_mode = Ready;
         Rtc_State.write_counter = 0;
         
      }
      else if(Rtc_State.rtc_mode == ReadingTime)
      {
        //Send repeated start
        TWCR = (1<<TWSTA)|(1<<TWEN)|(1<<TWIE);
      }
    break;

    ///MASTER RECIEVER MODE///

    case 0x40: //ACK recieved, waiting data
      TWCR |= (1 << TWEA)| (1 << TWIE);
    break;

    case 0x50:  //Recieving data    
      //Send ACK
      if(Rtc_State.read_counter < 3)
      {
        RtcReadBuffer[Rtc_State.read_counter] = TWDR;
        Rtc_State.read_counter++;

        TWCR |= (1 << TWEA)|(1 << TWIE);
      }
      //Send NACK to end transmition
      else if (Rtc_State.read_counter >= 3)
      {
         Rtc_State.rtc_mode = Idle;
         Rtc_State.read_counter = 0;
         Rtc_State.comm_mode = Ready;

         TWCR &= ~(1 << TWEA);
         TWCR |= (1 << TWIE);
      }
    break;

    case 0x58: //Send STOP
      
      TWCR |= (1 << TWSTO)|(1 << TWIE);
    break;
  }
  //Reset interrupt flag (writing 1)
  TWCR |= (1 << TWINT);
}
