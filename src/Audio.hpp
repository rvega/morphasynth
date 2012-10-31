#include <jack/jack.h>

class MainWindow;

class Audio {
   public:
      explicit Audio(const MainWindow& _window);
      virtual ~Audio();

      void start();
      void stop();

   private:
      Audio(const Audio&);
      Audio& operator=(const Audio&);

      void initJack();
      void initPd();

      const MainWindow& window;
      unsigned int sampleRate;
};
