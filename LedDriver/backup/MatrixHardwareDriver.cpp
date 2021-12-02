//
// Created by zed4805 on 23/11/21.
//


#include "MatrixHardwareDriver.h"



PinMapping AdafruitHatPWMPinMapping = {
        .OE = 18, //PWM hack
        .CLK = 17,
        .STB = 21,

        .A = 22,
        .B = 26,
        .C = 27,
        .D = 20,

        .R1 = 5,
        .G1 = 13,
        .B1 = 6,

        .R2 = 12,
        .G2 = 16,
        .B2 = 23,

};

PinMapping AdafruitHatPinMapping = {
        .OE = 4, 
        .CLK = 17,
        .STB = 21,

        .A = 22,
        .B = 26,
        .C = 27,
        .D = 20,

        .R1 = 5,
        .G1 = 13,
        .B1 = 6,

        .R2 = 12,
        .G2 = 16,
        .B2 = 23,

};



uint32_t *MatrixHardwareDriver::map_register(off_t reg_off) {
    int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);

    if (mem_fd < 0)
        return nullptr;

    auto *result = (uint32_t *) mmap(
            nullptr,
            REGISTER_BLOCK_SIZE,
            PROT_READ | PROT_WRITE,
            MAP_SHARED,
            mem_fd,
            BCM2711_PERI_BASE_ADDR + reg_off
            );

    close(mem_fd);

    if(result == MAP_FAILED){
        perror("mmap error: ");
        fprintf(stderr, "MMapping from base 0x%x, offset 0x%lx\n", BCM2711_PERI_BASE_ADDR, reg_off);
        return nullptr;
    }

    return result;
}

bool MatrixHardwareDriver::map_all_registers(){
    printf("\tMapping registers...\n");
    if(s_gpio_registers != nullptr)
        return true;

    s_gpio_registers = map_register(GPIO_REG_OFFSET);
    if(s_gpio_registers == nullptr)
        return false; //could not map gpio registers

    printf("\t\tMapped s_gpio_registers\n");

    uint32_t * timer_reg = map_register(COUNTER_1MHz_REGISTER_OFFSET);
    if(timer_reg != nullptr){
        s_timer_registers = timer_reg + 1;
        printf("\t\tSuccessfully mapped timer\n");
    }

    s_PWM_registers = map_register(GPIO_PWM_BASE_OFFSET);
    printf("\t\tMapped s_PWM_registers\n");
    s_CLK_registers = map_register(GPIO_CLK_BASE_OFFSET);
    printf("\t\tMapped s_CLK_registers\n");

    s_gpio_output_high = s_gpio_registers + (0x1c / sizeof(uint32_t));
    s_gpio_output_low = s_gpio_registers + (0x28 / sizeof(uint32_t));
    s_gpio_read = s_gpio_registers + (0x34 / sizeof(uint32_t));

    return true;
}

bool MatrixHardwareDriver::checkForDDriver() {
    FILE *f = fopen("/dev/modules/ledmatrix", "r");

    if(f == nullptr)
        return false;

    fclose(f);

    return true;
}

void MatrixHardwareDriver::InitOutputs() {
    PIN_SET_INPUT(p.OE);
    PIN_SET_INPUT(p.CLK);
    PIN_SET_INPUT(p.STB);

    PIN_SET_INPUT(p.A);
    PIN_SET_INPUT(p.B);
    PIN_SET_INPUT(p.C);
    PIN_SET_INPUT(p.D);

    PIN_SET_INPUT(p.R1);
    PIN_SET_INPUT(p.G1);
    PIN_SET_INPUT(p.B1);

    PIN_SET_INPUT(p.R2);
    PIN_SET_INPUT(p.G2);
    PIN_SET_INPUT(p.B2);

    PIN_SET_OUTPUT(p.OE);
    PIN_SET_OUTPUT(p.CLK);
    PIN_SET_OUTPUT(p.STB);

    PIN_SET_OUTPUT(p.A);
    PIN_SET_OUTPUT(p.B);
    PIN_SET_OUTPUT(p.C);
    PIN_SET_OUTPUT(p.D);

    PIN_SET_OUTPUT(p.R1);
    PIN_SET_OUTPUT(p.G1);
    PIN_SET_OUTPUT(p.B1);

    PIN_SET_OUTPUT(p.R2);
    PIN_SET_OUTPUT(p.G2);
    PIN_SET_OUTPUT(p.B2);
}

