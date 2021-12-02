#include "Drawer.hpp"

Drawer::Drawer(uint32_t* bufferPointer, int row_n, int col_n)
{
    this->buffer = bufferPointer;
    this->row_n = row_n;
    this->col_n = col_n;
}

Drawer::~Drawer()
{
}

void Drawer::zero(){
    memset(buffer, 0, row_n * col_n * sizeof(uint32_t));
}

void Drawer::fill(uint32_t color){
    for(int i = 0; i < row_n * col_n; i++){
        memcpy(buffer + i, &color, sizeof(uint32_t));
        //buffer[i] = color;
    }
}

void Drawer::line(uint32_t color, int x1, int y1, int x2, int y2){
    if(x1 > x2){ //reorder
        int tmp = x2;
        x2 = x1;
        x1 = tmp;
    }
    if(y1 > y2){
        int tmp = y2;
        y2 = y1;
        y1 = tmp;
    }

    //sanatize variables
    y1 = y1 < 0? 0 : y1;
    y2 = y2 > col_n? col_n : y2;
    x1 = x1 < 0? 0 : x1;
    x2 = x2 > row_n? row_n : x2;

    for(; x1 != x2 + 1; x1++){
        for(; y1 != y2 + 1; y1++){
            buffer[x1 * col_n + y1] = color;
        }
    }
}

void Drawer::fastHLine(uint32_t color, int x, int y1, int y2){
    if(y1 > y2){
        int tmp = y2;
        y2 = y1;
        y1 = tmp;
    }

    for(; y1 != y2; y1++){
        buffer[x * col_n + y1] = color;
    }
}

void Drawer::pixel(uint32_t color, int x, int y){
    if(x < 0 || x > row_n)
        return;

    if(y < 0 || x > col_n)
        return;

    buffer[(x * col_n) + y] = color;

}

void Drawer::rainbowFill(){
    for(int x = 0; x < row_n; x++){
        for(int y = 0; y < col_n; y++){
            uint32_t red = (x/(float)row_n) * 255;
            red &= 0xFF;
            uint32_t green = (y/(float)col_n) * 255;
            green &= 0xFF;
            buffer[x*col_n + y] = red << 24 | green << 16;
        }
    }
}

void Drawer::rainbowTriangle(){
    for(int x = 0; x < row_n; x++){
        for(int y = 0; y < col_n; y++){
            uint32_t red = ((63-y)/(float)(col_n/2)) * 255;
            red &= 0xFF;
            uint32_t green = (y/(float)(col_n/2)) * 255;
            green &= 0xFF;
            uint32_t blue = (x /(float)col_n) * 255;

            if(y < x || y > 63-x)
                continue;
            buffer[x*col_n + y] = red << 24 | green << 16 | blue << 8;
        }
    }
}