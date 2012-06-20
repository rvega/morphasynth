import("maxmsp.lib");
import("filter.lib");

/*G = hslider("Gain [unit:dB]", 0, -10, 10, 0.1);*/
F = hslider("Freq", 1000, 1, 22000, 1) : smooth(0.999);
Q = hslider("Q", 1, 0.07, 100, 0.01) : smooth(0.999);

process(x) = HPF(x,F,0,Q);

