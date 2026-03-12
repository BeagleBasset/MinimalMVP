#include "renderer.h"
#include <stdio.h>
#include <math.h>

static const int spectroColors[] =
{
    16,
    17,18,19,20,21,
    27,33,39,45,51,
    50,49,48,47,46,
    82,118,154,190,226,
    220,214,208,202,
    196,197,198,199,
    15
};

static const int COLOR_COUNT =
    sizeof(spectroColors) / sizeof(int);

inline int mag_to_color(float mag)
{
    mag = powf(mag, 0.6f);

    if (mag < 0) mag = 0;
    if (mag > 1) mag = 1;

    int idx = (int)(mag * (COLOR_COUNT - 1));
    return spectroColors[idx];
}

void format_time(float sec, int& m, int& s)
{
    m = (int)(sec / 60);
    s = (int)sec % 60;
}

void render(AppState* app)
{
    printf("\x1b[H");   // cursor home

    int width = app->timeSteps;
    int height = app->freqBins;

    // ===== spectrogram =====

    for (int f = height - 1; f >= 1; f -= 2)
    {
        for (int t = 0; t < width; t++)
        {
            int col = (app->currentColumn + t) % width;

            float top = SPEC(app, f, col);
            float bottom = SPEC(app, f - 1, col);

            if(top < 0.01f && bottom < 0.01f)
            {
                printf("\x1b[48;5;16m ");
                continue;
            }

            int topColor = mag_to_color(top);
            int bottomColor = mag_to_color(bottom);

            printf("\x1b[38;5;%dm\x1b[48;5;%dm▄",
                   bottomColor,
                   topColor);
        }

        printf("\x1b[0m\n");
    }

    // ===== progress =====

    float progress = 0.0f;

    if (app->totalFrames > 0)
        progress =
            (float)app->framesPlayed.load() /
            (float)app->totalFrames;

    // offset to set align the progress bar with the spectrogram
    const int OFFSET = 17;
    int barWidth = width - OFFSET;
    int filled = (int)(barWidth * progress);

    float sampleRate =
        (float)app->decoder.outputSampleRate;

    float elapsedSec =
        (float)app->framesPlayed.load() / sampleRate;

    float totalSec =
        (float)app->totalFrames / sampleRate;

    int em, es, tm, ts;

    format_time(elapsedSec, em, es);
    format_time(totalSec, tm, ts);

    const char* icon =
        app->paused ? "⏸" : "▶";

    printf("\x1b[0m%s %02d:%02d / %02d:%02d [",
           icon, em, es, tm, ts);

    for (int i = 0; i < barWidth; i++)
    {
        if (i < filled) printf("█");
        else           printf("░");
    }

    app->isFinished = elapsedSec >=  totalSec;

    printf("]\n");

    fflush(stdout);
}
