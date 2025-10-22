// main.c — MSPM0G3507 + CC26xx SNP: advertise + LED (PB27) control via GATT write
// - UART1: BLE link (NPI) via MRDY/SRDY/RESET (per your SysConfig)
// - UART0: debug back channel (DBG_* prints)
// - LED: GPIOB.27 (LED_PIN_3) configured in your SysConfig

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "ti_msp_dl_config.h"
#include "AP_MSPM0.h"
#include "uart.h"
#include "dbg_uart0.h"
#include <ti/driverlib/dl_gpio.h>

#define SOF 0xFE
#define RECVSIZE 128
static uint8_t rx[RECVSIZE];

/* ===== LED selection (from your SysConfig; now PB27) ===== */
#define LED_GPIO_PORT   LED_PORT        // GPIOB
#define LED_GPIO_PIN    LED_PIN_3_PIN   // DL_GPIO_PIN_27
/* ======================================================== */

static inline void LED_init(void){
    // SysConfig already:
    //  - init IOMUX for LED_PIN_3_IOMUX
    //  - set PB27 low (off, assuming active-high)
    //  - enabled output
    // Nothing else required here.
}
static inline void LED_on(void){
    DL_GPIO_setPins(LED_GPIO_PORT, LED_GPIO_PIN);     // active-high
}
static inline void LED_off(void){
    DL_GPIO_clearPins(LED_GPIO_PORT, LED_GPIO_PIN);
}

/* ---------- Debug helpers ---------- */
static void print_frame(const char *tag, const uint8_t *buf) {
    uint16_t len = (uint16_t)buf[1] | ((uint16_t)buf[2] << 8);
    uint16_t total = (uint16_t)(1 + 2 + 2 + len + 1);
    DBG_write(tag); DBG_write(": "); DBG_hex(buf, total);
}
static int sendrx(const uint8_t *tx) {
    print_frame("TX", tx);
    int r = AP_SendMessageResponse((uint8_t*)tx, rx, RECVSIZE);
    if (r != APOK) { DBG_write("ERR: AP_SendMessageResponse failed\r\n"); return r; }
    uint16_t len = (uint16_t)rx[1] | ((uint16_t)rx[2] << 8);
    uint16_t total = (uint16_t)(1 + 2 + 2 + len + 1);
    DBG_write("RX: "); DBG_hex(rx, total);
    return r;
}

/* ---------- Basic diag ---------- */
static const uint8_t MSG_GetStatus[]  = {SOF,0x00,0x00, 0x55,0x06, 0x00};
static const uint8_t MSG_GetVersion[] = {SOF,0x00,0x00, 0x35,0x03, 0x00};

/* ---------- Advertising (same as before) ---------- */
// Adv data (flags + small mfg payload) — header byte 0x01 = AdvData
static const uint8_t MSG_SetAdvData[] = {
    SOF, 11, 0x00,
    0x55, 0x43,
    0x01,
    0x02, 0x01, 0x06,
    0x06, 0xFF,
    0x0D, 0x00,
    0x03, 0x00, 0x00,
    0x00
};

#define NAME "MSPM0-CC26xx"
static uint8_t MSG_SetScanRsp[64];

static const uint8_t MSG_StartAdv[] = {
    SOF, 14, 0x00,
    0x55, 0x42,
    0x00,        // Connectable Undirected
    0x00, 0x00,  // infinite
    0x64, 0x00,  // ~100ms
    0x00, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x00, 0xC5,
    0x02,
    0x00
};

static void buildScanRsp(void) {
    // [SOF][lenLSB][lenMSB][55][43][00][AD...][FCS]
    uint8_t *p = MSG_SetScanRsp;
    *p++ = SOF;
    uint8_t *lenLSB = p++;  // placeholder
    uint8_t *lenMSB = p++;  // placeholder
    *p++ = 0x55; *p++ = 0x43; // Set Adv/ScanResp Data
    *p++ = 0x00;              // 0 = ScanRsp

    // Complete Local Name
    const char *nm = NAME;
    uint8_t n = (uint8_t)strlen(nm);
    *p++ = (uint8_t)(1 + n);
    *p++ = 0x09; // Complete Local Name
    memcpy(p, nm, n); p += n;

    // Conn Interval Range
    *p++ = 0x05; *p++ = 0x12;
    *p++ = 0x50; *p++ = 0x00; // min 50ms
    *p++ = 0x20; *p++ = 0x03; // max 1.25s

    // Tx Power 0 dBm
    *p++ = 0x02; *p++ = 0x0A; *p++ = 0x00;

    // length (cmd0..payload), excluding SOF/FCS
    uint16_t plen = (uint16_t)((p - MSG_SetScanRsp) - 3);
    *lenLSB = (uint8_t)(plen & 0xFF);
    *lenMSB = (uint8_t)(plen >> 8);
    *p++ = 0x00; // FCS placeholder
}