MatrixHardwareDriver::MatrixHardwareDriver() {
    
}

bool MatrixHardwareDriver::Init(int row_number, int col_number){
    printf("Initializing matrix...\n");

    p = AdafruitHatPinMapping;
    
    s_gpio_registers = NULL;
    s_PWM_registers = NULL;
    s_timer_registers = NULL;
    s_CLK_registers = NULL;
    
    row_n = row_number;
    col_n = col_number;
    color_depth = 8;
    
    pixelBuffer = static_cast<uint32_t *>(calloc(row_n * col_n, sizeof(uint32_t)));
    if(pixelBuffer != nullptr){
        printf("\tSuccessfully allocated pixel buffer\n");
    } else{
        printf("\tFailed to allocate pixel buffer\n");
        return false;
    }
    
    /*for(int i = 0; i < row_n * col_n; i++){
        pixelBuffer[i] = 0xFFFFFFFF;
    }*/
    
    
    //Red cross
    for (int i = 0; i < row_n; ++i) {
        //Left corner
        pixelBuffer[col_n * i + 2 * i] = 0xFF0000FF;
        pixelBuffer[col_n * i + 2 * i + 1] = 0xFF0000FF;
        //Right corner
        pixelBuffer[col_n * i + 63 - 2 * i] = 0xFF0000FF;
        pixelBuffer[col_n * i + 63 - 2 * i - 1] = 0xFF0000FF;
    }

    create_bit_mask();
    if(bitMask != 0){
        printf("\tSuccessfully created bit mask\n");
    } else{
        printf("\tFailed to create bit mask\n");
        return false;
    }

    bool successful = map_all_registers();
    if(successful){
        printf("\tSuccessfully mapped all registers\n");
    } else{
        printf("\tFailed to map registers\n");
        return false;
    }

    InitOutputs();
    printf("\tSuccessfully initialized all output pins\n");


    //PIN_OUTPUT_HIGH(1 << p.OE);

    return true;
}

void MatrixHardwareDriver::create_bit_mask() {
    bitMask |= 1 << p.OE;
    bitMask |= 1 << p.CLK;
    bitMask |= 1 << p.STB;

    bitMask |= 1 << p.A;
    bitMask |= 1 << p.B;
    bitMask |= 1 << p.C;
    bitMask |= 1 << p.D;

    bitMask |= 1 << p.R1;
    bitMask |= 1 << p.G1;
    bitMask |= 1 << p.B1;

    bitMask |= 1 << p.R2;
    bitMask |= 1 << p.G2;
    bitMask |= 1 << p.B2;
}

void MatrixHardwareDriver::drive_row2(){
    static uint32_t address = 0;

    static uint32_t color_clk_mask = 1 << p.CLK | 1 << p.R1 | 1 << p.G1 | 1 << p.B1 | 1 << p.R2 | 1 << p.G2 | 1 << p.R2;
    static uint32_t address_mask = 1 << p.A | 1 << p.B | 1 << p.C | 1 << p.D;

    uint32_t addressBits = set_address(address);
    bool toggle = false;
    for(int loop = 0; loop < 1000000; loop++){
        for(int col; col < col_n; col++){
            uint32_t color_bits = RGBA8tobitMapping(
                toggle? 0xFF0000FF : 0x00FF00FF,
                toggle? 0xFF00FFFF : 0x000000FF,
                0);
            toggle = !toggle;
            //WriteMaskedBits
            *s_gpio_output_low = (~color_bits & color_clk_mask);
            *s_gpio_output_high = (color_bits & color_clk_mask);
            //SetBits
            *s_gpio_output_high = 1 << p.CLK;
        }
        //ClearBits
        *s_gpio_output_low = color_clk_mask;

        *s_gpio_output_low = (~addressBits & address_mask);
        *s_gpio_output_high = addressBits & address_mask;

        //Set Bits
        *s_gpio_output_high = 1 << p.STB;
        //Clear Bits
        *s_gpio_output_low = 1 << p.STB;

        //ClearBits
        *s_gpio_output_low = 1 << p.OE;
        wait_nanoseconds(300);
        //SetBits
        *s_gpio_output_high = 1 << p.OE;
    }

    address++;
    address &= 0x0F;

    printf("\ts_gpio_read -> %x\n", *s_gpio_read);
}

