CXX = g++
CXXFLAGS = -std=c++17 -O2 -I./deps/miniaudio

SRC = \
    src/main.cpp \
    src/audio.cpp \
    src/fft.cpp \
    src/spectrogram.cpp \
    src/renderer.cpp

TARGET = player

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) -lpthread -lm

clean:
	rm -f $(TARGET)
