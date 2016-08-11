#include <loaders/LoaderTXD.hpp>
#include <gl/TextureData.hpp>

#include <iostream>
#include <algorithm>

GLuint gErrorTextureData[] = { 0xFFFF00FF, 0xFF000000, 0xFF000000, 0xFFFF00FF };
GLuint gDebugTextureData[] = {0xFF0000FF, 0xFF00FF00};
GLuint gTextureRed[] = {0xFF0000FF};
GLuint gTextureGreen[] = {0xFF00FF00};
GLuint gTextureBlue[] = {0xFFFF0000};

TextureData::Handle getErrorTexture()
{
	static GLuint errTexName = 0;
	static TextureData::Handle tex;
	if(errTexName == 0)
	{
		glGenTextures(1, &errTexName);
		glBindTexture(GL_TEXTURE_2D, errTexName);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA,
			2, 2, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, gErrorTextureData
		);
		glGenerateMipmap(GL_TEXTURE_2D);

		tex = TextureData::create(errTexName, {2, 2}, false);
	}
	return tex;
}

const size_t paletteSize = 1024;
void processPalette(uint32_t* fullColor, RW::BinaryStreamSection& rootSection)
{
	uint8_t* dataBase = reinterpret_cast<uint8_t*>(rootSection.raw() + sizeof(RW::BSSectionHeader) + sizeof(RW::BSTextureNative) - 4);

	uint8_t* coldata = (dataBase + paletteSize + sizeof(uint32_t));
	uint32_t raster_size = *reinterpret_cast<uint32_t*>(dataBase + paletteSize);
	uint32_t* palette = reinterpret_cast<uint32_t*>(dataBase);

	for(size_t j = 0; j < raster_size; ++j)
	{
		*(fullColor++) = palette[coldata[j]];
	}

}

