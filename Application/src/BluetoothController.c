#ifdef __MSPM0G3507__

// main.c — MSPM0G3507 + CC26xx SNP (BOOSTXL-CC2650MA)
// BLE Joystick control -> Drive motors (2-byte protocol)
//
// Packet format (per write to FFE1):
//   byte0 = command character
//   byte1 = speed (0..255 -> 0.0..1.0)
//
// Commands (uppercase only; lowercase ignored):
//   A = DRIVE_FWD
//   B = DRIVE_TURN_RIGHT_IN_PLACE
//   C = DRIVE_REV
//   D = DRIVE_TURN_LEFT_IN_PLACE
//   E = ARC_RIGHT_FWD
//   F = ARC_LEFT_FWD
//   G = ARC_RIGHT_REV
//   H = ARC_LEFT_REV
//   '0' = release/stop (speed byte ignored)
//
// IMPORTANT: iOS caches GATT. After flashing new GATT, "Forget" the device
// or toggle Bluetooth on iOS so it re-reads services/characteristics.

#include "../inc/BluetoothController.h"
#include "../inc/ApplicationSwitcher.h"


/* ===== Simple fallback debug helpers (link-safe) ===== */
static void DBG_dec(uint32_t v){
    char buf[12]; int i = sizeof(buf)-1; buf[i]=0;
    if(!v) buf[--i]='0';
    while(v && i){
        buf[--i]=(char)('0'+(v%10));
        v/=10;
    }
    DBG_write(&buf[i]);
}
static void DBG_hex16(uint16_t v){
    static const char hx[]="0123456789ABCDEF"; char s[7];
    s[0]='0'; s[1]='x';
    s[2]=hx[(v>>12)&0xF]; s[3]=hx[(v>>8)&0xF];
    s[4]=hx[(v>>4)&0xF];  s[5]=hx[v&0xF];
    s[6]=0;
    DBG_write(s);
}
static void DBG_hexb(const uint8_t *p, uint16_t n){ DBG_hex(p, n); }
/* ===================================================== */

#define SOF        0xFE
#define RECVSIZE   128
#define NAME       "MSPM0-CC26xx"

static uint8_t rx[RECVSIZE];
char lastCommand = '0';

/* ===== LED: PB27 now named MSPM0_USER_LED2 in SysConfig ===== */
/* SysConfig still defines MSPM0_USER_LED2_PORT / PIN; we just wrap them. */

static GPIO_Handle led2 = {
    .peripheral = MSPM0_USER_LED2_PORT,  // e.g., (GPIO_Regs *)GPIOB
    .pin        = MSPM0_USER_LED2_PIN    // bit mask for PB27
};

static inline void LED_init(void)
{
    /*
     * Direction (output) is typically configured by SysConfig via SYSCFG_DL_init().
     * Here we only ensure a known default state (off).
     */
    gpio_write_pin(led2, 0);
}

static inline void LED_on(void)
{
    gpio_write_pin(led2, 1);
}

static inline void LED_off(void)
{
    gpio_write_pin(led2, 0);
}

/* ===== Frame debug helpers ===== */
static void print_frame(const char *tag, const uint8_t *buf) {
    uint16_t len   = (uint16_t)buf[1] | ((uint16_t)buf[2] << 8);
    uint16_t total = (uint16_t)(1 + 2 + 2 + len + 1);
    DBG_write(tag); DBG_write(": "); DBG_hex(buf, total);
}

