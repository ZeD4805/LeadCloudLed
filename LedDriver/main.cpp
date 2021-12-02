//
// Created by zed4805 on 27/11/21.
//

//#include "MatrixHardwareDriver.h"
#include "MatrixDriver.hpp"
#include "Drawer.hpp"

#define ONE_SEC_IN_NANOS 1000000000

/*int main(){
    MatrixHardwareDriver matrix;
    if(matrix.Init(32, 64))
        printf("Successfully initialized matrix\n");
    else{
        printf("Failed to initialize matrix\n");
        return -1;
    }

    //matrix.clearPins();

    while (true){
        struct timespec sleep_time = {0, ONE_SEC_IN_NANOS/2};
        //nanosleep(&sleep_time, nullptr);
        sleep(2);
        //printf("Slept great, thanks...\n");
        //matrix.drive_matrix();
        matrix.drive_row2();
        //matrix.clearPins();
        //matrix.testPins();
    }
    
}*/

int main(){
    MatrixDriver matrix;
    if(matrix.Init(32, 64))
        printf("Successfully intialized matrix\n\n");
    else{
        printf("Failed to initialize matrix\n");
        return -1;
    }
    /*
    clock_t begin = clock();
    int updates = 10000;
    for(int i = 0; i < updates; i++){

        matrix.driveMatrix();
    }
    clock_t end = clock();
    double time = (double)(end - begin);
    printf("Performed %d updates in %f microseconds -> %f fps\n", updates, time, (updates/time * 1000000));*/
    
    Drawer d(matrix.getBuffer(), 32, 64);

    d.zero();
    matrix.driveMatrix();

    /*
    d.fastHLine(0xFF000000, 5, 10, 22);
    d.pixel(0xFF000000, 8, 16);
    d.fastHLine(0xFF000000, 5, 63-22, 63-10);
    d.pixel(0xFF000000, 9, 63-16);

    d.fastHLine(0xFFFF0000, 31-8, 8, 63-8);
    d.fastHLine(0xFFFF0000, 31-6, 8, 63-8);
    */
    //d.rainbowFill();
    d.rainbowTriangle();

    while (true)
    {
        matrix.driveMatrix();
    }
    
}