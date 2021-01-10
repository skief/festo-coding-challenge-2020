#pragma once

#include <chrono>
#include <cuda_runtime.h>

typedef std::chrono::high_resolution_clock timer_clock;
typedef std::chrono::high_resolution_clock::time_point timer_tp;

timer_tp timer_now()
{
    return timer_clock::now();
}

float timer_elapsed(const timer_tp& start, const timer_tp& end)
{
    return std::chrono::duration<double, std::milli>(end - start).count();
}

void cuda_timer_now(cudaEvent_t& evt) {
    cudaEventCreate(&evt);
    cudaEventRecord(evt, 0);
}

float cuda_elapsed_time(cudaEvent_t& start, cudaEvent_t& stop) {
    cudaEventSynchronize(stop);
    float elapsedTime_ms;

    cudaEventElapsedTime(&elapsedTime_ms, start, stop);
    return elapsedTime_ms;
}