/* ---------- Minimal “LEDs” GATT like the legacy code ---------- */
static uint16_t gHandleLEDs = 0;

// Primary service: 0xFFF0
static const uint8_t MSG_AddService[] = {
    SOF, 3, 0x00,
    0x35, 0x81,    // Add Service
    0x01,          // Primary
    0xF0, 0xFF,    // UUID 0xFFF0
    0x00
};

// Char Value: UUID 0xFFF3, writeable, accept Write + Write Without Response
// Permissions: Write (0x02), Properties: 0x000C (WWR 0x0004 | Write 0x0008)
static const uint8_t MSG_AddCharValueLEDs[] = {
    SOF, 0x08, 0x00,
    0x35, 0x82,   // Add Char Value
    0x02,         // GATT Write Permission
    0x0C, 0x00,   // Properties = Write Without Response + Write
    0x00,         // RFU
    0x00, 0x02,   // Max len = 512
    0xF3, 0xFF,   // UUID 0xFFF3
    0x00
};

// Char Descriptor: “LEDs”
static const uint8_t MSG_AddCharDescLEDs[] = {
    SOF, 11, 0x00,
    0x35, 0x83,   // Add Char Descriptor
    0x80,         // User Description
    0x01,         // Readable
    0x05, 0x00,   // Max len = 5
    0x05, 0x00,   // Initial len = 5
    'L','E','D','s',0,
    0x00
};

// Register service
static const uint8_t MSG_RegisterService[] = {
    SOF, 0x00, 0x00,
    0x35, 0x84,
    0x00
};

// Write Confirmation (only if SNP asks for response)
static uint8_t MSG_WriteCnf[] = {
    SOF, 0x03, 0x00,
    0x55, 0x88,
    0x00,       // Success
    0x00, 0x00, // Conn handle = 0
    0x00
};

static void BLE_InitAndAdvertise(void){
    AP_Reset();

    (void)sendrx(MSG_GetStatus);
    (void)sendrx(MSG_GetVersion);

    // Add primary service and LED characteristic
    DBG_write("Add Service\r\n");
    (void)sendrx(MSG_AddService);

    DBG_write("Add LED Char Value\r\n");
    (void)sendrx(MSG_AddCharValueLEDs);
    // Handle returned in rx[6..7] (LSB..MSB) after Add Char Value
    gHandleLEDs = ((uint16_t)rx[7] << 8) | rx[6];

    DBG_write("Add LED Char Descriptor\r\n");
    (void)sendrx(MSG_AddCharDescLEDs);

    DBG_write("Register Service\r\n");
    (void)sendrx(MSG_RegisterService);

    // Advertising sequence
    buildScanRsp();
    (void)sendrx(MSG_SetAdvData);
    (void)sendrx(MSG_SetScanRsp);
    (void)sendrx(MSG_StartAdv);
}

int main(void){
    SYSCFG_DL_init();   // UART0/1 + MRDY/SRDY/RESET + PB27 LED from your SysConfig
    UART_initBLE();     // UART1 for BLE link
    DBG_init();         // UART0 debug console
    LED_init();

    DBG_write("\r\n=== Boot (MSPM0 LED demo, LED=PB27) ===\r\n");
    BLE_InitAndAdvertise();

    // Main loop: poll SRDY and process incoming SNP frames
    for(;;){
        if(AP_RecvStatus()){ // SRDY low (SNP has something)
            if(AP_RecvMessage(rx, RECVSIZE) == APOK){
                print_frame("RX", rx);

                // SNP Characteristic Write Indication (CMD0/CMD1 = 0x55/0x88)
                if(rx[3] == 0x55 && rx[4] == 0x88){
                    // Payload: [5..6]=conn, [7..8]=attr handle, [9]=resp, [10..11]=offset, [12..]=data
                    uint16_t h = ((uint16_t)rx[8] << 8) | rx[7];
                    uint8_t respNeeded = rx[9];

                    if(h == gHandleLEDs){
                        uint8_t v = rx[12];
                        if (v) {
                            LED_on();
                            DBG_write("LED write -> ON\r\n");
                        } else {
                            LED_off();
                            DBG_write("LED write -> OFF\r\n");
                        }
                    }
                    if(respNeeded){
                        (void)AP_SendMessage(MSG_WriteCnf);
                        print_frame("TX", MSG_WriteCnf);
                    }
                }
            }
        }
        AP_Delay1ms(1); // gentle polling
    }
}