TextureData::Handle createTexture(RW::BSTextureNative& texNative, RW::BinaryStreamSection& rootSection)
{
	// TODO: Exception handling.
	if(texNative.platform != 8) {
		std::cerr << "Unsupported texture platform " << std::dec << texNative.platform << std::endl;
		return getErrorTexture();
	}

	bool isPal4 = (texNative.rasterformat & RW::BSTextureNative::FORMAT_EXT_PAL4); //FIXME!
	bool isPal8 = (texNative.rasterformat & RW::BSTextureNative::FORMAT_EXT_PAL8) == RW::BSTextureNative::FORMAT_EXT_PAL8;

	// Export this value
	bool transparent = !((texNative.rasterformat&RW::BSTextureNative::FORMAT_888) == RW::BSTextureNative::FORMAT_888);

  // Strip the flags away
  uint32_t rasterformat = texNative.rasterformat & 0xFFF;

	GLuint textureName = 0;

	if(isPal4)
	{
		std::cerr << "Unsuported palette mode" << std::endl;
		return getErrorTexture();
	} else if(isPal8)
	{
		std::vector<uint32_t> fullColor(texNative.width * texNative.height);

		processPalette(fullColor.data(), rootSection);

		glGenTextures(1, &textureName);
		glBindTexture(GL_TEXTURE_2D, textureName);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA,
			texNative.width, texNative.height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, fullColor.data()
		);
	}
	else
	{
    bool isDxt = (texNative.dxttype != 0x00);

		auto coldata = rootSection.raw() + sizeof(RW::BSTextureNative);
		coldata += sizeof(uint32_t);
		coldata += 8;

    if(isDxt) {
      GLenum internalFormat;
      switch(rasterformat)
		  {
		  case RW::BSTextureNative::FORMAT_1555:
        internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			  break;
		  case RW::BSTextureNative::FORMAT_565:
        internalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
			  break;
		  case RW::BSTextureNative::FORMAT_4444:
        internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			  break;
		  default:
        assert(false); // This should never happen, only the formats above are known.
        return getErrorTexture();
		  }

      // Complain if the s3tc-extension is not available
      if(!ogl_ext_EXT_texture_compression_s3tc) {
        static bool warned = false;
        if (!warned) {
          std::cerr << "Your graphics driver doesn't support EXT_texture_compression_s3tc. Some textures will not load correctly." << std::endl;
          warned = true;
        }
        debugLabel += "no-s3tc-extension;";
        return getErrorTexture(debugLabel);
      }

		  glGenTextures(1, &textureName);
		  glBindTexture(GL_TEXTURE_2D, textureName);
		  glCompressedTexImage2D(
			  GL_TEXTURE_2D, 0, internalFormat,
			  texNative.width, texNative.height, 0,
			  texNative.datasize, coldata
		  );
    }
    else
    {
	    GLenum type = GL_UNSIGNED_BYTE, format = GL_RGBA;
	    switch(rasterformat)
	    {
		    case RW::BSTextureNative::FORMAT_1555:
			    format = GL_RGBA;
			    type = GL_UNSIGNED_SHORT_1_5_5_5_REV;
			    break;
		    case RW::BSTextureNative::FORMAT_565:
			    format = GL_RGB;
			    type = GL_UNSIGNED_SHORT_5_6_5_REV;
			    break;
        case RW::BSTextureNative::FORMAT_4444:
			    format = GL_RGBA;
			    type = GL_UNSIGNED_SHORT_4_4_4_4_REV;
          break;
		    case RW::BSTextureNative::FORMAT_8888:
			    format = GL_BGRA;
			    type = GL_UNSIGNED_BYTE;
			    break;
		    case RW::BSTextureNative::FORMAT_888:
			    format = GL_BGRA;
			    type = GL_UNSIGNED_BYTE;
			    break;
	    default:
          std::cerr << "Unsuported raster format " << std::dec << rasterformat << (isDxt ? " (Compressed)" : "") << std::endl;
          return getErrorTexture();
	    }
		  glGenTextures(1, &textureName);
		  glBindTexture(GL_TEXTURE_2D, textureName);
		  glTexImage2D(
			  GL_TEXTURE_2D, 0, GL_RGBA,
			  texNative.width, texNative.height, 0,
			  format, type, coldata
		  );
    }
  }

  auto glTexFilter = [](uint8_t filter, bool mipmap) -> GLenum {
    switch(filter) {
    case RW::BSTextureNative::FILTER_NEAREST:
      return GL_NEAREST;
    case RW::BSTextureNative::FILTER_LINEAR:
      return GL_LINEAR;
    case RW::BSTextureNative::FILTER_MIP_NEAREST:
			// @todo Verify
      return mipmap ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
    case RW::BSTextureNative::FILTER_MIP_LINEAR:
			// @todo Verify
      return mipmap ? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST;
    case RW::BSTextureNative::FILTER_LINEAR_MIP_NEAREST:
      return mipmap ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR;
    case RW::BSTextureNative::FILTER_LINEAR_MIP_LINEAR:
      return mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
    case RW::BSTextureNative::FILTER_NONE: // @todo Check what this is
    default:
      assert(false);
      break;
    }
    return GL_LINEAR;
  };

  auto glWrapMode = [](uint8_t mode) -> GLenum {
    switch(mode) {
    case RW::BSTextureNative::WRAP_WRAP:
      return GL_REPEAT;
    case RW::BSTextureNative::WRAP_CLAMP:
      return GL_CLAMP_TO_EDGE;
    case RW::BSTextureNative::WRAP_MIRROR:
      return GL_MIRRORED_REPEAT;
    case RW::BSTextureNative::WRAP_NONE: // @todo Check what this is
    default:
      assert(false);
      break;
    }
    return GL_REPEAT;
  };

	bool useMipMaps = false;
  if (generateMipMaps) {
	  glGenerateMipmap(GL_TEXTURE_2D);
		useMipMaps = true;
	} else if (hasMipMaps) {
		// @todo Load mipmaps from file instead
	  glGenerateMipmap(GL_TEXTURE_2D);
		useMipMaps = true;
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glTexFilter(texNative.filterflags, useMipMaps));

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrapMode(texNative.wrapU));
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrapMode(texNative.wrapV));

	return TextureData::create( textureName, { texNative.width, texNative.height }, transparent );
}

bool TextureLoader::loadFromMemory(FileHandle file, TextureArchive &inTextures)
{
	auto data = file->data;
	RW::BinaryStreamSection root(data);
	/*auto texDict =*/ root.readStructure<RW::BSTextureDictionary>();

	size_t rootI = 0;
	while (root.hasMoreData(rootI)) {
		auto rootSection = root.getNextChildSection(rootI);

		if (rootSection.header.id != RW::SID_TextureNative)
			continue;

		RW::BSTextureNative texNative = rootSection.readStructure<RW::BSTextureNative>();
		std::string name = std::string(texNative.diffuseName);
		std::string alpha = std::string(texNative.alphaName);
		std::transform(name.begin(), name.end(), name.begin(), ::tolower );
		std::transform(alpha.begin(), alpha.end(), alpha.begin(), ::tolower );

		auto texture = createTexture(texNative, rootSection);

		inTextures[{name, alpha}] = texture;

		if( !alpha.empty() ) {
			inTextures[{name, ""}] = texture;
		}
	}

	return true;
}

// TODO Move the Job system out of the loading code
#include <platform/FileIndex.hpp>

LoadTextureArchiveJob::LoadTextureArchiveJob(WorkContext *context, FileIndex* index, TextureArchive &inTextures, const std::string &file)
	: WorkJob(context)
	, archive(inTextures)
	, fileIndex(index)
	, _file(file)
{

}

void LoadTextureArchiveJob::work()
{
	data = fileIndex->openFile(_file);
}

void LoadTextureArchiveJob::complete()
{
	// TODO error status
	if(data) {
		TextureLoader loader;
		loader.loadFromMemory(data, archive);
	}
}
