#include "Synthesizer.h"

Synthesizer::Synthesizer(): 
   sampleRate(0), 
   outputBuffer(NULL), 
   out(0.0), 
   oscillator1(new Oscillator()),
   envelope1(new ADSR())
{ 
      eventsBuffer = jack_ringbuffer_create(128 * sizeof(Event));
      jack_ringbuffer_mlock(eventsBuffer);
}

Synthesizer::~Synthesizer(){
   jack_ringbuffer_free(eventsBuffer);
   delete oscillator1;
   delete envelope1;
}

void Synthesizer::setSampleRate(unsigned int sampleRate){
   this->sampleRate = sampleRate;
   Stk::setSampleRate( (StkFloat)sampleRate );
}

void Synthesizer::processEvent(Event* event){
   if(!strcmp("NOTE ON", event->parameter)){
      if(event->value == 1){
         oscillator1->reset();
         envelope1->keyOn();
      }
      else{
         envelope1->keyOff();
      }
   }   

   // Oscillator 1 params
   else if(!strcmp("FREQUENCY", event->parameter)){
      oscillator1->setFrequency(event->value);
   }
   else if(!strcmp("AMPLITUDE", event->parameter)){
      oscillator1->setAmplitude(event->value);
   }
   else if(!strcmp("WAVEFORM", event->parameter)){
      oscillator1->setWaveform(event->value);
   }

   // Envelope 1 params
   else if(!strcmp("ATTACK TIME", event->parameter)){
      envelope1->setAttackTime(event->value);
   }
   else if(!strcmp("DECAY TIME", event->parameter)){
      envelope1->setDecayTime(event->value);
   }
   else if(!strcmp("SUSTAIN LEVEL", event->parameter)){
      envelope1->setSustainLevel(event->value);
   }
   else if(!strcmp("RELEASE TIME", event->parameter)){
      envelope1->setReleaseTime(event->value);
   }

   // Invalid parameter
   else{
      std::cout << "Trying to set unknown parameter " << event->parameter << ": " << event->value << "\n";
   }
}

int Synthesizer::process(void *outBuffer, void *inBuffer, unsigned int bufferSize){
   // Process events from GUI
   while(jack_ringbuffer_read_space(eventsBuffer) >= sizeof(Event)){ // If there's at least one event to read
      jack_ringbuffer_read(eventsBuffer, (char*)&event, sizeof(Event));
      processEvent(&event);
   }

   // Make some sound
   outputBuffer = (StkFloat *)outBuffer;   
   for(unsigned int i = 0; i < bufferSize; i++) {
      out = oscillator1->tick() * envelope1->tick();
      *outputBuffer++ = out;
      *outputBuffer++ = out;
   }
   return 0;
}

void Synthesizer::pushEvent(Event* event){
   if(jack_ringbuffer_write_space(eventsBuffer) < sizeof(Event)){
      std::cout << "Could not add " << event->parameter << ": " << event->value << " to synth's event buffer." << "\n";
   }
   else{
      jack_ringbuffer_write(eventsBuffer, (const char *)event, sizeof(Event));
   }
}
