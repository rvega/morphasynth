declare name "resonant_hpf";

import("filter.lib");

//G = hslider("g", 0, -10, 10, 0.1) : smooth(0.999);
F = hslider("f", 1000, 20, 22000, 1) : smooth(0.999);
Q = hslider("q", 1, 0.01, 100, 0.01) : smooth(0.999);

process(x) = resonhp(F, Q, 1, x);
