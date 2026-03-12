#define MINIAUDIO_IMPLEMENTATION
#include "../deps/miniaudio/miniaudio.h"
#include "audio.h"
#include "fft.h"
#include "spectrogram.h"
#include <stdio.h>
#include <stdlib.h>

static void data_callback(
    ma_device* device,
    void* output,
    const void* input,
    ma_uint32 frameCount)
{
    AppState* app = (AppState*)device->pUserData;

    float* out = (float*)output;

    ma_decoder_read_pcm_frames(
        &app->decoder,
        out,
        frameCount,
        NULL
    );

    for (ma_uint32 i = 0; i < frameCount; i++)
    {
        float sample = out[i];

        app->sampleBuffer[app->sampleIndex] = sample;

        app->sampleIndex++;
        if(app->sampleIndex >= AppState::FFT_SIZE)
            app->sampleIndex = 0;

        app->samplesSinceFFT++;

        if(app->samplesSinceFFT >= AppState::STEP_SIZE)
        {
            // FFT buffer feltöltése időrendben
            for(int j = 0; j < AppState::FFT_SIZE; j++)
            {
                int idx = (app->sampleIndex + j) % AppState::FFT_SIZE;

                app->fftReal[j] = app->sampleBuffer[idx];
                app->fftImag[j] = 0.0f;
            }

            fft_compute(app);
            spectrogram_push(app);

            app->samplesSinceFFT = 0;
        }
    }

    app->framesPlayed += frameCount;

    (void)input;
}

void audio_init(AppState* app, const char* file)
{
    ma_decoder_config decoderConfig =
        ma_decoder_config_init(ma_format_f32, 1, 44100);

    if (ma_decoder_init_file(file, &decoderConfig, &app->decoder) != MA_SUCCESS) {
        printf("Failed to load file\n");
        exit(1);
    }

    ma_data_source_get_length_in_pcm_frames(
        &app->decoder,
        &app->totalFrames
    );

    ma_device_config deviceConfig =
        ma_device_config_init(ma_device_type_playback);

    deviceConfig.playback.format   = app->decoder.outputFormat;
    deviceConfig.playback.channels = app->decoder.outputChannels;
    deviceConfig.sampleRate        = app->decoder.outputSampleRate;

    deviceConfig.dataCallback   = data_callback;
    deviceConfig.pUserData      = app;

    if (ma_device_init(NULL, &deviceConfig, &app->device) != MA_SUCCESS)
    {
        printf("Failed to open audio device\n");
        exit(1);
    }

    ma_device_start(&app->device);

    app->sampleIndex = 0;
    app->samplesSinceFFT = 0;
}