static int sendrx(const uint8_t *tx) {
    print_frame("TX", tx);
    int r = AP_SendMessageResponse((uint8_t*)tx, rx, RECVSIZE);
    if (r != APOK) {
        DBG_write("ERR: AP_SendMessageResponse failed\r\n");
        return r;
    }
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

/* ===== Drive helpers for 2-byte joystick protocol ===== */

static void drive_stop(void)
{
    motor_set_speed_left(0.0f);
    motor_set_speed_right(0.0f);
    LED_off();
}

/* Map 0..255 -> 0.0..1.0 */
static inline float speed_from_byte(uint8_t b)
{
    return ((float)b) / 255.0f;
}

/* How “curvy” the arc is: 0 = straight, 1 = very tight */
#define ARC_GAIN 0.5f

/* Interpret joystick bytes:
 *   Packets are 2 bytes: [command, speed]
 *   command: 'A'..'H' as per table, '0' = stop
 *   speed:   0..255 mapped to 0.0..1.0
 *
 *   Lowercase is ignored; odd trailing bytes are ignored.
 */
static void handle_bytes(const uint8_t *data, uint16_t n)
{
    uint16_t i = 0;

    while (i < n) {
        char cmd = (char)data[i];

        /* Stop command: '0' or NUL (for compatibility) */
        if (cmd == '0' || cmd == 0x00) {
            drive_stop();
            DBG_write("DRIVE: stop ('0' or NUL)\r\n");
            i += 1;
            continue;
        }

        /* For motion commands we need a speed byte */
        if (i + 1 >= n) {
            DBG_write("WARN: partial command without speed byte, ignored\r\n");
            break;
        }

        uint8_t speed_byte = data[i + 1];
        float   s          = speed_from_byte(speed_byte);

        if (speed_byte == 0) {
            drive_stop();
            DBG_write("DRIVE: stop (speed=0)\r\n");
            i += 2;
            continue;
        }

        switch (cmd) {
            case 'A':  // DRIVE_FWD
                motor_set_speed_left(+s);
                motor_set_speed_right(+s);
                LED_on();
                DBG_write("DRIVE: forward ('A'), speed="); DBG_dec(speed_byte); DBG_write("\r\n");
                break;

            case 'C':  // DRIVE_REV
                motor_set_speed_left(-s);
                motor_set_speed_right(-s);
                LED_on();
                DBG_write("DRIVE: reverse ('C'), speed="); DBG_dec(speed_byte); DBG_write("\r\n");
                break;

            case 'B':  // DRIVE_TURN_RIGHT_IN_PLACE
                motor_set_speed_left(+s);
                motor_set_speed_right(-s);
                LED_on();
                DBG_write("DRIVE: turn right in place ('B'), speed="); DBG_dec(speed_byte); DBG_write("\r\n");
                break;

            case 'D':  // DRIVE_TURN_LEFT_IN_PLACE
                motor_set_speed_left(-s);
                motor_set_speed_right(+s);
                LED_on();
                DBG_write("DRIVE: turn left in place ('D'), speed="); DBG_dec(speed_byte); DBG_write("\r\n");
                break;

            case 'E':  // ARC_RIGHT_FWD
                // Forward, curving right: translational +, angular -
                drive_arc(+s, -s * ARC_GAIN);
                LED_on();
                DBG_write("DRIVE: arc right fwd ('E'), speed="); DBG_dec(speed_byte); DBG_write("\r\n");
                break;

            case 'F':  // ARC_LEFT_FWD
                // Forward, curving left: translational +, angular +
                drive_arc(+s, +s * ARC_GAIN);
                LED_on();
                DBG_write("DRIVE: arc left fwd ('F'), speed="); DBG_dec(speed_byte); DBG_write("\r\n");
                break;

            case 'G':  // ARC_RIGHT_REV
                // Reverse, curving right: translational -, angular +
                drive_arc(-s, +s * ARC_GAIN);
                LED_on();
                DBG_write("DRIVE: arc right rev ('G'), speed="); DBG_dec(speed_byte); DBG_write("\r\n");
                break;

            case 'H':  // ARC_LEFT_REV
                // Reverse, curving left: translational -, angular -
                drive_arc(-s, -s * ARC_GAIN);
                LED_on();
                DBG_write("DRIVE: arc left rev ('H'), speed="); DBG_dec(speed_byte); DBG_write("\r\n");
                break;

            default:
                // Ignore everything else (including lowercase letters)
                DBG_write("ignored cmd byte: ");
                DBG_hexb((const uint8_t *)&data[i], 1);
                DBG_write("\r\n");
                break;
        }

        // Commands with speed consume 2 bytes
        i += 2;
        if (cmd != lastCommand) {
            switch (cmd) {
                case 'A':  // DRIVE_FWD
                    SSD1306_DrawBMP(32, 64, up_arrow, 7, SSD1306_WHITE);
                    SSD1306_OutBuffer();  // Output to the OLED
                    break;

                case 'C':  // DRIVE_REV
                    SSD1306_DrawBMP(32, 64, down_arrow, 7, SSD1306_WHITE);
                    SSD1306_OutBuffer();  // Output to the OLED
                    break;

                case 'B':  // DRIVE_TURN_RIGHT_IN_PLACE
                    SSD1306_DrawBMP(32, 64, right_arrow, 7, SSD1306_WHITE);
                    SSD1306_OutBuffer();
                    break;

                case 'D':  // DRIVE_TURN_LEFT_IN_PLACE
                    SSD1306_DrawBMP(32, 64, left_arrow, 7, SSD1306_WHITE);
                    SSD1306_OutBuffer();
                    break;
            }
        }
        lastCommand = cmd;
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
int run_bluetooth_controller(void){
    SYSCFG_DL_init();   // UART0/1 + MRDY/SRDY/RESET + LED via SysConfig
    UART_initBLE();     // UART1 for SNP link
    DBG_init();         // UART0 debug console
    LED_init();
    hardware_init();
    SSD1306_ClearBuffer();
    SSD1306_OutBuffer();
    SSD1306_OutString("Running BT Remote Control");
    // SSD1306_ClearBuffer(); // Clear the data buffer for the OLED controller


    // --- Bind & init your motor handles here ---
    // Provide valid PWM handles and encoder GPIOs before using motors.
    // Example (replace with real handles generated by SysConfig):
    //
    // Motor_Handle mh = {
    //     .left_motor_forward   = <PWM TIMA1.CCP0>,
    //     .left_motor_backward  = <PWM TIMA1.CCP1>,
    //     .right_motor_forward  = <PWM TIMG7.CCP1>,
    //     .right_motor_backward = <PWM TIMA0.CCP2>,
    //     .left_encoder_a  = <GPIO MOTOR_ENCODERS_MOTOR_LEFT_A>,
    //     .left_encoder_b  = <GPIO MOTOR_ENCODERS_MOTOR_LEFT_B>,
    //     .right_encoder_a = <GPIO MOTOR_ENCODERS_MOTOR_RIGHT_A>,
    //     .right_encoder_b = <GPIO MOTOR_ENCODERS_MOTOR_RIGHT_B>,
    // };
    // motor_init(mh);
    // -------------------------------------------

    DBG_write("\r\n=== Boot (BLE Joystick HM-10 UART, 2-byte commands) ===\r\n");
    BLE_InitAndAdvertise();

    while(!application_yield) {
        if(AP_RecvStatus()){ // SRDY low: SNP has something
            if(AP_RecvMessage(rx, RECVSIZE) == APOK){
                print_frame("RX", rx);

                // Characteristic Write Indication (0x55/0x88)
                if(rx[3] == 0x55 && rx[4] == 0x88){
                    // [5..6]=conn, [7..8]=attr handle, [9]=resp, [10..11]=offset, [12..]=data
                    uint16_t h = ((uint16_t)rx[8] << 8) | rx[7]; // debug
                    uint8_t  respNeeded = rx[9];
                    const uint8_t *data = &rx[12];

                    uint16_t payLen = snp_write_payload_len(rx); // payload length = len - 7

                    DBG_write("WRITE h="); DBG_hex16(h);
                    DBG_write(" len="); DBG_dec(payLen);
                    DBG_write(" data=");
                    if (payLen) DBG_hexb(data, payLen);
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
    return 0;
}


#endif // __MSPM0G3507__