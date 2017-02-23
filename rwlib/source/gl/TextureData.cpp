#include <gl/TextureData.hpp>

static constexpr uint32_t gErrorTextureData[] = {
	0xFF0000FF, 0xFFFF00FF, 0xFF0000FF, 0xFFFF00FF,
	0xFFFF00FF, 0xFF0000FF, 0xFFFF00FF, 0xFF0000FF,
	0xFF0000FF, 0xFFFF00FF, 0xFF0000FF, 0xFFFF00FF,
	0xFFFF00FF, 0xFF0000FF, 0xFFFF00FF, 0xFF0000FF,
};

TextureData::Handle getErrorTexture(std::string debugLabel)
{
	static GLuint errTexName = 0;
	static TextureData::Handle tex;
	if(errTexName == 0)
	{
		glGenTextures(1, &errTexName);
		glBindTexture(GL_TEXTURE_2D, errTexName);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA,
			4, 4, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, gErrorTextureData
		);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

 		tex = TextureData::create(errTexName, {4, 4}, false);
  }

  glObjectLabel(GL_TEXTURE, errTexName, -1, (debugLabel != "" ? debugLabel.c_str() : NULL));

  return tex;
}
