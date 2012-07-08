#pragma once

#include <vector>
// #include <map>
#include <iostream>

class Controller;

class MidiMap{
	public:
      MidiMap(Controller* ctlr);
      ~MidiMap();

      /**
       * Receives a midi message, translates it into a midi event that we can understand
       * and sends it to the controller
       */
      void process(double deltatime, std::vector< unsigned char > *bytes);

   private:
      Controller* controller;
      void print(double deltatime, std::vector<unsigned char>* bytes);
};
