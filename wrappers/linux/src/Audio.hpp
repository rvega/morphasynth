#include <jack/jack.h>

class Audio {
   public:
      explicit Audio();
      virtual ~Audio();

      void start();
      void stop();
      void setSampleRate(unsigned int sr);
      unsigned int getSampleRate();

   private:
      Audio(const Audio&);
      Audio& operator=(const Audio&);

      void initJack();
      void initPd();

      unsigned int sampleRate;
};
