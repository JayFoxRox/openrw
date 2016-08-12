#include <loaders/LoaderGXT.hpp>
#include <iconv.h>
#include <cassert>

void LoaderGXT::load(GameTexts &texts, FileHandle &file)
{
	auto data = file->data;

#if GAME == GAME_III
	data += 4; // TKEY

	std::uint32_t blocksize = *(std::uint32_t*)data;

	data += 4;

	auto tdata = data+blocksize+8;

	// This is not supported in GCC 4.8.1
	//std::wstring_convert<std::codecvt<char16_t,char,std::mbstate_t>,char16_t> convert;

	auto icv = iconv_open("UTF-8", "UTF-16");

	for( size_t t = 0; t < blocksize/12; ++t ) {
		size_t offset = *(std::uint32_t*)(data+(t * 12 + 0));
		std::string id(data+(t * 12 + 4));

		// Find the terminating bytes
		size_t bytes = 0;
		for(;; bytes++ ) {
			if(tdata[offset+bytes-1] == 0 && tdata[offset+bytes] == 0) break;
		}
		size_t len = bytes/2;

		size_t outSize = 1024;
		char u8buff[1024];
		char *uwot = u8buff;

		char* strbase = tdata+offset;

		iconv(icv, &strbase, &bytes, &uwot, &outSize);

		u8buff[len] = '\0';

		std::string message(u8buff);

		texts.addText(id, message);
	}

	iconv_close(icv);

#elif GAME == GAME_VC

	auto icv = iconv_open("UTF-8", "UTF-16");

  // Block 1 (TABL)
  auto tabl_data = data;
  tabl_data += 4; // TABL magic
	std::uint32_t tabl_blocksize = *(std::uint32_t*)tabl_data;
	tabl_data += 4;
	for(size_t tabl_index = 0; tabl_index < tabl_blocksize / 12; tabl_index++) {
		std::string tabl_tkey_id(tabl_data + (tabl_index * 12 + 0));
		size_t tabl_tkey_offset = *(std::uint32_t*)(tabl_data + (tabl_index * 12 + 8));

    // Block 2 (TKEY)
    //NOTE: Different for the first block! Omits the id
    auto tkey_data = data + tabl_tkey_offset;
		std::string tkey_id;
    if (tabl_index == 0) {
      tkey_id = "MAIN";
    } else {
      tkey_id = std::string(tkey_data);
      tkey_data += 8;
    }
    assert(tkey_id == tabl_tkey_id);
    tkey_data += 4; // TKEY magic
    std::uint32_t tkey_blocksize = *(std::uint32_t*)tkey_data;
    tkey_data += 4;
	  for(size_t tkey_index = 0; tkey_index < tkey_blocksize / 12; tkey_index++) {
		  size_t tkey_tdat_offset = *(std::uint32_t*)(tkey_data + (tkey_index * 12 + 0));
		  std::string tkey_tdat_id(tkey_data + (tkey_index * 12 + 4));

      // Block 3 (TDAT)
    	auto tdat_data = tkey_data + tkey_blocksize + tkey_tdat_offset;
		  size_t tdat_blocksize = *(std::uint32_t*)(tdat_data);
      tdat_data += 4;
      size_t bytes = 0;
		  for(;; bytes++) {
        assert(bytes < tdat_blocksize);
			  if(tdat_data[bytes] == 0 && tdat_data[bytes + 1] == 0) break;
		  }
		  size_t len = bytes / 2;

		  size_t outSize = 1024;
		  char u8buff[1024];
		  char *uwot = u8buff;

		  char* strbase = tdat_data;
		  iconv(icv, &strbase, &bytes, &uwot, &outSize);

		  u8buff[len] = '\0';

		  std::string tdat_message(u8buff);

		  texts.addText(tabl_tkey_id, tkey_tdat_id, tdat_message);
    }
	}

	iconv_close(icv);
#endif
}
