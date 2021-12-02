#ifndef LEADCLOUDDDRIVER_DRAWER_H
#define LEADCLOUDDDRIVER_DRAWER_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

class Drawer
{
private:
    /* data */
    uint32_t* buffer;
    int row_n, col_n;
public:
    Drawer(uint32_t* bufferPointer, int row_n, int col_n);
    ~Drawer();

    void zero();
    void fill(uint32_t color);

    void line(uint32_t color, int x1, int y1, int x2, int y2);
    void fastHLine(uint32_t color, int x, int y1, int y2);

    void pixel(uint32_t color, int x, int y);

    void rainbowFill();

    void rainbowTriangle();
};





#endif