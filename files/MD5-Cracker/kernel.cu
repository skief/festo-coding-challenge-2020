
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "md5.h"
#include "timer.h"
#include <stdio.h>
#include <iostream>
#include <iomanip>

#define CUDA_CHECK_ERROR                                                       \
    do {                                                                       \
        const cudaError_t err = cudaGetLastError();                            \
        if (err != cudaSuccess) {                                              \
            const char *const err_str = cudaGetErrorString(err);               \
            std::cerr << "Cuda error in " << __FILE__ << ":" << __LINE__ - 1   \
                      << ": " << err_str << " (" << err << ")" << std::endl;   \
            exit(EXIT_FAILURE);                                                \
        }                                                                      \
    } while(0)

#define N_THREADS 32 * 32
#define N_GRID 1000

int pos(char s) {
    for (int i = 0; i < posLen; i++) {
        if (possible[i] == s) {
            return i;
        }
    }
    return 0;
}


int main() {
    dim3 threads(N_THREADS); // (32 * 32);
    dim3 grid(N_GRID); //(1000);

    unsigned char* h_out = (unsigned char*)malloc(sizeof(unsigned char) * 8);
    unsigned char* h_input = (unsigned char*)malloc(sizeof(unsigned char) * 8 * N_THREADS * N_GRID); //[] = "sQyWhn3wsQyWhn4x";
    unsigned char* h_hit = (unsigned char*)malloc(sizeof(unsigned char) * N_THREADS * N_GRID);
    //unsigned char h_pattern[8] = { 0x00, 0x2a, 0x8a, 0x8b, 0x23, 0xd0, 0x3e, 0x70 };
    //unsigned char h_pattern[8] = { 0x00, 0x2a, 0x8a, 0x8b, 0x23, 0xd0, 0x3e, 0x70 };
    unsigned char h_pattern[8] = { 0xa8, 0x4b, 0xa6, 0x51, 0xfd, 0x12, 0x2e, 0xf5 };
    unsigned char* d_out;
    unsigned char* d_input;
    unsigned char* d_hit;
    unsigned char* d_pattern;
    cudaMalloc(&d_out, sizeof(unsigned char) * 8);
    cudaMalloc(&d_input, sizeof(unsigned char) * N_GRID * N_THREADS * 8);
    cudaMalloc(&d_hit, sizeof(unsigned char) * N_GRID * N_THREADS);
    cudaMalloc(&d_pattern, sizeof(unsigned char) * 16);

    cudaMemcpy(d_pattern, h_pattern, sizeof(unsigned char) * 8, cudaMemcpyHostToDevice);

    md5_init();

    int count = 0;
    int first = 0;

    timer_tp start = timer_now();
    timer_tp stop;
    timer_tp progstart = timer_now();

    float allpos = posLen * posLen;
    int posTested = 0;
    unsigned int hashesDone = 0;
	
    for (int a = 0; a < posLen; a++) {
        for (int b = 0; b < posLen; b++) {
            stop = timer_now();
            float timePerPos = timer_elapsed(start, stop);
            posTested += 1;
            float possibilitiesLeft = allpos - posTested;
            start = timer_now();
            
            hashesDone = 0;

            for (int c = 0; c < posLen; c++) {
                for (int d = 0; d < posLen; d++) {
                    for (int e = 0; e < posLen; e++) {
                        for (int f = 0; f < posLen; f++) {
                            for (int g = 0; g < posLen; g++) {
                                h_input[8 * count + 0] = possible[a];
                                h_input[8 * count + 1] = possible[b];
                                h_input[8 * count + 2] = possible[c];
                                h_input[8 * count + 3] = possible[d];
                                h_input[8 * count + 4] = possible[e];
                                h_input[8 * count + 5] = possible[f];
                                h_input[8 * count + 6] = possible[g];

                                count += 1;
                                if (count == N_GRID * N_THREADS) {
                                    cudaMemcpy(d_input, h_input, sizeof(unsigned char) * N_GRID * N_THREADS * 8, cudaMemcpyHostToDevice);

                                    if (first != 0) {
                                        cudaMemcpy(h_hit, d_hit, sizeof(unsigned char) * N_THREADS * N_GRID, cudaMemcpyDeviceToHost);

                                        for (int i = 0; i < N_THREADS * N_GRID; i++) {
                                           // std::cout << (int)h_hit[i] << " ";
                                            if (h_hit[i] == 1) {
                                                cudaMemcpy(h_out, d_out, sizeof(unsigned char) * 8, cudaMemcpyDeviceToHost);
                                                for (int j = 0; j < 8; j++) {
                                                    printf("%c", h_out[j]);
                                                }
                                                std::cout << std::endl;
                                                return;
                                            }
                                        }
                                    }

                                    Parallel_Hash << <grid, threads >> > (d_input, d_out, d_hit, d_pattern, 8);
                                    first = 1;
                                    hashesDone += 1;
                                    count = 0;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    CUDA_CHECK_ERROR;
	return 0;
}