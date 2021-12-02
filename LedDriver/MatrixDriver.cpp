#include "MatrixDriver.hpp"

MatrixDriver::MatrixDriver() : gpio(AdafruitHat, 4)
{
    pixelBuffer = nullptr;
    color_clk_mask = 0;
    address_mask = 0;
}

MatrixDriver::~MatrixDriver()
{
    if(pixelBuffer != nullptr)
        free(pixelBuffer);
}

bool MatrixDriver::Init(int row_n, int col_n){
    if(row_n < 0 || col_n < 0){
        printf("Forbidden matrix dimensions %dx%d", row_n, col_n);
        return false;
    }
    this->row_n = row_n;
    this->col_n = col_n;
    this->color_depth = 8;

    if(!gpio.Init())
        return false;

    pixelBuffer = static_cast<uint32_t *>(calloc(row_n * col_n, sizeof(uint32_t)));
    if(pixelBuffer != nullptr){
        printf("\tSuccessfully allocated pixel buffer\n");
    } else{
        printf("\tFailed to allocate pixel buffer\n");
        return false;
    }

    rowBuffer = static_cast<uint32_t *>(calloc(col_n * color_depth, sizeof(uint32_t)));
    if(rowBuffer != nullptr){
        printf("\tSuccessfully allocated row buffer\n");
    }
    else{
        free(pixelBuffer);
        printf("\tFailed to allocate row buffer\n");
        return false;
    }

#ifdef DEBUGCROSS
    //Red cross
    for (int i = 0; i < row_n; ++i) {
        //Left corner
        pixelBuffer[col_n * i + 2 * i] = 0x00FF00FF;
        pixelBuffer[col_n * i + 2 * i + 1] = 0x0000FFFF;
        //Right corner
        pixelBuffer[col_n * i + 63 - 2 * i] = 0xFF0000FF;
        pixelBuffer[col_n * i + 63 - 2 * i - 1] = 0xFF0000FF;
    }

#endif
    GenerateMasks();

    return true;
}

void MatrixDriver::GenerateMasks(){
    color_clk_mask = gpio.generateRGBCLKMask();
    address_mask = gpio.generateAddressMask();

}

void MatrixDriver::driveMatrix(){
    static int address = -1;
    address++;
    address &= 0x0F;
    for(int row = address; row == address; row++){
    //for(int row = 0; row < row_n / 2; row++){
        generateRow(row);
        uint32_t address_bits = set_address(row);

        uint32_t* row_p = rowBuffer;
        for(int depth = 0; depth < color_depth; depth++){
            for(int col = 0; col < col_n; col++){
                gpio.WriteMaskedBits(*row_p, color_clk_mask);
                gpio.SetBits(1 << gpio.p.CLK);
                
                row_p++;
            }
            gpio.ClearBits(color_clk_mask);

            gpio.WriteMaskedBits(address_bits, address_mask);

            gpio.SetBits(1 << gpio.p.STB);
            gpio.ClearBits(1 << gpio.p.STB);

            gpio.SendPulse(depth);
        }
    }
}

//depth/col/col/.../col/depth -> better for sequential reads (physical proximity)
// column 0, depth 0 | column 1, depth 0|...|column 0 , depth 1|...
void MatrixDriver::generateRow(int row){ 
    for(int col = 0; col < col_n; col++){
        uint32_t color1 = pixelBuffer[row * col_n + col];
        uint32_t color2 = pixelBuffer[(row + row_n/2) * col_n + col];

        uint32_t* row_p = rowBuffer + col;
        for(int depth = 0; depth < color_depth; depth++, row_p += col_n){
            *row_p = (((color1 >> 24) >> depth) & 0x1) << gpio.p.R1;
            *row_p |= (((color1 >> 16) >> depth) & 0x1) << gpio.p.G1;
            *row_p |= (((color1 >> 8) >> depth) & 0x1) << gpio.p.B1;

            *row_p |= (((color2 >> 24) >> depth) & 0x1) << gpio.p.R2;
            *row_p |= (((color2 >> 16) >> depth) & 0x1) << gpio.p.G2;
            *row_p |= (((color2 >> 8) >> depth) & 0x1) << gpio.p.B2;
        }
    }
}

uint32_t MatrixDriver::set_address(int address) {
    uint32_t pins = 0;
    /*
    pins |= (address & 0x1) << p.D;    //abcD
    pins |= ((address & 0x2) >> 1) << p.C;   //abCd
    pins |= ((address & 0x4) >> 2) << p.B;  //aBcd
    pins |= ((address & 0x8) >> 3) << p.A; //Abcd
    */
    
    pins |= ((address & 0x1) >> 0) << gpio.p.A; //Abcd
    pins |= ((address & 0x2) >> 1) << gpio.p.B; //aBcd
    pins |= ((address & 0x4) >> 2) << gpio.p.C; //abCd
    pins |= ((address & 0x8) >> 3) << gpio.p.D; //abcD
    
    //printf("Address %d-> ABCD to %c%c%c%c -> %x\n", address, (pins & (1 << gpio.p.A))? '1' : '0', (pins & (1 << gpio.p.B))? '1' : '0', (pins & (1 << gpio.p.C))? '1' : '0', (pins & (1 << gpio.p.D)? '1' : '0'), pins);
    return pins;
}

uint32_t* MatrixDriver::getBuffer(){
    return pixelBuffer;
}

int MatrixDriver::getRowN(){
    return row_n;
}

int MatrixDriver::getColN(){
    return col_n;
}