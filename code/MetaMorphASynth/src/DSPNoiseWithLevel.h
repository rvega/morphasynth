#include "Noise.h"
#include "Stk.h"

using namespace stk;

class DSPNoiseWithLevel : public Noise{
   public:
      DSPNoiseWithLevel();
      ~DSPNoiseWithLevel();

      void setLevel(StkFloat level);
      StkFloat tick(void);

   protected:
      StkFloat level;
};

inline StkFloat DSPNoiseWithLevel::tick( void ) {
  return level * Noise::tick();
}
