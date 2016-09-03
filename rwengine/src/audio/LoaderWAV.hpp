#include "FFMPEGStream.hpp"

class LoaderWAV : SoundLoader {

private:
  FFMPEGStream ffmpeg;

public:

  LoaderWAV(std::string path) {
    ffmpeg = FFMPEGStream(path);
//    auto codec = ffmpeg.getCodec();
//    assert(codec == || codec == ); //FIXME!
  }

  std::vector<int16_t> read(unsigned int offset, unsigned int samples) {
    //SoundSource& soundSource
    //soundSource.fileInfo.channels == 1
    return ffmpeg.read(offset, samples);
  }

};
