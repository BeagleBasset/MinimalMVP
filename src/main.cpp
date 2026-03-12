#include "app_state.h"
#include "audio.h"
#include "renderer.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

// Terminal input handling (space: pause/play, q: exit)
void handleInputs(AppState* app)
{
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    char c;
    if (read(STDIN_FILENO, &c, 1) > 0)
    {
        switch(c)
        {
            case ' ':
                app->paused = !app->paused;
                if (app->paused) ma_device_stop(&app->device);
                else ma_device_start(&app->device);
                break;
            case 'q':
                tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
                exit(0);
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, flags);
}

int main(int argc, char** argv)
{
    if (argc < 2) { 
        printf("Usage: %s <file.wav>\n", argv[0]); 
        return 1; 
    }

    AppState app{};
    app.fftWriteIndex   = 0;
    app.framesPlayed    = 0;
    app.paused          = false;
    app.freqBins        = 64;     
    app.timeSteps       = 128;
    app.currentColumn   = 0;
    app.isFinished      = false;

    // Spectrogram buffer
    app.spectrogram = (float*)calloc(app.freqBins * app.timeSteps, sizeof(float));
    if (!app.spectrogram) { 
        printf("Failed to allocate spectrogram buffer\n"); 
        return 1; 
    }

    audio_init(&app, argv[1]);   
    printf("\x1b[2J");            

    /////////////////////////
    // Main loop
    /////////////////////////
    while (!app.isFinished)
    {
        handleInputs(&app);    
        render(&app);          
        usleep(16000);         // ~60 FPS
    }

    // Cleanup
    free(app.spectrogram);
    ma_device_uninit(&app.device);
    ma_decoder_uninit(&app.decoder);
    return 0;
}
