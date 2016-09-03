// This encapsulates an FFMPEG audio stream

#include "SoundManager.hpp"

#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>

class FFMPEGStream : SoundManager::SoundLoader {
  int y;
private:
#if 0
  AVFormatContext* ic;
  int stream_idx;

  // Register FFMPEG codecs and formats
  void initializeFFMPEG() {
    static bool registered = false;
    if (!registered) {
      av_register_all(); //FIXME: Only register necessary codecs / formats
      registered = true;
    }
  }

  // This assumes that ic and stream_idx have been set up already
  void initializeStream() {
    initializeFFMPEG();

    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        exit(1);
    }
    //FIXME: assert that this is indeed a supported wave codec

    
  }
#endif

public:

#if 0
  // Open from an existing stream.
  // Note that this will take full control over the stream, including cleanup!
  FFMPEGStream(AVFormatContext* ic, int stream_idx) : ic(ic), stream_idx(stream_idx) {
    initialize();
  }

  FFMPEGStream(std::string path) {

    initializeFFMPEG();

    // Open the file
    const char* cPath = path.c_str()
    if (avformat_open_input(&fmt_ctx, cPath, NULL, NULL) < 0) {
        fprintf(stderr, "Could not open source file %s\n", cPath);
        exit(1);
    }

    // Forward to our more generic constructor for existing FFMPEG streams
    initialize();
  }

  ~FFMPEGStream() {
    //FIXME!
  }

  // Returns wether the current chunk is stereo (mono otherwise)
  bool isStereo() {
    switch(numberOfChannels) {
    case 1: return false; // Mono
    case 2: return true; // Stereo
    default: // Something else
      assert(false);
      break;
    }
    return false;
  }

  // Get samplerate of current chunk
  void getSamplerate() {
  }

  // Returns the number of samples in the stream
  uint64_t getSize() {
    
  }
#endif
  // Returns up to `samples` number of samples from the stream as int16
  std::vector<int16_t> read(uint64_t offset, uint64_t samples) {
    
  }

  // Seek to a given sample
  void seek(unsigned int sample) {
    
  }
};
