import("filter.lib");

//----------------------- moog_vcf_2b[n] ---------------------------
// Moog "Voltage Controlled Filter" (VCF) as two biquads
//
// USAGE:
//   moog_vcf_2b(res,fr)
//   moog_vcf_2bn(res,fr)
// where
//   fr = corner-resonance frequency in Hz
//   res  = Normalized amount of corner-resonance between 0 and 1
//        (0 is min resonance, 1 is maximum)
//
// DESCRIPTION: Implementation of the ideal Moog VCF transfer
//   function factored into second-order sections.  As a result, it is
//   more accurate than moog_vcf above, but its coefficient formulas are
//   more complex when one or both parameters are varied.  Here, res
//   is the fourth root of that in moog_vcf, so, as the sampling rate
//   approaches infinity, moog_vcf(res,fr) becomes equivalent
//   to moog_vcf_2b[n](res^4,fr) (when res and fr are constant).
//
//   moog_vcf_2b  uses two direct-form biquads (tf2)
//   moog_vcf_2bn uses two protected normalized-ladder biquads (tf2np)
//
// REQUIRES: filter.lib
//
/*moog_vcf_2b(res,fr) = tf2s(0,0,b0,a11,a01,w1) : tf2s(0,0,b0,a12,a02,w1)*/
/*with {*/
/* s = 1; // minus the open-loop location of all four poles*/
/* frl = max(20,min(10000,fr)); // limit fr to reasonable 20-10k Hz range*/
/* w1 = 2*PI*frl; // frequency-scaling parameter for bilinear xform*/
/* // Equivalent: w1 = 1; s = 2*PI*frl;*/
/* kmax = sqrt(2)*0.999; // 0.999 gives stability margin (tf2 is unprotected)*/
/* k = min(kmax,sqrt(2)*res); // fourth root of Moog VCF feedback gain*/
/* b0 = s^2;*/
/* s2k = sqrt(2) * k;*/
/* a11 = s * (2 + s2k);*/
/* a12 = s * (2 - s2k);*/
/* a01 = b0 * (1 + s2k + k^2);*/
/* a02 = b0 * (1 - s2k + k^2);*/
/*};*/

moog_vcf_2bn(res,fr) = tf2snp(0,0,b0,a11,a01,w1) : tf2snp(0,0,b0,a12,a02,w1)
with {
 s = 1; // minus the open-loop location of all four poles
 w1 = 2*PI*max(fr,20); // frequency-scaling parameter for bilinear xform
 k = sqrt(2)*0.999*res; // fourth root of Moog VCF feedback gain
 b0 = s^2;
 s2k = sqrt(2) * k;
 a11 = s * (2 + s2k);
 a12 = s * (2 - s2k);
 a01 = b0 * (1 + s2k + k^2);
 a02 = b0 * (1 - s2k + k^2);
};


///////////////////////////////////////////////////////////////////////////////////////////////////

resonance = hslider("resonance", 0.9, 0, 1, 0.01): smooth(0.999);
frequency = hslider("frequency", 22000, 20, 22000, 1) : smooth(0.999);
process = moog_vcf_2bn(resonance,frequency);