void MatrixHardwareDriver::drive_row(){
    clearPins();
    static uint32_t address = 0;

    uint32_t addressBits = set_address(address);
    
    *s_gpio_output_low = (~addressBits) & 0x0F;
    *s_gpio_output_high = addressBits;


    wait_nanoseconds(100);

    for(int j  = 0; j < 12800; j++){

        for (int i = 0; i < col_n; i++)
        {
            uint32_t colorBits = RGBA8tobitMapping(
                        0x00000000,//0xFF0000FF,
                        0xFF00FFFF,//0x00000000,//0x7FFF7FFF,
                        0);

            *s_gpio_output_low |= 1 << p.CLK;//reset clock
            *s_gpio_output_high |= addressBits | colorBits | 1 << p.CLK; //place color, address
            //*s_gpio_output_high |= 1 << p.CLK; //clock in
        }
        //OE interaction here?
        *s_gpio_output_low |= 1 << p.CLK; //clock in

        //finish row
        *s_gpio_output_high |= 1 << p.STB; 
        *s_gpio_output_low |= 1 << p.STB;

        //start OE pulse
        *s_gpio_output_low |= 1 << p.OE;
        wait_nanoseconds(180); //80 40 10
        //wait nanoseconds
        *s_gpio_output_high |= 1 << p.OE;
    }

    printf("\tOutputs %x\n", *s_gpio_read);

    address++;
    address &= 0x0F;
}

int printColormgs(int row, uint32_t C1, uint32_t C2){
    asm("");
    printf("Row %d, left pixel C1 %x C2 %x\n", row, C1, C2);

    return 0;
}

void MatrixHardwareDriver::drive_matrix() {
    //for each row
    //  for each color depth bit
    //      for each pixel

    uint32_t currBits = 0;
    //ABCD set to the first row
    //CLK set to 0
    //STB set to 0
    //OE set to 0

    //matrix is double driven
    //  RGB1 to drive rows 0-15
    //  RGB2 to drive rows 16-31

    //reset pins


    for (int row = 0; row < row_n / 2; ++row) {
        PIN_OUTPUT_LOW(1 << p.STB | 1 << p.CLK | 1 << p.R1 | 1 << p.R2 | 1 << p.G1 | 1 << p.G2 |1 << p.B1 | 1 << p.B2 | 1 << p.A | 1 << p.B | 1 << p.C | 1 << p.D);
        PIN_OUTPUT_HIGH(1 << p.OE);
        
        currBits = set_address(row);
        for (int depth = 0; depth < color_depth; ++depth){//< color_depth; ++depth) {
            for (int col = 0; col < col_n; ++col) {
                currBits |= RGBA8tobitMapping(
                        pixelBuffer[row * col_n + col],
                        pixelBuffer[(row + row_n/2) * col_n + col],
                        depth);
                PIN_OUTPUT_LOW(1 << p.CLK); //reset clock
                PIN_OUTPUT_HIGH(currBits); //place color, address
                //printf("\t%x\n", currBits);
                PIN_OUTPUT_HIGH(1 << p.CLK); //clock in
            }

            PIN_OUTPUT_LOW(1 << p.CLK);


            //finish row
            PIN_OUTPUT_HIGH(1 << p.STB);
            PIN_OUTPUT_LOW(1 << p.STB);

            //start OE pulse
            PIN_OUTPUT_LOW(1 << p.OE);
            wait_nanoseconds(180); //80 40 10
            //wait nanoseconds
            PIN_OUTPUT_HIGH(1 << p.OE);
        }

    }
}

uint32_t MatrixHardwareDriver::RGBA8tobitMapping(uint32_t color1, uint32_t color2, int bit) {
    uint32_t bitMapping = (((color1 >> 24) >> bit) & 0x1) << p.R1;
                    //24 -> R channel
                    //>> bit: get bit 2 -> >> 2
                    //0x1 : get the bit
                    //<< p.R1 : shift it to the correct place
    bitMapping |= (((color1 >> 16) >> bit) & 0x1) << p.G1;
    bitMapping |= (((color1 >> 8) >> bit) & 0x1) << p.B1;

    bitMapping |= (((color2 >> 24) >> bit) & 0x1) << p.R2;
    bitMapping |= (((color2 >> 16) >> bit) & 0x1) << p.G2;
    bitMapping |= (((color2 >> 8) >> bit) & 0x1) << p.B2;

    return bitMapping;
}

