#include "../inc/AP_MSPM0.h"
#include "../../LowLevelDrivers/inc/gpio.h"
#include "../../LowLevelDrivers/inc/sleep.h"
#include "../../LowLevelDrivers/inc/uart.h"
#include <stdbool.h>


AP_Handle handle;

void AP_init(AP_Handle ap_handle) {
    handle = ap_handle;

    UART_open(handle.uart);
    UART_flushRX(handle.uart);
    UART_flushTX(handle.uart);
}


// ---------------- timing/config ----------------
#ifndef APTIMEOUT
// Software timeout loop counts while waiting on SRDY transitions.
// If you see premature timeouts, increase this value.
#define APTIMEOUT (500000u)
#endif

// ---------------- debug counters ---------------
volatile uint32_t TimeOutErr = 0;
volatile uint32_t NoSOFErr   = 0;
volatile uint32_t fcserr     = 0;
volatile uint32_t RecvCount  = 0;

// ---------------- GPIO helpers (ACTIVE-LOW lines) ----------------
// MRDY: host -> SNP  (assert = 0, deassert = 1)
// SRDY: SNP  -> host (assert = 0, deassert = 1)  [read-only]
// RESET: host -> SNP (assert reset = 0, release = 1)

void SetMRDY(void)    { 
    gpio_write_pin(handle.master_rdy, 1); // 1 = not ready
}
void ClearMRDY(void)  { 
    gpio_write_pin(handle.master_rdy, 0); // 0 = ready
} 
void SetReset(void)   { 
    gpio_write_pin(handle.reset, 1); // 1 = run
}    
void ClearReset(void) { 
    gpio_write_pin(handle.reset, 0); // 0 = reset
}   
uint32_t ReadSRDY(void){
    return (gpio_read_pin(handle.slave_rdy) ? 1u : 0u);
}


void AP_Reset(void) {
    // Per SNP host interface: MRDY must be deasserted during reset
    SetMRDY();       // not ready (idle high)
    ClearReset();    // hold CC26xx in reset (active-low)
    sleep_ms(10);    // ~10 ms
    SetReset();      // release reset
}

// Compute NPI FCS = XOR of bytes from Len LSB through last payload byte
static uint8_t AP_ComputeFCS(const uint8_t *msg) {
    uint8_t lsb = msg[1];
    uint8_t msb = msg[2];
    uint32_t size = ((uint32_t)msb<<8) | lsb;
    uint8_t fcs = 0;
    // bytes 1..(size+4) => lenLSB,lenMSB,cmd0,cmd1,payload[size]
    for(uint32_t i = 1; i < (size + 5u); ++i) {
        fcs ^= msg[i];
    }
    return fcs;
}

//------------ AP_RecvStatus ------------
uint32_t AP_RecvStatus(void){
    // CC26xx asserts SRDY low when it has data or is ready to receive
    return (ReadSRDY()==0);
}

//------------ AP_RecvMessage ------------
int AP_RecvMessage(uint8_t *pt, uint32_t max){
  uint8_t data, lsb, msb, cmd0, cmd1, fcs;
  uint32_t count, size;

  // 1) wait for SRDY low (slave-initiated transfer)
  uint32_t to = APTIMEOUT;
  while(ReadSRDY() && to){ to--; }
  if(to==0){ TimeOutErr++; return APFAIL; }

  // 2) assert MRDY low
  ClearMRDY();

  // 3) receive NPI package
  // wait for SOF
  uint32_t sof_to = APTIMEOUT;
  do{
    data = UART_getcBlocking(handle.uart);
    if(--sof_to == 0){
      SetMRDY();   // release MRDY before bailing
      NoSOFErr++;
      return APFAIL;
    }
  }while(data != AP_SOF);
  if(max){ *pt++ = data; }
  fcs = 0;

  // get size (LSB/MSB)
  lsb = UART_getcBlocking(handle.uart); if(max){ *pt++ = lsb; } fcs ^= lsb;
  msb = UART_getcBlocking(handle.uart); if(max){ *pt++ = msb; } fcs ^= msb;

  // get command
  cmd0 = UART_getcBlocking(handle.uart); if(max){ *pt++ = cmd0; } fcs ^= cmd0;
  cmd1 = UART_getcBlocking(handle.uart); if(max){ *pt++ = cmd1; } fcs ^= cmd1;

  count = 5;
  size  = ((uint32_t)msb<<8) | lsb;

  // payload
  for(uint32_t i=0;i<size;i++){
    data = UART_getcBlocking(handle.uart);
    fcs ^= data;
    count++;
    if(count <= max){ *pt++ = data; }
  }

  // FCS byte
  data = UART_getcBlocking(handle.uart);
  count++;
  if(count <= max){ *pt++ = data; }

  if(data != fcs){
    fcserr++;
    SetMRDY();        // release MRDY
    return APFAIL;
  }

  // 4) release MRDY
  SetMRDY();

  // 5) wait for SRDY to be deasserted (high)
  to = APTIMEOUT;
  while((ReadSRDY()==0) && to){ to--; }
  if(to==0){ TimeOutErr++; return APFAIL; }

  RecvCount++;
  return APOK;
}

//------------ AP_SendMessage ------------
// MRDY-first host-initiated flow: MRDY↓ → wait SRDY↓ → send → MRDY↑ → wait SRDY↑
int AP_SendMessage(uint8_t *msg){
  if(msg[0] != AP_SOF) return APFAIL;

  // 1) Assert MRDY first to wake SNP
  ClearMRDY();   // MRDY low (active)

  // 2) Wait for SRDY low (SNP ready)
  uint32_t to = APTIMEOUT;
  while(ReadSRDY() && to){ to--; }
  if(to==0){
    SetMRDY();    // release MRDY before failing
    TimeOutErr++;
    return APFAIL;
  }

  // 3) compute length and FCS
  uint8_t lsb = msg[1];
  uint8_t msb = msg[2];
  uint32_t size = ((uint32_t)msb<<8) | lsb;
  uint8_t fcs  = AP_ComputeFCS(msg);

  // 4) send SOF..payload
  UART_write(handle.uart, msg, 1u + 2u + 2u + size);

  // 5) send FCS
  UART_putc(handle.uart, fcs);

  // 6) MRDY high (done)
  SetMRDY();

  // 7) wait for SRDY release (high)
  to = APTIMEOUT;
  while((ReadSRDY()==0) && to){ to--; }
  if(to==0){ TimeOutErr++; return APFAIL; }

  return APOK;
}

//------------ AP_SendMessageResponse ------------
int AP_SendMessageResponse(uint8_t *msgOut, uint8_t *msgIn, uint32_t maxIn){
  if(AP_SendMessage(msgOut) != APOK) return APFAIL;
  return AP_RecvMessage(msgIn, maxIn);
}

