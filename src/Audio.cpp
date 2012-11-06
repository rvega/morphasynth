#include <jack/midiport.h>
#include <QResource>
#include <iostream>
#include "z_libpd.h"
#include "Audio.hpp"
#include "m_pd.h"

// These guys are defined in c files in the src/externals directory
extern "C" void moog_lopass_tilde_setup();
extern "C" void resonant_hipass_tilde_setup();
extern "C" void expr_setup();
extern "C" void limit_setup();
extern "C" void list2symbol_setup();

/**********************************************************************************************************
 * Jack callbacks are here (C style functions and variables)
 **********************************************************************************************************/
jack_port_t *outputPort;
jack_port_t *inputPort;
jack_client_t *jackClient;

/**
 * This is the audio callback
 */
int process(jack_nframes_t nframes, void *) {
   // Catch incoming midi events
   unsigned char byte1, byte2, byte3;
   jack_midi_event_t midiEvent;
	void* in = jack_port_get_buffer(inputPort, nframes);
   jack_nframes_t eventCount = jack_midi_get_event_count(in);
   for (unsigned int i=0; i<eventCount; i++) {
      jack_midi_event_get(&midiEvent, in, i);
      byte1 = (unsigned char) ( *(midiEvent.buffer) );
      byte2 = (unsigned char) ( *(midiEvent.buffer + sizeof(char)) );
      byte3 = (unsigned char) ( *(midiEvent.buffer + 2*sizeof(char)) );

      // TODO: detect and send all midi event types supported by pd
      // Note off
      byte1 = byte1 & 0xF0;
      if(byte1==0x80){
         libpd_noteon(0, byte2, 0); //Note on with velocity 0
      }
      // Note on
      else if(byte1==0x90){
         libpd_noteon(0, byte2, byte3);
      }
      // Pitch bend
      // else if(byte1==0xE0){
         // Pitchwheel bytess use 2 bytes to express a value between -8192 and 8191
         // Byte 2 is the most significant one
         // event.command = PITCH_BEND;
         // event.value1 = ((bytes->at(2) << 7) | bytes->at(1)) - 8192;
      // }
   }

   // Generate sound 
	jack_default_audio_sample_t *out = (jack_default_audio_sample_t *) jack_port_get_buffer(outputPort, nframes);
   libpd_process_float(nframes/64, 0, out); // Libpd processes 64 samples each tick

	return 0;      
}

/**
 * Called when jack server changes the sample rate
 */
int sampleRateChanged (jack_nframes_t nframes, void * arg) {
   Audio* audio = static_cast<Audio*>(arg);
   audio->setSampleRate(nframes);
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

void pdPrint(const char *s) {
   std::cout << s;
}

/**********************************************************************************************************
 * Audio C++ class
 **********************************************************************************************************/
/**
 * Constructor, destructor & init
 */
Audio::Audio() {
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
   outputPort = jack_port_register(jackClient, "out", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput|JackPortIsTerminal, 0);
   inputPort = jack_port_register(jackClient, "in", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput|JackPortIsTerminal, 0);
}


void Audio::initPd(){
   libpd_printhook = (t_libpd_printhook) pdPrint;
   libpd_init();

   // Manually initialize externals
   moog_lopass_tilde_setup();
   resonant_hipass_tilde_setup();
   expr_setup();
   limit_setup();
   list2symbol_setup();

   libpd_init_audio(0, 1, sampleRate); // 0 inputs, 1 output
   libpd_openfile("main_patch.pd", "../res/pd");

   // send "dsp 1" message 
   libpd_start_message(1);
   libpd_add_float(1.0f);
   libpd_finish_message("pd", "dsp");
   
   // test save a file
   // libpd_float("osc1-amplitude", 0.77);
   // libpd_bang("save");
   // libpd_bang("load");
   
}

unsigned int Audio::getSampleRate(){
   return sampleRate;
}

void Audio::setSampleRate(unsigned int sr){
   sampleRate = sr;
}