uint32_t MatrixHardwareDriver::set_address(int address) {
    uint32_t pins = 0;
    /*
    pins |= (address & 0x1) << p.D;    //abcD
    pins |= ((address & 0x2) >> 1) << p.C;   //abCd
    pins |= ((address & 0x4) >> 2) << p.B;  //aBcd
    pins |= ((address & 0x8) >> 3) << p.A; //Abcd
    */
    
    pins |= ((address & 0x1) >> 0) << p.A; //Abcd
    pins |= ((address & 0x2) >> 1) << p.B; //aBcd
    pins |= ((address & 0x4) >> 2) << p.C; //abCd
    pins |= ((address & 0x8) >> 3) << p.D; //abcD
    

    printf("Address %d-> ABCD to %c%c%c%c -> %x\n", address, (pins & (1 << p.A))? '1' : '0', (pins & (1 << p.B))? '1' : '0', (pins & (1 << p.C))? '1' : '0', (pins & (1 << p.D)? '1' : '0'), pins);
    return pins;
}

void MatrixHardwareDriver::wait_nanoseconds(long nanos) {
    static long kJitterAllowanceNanos = (EMPIRICAL_NANOSLEEP_OVERHEAD_US + 10) * 1000;
    if (nanos > kJitterAllowanceNanos + MINIMUM_NANOSLEEP_TIME_US*1000){
        const uint32_t before = *s_timer_registers;
        struct timespec sleep_time{0, nanos - kJitterAllowanceNanos};
        nanosleep(&sleep_time, nullptr);
        const uint32_t after = *s_timer_registers;
        const long nanoseconds_passed = 1000 * (uint32_t)(after - before);
        if(nanoseconds_passed > nanos){
            return;
        } else {
            nanos -= nanoseconds_passed;
        }
    } else {
        if(nanos > (EMPIRICAL_NANOSLEEP_OVERHEAD_US + MINIMUM_NANOSLEEP_TIME_US) * 1000){
            struct timespec sleep_time = {0, nanos - EMPIRICAL_NANOSLEEP_OVERHEAD_US*1000};
            nanosleep(&sleep_time, nullptr);
            return;
        }
    }

    busy_wait(nanos);
}

void MatrixHardwareDriver::busy_wait(long nanos) {
    if (nanos < 20) return;
    // Interesting, the Pi4 is _slower_ than the Pi3 ? At least for this busy loop
    for (uint32_t i = (nanos - 5) * 100 / 132; i != 0; --i) {
        asm("");
    }
}

void MatrixHardwareDriver::clearPins(){
    uint32_t last_read = *s_gpio_read;
    *s_gpio_output_low = 0xFFFFFFFF & ~(1 << p.OE);
    last_read = *s_gpio_read;
}


