#include "Synthesizer.h"

Synthesizer::Synthesizer(): 
   sampleRate(0), 
   outputBuffer(NULL), 
   out(0.0), 
   noise(new NoiseWithLevel()),
   oscillator1(new Oscillator()),
   envelope(new ADSR()),
   lopass(new LoPass())
{ 
      eventsBuffer = jack_ringbuffer_create(128 * sizeof(Event));
      jack_ringbuffer_mlock(eventsBuffer);
}

Synthesizer::~Synthesizer(){
   jack_ringbuffer_free(eventsBuffer);
   delete noise;
   delete oscillator1;
   delete envelope;
   delete lopass;
}

void Synthesizer::setSampleRate(unsigned int sampleRate){
   this->sampleRate = sampleRate;
   Stk::setSampleRate( (StkFloat)sampleRate );
   lopass->setSampleRate( sampleRate );
}

void Synthesizer::processEvent(Event* event){

   if(!strcmp("NOTE ON", event->parameter)){
      if(event->value == 1){
         oscillator1->reset();
         envelope->keyOn();
      }
      else{
         envelope->keyOff();
      }
   }   

   // Noise 1 params
   else if(!strcmp("NOISE_LEVEL", event->parameter)){
      noise->setLevel(event->value);
   }

   // Oscillator 1 params
   else if(!strcmp("OSCILLATOR_1_FREQUENCY", event->parameter)){
      oscillator1->setFrequency(event->value);
   }
   else if(!strcmp("OSCILLATOR_1_AMPLITUDE", event->parameter)){
      oscillator1->setAmplitude(event->value);
   }
   else if(!strcmp("OSCILLATOR_1_WAVEFORM", event->parameter)){
      oscillator1->setWaveform(event->value);
   }

   // Low pass filter parameters
   else if(!strcmp("LOPASS_FREQUENCY", event->parameter)){
      lopass->setFrequency(event->value);
   }
   else if(!strcmp("LOPASS_RESONANCE", event->parameter)){
      lopass->setResonance(event->value);
   }
   else if(!strcmp("LOPASS_GAIN", event->parameter)){
      lopass->setGain(event->value);
   }
   
   // Envelope params
   else if(!strcmp("ENVELOPE_ATTACK TIME", event->parameter)){
      envelope->setAttackTime(event->value);
   }
   else if(!strcmp("ENVELOPE_DECAY TIME", event->parameter)){
      envelope->setDecayTime(event->value);
   }
   else if(!strcmp("ENVELOPE_SUSTAIN LEVEL", event->parameter)){
      envelope->setSustainLevel(event->value);
   }
   else if(!strcmp("ENVELOPE_RELEASE TIME", event->parameter)){
      envelope->setReleaseTime(event->value);
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
      out = lopass->tick( noise->tick() + oscillator1->tick() ) * envelope->tick();
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
