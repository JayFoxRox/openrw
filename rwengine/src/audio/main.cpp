#include "SoundManager.hpp"

#include "LoaderWAV.hpp"
#if 0
#include "LoaderMP3.hpp"
#include "LoaderADF.hpp"
#include "LoaderSDT.hpp"
#endif

#include <iostream>

#include <unistd.h>

int main(int argc, char* argv[]) {
  std::cout << "OpenRW audio player" << std::endl;

  std::string command = argv[1];
  std::string path = argv[2];
  {
    SoundManager m;

    // Load a file
    SoundManager::Sound* s = nullptr;
    if (command == "wav") {
      LoaderWAV l(path);
      s = m.createSound(l);
#if 0
    } else if (command == "mp3") {
      LoaderMP3 l(path);
      s = m.createSound(l);
    } else if (command == "adf") {
      LoaderADF l(path);
      s = m.createSound(l);
    } else if (command == "sdt") {
      int index = atoi(argv[3]);
      LoaderSDT sdt(path);
      std::cout << "Seeking " << index << std::endl;
      auto loader l = sdt.getSoundLoader(index);
      s = m.createSound(l);
#endif
    } else {
      std::cout << "Unknown command '" << command << "'" << std::endl;
    }

    // Start playback
    s->play(false);

    // Print status while playing the file
    while(s->isPlaying()) {
      std::cout << s->getTime() << " / " << s->getLength();
      m.update();
      usleep(1000);
    }
  }

  std::cout << "done." << std::endl;
  return 0;
}