void MatrixHardwareDriver::pinTestHelper(char* pinName, int pin){
    static uint32_t base = 0;
    if(base == 0){
        base = *s_gpio_read;
        printf("\nBase defined as: 0x%x\n\n", base);

        const char* OE__ = (((*s_gpio_read) & (1 << p.OE)) != 0)? "OE " : "";
        const char* CLK__ = (((*s_gpio_read) & (1 << p.CLK)) != 0)? "CLK " : "";
        const char* STB__ = (((*s_gpio_read) & (1 << p.STB)) != 0)? "STB " : "";
        const char* A__ = (((*s_gpio_read) & (1 << p.A)) != 0)? "A " : "";
        const char* B__ = (((*s_gpio_read) & (1 << p.B)) != 0)? "B " : "";
        const char* C__ = (((*s_gpio_read) & (1 << p.C)) != 0)? "C " : "";
        const char* D__ = (((*s_gpio_read) & (1 << p.D)) != 0)? "D " : "";

        const char* R1__ = (((*s_gpio_read) & (1 << p.R1)) != 0)? "R1 " : "";
        const char* G1__ = (((*s_gpio_read) & (1 << p.G1)) != 0)? "G1 " : "";
        const char* B1__ = (((*s_gpio_read) & (1 << p.B1)) != 0)? "B1 " : "";

        const char* R2__ = (((*s_gpio_read) & (1 << p.R2)) != 0)? "R2 " : "";
        const char* G2__ = (((*s_gpio_read) & (1 << p.G2)) != 0)? "G2 " : "";
        const char* B2__ = (((*s_gpio_read) & (1 << p.B2)) != 0)? "B2 " : "";


        printf("\tBase\t-> 0x%x\n\t\t%s%s%s%s%s%s%s%s%s%s%s%s%s\n", *s_gpio_read,
            OE__, CLK__, STB__, A__, B__, C__, D__, R1__, G1__, B1__, R2__, G2__, B2__);
    }

    printf("%s:\n", pinName);
    *s_gpio_output_high = 1 << pin;
    const char* OE = (((*s_gpio_read) & (1 << p.OE)) != 0)? "OE " : "";
    const char* CLK = (((*s_gpio_read) & (1 << p.CLK)) != 0)? "CLK " : "";
    const char* STB = (((*s_gpio_read) & (1 << p.STB)) != 0)? "STB " : "";
    const char* A = (((*s_gpio_read) & (1 << p.A)) != 0)? "A " : "";
    const char* B = (((*s_gpio_read) & (1 << p.B)) != 0)? "B " : "";
    const char* C = (((*s_gpio_read) & (1 << p.C)) != 0)? "C " : "";
    const char* D = (((*s_gpio_read) & (1 << p.D)) != 0)? "D " : "";

    const char* R1 = (((*s_gpio_read) & (1 << p.R1)) != 0)? "R1 " : "";
    const char* G1 = (((*s_gpio_read) & (1 << p.G1)) != 0)? "G1 " : "";
    const char* B1 = (((*s_gpio_read) & (1 << p.B1)) != 0)? "B1 " : "";

    const char* R2 = (((*s_gpio_read) & (1 << p.R2)) != 0)? "R2 " : "";
    const char* G2 = (((*s_gpio_read) & (1 << p.G2)) != 0)? "G2 " : "";
    const char* B2 = (((*s_gpio_read) & (1 << p.B2)) != 0)? "B2 " : "";

    *s_gpio_output_high = 1 << p.A;


    printf("\tHIGH\t-> 0x%x\n\t\t%s%s%s%s%s%s%s%s%s%s%s%s%s\n", *s_gpio_read & ~(base),
        OE, CLK, STB, A, B, C, D, R1, G1, B1, R2, G2, B2);
    *s_gpio_output_low = 1 << pin;

    const char* OE_ = (((*s_gpio_read) & (1 << p.OE)) != 0)? "OE " : "";
    const char* CLK_ = (((*s_gpio_read) & (1 << p.CLK)) != 0)? "CLK " : "";
    const char* STB_ = (((*s_gpio_read) & (1 << p.STB)) != 0)? "STB " : "";
    const char* A_ = (((*s_gpio_read) & (1 << p.A)) != 0)? "A " : "";
    const char* B_ = (((*s_gpio_read) & (1 << p.B)) != 0)? "B " : "";
    const char* C_ = (((*s_gpio_read) & (1 << p.C)) != 0)? "C " : "";
    const char* D_ = (((*s_gpio_read) & (1 << p.D)) != 0)? "D " : "";

    const char* R1_ = (((*s_gpio_read) & (1 << p.R1)) != 0)? "R1 " : "";
    const char* G1_ = (((*s_gpio_read) & (1 << p.G1)) != 0)? "G1 " : "";
    const char* B1_ = (((*s_gpio_read) & (1 << p.B1)) != 0)? "B1 " : "";

    const char* R2_ = (((*s_gpio_read) & (1 << p.R2)) != 0)? "R2 " : "";
    const char* G2_ = (((*s_gpio_read) & (1 << p.G2)) != 0)? "G2 " : "";
    const char* B2_ = (((*s_gpio_read) & (1 << p.B2)) != 0)? "B2 " : "";

    printf("\tLOW\t-> 0x%x\n\t\t%s%s%s%s%s%s%s%s%s%s%s%s%s\n", *s_gpio_read & ~(base),
        OE_, CLK_, STB_, A_, B_, C_, D_, R1_, G1_, B1_, R2_, G2_, B2_);
}

