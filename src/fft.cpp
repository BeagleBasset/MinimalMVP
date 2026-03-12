#include <math.h>
#include "fft.h"

static void bit_reverse(AppState* app)
{
    int N = AppState::FFT_SIZE;
    int j = 0;

    for (int i = 1; i < N; i++)
    {
        int bit = N >> 1;

        while (j & bit)
        {
            j ^= bit;
            bit >>= 1;
        }

        j |= bit;

        if (i < j)
        {
            float tr = app->fftReal[i];
            float ti = app->fftImag[i];

            app->fftReal[i] = app->fftReal[j];
            app->fftImag[i] = app->fftImag[j];

            app->fftReal[j] = tr;
            app->fftImag[j] = ti;
        }
    }
}

void fft_compute(AppState* app)
{
    int N = AppState::FFT_SIZE;

    for (int i = 0; i < N; i++) {
        app->fftImag[i] = 0.0f;
    }

    float mean = 0.0f;
    for (int i = 0; i < N; i++)
        mean += app->fftReal[i];

    mean /= N;

    for (int i = 0; i < N; i++)
        app->fftReal[i] -= mean;

    for (int i = 0; i < N; i++)
    {
        float w = 0.5f * (1.0f - cosf(2.0f * M_PI * i / (N - 1)));
        app->fftReal[i] *= w;
    }

    bit_reverse(app);

    for (int len = 2; len <= N; len <<= 1)
    {
        float angle = -2.0f * M_PI / len;

        float wlen_r = cosf(angle);
        float wlen_i = sinf(angle);

        for (int i = 0; i < N; i += len)
        {
            float wr = 1.0f;
            float wi = 0.0f;

            for (int j = 0; j < len / 2; j++)
            {
                int u = i + j;
                int v = i + j + len / 2;

                float vr = app->fftReal[v] * wr - app->fftImag[v] * wi;
                float vi = app->fftReal[v] * wi + app->fftImag[v] * wr;

                float ur = app->fftReal[u];
                float ui = app->fftImag[u];

                app->fftReal[u] = ur + vr;
                app->fftImag[u] = ui + vi;

                app->fftReal[v] = ur - vr;
                app->fftImag[v] = ui - vi;

                float next_wr = wr * wlen_r - wi * wlen_i;
                wi = wr * wlen_i + wi * wlen_r;
                wr = next_wr;
            }
        }
    }

    float normFactor = 2.0f / N; 
    for (int i = 0; i < N / 2; i++)
    {
        float r = app->fftReal[i];
        float im = app->fftImag[i];

        app->fftReal[i] = sqrtf(r*r + im*im) * normFactor;
    }
}
