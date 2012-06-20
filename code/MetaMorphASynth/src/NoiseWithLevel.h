#include "Noise.h"
#include "Stk.h"

using namespace stk;

class NoiseWithLevel : public Noise{
   public:
      NoiseWithLevel();
      ~NoiseWithLevel();

      void setLevel(StkFloat level);
      StkFloat tick(void);

   protected:
      StkFloat level;
};

inline StkFloat NoiseWithLevel::tick( void ) {
  return level * Noise::tick();
}