void MatrixHardwareDriver::testPins(){
    clearPins();

    printf("Base val: %x\n", *s_gpio_read);
    uint32_t base = *s_gpio_read;

    PIN_OUTPUT_HIGH(p.OE);
    printf("OE:\n\t%x\n",*s_gpio_read & ~(base));
    sleep(1);
    PIN_OUTPUT_LOW(p.OE);
    printf("OE:\n\t%x\n",*s_gpio_read & ~(base));
    sleep(1);

    PIN_OUTPUT_HIGH(p.CLK);
    printf("CLK:\n\t%x\n",*s_gpio_read & ~(base));
    sleep(1);
    PIN_OUTPUT_LOW(p.CLK);
    printf("CLK:\n\t%x\n",*s_gpio_read & ~(base));
    sleep(1);

    PIN_OUTPUT_HIGH(p.STB);
    printf("STB:\n\t%x\n",*s_gpio_read & ~(base));
    sleep(1);
    PIN_OUTPUT_LOW(p.STB);
    printf("STB:\n\t%x\n",*s_gpio_read & ~(base));
    sleep(1);


    PIN_OUTPUT_HIGH(p.A);
    printf("A:\n\t%x\n",*s_gpio_read & ~(base));
    sleep(1);
    PIN_OUTPUT_LOW(p.A);
    printf("A:\n\t%x\n",*s_gpio_read & ~(base));
    sleep(1);

    PIN_OUTPUT_HIGH(p.B);
    printf("B:\n\t%x\n",*s_gpio_read & ~(base));
    sleep(1);
    PIN_OUTPUT_LOW(p.B);
    printf("B:\n\t%x\n",*s_gpio_read & ~(base));
    sleep(1);

    PIN_OUTPUT_HIGH(p.C);
    printf("C:\n\t%x\n",*s_gpio_read & ~(base));
    sleep(1);
    PIN_OUTPUT_LOW(p.C);
    printf("C:\n\t%x\n",*s_gpio_read & ~(base));
    sleep(1);

    PIN_OUTPUT_HIGH(p.D);
    printf("D:\n\t%x\n",*s_gpio_read & ~(base));
    sleep(1);
    PIN_OUTPUT_LOW(p.D);
    printf("D:\n\t%x\n",*s_gpio_read & ~(base));
    sleep(1);

    PIN_OUTPUT_HIGH(p.R1);
    printf("R1:\n\t%x\n",*s_gpio_read & ~(base));
    sleep(1);
    PIN_OUTPUT_LOW(p.R1);
    printf("R1:\n\t%x\n",*s_gpio_read & ~(base));
    sleep(1);

    PIN_OUTPUT_HIGH(p.G1);
    printf("G1:\n\t%x\n",*s_gpio_read & ~(base));
    sleep(1);
    PIN_OUTPUT_LOW(p.G1);
    printf("G1:\n\t%x\n",*s_gpio_read & ~(base));
    sleep(1);

    PIN_OUTPUT_HIGH(p.B1);
    printf("B1:\n\t%x\n",*s_gpio_read & ~(base));
    sleep(1);
    PIN_OUTPUT_LOW(p.B1);
    printf("B1:\n\t%x\n",*s_gpio_read & ~(base));
    sleep(1);

    PIN_OUTPUT_HIGH(p.R2);
    printf("R2:\n\t%x\n",*s_gpio_read & ~(base));
    sleep(1);
    PIN_OUTPUT_LOW(p.R2);
    printf("R2:\n\t%x\n",*s_gpio_read & ~(base));
    sleep(1);

    PIN_OUTPUT_HIGH(p.G2);
    printf("G2:\n\t%x\n",*s_gpio_read & ~(base));
    sleep(1);
    PIN_OUTPUT_LOW(p.G2);
    printf("G2:\n\t%x\n",*s_gpio_read & ~(base));
    sleep(1);

    PIN_OUTPUT_HIGH(p.B2);
    printf("B2:\n\t%x\n",*s_gpio_read & ~(base));
    sleep(1);
    PIN_OUTPUT_LOW(p.B2);
    printf("B2:\n\t%x\n",*s_gpio_read & ~(base));
    sleep(1);

    printf("Pre-clear val: 0x%x\n", *s_gpio_read);

    clearPins();
    printf("Post-clear val: 0x%x\n", *s_gpio_read);


    pinTestHelper((char*)"OE", p.OE);
    sleep(1);
    pinTestHelper((char*)"CLK", p.CLK);
    sleep(1);
    pinTestHelper((char*)"STB", p.STB);
    sleep(1);

    pinTestHelper((char*)"A", p.A);
    sleep(1);
    pinTestHelper((char*)"B", p.B);
    sleep(1);
    pinTestHelper((char*)"C", p.C);
    sleep(1);
    pinTestHelper((char*)"D", p.D);
    sleep(1);

    pinTestHelper((char*)"R1", p.R1);
    sleep(1);
    pinTestHelper((char*)"G1", p.G1);
    sleep(1);
    pinTestHelper((char*)"B1", p.B1);
    sleep(1);

    pinTestHelper((char*)"R2", p.R2);
    sleep(1);
    pinTestHelper((char*)"G2", p.G2);
    sleep(1);
    pinTestHelper((char*)"B2", p.B2);
}
