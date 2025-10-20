
//------------------------COMMENTS FROM ORIGINAL AP.c -----------------------------
// AP.c
// Runs on either MSP432 or TM4C123
// see GPIO.c file for hardware connections 
// Student version, 
// The CC2650 is running with SimpleNP 2.2 (Powersave, MRDY, SRDY signals). 
// Tested with CC2650 BoosterPack
//    cc2650bp_simple_np_uart_pm_xsbl_mooc_custom.hex
//    simple_np_cc2650bp_uart_pm_sbl.hex
// Tested with CC2650 LaunchPad
//    cc2650lp_simple_np_uart_pm_xsbl_mooc_custom.hex 
//    simple_np_cc2650lp_uart_pm_sbl.hex
// It doesn't matter if bootload mode is enabled (sbl) or not enabled (xsbl)
// Transmit and receive interrupts are implemented in UART1.c on UCA2.
// GPIO pins are implemented in GPIO.c
// Daniel Valvano and Jonathan Valvano
// February 28, 2017

/* This example accompanies the books
   "Embedded Systems: Introduction to the MSP432 Microcontroller",
       ISBN: 978-1512185676, Jonathan Valvano, copyright (c) 2017
   "Embedded Systems: Real-Time Interfacing to the MSP432 Microcontroller",
       ISBN: 978-1514676585, Jonathan Valvano, copyright (c) 2017
   "Embedded Systems: Real-Time Operating Systems for ARM Cortex-M Microcontrollers",
       ISBN: 978-1466468863, , Jonathan Valvano, copyright (c) 2017
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/

Simplified BSD License (FreeBSD License)
Copyright (c) 2017, Jonathan Valvano, All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are
those of the authors and should not be interpreted as representing official
policies, either expressed or implied, of the FreeBSD Project.
*/

// CC2650 booster or CC2650 LaunchPad, CC2650 needs to be running SimpleNP 2.2 (POWERSAVE)
// UART serial from CC2650 to TM4C123/MSP432 
// UART serial from TM4C123/MSP432 to CC2650
// MRDY, an output from TM4C123/MSP432 and an input to CC2650
// SRDY, an input to TM4C123/MSP432 and an output from CC2650
// Reset, an output from TM4C123/MSP432 to hardware reset to CC2650
// MRDY is a GPIO pin whose logic level indicates the state of the master processor. 
// MRDY is an active low (low true) signal. 
// An MRDY event indicates that the master processor is ready to send or receive data.
// SRDY is a GPIO pin whose logic level indicates the state of the slave processor. 
// SRDY is an active low (low true) signal. 
// An SRDY event indicates that the slave processor is ready to send or receive data. 
// see GPIO.c file for hardware connections 

// ----------------------------------------------------------------------------------


#include <stdint.h>
#include "../inc/AP.h"
#include "../inc/clock.h"
#include "../inc/gpio.h"

#include "../inc/CortexM.h"
#include "../inc/UART0.h"
#include "../inc/UART1.h"
#include "msp.h"

#define RECVSIZE 128
uint8_t RecvBuf[RECVSIZE];

uint32_t fcserr;      // debugging counts of errors
uint32_t TimeOutErr;  // debugging counts of no response errors
uint32_t NoSOFErr;    // debugging counts of no SOF errors

#define APTIMEOUT 40000   // 10 ms

#define APDEBUG 1
//**debug macros**********
#ifdef APDEBUG
#define OutString(STRING) UART0_OutString(STRING)
#define OutUHex(NUM) UART0_OutUHex(NUM)
#define OutUHex2(NUM) UART0_OutUHex2(NUM)
#define OutChar(N) UART0_OutChar(N)
#else
#define OutString(STRING)
#define OutUHex(NUM)
#define OutUHex2(NUM)
#define OutChar(N)
#endif

// Pin Definitions
#define SRDY_PORT GPIOB
#define SRDY_PIN  12
#define MRDY_PORT GPIOA
#define MRDY_PIN 25
#define RST_PORT GPIOB
#define RST_PIN 13

// GPIO functions
uint32_t ReadSRDY() {
    return 0; //todo: update to use gpio library
}

void SetMRDY() {
    //todo: update to use gpio library
}

void ClearMRDY() {
    //todo: update to use gpio library
}

void SetReset() {
    //todo: update to use gpio library
}

void ClearReset() {
    //todo: update to use gpio library
}

//------------AP_RecvStatus------------
// check to see if Bluetooth module wishes to send packet
// Inputs: none
// Outputs: 0 if no communication needed, 
//          nonzero for communication ready 
uint32_t AP_RecvStatus(void){
    // check if SRDY_Pin is 0
  return (ReadSRDY()==0); 
}

//------------AP_RecvMessage------------
// receive a message from the Bluetooth module
// 1) receive NPI package
// 2) Wait for entire message to be received
// Input: pointer to empty buffer into which data is returned
//        maximum size (discard data beyond this limit)
// Output: APOK if ok, APFAIL on error (timeout or fcs error)
int AP_RecvMessage(uint8_t *pt, uint32_t max){
  uint8_t fcs; uint32_t waitCount; uint8_t data,cmd0,cmd1; 
  uint8_t msb,lsb;
  uint32_t size,count,SOFcount=10;
// 1) wait for SRDY to be low
  waitCount = 0;
  while(ReadSRDY()){ // todo: update to use gpio library
    waitCount++;
    if(waitCount>APTIMEOUT){
      TimeOutErr++;  // no response error
      return APFAIL; // timeout??
    }      
  }
// 2) Make MRDY=0
  ClearMRDY(); // todo: update to use gpio library
// 3) receive NPI package
  // wait for SOF
  do{
    data = UART1_InChar();
    SOFcount--;
    if(SOFcount==0){
      SetMRDY();     //   MRDY=1  
      NoSOFErr++;    // no SOF error
      return APFAIL;
    }
  }while(data != SOF);
  *pt = data; pt++;
  fcs = 0;
// get size, once we get SOF, it is highly likely for the rest to come
  lsb = UART1_InChar(); *pt = lsb; fcs=fcs^lsb; pt++;
  msb = UART1_InChar(); *pt = msb; fcs=fcs^msb; pt++;
// get command
  cmd0 = UART1_InChar(); *pt = cmd0; fcs=fcs^cmd0; pt++;
  cmd1 = UART1_InChar(); *pt = cmd1; fcs=fcs^cmd1; pt++;
  count = 5;
  size = (msb<<8)+lsb;
// get payload
  for(int i=0;i<size;i++){
    data = UART1_InChar(); 
    fcs = fcs^data; 
    count++;
    if(count <= max){
      *pt = data; 
      pt++; // payload
    }
  }
// get FCB
  data = UART1_InChar(); 
  count++;
  if(count <= max){
    *pt = data; 
  }    
  if(data != fcs){ 
    fcserr++;
    SetMRDY();        //   MRDY=1  
    return APFAIL;
  }
// 4) Make MRDY=1
  SetMRDY();        //   MRDY=1  
// 5) wait for SRDY to be high
  waitCount = 0;
  while(ReadSRDY()==0){
    waitCount++;
  }
  return APOK;
}