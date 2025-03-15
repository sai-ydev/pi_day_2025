#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "string.h"
#include "lsm6ds3tr-c_reg.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA         24
#define I2C_SCL         25
#define LSM6_ADDR       0x6B

#define LSM6_OK 0
#define LSM6_TIMEOUT 4

/** Please note that is MANDATORY: return 0 -> no Error.**/
int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len);
int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);

/** Optional (may be required by driver) **/
void platform_delay(uint32_t millisec);



int main()
{
    stmdev_ctx_t lsm6_sensor;
    uint8_t whoamI = 0;

    stdio_init_all();
    

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 100*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    
    // PIO Blinking example
    PIO pio = pio0;
    lsm6_sensor.write_reg = platform_write;
    lsm6_sensor.read_reg = platform_read;
    lsm6_sensor.mdelay = platform_delay;
    lsm6_sensor.handle = I2C_PORT;

    for(uint8_t i = 0; i < 255; i++){
        i2c_write_blocking(i2c0, LSM6_ADDR, &whoamI, 1, true);
    }

    while(whoamI != LSM6_ADDR){
        lsm6ds3tr_c_device_id_get(&lsm6_sensor, &whoamI);
        sleep_ms(1000);
        printf("Device ID %X\n", whoamI);
    }
    

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}

int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len)
{
    int32_t result = LSM6_OK;
    uint8_t write_buf[len + 1];

    write_buf[0] = reg;
    memcpy(&write_buf[1], bufp, len);

    result = i2c_write_blocking_until(handle, LSM6_ADDR, write_buf, len + 1,
                                    true, make_timeout_time_ms(LSM6_TIMEOUT));
    if(result != len + 1){
        printf("Failed to write to I2C bus\n");
        return result;
    }
    return LSM6_OK;
}

int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{

    int32_t result = 0;
    uint8_t reg_buf = reg;

    i2c_write_blocking(I2C_PORT, LSM6_ADDR, &reg_buf, 1, true);
    

    i2c_read_blocking(I2C_PORT, LSM6_ADDR, bufp, len, false);
    

    return LSM6_OK;
}

void platform_delay(uint32_t millisec)
{
    sleep_ms(millisec);
}

