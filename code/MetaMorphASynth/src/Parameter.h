#pragma once


// TODO: The assignment of numeric values for these is not trivial and needs more discussion

enum Parameter {
   NOTE_ON = 0 ,  // Not really a parameter but we're using these for midi events as well
   NOTE_OFF, 
   
   NOISE_LEVEL,

	OSCILLATOR_FREQUENCY,
   OSCILLATOR_AMPLITUDE,
   OSCILLATOR_WAVEFORM,
   OSCILLATOR_PHASE,

	LOW_PASS_FREQUENCY,
   LOW_PASS_RESONANCE,
   LOW_PASS_GAIN,
   LOW_PASS_KEYFOLLOW,

	HI_PASS_FREQUENCY,
   HI_PASS_RESONANCE,
   HI_PASS_GAIN,
   HI_PASS_KEYFOLLOW,

	ENVELOPE_ATTACK,
   ENVELOPE_DECAY,
   ENVELOPE_SUSTAIN,
   ENVELOPE_RELEASE
};


// The following allows to do stuff like: 
//     std::cout << ENVELOPE_RELEASE << "\n" 
// which will print "ENVELOPE_RELEASE" instead of "18"
// std::ostream& operator<<(std::ostream &o,Parameter n) {
//    switch(n){
//       case NOTE_ON: return o<<"NOTE_ON";
//       case NOTE_OFF: return o<<"NOTE_OFF";
//       case NOISE_LEVEL: return o<<"NOISE_LEVEL";
//       case OSCILLATOR_FREQUENCY: return o<<"OSCILLATOR_FREQUENCY";
//       case OSCILLATOR_AMPLITUDE: return o<<"OSCILLATOR_AMPLITUDE";
//       case OSCILLATOR_WAVEFORM: return o<<"OSCILLATOR_WAVEFORM";
//       case OSCILLATOR_PHASE: return o<<"OSCILLATOR_PHASE";
//       case LOW_PASS_FREQUENCY: return o<<"LOW_PASS_FREQUENCY";
//       case LOW_PASS_RESONANCE: return o<<"LOW_PASS_RESONANCE";
//       case LOW_PASS_GAIN: return o<<"LOW_PASS_GAIN";
//       case LOW_PASS_KEYFOLLOW: return o<<"LOW_PASS_KEYFOLLOW";
//       case HI_PASS_FREQUENCY: return o<<"HI_PASS_FREQUENCY";
//       case HI_PASS_RESONANCE: return o<<"HI_PASS_RESONANCE";
//       case HI_PASS_GAIN: return o<<"HI_PASS_GAIN";
//       case HI_PASS_KEYFOLLOW: return o<<"HI_PASS_KEYFOLLOW";
//       case ENVELOPE_ATTACK: return o<<"ENVELOPE_ATTACK";
//       case ENVELOPE_DECAY: return o<<"ENVELOPE_DECAY";
//       case ENVELOPE_SUSTAIN: return o<<"ENVELOPE_SUSTAIN";
//       case ENVELOPE_RELEASE: return o<<"ENVELOPE_RELEASE";
//       default: return o<<("invalid value"); 
//    }
// }
