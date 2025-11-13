#include "../inc/i2c.h"
#include "../inc/sleep.h"

uint32_t I2C_error;

// This initialization code is based on the example from the MSPM0 academy
// I2C Introduction lab.
// Note: this function doesn't need to be called if the I2C interface is initialized using the syscfg filee
void i2c_init(I2C_Handle *i2cHandle, uint8_t SCLIndex, uint8_t SDAIndex)
{
    I2C_Regs *i2c = i2cHandle->inst;

    uint32_t Mask;

    Mask = I2C_RSTCTL_KEY_UNLOCK_W;
    Mask |= I2C_RSTCTL_RESETASSERT_ASSERT;
    Mask |= I2C_RSTCTL_RESETSTKYCLR_CLR;
    i2c->GPRCM.RSTCTL = Mask;

    Mask = I2C_PWREN_KEY_UNLOCK_W;
    Mask |= I2C_PWREN_ENABLE_ENABLE;
    i2c->GPRCM.PWREN = Mask;

    // ClockDelay(POWER_STARTUP_DELAY); // time for i2c to power up
    sleep_ms(POWER_STARTUP_DELAY);

    // Configure port pins for alternate i2c function
    Mask = IOMUX_PINCM_PC_CONNECTED;    // peripheral connected.
    Mask |= IOMUX_MODE4;                // i2c alternate function
    Mask |= IOMUX_PINCM_INV_DISABLE;    // disable data inversion
    Mask |= IOMUX_PINCM_PIPD_DISABLE;   // disable pull-up/pull-down resistors
    Mask |= IOMUX_PINCM_PIPU_DISABLE;
    Mask |= IOMUX_PINCM_HYSTEN_DISABLE; // hysteresis disabled
    Mask |= IOMUX_PINCM_WUEN_DISABLE;   // wakeup disabled
    Mask |= IOMUX_PINCM_HIZ1_ENABLE;    // high Z (open-drain enable)
    Mask |= IOMUX_PINCM_INENA_ENABLE;   // enable as input
    IOMUX->SECCFG.PINCM[SCLIndex]  = Mask;
    IOMUX->SECCFG.PINCM[SDAIndex]  = Mask;

    i2c->CLKSEL = I2C_CLKSEL_BUSCLK_SEL_ENABLE;
    i2c->CLKDIV = I2C_CLKDIV_RATIO_DIV_BY_1;

    i2c->GFCTL = I2C_GFCTL_AGFEN_DISABLE; // disable analog glitch filter

    i2c->MASTER.MCTR = 0; // Resets all Controller functionality

    // I2C timer period register
    // bits 6-0 TPR (0 to 63), divide by TPR+1
    // SCL clock period (ns) = clock period (ns) * (1 + MTPR) * 10
    // Therefore, SCL (frequency) = clock frequency/10/(1 + MTPR)
    //
    // For 32 MHz bus clock, SYSCLK clock is 32 MHz.
    // For 40 MHz bus clock, SYSCLK clock is ULPCLK 20 MHz.
    // For 80 MHz bus clock, SYSCLK clock is ULPCLK 40 MHz.
    // Set frequency to 400000 Hz

    // bus clock = 40 MHz, MTPR = (20 MHz/400 kHz)/10 - 1 = 4
    if (CLOCK_FREQUENCY == 40000000){
        i2c->MASTER.MTPR = 4;
    }
    // bus clock=32MHz, MTPR = (32 MHz/400 kHz)/10 - 1 = 7
    else if (CLOCK_FREQUENCY == 32000000){
        i2c->MASTER.MTPR = 7;
    }
    // bus clock=80MHz, MTPR = (40 MHz/400 kHz)/10 - 1 = 9
    else if (CLOCK_FREQUENCY == 80000000){
        i2c->MASTER.MTPR = 9;
    }
    else
        Error_Handler();

    Mask = I2C_MFIFOCTL_RXTRIG_LEVEL_1;
    Mask |= I2C_MFIFOCTL_TXTRIG_EMPTY;
    i2c->MASTER.MFIFOCTL = Mask;

    Mask = I2C_MCR_CLKSTRETCH_ENABLE; // enable clock stretching
    i2c->MASTER.MCR = Mask;

    // Enable I2C module.
    i2c->MASTER.MCR |= I2C_MCR_ACTIVE_ENABLE;
}

