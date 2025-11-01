#ifndef AP_H
#define AP_H
/**
 * @file      AP.h
 * @brief     BLE Application Processor transport for CC26xx SNP over UART (MSPM0)
 * @details   UART1 + MRDY/SRDY/RESET handshake transport and helpers.
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ----------------- status codes -----------------
#define APOK    0
#define APFAIL  1

// NPI framing constants
#define AP_SOF  0xFE

/**
 * @brief Check if the CC26xx is asserting SRDY (active-low).
 * @return non-zero if SRDY is asserted (ready), 0 if idle.
 */
uint32_t AP_RecvStatus(void);

/**
 * @brief Receive one NPI frame from the CC26xx over UART1.
 * Performs MRDY/SRDY handshake; blocks with a software timeout.
 * @param pt  destination buffer (the SOF byte is stored first)
 * @param max max bytes to store (excess is discarded but counted)
 * @return APOK on success, APFAIL on timeout/format/FCS error
 */
int AP_RecvMessage(uint8_t *pt, uint32_t max);

/**
 * @brief Send one NPI frame to the CC26xx over UART1.
 * Buffer must start at SOF (0xFE). This function computes and appends FCS.
 * @param msg pointer to SOF byte
 * @return APOK on success, APFAIL on timeout
 */
int AP_SendMessage(uint8_t *msg);

/**
 * @brief Send a frame and wait for the response.
 * @param msgOut outgoing frame (starts at SOF)
 * @param msgIn  destination buffer for the received frame
 * @param maxIn  capacity of msgIn
 * @return APOK or APFAIL
 */
int AP_SendMessageResponse(uint8_t *msgOut, uint8_t *msgIn, uint32_t maxIn);

/**
 * @brief Pulse RESET low (~10 ms) with MRDY deasserted, then release.
 */
void AP_Reset(void);

/**
 * @brief Busy-wait approximate millisecond delay.
 */
void AP_Delay1ms(uint32_t n);

// --------- optional debug counters ----------
extern volatile uint32_t TimeOutErr;
extern volatile uint32_t NoSOFErr;
extern volatile uint32_t fcserr;
extern volatile uint32_t RecvCount;

#ifdef __cplusplus
}
#endif

#endif // AP_H