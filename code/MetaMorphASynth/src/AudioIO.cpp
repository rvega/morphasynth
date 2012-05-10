#include "AudioIO.h"

using namespace std;

//============================//
//  Constructor & Destructor  //
//============================//

int audioCallback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *userData ) {
   std::cout << nBufferFrames << "\n";
   return 0;
}

AudioIO::AudioIO(){
   if ( dac.getDeviceCount() == 0 ) return;

   // Connect to JACK's default output.
   unsigned int deviceId = dac.getDefaultOutputDevice(); 
   RtAudio::DeviceInfo device = dac.getDeviceInfo(deviceId);

   sampleRate = device.sampleRates[0]; // JACK will only return one available sample rate.
   unsigned int bufferFrames = 512;

   RtAudio::StreamParameters parameters;
   parameters.deviceId = deviceId;
   parameters.nChannels = 2;

   RtAudio::StreamOptions options;
   options.flags = RTAUDIO_MINIMIZE_LATENCY | RTAUDIO_SCHEDULE_REALTIME;
   options.streamName = "Metamorph-A-Synth";

   try {
      dac.openStream( &parameters, NULL, RTAUDIO_FLOAT32, sampleRate, &bufferFrames, &audioCallback, NULL, &options );
      std::cout << bufferFrames << "\n";
      std::cout << options.flags << "\n";
      dac.startStream();
   }
   catch ( RtError& e ) {
      std::cout << "Eh!";
      std::cout << '\n' << e.getMessage() << '\n' << std::endl;
   }

// 
//    try{
//       // Determine the number of devices available
//       unsigned int devices = audio.getDeviceCount();
// 
//       std::cout << "Sample rate: " << audio.getStreamSampleRate() << "\n";
// 
//       // Scan through devices for various capabilities
//       RtAudio::DeviceInfo info;
//       for(unsigned int i=0; i<devices; i++) {
//          info = audio.getDeviceInfo(i);
// 
//          if ( info.probed == true ) {
//             // Print, for example, the maximum number of output channels for each device
//             std::cout << "device = " << i;
//             std::cout << ": Maximum output channels = " << info.outputChannels << "\n";
//             std::cout << ": Name = " << info.name << "\n";
//          }
//       }
//    }
//    catch(RtError& e){
//       e.printMessage();
//    }

   // if(audio.isStreamOpen()) audio.closeStream();

   // outputDevice = -1;
   // sampleRate = -1;
   // bufferLength = -1;

   // drivers = new vector<const PaHostApiInfo*>;
   // inputDevices = new vector<const PaDeviceInfo*>;
   // outputDevices = new vector<const PaDeviceInfo*>;

   // init();
}

AudioIO::~AudioIO(){

}

//===============//
//  Setup Audio  //
//===============//
// void AudioIO::init(){
   // PaJack_SetClientName("Oscillatron!");

   // PaError err;
   // err = Pa_Initialize();
   // if(err!= paNoError){ cout << "PortAudio error: " << Pa_GetErrorText(err) << "\n"; exit(1);}

   // // Audio device count
   // PaDeviceIndex ndev = Pa_GetDeviceCount();
   // if(ndev<0){ std::cout << "PortAudio error: " << Pa_GetErrorText(ndev) << "\n"; exit(1);} 

   // // Output device list
   // std::cout << "Audio output devices:\n";
   // for(int i=0; i<ndev; i++){
   //    const PaDeviceInfo *info = Pa_GetDeviceInfo((PaDeviceIndex) i);
   //    if(info->maxOutputChannels > 0){
   //       std::cout << i << " " << info->name << " " << info->maxOutputChannels << "  ";
   //       
   //       // Shows if the device is alsa or jack or whatever:
   //       const PaHostApiInfo *apiInfo = Pa_GetHostApiInfo(info->hostApi);
   //       std::cout << apiInfo->name << "\n";
   //    } 
   // }
   // std::cout << "Dafault output device: " << Pa_GetDefaultOutputDevice() << "\n";


   // // Connect to device 
   // PaStreamParameters outParameters;
   // memset(&outParameters, '\0', sizeof(outParameters)); // Set all params to zero 
   // outParameters.channelCount = 1;
   // outParameters.device = Pa_GetDefaultOutputDevice();
   // outParameters.sampleFormat = paFloat32;
   // err = Pa_OpenStream(&outputStream, NULL, &outParameters, sampleRate, bufferLength, paNoFlag, &AudioIO::paCallback, this);
   // if(err!= paNoError){ printf("PortAudio error:%s", Pa_GetErrorText(err)); exit(1); }
   // err = Pa_StartStream( outputStream );
   // if(err!= paNoError){ printf("PortAudio error:%s", Pa_GetErrorText(err)); exit(1); }
// }

// vector<string> AudioIO::getDriverNames(){
// 
//    drivers->clear();
//    vector<string> strings;
//    int c = Pa_GetHostApiCount();
//    for (int i = 0; i < c; i++) {
//       const PaHostApiInfo *info = Pa_GetHostApiInfo(i); 
//       strings.push_back(info->name);
// 
//       drivers->push_back(info);
//    }
// 
//    return strings;
// }
// 
// std::vector<std::string> AudioIO::getOutputDeviceNamesForDriver(std::string driverName){
//    for(int i = 0; i < (int)drivers->size(); i++) {
//       const PaHostApiInfo *driver = drivers->at(i);
//       if(driver->name == driverName){
//          // this is the requested driver, get the available output devices
//          vector<string> outputDeviceNames; 
//          outputDevices->clear(); 
//          
//          for(int j=0; j<driver->deviceCount; j++) {
//             int id = Pa_HostApiTypeIdToHostApiIndex(driver->type);
//             int idx = Pa_HostApiDeviceIndexToDeviceIndex(id, j);
//             const PaDeviceInfo* device = Pa_GetDeviceInfo(idx);
// 
//             outputDevices->push_back(device);
//             if(device->maxOutputChannels > 0){
//                outputDeviceNames.push_back(device->name);
//             }
//          }
// 
//          return outputDeviceNames;
//       }
//    }
// 
//    vector<string> empty;
//    return empty;
// }
// 
// void AudioIO::selectOutputDevice(std::string deviceName){
// 
// }
// 
// void AudioIO::start(){
// 
// }

//==================//
//  Audio Callback  //
//==================//
// int AudioIO::process(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags){
//    float *out = (float*)outputBuffer;
//    for (unsigned long i = 0; i < framesPerBuffer; i++) {
//       out[i] = 0.0;
//    }
// 
//    return 0;
// }
