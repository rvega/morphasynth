#include "DSPSynthesizer.h"

DSPSynthesizer::DSPSynthesizer(): 
   sampleRate(0), 
   outputBuffer(NULL), 
   noise(new DSPNoiseWithLevel()),
   oscillator1(new DSPOscillator()),
   envelope(new ADSR()),
   lopass(new DSPLoPass()),
   hipass(new DSPHiPass()),
   out(0.0), 
   s1(0.0)
{ 
      guiEventsBuffer = jack_ringbuffer_create(128 * sizeof(Event));
      jack_ringbuffer_mlock(guiEventsBuffer);
}

DSPSynthesizer::~DSPSynthesizer(){
   jack_ringbuffer_free(guiEventsBuffer);
   delete noise;
   delete oscillator1;
   delete envelope;
   delete lopass;
   delete hipass;
}

void DSPSynthesizer::setSampleRate(unsigned int sampleRate){
   this->sampleRate = sampleRate;
   Stk::setSampleRate( (StkFloat)sampleRate );
   lopass->setSampleRate( sampleRate );
   hipass->setSampleRate( sampleRate );
}

void DSPSynthesizer::processEvent(Event* event){
   switch(event->parameter){

      
      case(NOTE_ON):
         if(event->value1 == 1){
            oscillator1->reset();
            envelope->keyOn();
         }
         else{
            envelope->keyOff();
         }
         break;
      

      case(NOISE_LEVEL):
         noise->setLevel(event->value1);
         break;
      case(OSCILLATOR_FREQUENCY):
         oscillator1->setFrequency(event->value1);
         break;
      case(OSCILLATOR_AMPLITUDE):
         oscillator1->setAmplitude(event->value1);
         break;
      case(OSCILLATOR_WAVEFORM):
         oscillator1->setWaveform(event->value1);
         break;


      case(LOW_PASS_FREQUENCY):
         lopass->setFrequency(event->value1);
         break;
      case(LOW_PASS_RESONANCE):
         lopass->setResonance(event->value1);
         break;
      case(LOW_PASS_GAIN):
         lopass->setGain(event->value1);
         break;
      case(LOW_PASS_KEYFOLLOW):
         // TODO
         break;



      case(HI_PASS_FREQUENCY):
         hipass->setFrequency(event->value1);
         break;
      case(HI_PASS_RESONANCE):
         hipass->setResonance(event->value1);
         break;
      case(HI_PASS_GAIN):
         hipass->setGain(event->value1);
         break;
      case(HI_PASS_KEYFOLLOW):
         // TODO
         break;



      case(ENVELOPE_ATTACK):
         envelope->setAttackTime(event->value1);
         break;
      case(ENVELOPE_DECAY):
         envelope->setDecayTime(event->value1);
         break;
      case(ENVELOPE_SUSTAIN):
         envelope->setSustainLevel(event->value1);
         break;
      case(ENVELOPE_RELEASE):
         envelope->setReleaseTime(event->value1);
         break;


      default:
         std::cout << "Synth is trying to set unknown parameter from received event " << event->parameter << ": " << event->value1 << "\n";
         break;
   }
}

int DSPSynthesizer::process(void *outBuffer, void *inBuffer, unsigned int bufferSize){
   // Process events from GUI
   while(jack_ringbuffer_read_space(guiEventsBuffer) >= sizeof(Event)){ // If there's at least one event to read
      jack_ringbuffer_read(guiEventsBuffer, (char*)&event, sizeof(Event));
      processEvent(&event);
   }

   // Make some sound
   outputBuffer = (StkFloat *)outBuffer;   
   for(unsigned int i = 0; i < bufferSize; i++) {
      s1 = noise->tick() + oscillator1->tick();
      out = lopass->tick( hipass->tick(s1) ) * envelope->tick();
      *outputBuffer++ = out;
      *outputBuffer++ = out;
   }
   return 0;
}

void DSPSynthesizer::addMidiEvent(Event event){}

void DSPSynthesizer::addGUIEvent(Event event){
   if(jack_ringbuffer_write_space(guiEventsBuffer) < sizeof(Event)){
      std::cout << "Could not add " << event.parameter << ": " << event.value1 << " to synth's event buffer." << "\n";
   }
   else{
      jack_ringbuffer_write(guiEventsBuffer, (const char *)&event, sizeof(Event));
   }
}
