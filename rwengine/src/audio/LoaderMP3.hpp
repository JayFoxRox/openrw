class LoaderMP3 : SoundLoader {
private:
  // This is a little hack for LoaderADF
  virtual void preprocess(std::vector<int16_t> data) = nullptr;
};

