//
// Created by zed4805 on 23/11/21.
//

#ifndef LEADCLOUDDDRIVER_MATRIXHARDWAREDRIVER_H
#define LEADCLOUDDDRIVER_MATRIXHARDWAREDRIVER_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>

/////////////////////////
//////OFFSETS////////////
//////////////////////////////////////////////////
#define BCM2711_PERI_BASE_ADDR          0xFE000000

#define GPIO_REG_OFFSET                 0x200000
#define COUNTER_1MHz_REGISTER_OFFSET    0x3000

#define GPIO_PWM_BASE_OFFSET            (GPIO_REG_OFFSET + 0xC000)
#define GPIO_CLK_BASE_OFFSET            0x101000

#define REGISTER_BLOCK_SIZE             4*1024
//TODO justify these values

#define PIN_SET_INPUT(x)                *(s_gpio_registers + (x/10)) &= ~(7ull<<(((x)%10)*3))
#define PIN_SET_OUTPUT(x)               *(s_gpio_registers + (x/10)) |= (1ull<<(((x)%10)*3))
//#define PIN_SET_OUTPUT(x)               PIN_SET_INPUT(x); *(s_gpio_registers + (x/10)) |= (1ull<<(((x)%10)*3))
                                            //  x/10    -> 10 gpio pin definitions per GPFSELX
                                            //  & / |   -> clear / set pins
                                            //  % 10    -> subindex to get actual 3 bit register
                                            //  * 3     -> 3 bits per register

                                            //calls set input to clear the bits first

#define PIN_OUTPUT_HIGH(x)              *(s_gpio_registers + (0x1c / sizeof(uint32_t))) |= (x)
#define PIN_OUTPUT_LOW(x)               *(s_gpio_registers + (0x28 / sizeof(uint32_t))) |= (x)

//////////////////////////////////////////////////


/////////////////////////
//////CLOCK//////////////
//////////////////////////////////////////////////

#define EMPIRICAL_NANOSLEEP_OVERHEAD_US 12
#define MINIMUM_NANOSLEEP_TIME_US 5

/////////////////////////////////////////////////
struct PinMapping {
    int OE; //output enable
    int CLK; // clock
    int STB; //strobe / latch

    int A, B, C, D; //row addressing

    int R1, G1, B1; //first block
    int R2, G2, B2; //second block
};

class MatrixHardwareDriver {
private:
    uint32_t *s_gpio_registers;
    uint32_t *s_timer_registers;
    uint32_t *s_PWM_registers;
    uint32_t *s_CLK_registers;

    uint32_t *s_gpio_output_high;
    uint32_t *s_gpio_output_low;
    uint32_t *s_gpio_read;

    PinMapping p;
    uint32_t bitMask;

    int n;

    int row_n;
    int col_n;
    int color_depth;

    uint32_t* pixelBuffer;
public:
    MatrixHardwareDriver();

    bool Init(int row_number, int col_number);


    void create_bit_mask();

    uint32_t *map_register(off_t reg_off);

    bool map_all_registers();

    static bool checkForDDriver();

    void InitOutputs();

    inline uint32_t RGBA8tobitMapping(uint32_t color1, uint32_t color2, int bit);

    inline uint32_t set_address(int address);

    void drive_row();
    void drive_row2();

    void drive_matrix();

    void wait_nanoseconds(long nanos);
    void busy_wait(long nanos);
    
    void clearPins();

    void pinTestHelper(char* pinname, int pin);
    void testPins();

    friend class gpio;
};

#endif //LEADCLOUDDDRIVER_MATRIXHARDWAREDRIVER_H
