// ADF is the same as MP3, but each byte is XOR'd with 0x22

#include "LoaderMP3.hpp"

class LoaderADF : LoaderMP3 {
  friend LoaderMP3;
private:
  void preprocess(std::vector& data) {
    std::transform(
      data.begin(), data.end()
      [](uint8_t byte) { return byte ^ 0x22; }
    );
  }
};
