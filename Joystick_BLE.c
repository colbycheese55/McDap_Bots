// main.c — MSPM0G3507 + CC26xx SNP (BOOSTXL-CC2650MA)
// BLE Joystick (iOS) compatible: HM-10 UART service 0xFFE0 / char 0xFFE1
// Mapping: 'A'/'a' => LED ON, 'B'/'b' => LED OFF, ignore '0' (release).
// We accept ANY Characteristic Write Indication (no handle filter), since
// actual attribute handles (e.g., 0x001E in your logs) can vary.
//
// IMPORTANT: iOS caches GATT. After flashing new GATT, "Forget" the device
// or toggle Bluetooth on iOS so it re-reads services/characteristics.

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "ti_msp_dl_config.h"
#include "HighLevelDrivers/inc/AP_MSPM0.h"
#include "LowLevelDrivers/inc/uart.h"
#include "LowLevelDrivers/inc/dbg_uart0.h"
#include <ti/driverlib/dl_gpio.h>

/* ===== Simple fallback debug helpers (link-safe) ===== */
static void DBG_dec(uint32_t v){
    char buf[12]; int i = sizeof(buf)-1; buf[i]=0;
    if(!v) buf[--i]='0'; while(v && i){ buf[--i]=(char)('0'+(v%10)); v/=10; }
    DBG_write(&buf[i]);
}
static void DBG_hex16(uint16_t v){
    static const char hx[]="0123456789ABCDEF"; char s[7];
    s[0]='0'; s[1]='x'; s[2]=hx[(v>>12)&0xF]; s[3]=hx[(v>>8)&0xF];
    s[4]=hx[(v>>4)&0xF]; s[5]=hx[v&0xF]; s[6]=0; DBG_write(s);
}
static void DBG_hexb(const uint8_t *p, uint16_t n){ DBG_hex(p, n); }
/* ===================================================== */

#define SOF        0xFE
#define RECVSIZE   128
#define NAME       "MSPM0-CC26xx"

static uint8_t rx[RECVSIZE];

/* ===== LED: PB27 (from SysConfig) ===== */
#define LED_GPIO_PORT   LED_PORT
#define LED_GPIO_PIN    LED_PIN_3_PIN  // PB27

static inline void LED_init(void){
    DL_GPIO_clearPins(LED_GPIO_PORT, LED_GPIO_PIN);
    DL_GPIO_enableOutput(LED_GPIO_PORT, LED_GPIO_PIN);
}
static inline void LED_on(void)  { DL_GPIO_setPins  (LED_GPIO_PORT, LED_GPIO_PIN); }
static inline void LED_off(void) { DL_GPIO_clearPins(LED_GPIO_PORT, LED_GPIO_PIN); }

/* ===== Frame debug helpers ===== */
static void print_frame(const char *tag, const uint8_t *buf) {
    uint16_t len   = (uint16_t)buf[1] | ((uint16_t)buf[2] << 8);
    uint16_t total = (uint16_t)(1 + 2 + 2 + len + 1);
    DBG_write(tag); DBG_write(": "); DBG_hex(buf, total);
}
static int sendrx(const uint8_t *tx) {
    print_frame("TX", tx);
    int r = AP_SendMessageResponse((uint8_t*)tx, rx, RECVSIZE);
    if (r != APOK) { DBG_write("ERR: AP_SendMessageResponse failed\r\n"); return r; }
    print_frame("RX", rx);
    return r;
}

/* ===== Basic diag ===== */
static const uint8_t MSG_GetStatus[]  = {SOF,0x00,0x00, 0x55,0x06, 0x00};
static const uint8_t MSG_GetVersion[] = {SOF,0x00,0x00, 0x35,0x03, 0x00};

/* ===== Advertising / Scan Response ===== */
static const uint8_t MSG_SetAdvData[] = {
    SOF, 11, 0x00,
    0x55, 0x43,       // Set Adv/ScanResp Data
    0x01,             // 0x01 = AdvData
    0x02, 0x01, 0x06, // Flags
    0x06, 0xFF,       // manufacturer data (dummy)
    0x0D, 0x00, 0x03, 0x00, 0x00,
    0x00
};

static uint8_t MSG_SetScanRsp[64];

static const uint8_t MSG_StartAdv[] = {
    SOF, 14, 0x00,
    0x55, 0x42,   // Start Adv
    0x00,         // Connectable Undirected
    0x00, 0x00,   // infinite
    0x64, 0x00,   // ~100ms
    0x00, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x00, 0xC5,
    0x02,
    0x00
};

/* Build Scan Response:
   - Complete Local Name
   - Conn Interval Range
   - Tx Power
   - Complete List of 16-bit UUIDs: 0xFFE0 (so the app finds us)
*/
static void buildScanRsp(void) {
    uint8_t *p = MSG_SetScanRsp;
    *p++ = SOF;
    uint8_t *lenLSB = p++;
    uint8_t *lenMSB = p++;
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

    // Complete List of 16-bit UUIDs: FFE0
    *p++ = 0x03; *p++ = 0x03;
    *p++ = 0xE0; *p++ = 0xFF;

    uint16_t plen = (uint16_t)((p - MSG_SetScanRsp) - 3);
    *lenLSB = (uint8_t)(plen & 0xFF);
    *lenMSB = (uint8_t)(plen >> 8);
    *p++ = 0x00; // FCS placeholder
}

/* ===== GATT: HM-10 UART service (FFE0/FFE1) ===== */
static uint16_t gHandleFFE1 = 0;

// Add Service (Primary, 0xFFE0)
static const uint8_t MSG_AddService_FFE0[] = {
    SOF, 3, 0x00,
    0x35, 0x81,    // Add Service
    0x01,          // Primary
    0xE0, 0xFF,    // UUID 0xFFE0
    0x00
};

