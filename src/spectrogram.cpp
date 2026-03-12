#include "spectrogram.h"
#include <math.h>

void spectrogram_push(AppState* app)
{
    int N = AppState::FFT_SIZE;
    int bins = app->freqBins;

    float sampleRate = (float)app->decoder.outputSampleRate;

    for(int f = 0; f < bins; f++)
    {
        float pos = (float)f / (bins - 1);

        float minFreq = 20.0f;
        float maxFreq = 8000.0f;

        float logMin = log10f(minFreq);
        float logMax = log10f(maxFreq);

        float freq = powf(10.0f, logMin + pos * (logMax - logMin));

        int idx = (int)(freq * N / sampleRate);

        if(idx >= N/2)
            idx = N/2 - 1;

        // bin averaging
        float mag = 0.0f;
        int count = 0;

        for(int k = -2; k <= 2; k++)
        {
            int i = idx + k;

            if(i >= 0 && i < N/2)
            {
                mag += app->fftReal[i];
                count++;
            }
        }

        mag /= (float)count;

        // dB scale
        float db = 20.0f * log10f(mag + 1e-6f);

        float norm = (db + 80.0f) / 80.0f;

        if(norm < 0) norm = 0;
        if(norm > 1) norm = 1;

        norm = sqrtf(norm);

        SPEC(app, f, app->currentColumn) = norm;
    }

    app->currentColumn++;

    if(app->currentColumn >= app->timeSteps)
        app->currentColumn = 0;
}
