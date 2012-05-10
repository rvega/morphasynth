#pragma once

// #include <iostream>
// #include <cstdlib>
// #include <vector>
// #include <string>
// 
// #include "portaudio.h"
// #include "pa_jack.h"
#include "RtAudio.h"

class AudioIO{
   protected:
      // PaStream *outputStream;
      // int outputDevice;
      // int sampleRate;
      // int bufferLength;

      // void init();

      // std::vector<const PaHostApiInfo*> *drivers;
      // std::vector<const PaDeviceInfo*> *inputDevices;
      // std::vector<const PaDeviceInfo*> *outputDevices;

      // int process(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags);
      // static int paCallback(const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData ) {
      //    return ((AudioIO*)userData)->process(input, output, frameCount, timeInfo, statusFlags); // See http://www.assembla.com/spaces/portaudio/wiki/Tips_CPlusPlus
      // }
      RtAudio dac;
      unsigned int sampleRate;

   public:
      AudioIO();
      ~AudioIO();

      // std::vector<std::string> getDriverNames();
      // std::vector<std::string> getOutputDeviceNamesForDriver(std::string driverName);
      // void selectOutputDevice(std::string deviceName);
      // void start();
};
