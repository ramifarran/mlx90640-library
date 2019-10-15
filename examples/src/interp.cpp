#include <stdint.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <chrono>
#include <thread>
#include "headers/MLX90640_API.h"

#define MLX_I2C_ADDR 0x33
#define IMAGE_SIZE 768
#define FPS 4
#define FRAME_TIME_MICROS (1000000/FPS)
#define OFFSET_MICROS 850

int main(){
    int state = 0;
    // printf("Starting...\n");
    static uint16_t eeMLX90640[832];
    float emissivity = 1;
    uint16_t frame[834];
    static float image[768];
    float eTa;
    static uint16_t data[768*sizeof(float)];
    // static long frame_time_micros = FRAME_TIME_MICROS;
    // auto frame_time = std::chrono::microseconds(frame_time_micros + OFFSET_MICROS);

    std::fstream fs;

    MLX90640_SetDeviceMode(MLX_I2C_ADDR, 0);
    MLX90640_SetSubPageRepeat(MLX_I2C_ADDR, 0);
    MLX90640_SetRefreshRate(MLX_I2C_ADDR, 0b010);
    MLX90640_SetChessMode(MLX_I2C_ADDR);
    // MLX90640_SetSubPage(MLX_I2C_ADDR, 0);
    // printf("Configured...\n");

    paramsMLX90640 mlx90640;
    MLX90640_DumpEE(MLX_I2C_ADDR, eeMLX90640);
    MLX90640_ExtractParameters(eeMLX90640, &mlx90640);

    int refresh = MLX90640_GetRefreshRate(MLX_I2C_ADDR);
    // printf("EE Dumped...\n");

    int frames = 30;
    int subpage;
    static float mlx90640To[768];
    while (1){
        // auto start = std::chrono::system_clock::now();
        state = !state;
        //printf("State: %d \n", state);
        MLX90640_GetFrameData(MLX_I2C_ADDR, frame);
        // MLX90640_InterpolateOutliers(frame, eeMLX90640);
        eTa = MLX90640_GetTa(frame, &mlx90640);
        subpage = MLX90640_GetSubPageNumber(frame);
        MLX90640_CalculateTo(frame, &mlx90640, emissivity, eTa, mlx90640To);

        MLX90640_BadPixelsCorrection((&mlx90640)->brokenPixels, mlx90640To, 1, &mlx90640);
        MLX90640_BadPixelsCorrection((&mlx90640)->outlierPixels, mlx90640To, 1, &mlx90640);

        for(int x = 0; x < 32; x++) {
            for(int y = 0; y < 24; y++){
                printf("%f", mlx90640To[32 * (23-y) + x]);
            }
            std::cout << std::endl;
        }
        //wite temperature array to stdout
        // fwrite(&mlx90640To, 4, IMAGE_SIZE, stdout);

        // auto end = std::chrono::system_clock::now();
        // auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        // std::this_thread::sleep_for(std::chrono::microseconds(frame_time - elapsed));
        // printf("\x1b[33A");
    }
    return 0;
}
