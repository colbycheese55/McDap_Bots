#include "AP_MSPM0.h"
#include "ti_msp_dl_config.h"

#include <ti/driverlib/dl_gpio.h>
#include <ti/driverlib/dl_uart_main.h>
#include <stdbool.h>

// ---- UART functions provided by your UART1 driver (no shim needed) ----
extern uint8_t UART1_InChar(void);
extern void    UART1_OutChar(uint8_t c);
extern void    UART1_Write(const uint8_t *buf, uint32_t len);

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

void SetMRDY(void)    { DL_GPIO_setPins(MRDY_PORT, MRDY_PIN_0_PIN); }   // 1 = not ready
void ClearMRDY(void)  { DL_GPIO_clearPins(MRDY_PORT, MRDY_PIN_0_PIN); } // 0 = ready

void SetReset(void)   { DL_GPIO_setPins(RST_PORT,  RST_PIN_2_PIN); }    // 1 = run
void ClearReset(void) { DL_GPIO_clearPins(RST_PORT, RST_PIN_2_PIN); }   // 0 = reset

uint32_t ReadSRDY(void){
    return (DL_GPIO_readPins(SRDY_PORT, SRDY_PIN_1_PIN) ? 1u : 0u);
}

// crude cycle-based delay using CPUCLK_FREQ from ti_msp_dl_config.h
static void delayCycles(uint32_t cycles) {
    while(cycles--) { __NOP(); }
}

void AP_Delay1ms(uint32_t n) {
#ifdef CPUCLK_FREQ
    const uint32_t per_ms = (CPUCLK_FREQ/1000u);
#else
    const uint32_t per_ms = 32000u; // fallback
#endif
    while(n--) delayCycles(per_ms);
}

void AP_Reset(void) {
    // Per SNP host interface: MRDY must be deasserted during reset
    SetMRDY();       // not ready (idle high)
    ClearReset();    // hold CC26xx in reset (active-low)
    AP_Delay1ms(10); // ~10 ms
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
    data = UART1_InChar();
    if(--sof_to == 0){
      SetMRDY();   // release MRDY before bailing
      NoSOFErr++;
      return APFAIL;
    }
  }while(data != AP_SOF);
  if(max){ *pt++ = data; }
  fcs = 0;

  // get size (LSB/MSB)
  lsb = UART1_InChar(); if(max){ *pt++ = lsb; } fcs ^= lsb;
  msb = UART1_InChar(); if(max){ *pt++ = msb; } fcs ^= msb;

  // get command
  cmd0 = UART1_InChar(); if(max){ *pt++ = cmd0; } fcs ^= cmd0;
  cmd1 = UART1_InChar(); if(max){ *pt++ = cmd1; } fcs ^= cmd1;

  count = 5;
  size  = ((uint32_t)msb<<8) | lsb;

  // payload
  for(uint32_t i=0;i<size;i++){
    data = UART1_InChar();
    fcs ^= data;
    count++;
    if(count <= max){ *pt++ = data; }
  }

  // FCS byte
  data = UART1_InChar();
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
  UART1_Write(msg, 1u + 2u + 2u + size);

  // 5) send FCS
  UART1_OutChar(fcs);

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

