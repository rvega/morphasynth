#include <QResource>
#include <iostream>

#include "z_libpd.h"
#include "Audio.hpp"
#include "MainWindow.hpp"


/**********************************************************************************************************
 * Jack callbacks are here (C style functions and variables)
 **********************************************************************************************************/

jack_port_t *outputPort;
jack_client_t *jackClient;

/**
 * This is the audio callback
 */
int process(jack_nframes_t nframes, void *) {
	jack_default_audio_sample_t *out = (jack_default_audio_sample_t *) jack_port_get_buffer(outputPort, nframes);

   // Libpd processes 64 samples each tick
   unsigned int numTicks = nframes/64; 
   // for(unsigned int i=0; i<numBlocks; i++){
   libpd_process_float(numTicks, 0, out);
   // }
	return 0;      
}

/**
 * Called when jack server changes the sample rate
 */
int sampleRateChanged (jack_nframes_t nframes, void *) {
   std::cout << "Sample rate is now: " << nframes << "\n";
	return 0;
}

/**
 * Server wants to shut us down
 */
void jackShutdown(void *) {
	// exit (1);
}


/**
 * Server is saying there's a problem
 */
void jackError(const char *desc) {
   std::cerr << "Jack Error" << "\n" << desc << "\n";
}

/**********************************************************************************************************
 * Audio C++ class
 **********************************************************************************************************/
/**
 * Constructor
 */
Audio::Audio(const MainWindow& _window):
   window(_window)
{
   initJack();
   initPd();
}
   
Audio::~Audio(){
   stop();
}

void Audio::start(){
   if(jack_activate(jackClient)) {
      //TODO: throw 
   }
}

void Audio::stop(){
   jack_client_close(jackClient);
}

void Audio::initJack(){
   // TODO: define app name in a header file
   if ((jackClient = jack_client_open("Morphasynth", JackNullOption, NULL)) == 0) {
      // TODO: throw error, catch from main
      // fprintf (stderr, "jack server not running?\n");
      // return 1;
   }
   jack_set_error_function(jackError);
   jack_set_process_callback(jackClient, process, this);
   jack_set_sample_rate_callback(jackClient, sampleRateChanged, this);
   jack_on_shutdown(jackClient, jackShutdown, this);
   outputPort = jack_port_register(jackClient, "output", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput|JackPortIsTerminal, 0);

   sampleRate = jack_get_sample_rate(jackClient);
   std::cout << "initial sample rate: " << sampleRate << "\n";
}

void Audio::initPd(){
   // libpd_printhook = (t_libpd_printhook) pdprint;
   // libpd_noteonhook = (t_libpd_noteonhook) pdnoteon;
   libpd_init();
   libpd_init_audio(0, 1, sampleRate); // 0 inputs, 1 output
   libpd_openfile("main_patch.pd", "../res/pd");

   // send "dsp 1" message 
   libpd_start_message(1);
   libpd_add_float(1.0f);
   libpd_finish_message("pd", "dsp");
}
