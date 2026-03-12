#pragma once

#include <atomic>
#include <stdint.h>
#include "../deps/miniaudio/miniaudio.h"

struct AppState
{
    // ===== AUDIO =====
    ma_device   device;
    ma_decoder  decoder;

    std::atomic<ma_uint64> framesPlayed;
    bool paused;

    ma_uint64 totalFrames;

    // ===== FFT =====
    static constexpr int FFT_SIZE   = 1024;
    static constexpr int STEP_SIZE  = 256;

    float   sampleBuffer[FFT_SIZE];
    int     sampleIndex;
    int     samplesSinceFFT;    

    float   fftReal[FFT_SIZE];
    float   fftImag[FFT_SIZE];
    int     fftWriteIndex;

    // ===== SPECTROGRAM =====
    int freqBins;
    int timeSteps;
    int currentColumn;

    float* spectrogram;

    // ===== TERMINAL =====
    int termWidth;
    int termHeight;

    bool isFinished;
};

inline float& SPEC(AppState* app, int f, int t)
{
    return app->spectrogram[t * app->freqBins + f];
}
