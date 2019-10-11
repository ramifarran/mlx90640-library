#include <stdint.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <chrono>
#include <thread>
#include "headers/MLX90640_API.h"

#define MLX_I2C_ADDR 0x33

int main(){
    int state = 0;
    printf("Starting...\n");
    static uint16_t eeMLX90640[832];
    float emissivity = 1;
    uint16_t frame[834];
    static float image[768];
    float eTa;
    static uint16_t data[768*sizeof(float)];

    std::fstream fs;

    MLX90640_SetDeviceMode(MLX_I2C_ADDR, 0);
    MLX90640_SetSubPageRepeat(MLX_I2C_ADDR, 0);
    MLX90640_SetRefreshRate(MLX_I2C_ADDR, 0b010);
    MLX90640_SetChessMode(MLX_I2C_ADDR);
    //MLX90640_SetSubPage(MLX_I2C_ADDR, 0);
    printf("Configured...\n");

    paramsMLX90640 mlx90640;
    MLX90640_DumpEE(MLX_I2C_ADDR, eeMLX90640);
    MLX90640_ExtractParameters(eeMLX90640, &mlx90640);

    int refresh = MLX90640_GetRefreshRate(MLX_I2C_ADDR);
    printf("EE Dumped...\n");

    int frames = 30;
    int subpage;
    static float mlx90640To[768];
    while (1){
        state = !state;
        //printf("State: %d \n", state);
        MLX90640_GetFrameData(MLX_I2C_ADDR, frame);
        // MLX90640_InterpolateOutliers(frame, eeMLX90640);
        eTa = MLX90640_GetTa(frame, &mlx90640);
        subpage = MLX90640_GetSubPageNumber(frame);
        MLX90640_CalculateTo(frame, &mlx90640, emissivity, eTa, mlx90640To);

        MLX90640_BadPixelsCorrection((&mlx90640)->brokenPixels, mlx90640To, 1, &mlx90640);
        MLX90640_BadPixelsCorrection((&mlx90640)->outlierPixels, mlx90640To, 1, &mlx90640);

        //wite temperature array to stdout
		fwrite(&mlx90640To, 1, sizeof(mlx90640To), stdout);
    }
    return 0;
}
