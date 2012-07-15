/**
 * Morphasynth
 * 
 * Copyright (C) 2012 Rafael Vega González
 * Copyright (C) 2012 Daniel Gómez Marín
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "AudioIO.h"

AudioIO::AudioIO(DSPSynthesizer* synthesizer){
   dac = new RtAudio();
   synth = synthesizer;
   sampleRate = 0;
}

AudioIO::~AudioIO(){
   delete dac;
}

void AudioIO::start(){
   if ( dac->getDeviceCount() == 0 ){
      std::cout << "Could not connect to audio device. Is Jack running?" << "\n";
      return;
   } 

   // Connect to JACK's default output.
   unsigned int deviceId = dac->getDefaultOutputDevice(); 
   RtAudio::DeviceInfo device = dac->getDeviceInfo(deviceId);

   sampleRate = device.sampleRates[0]; // JACK will only return one available sample rate.
   unsigned int bufferFrames = 512;

   synth->setSampleRate(sampleRate);

   RtAudio::StreamParameters parameters;
   parameters.deviceId = deviceId;
   parameters.nChannels = 2;

   RtAudio::StreamOptions options;
   options.flags = RTAUDIO_MINIMIZE_LATENCY | RTAUDIO_SCHEDULE_REALTIME;
   options.streamName = APPLICATION_NAME;

   try {
      dac->openStream( &parameters, NULL, RTAUDIO_FLOAT64, sampleRate, &bufferFrames, &AudioIO::audioIOCallback, this, &options );
      dac->startStream();
      std::cout << "Started audio stream. Sample rate is " << sampleRate << ". Buffer size is " << bufferFrames << ".\n";
   }
   catch ( RtError& e ) {
      std::cout << "Could not start audio connection. Is Jack Running?" << "\n";
      std::cout << e.getMessage() << "\n";
   }
}

int AudioIO::process( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *userData ) {
   return synth->process(outputBuffer, inputBuffer, nBufferFrames);
}

void AudioIO::stop(){
   try {
      dac->stopStream();
   }
   catch (RtError& e) {
      std::cout << "Could not close audio connection" << "\n";
      std::cout << e.getMessage() << "\n";
   }

   if ( dac->isStreamOpen() ) dac->closeStream();
}