// Add Char Value (0xFFE1, WRITE | WRITE_NO_RSP)
static const uint8_t MSG_AddCharValue_FFE1[] = {
    SOF, 0x08, 0x00,
    0x35, 0x82,   // Add Char Value
    0x02,         // GATT Write Permission
    0x0C, 0x00,   // Properties = WWR + Write
    0x00,         // RFU
    0x00, 0x02,   // Max len = 512
    0xE1, 0xFF,   // UUID 0xFFE1
    0x00
};

// Optional descriptor "UART RX"
static const uint8_t MSG_AddCharDesc_FFE1[] = {
    SOF, 12, 0x00,
    0x35, 0x83,
    0x80,          // User Description
    0x01,          // Readable
    0x08, 0x00,    // Max len
    0x08, 0x00,    // Initial len
    'U','A','R','T',' ','R','X',0,
    0x00
};

// Register last-added service
static const uint8_t MSG_RegisterService[] = {
    SOF, 0x00, 0x00,
    0x35, 0x84,
    0x00
};

/* ===== Write Confirmation (if SNP asks) ===== */
static uint8_t MSG_WriteCnf[] = {
    SOF, 0x03, 0x00,
    0x55, 0x88,
    0x00,       // Success
    0x00, 0x00, // Conn handle
    0x00
};

/* ===== Utilities ===== */
// For SNP Char Write Indication, f[1..2] is PAYLOAD length only (excludes cmd0/cmd1).
// Payload layout: conn(2) + attr(2) + resp(1) + offset(2) + data(N).
// So data length = len - 7 (if len > 7).
static inline uint16_t snp_write_payload_len(const uint8_t *f){
    uint16_t len_payload = (uint16_t)f[1] | ((uint16_t)f[2] << 8);
    const uint16_t payload_header = 2 + 2 + 1 + 2; // = 7
    return (len_payload > payload_header) ? (uint16_t)(len_payload - payload_header) : 0;
}

/* ===== Interpret BLE Joystick bytes ===== */
static void handle_bytes(const uint8_t *data, uint16_t n){
    for(uint16_t i=0;i<n;i++){
        char ch = (char)data[i];

        // Normalize case
        if (ch >= 'A' && ch <= 'Z') ch = (char)(ch + ('a' - 'A'));

        // Ignore '0' (release)
        if (ch == '0') {
            DBG_write("release ignored\r\n");
            continue;
        }

        if (ch == 'a') {
            LED_on();  DBG_write("LED -> ON  ('A'/'a')\r\n");
        } else if (ch == 'b') {
            LED_off(); DBG_write("LED -> OFF ('B'/'b')\r\n");
        } else {
            // other letters (c..h) not used yet
            DBG_write("ignored: "); DBG_hexb((const uint8_t*)&data[i],1); DBG_write("\r\n");
        }
    }
}

/* ===== BLE init + advertise ===== */
static void BLE_InitAndAdvertise(void){
    AP_Reset();

    (void)sendrx(MSG_GetStatus);
    (void)sendrx(MSG_GetVersion);

    // HM-10 UART service
    DBG_write("Add Service FFE0\r\n");
    (void)sendrx(MSG_AddService_FFE0);

    DBG_write("Add Char FFE1\r\n");
    (void)sendrx(MSG_AddCharValue_FFE1);
    gHandleFFE1 = ((uint16_t)rx[7] << 8) | rx[6];   // not required, handy for debug

    DBG_write("Add Desc FFE1\r\n");
    (void)sendrx(MSG_AddCharDesc_FFE1);

    DBG_write("Register Service (FFE0)\r\n");
    (void)sendrx(MSG_RegisterService);

    // Advertising
    buildScanRsp();
    (void)sendrx(MSG_SetAdvData);
    (void)sendrx(MSG_SetScanRsp);
    (void)sendrx(MSG_StartAdv);

    DBG_write("Advertising \"" NAME "\" with HM-10 UART service (FFE0/FFE1)\r\n");
}

/* ===== main ===== */
int main(void){
    SYSCFG_DL_init();   // UART0/1 + MRDY/SRDY/RESET + PB27 via SysConfig
    UART_initBLE();     // UART1 for SNP link
    DBG_init();         // UART0 debug console
    LED_init();

    DBG_write("\r\n=== Boot (BLE Joystick HM-10 UART) ===\r\n");
    BLE_InitAndAdvertise();

    for(;;){
        if(AP_RecvStatus()){ // SRDY low: SNP has something
            if(AP_RecvMessage(rx, RECVSIZE) == APOK){
                print_frame("RX", rx);

                // Characteristic Write Indication (0x55/0x88)
                if(rx[3] == 0x55 && rx[4] == 0x88){
                    // [5..6]=conn, [7..8]=attr handle, [9]=resp, [10..11]=offset, [12..]=data
                    uint16_t h = ((uint16_t)rx[8] << 8) | rx[7]; // debug
                    uint8_t  respNeeded = rx[9];
                    const uint8_t *data = &rx[12];

                    uint16_t payLen = snp_write_payload_len(rx); // <-- FIXED: len - 7

                    DBG_write("WRITE h="); DBG_hex16(h);
                    DBG_write(" len="); DBG_dec(payLen);
                    DBG_write(" data=");
                    if (payLen) DBG_hexb(data, 1);
                    DBG_write("\r\n");

                    if (payLen) handle_bytes(data, payLen);

                    if(respNeeded){
                        (void)AP_SendMessage(MSG_WriteCnf);
                        print_frame("TX", MSG_WriteCnf);
                    }
                }
            }
        }
        AP_Delay1ms(1);
    }
}