#define FALSE 0
#define TRUE 1
uint16_t static I2CFillTXFIFO(I2C_Regs *i2c, uint8_t *buffer, uint16_t count)
{
    uint16_t i;
    uint16_t FIFONotFull = TRUE;

    for (i = 0; (i < count) && FIFONotFull; i++) {

        // Check to see whether the TX FIFO is full, which is true
        // if the FIFO Status Register (SR) indicates that the minimum
        // number of bytes (0) can be copied to the FIFO. If true, then
        // update FIFONotFull to exit loop. Otherwise, copy buffer
        // data to TX data register.
        if ((i2c->MASTER.MFIFOSR & I2C_MFIFOSR_TXFIFOCNT_MASK) == I2C_MFIFOSR_TXFIFOCNT_MINIMUM) {
            FIFONotFull = FALSE;
        }
        else {
            i2c->MASTER.MTXDATA = buffer[i];
        }
    }
    return FIFONotFull;
}

uint16_t i2c_send(I2C_Handle *i2cHandle, uint8_t target_address, uint8_t *buffer, uint16_t count)
{
    I2C_Regs *i2c = i2cHandle->inst;    
    uint16_t ReturnValue = TRUE;
    uint32_t Mask;
    uint32_t Value;

    // If the buffer data is successfully copied to the FIFO, then transmit.
    // Otherwise, return 0, indicating no data was transmitted.
    if (I2CFillTXFIFO(i2c,buffer,count) == TRUE){

        // Wait for I2C to be idle. When not idle, the idle bit in the
        // Status Register (SR) is cleared, so wait until it is set.
        while (!(i2c->MASTER.MSR & I2C_MSR_IDLE_MASK));

        // Specify target address and transmit mode
        Value = (target_address << I2C_MSA_SADDR_OFS) | I2C_MSA_DIR_TRANSMIT;
        i2c->MASTER.MSA = Value;

        // STOP bit is generated after burst length number of bytes transferred
        Mask = (I2C_MCTR_MBLEN_MASK | I2C_MCTR_BURSTRUN_MASK | I2C_MCTR_START_MASK | I2C_MCTR_STOP_MASK);
        Value = (uint32_t) (count << I2C_MCTR_MBLEN_OFS);
        Value |= I2C_MCTR_BURSTRUN_ENABLE;
        Value |= I2C_MCTR_START_ENABLE;
        Value |= I2C_MCTR_STOP_ENABLE;
        i2c->MASTER.MCTR = Value | (~Mask & i2c->MASTER.MCTR);

        // Poll until all bytes have been sent.
        while ((i2c->MASTER.MSR & I2C_MSR_BUSBSY_MASK) == I2C_MSR_BUSBSY_SET);

        // Check to see if there was an error.
        if (((i2c->MASTER.MSR & I2C_MSR_ERR_MASK) == I2C_MSR_ERR_SET) ||
            ((i2c->MASTER.MSR & I2C_MSR_ARBLST_MASK) == I2C_MSR_ARBLST_SET)) {
            ReturnValue = FALSE;
        }

        // Wait for I2C to be idle.
        while (!(i2c->MASTER.MSR & I2C_MSR_IDLE_MASK));
    }
    else {
        ReturnValue = FALSE;
    }

    return ReturnValue;
}

void i2c_receive(I2C_Handle *i2cHandle, uint8_t target_address, uint8_t *buffer, uint16_t count)
{
    I2C_Regs *i2c = i2cHandle->inst;
    uint32_t Mask;
    uint32_t Value;

    // Wait for I2C to be idle.
    while (!(i2c->MASTER.MSR & I2C_MSR_IDLE_MASK));

    // Specify target address and receive mode
    Value = (target_address << I2C_MSA_SADDR_OFS) | I2C_MSA_DIR_RECEIVE;
    i2c->MASTER.MSA = Value;

    // STOP bit is generated after burst length number of bytes transferred
    Mask = (I2C_MCTR_MBLEN_MASK | I2C_MCTR_BURSTRUN_MASK | I2C_MCTR_START_MASK | I2C_MCTR_STOP_MASK);
    Value = (uint32_t) (count << I2C_MCTR_MBLEN_OFS);
    Value |= I2C_MCTR_BURSTRUN_ENABLE;
    Value |= I2C_MCTR_START_ENABLE;
    Value |= I2C_MCTR_STOP_ENABLE;
    i2c->MASTER.MCTR = Value | (~Mask & i2c->MASTER.MCTR);

    // Receive all bytes from target.
    for (uint8_t i = 0; i < count; i++) {

        // Poll while RX FIFO is empty.
        while ((i2c->MASTER.MFIFOSR & I2C_MFIFOSR_RXFIFOCNT_MASK) == I2C_MFIFOSR_RXFIFOCNT_MINIMUM);
        buffer[i] = (uint8_t) (i2c->MASTER.MRXDATA & I2C_MRXDATA_VALUE_MASK);
    }
}

