#pragma once
#ifndef _GAMETEXTS_HPP_
#define _GAMETEXTS_HPP_
#include <string>
#include <unordered_map>

class GameTexts
{
#if GAME == GAME_III
	std::unordered_map<std::string, std::string> _textDB;
public:

	void addText(const std::string& id, const std::string& text) {
		_textDB.insert({ id, text });
	}

	std::string text(const std::string& id) {
		auto a = _textDB.find(id);
		if( a != _textDB.end() ) {
			return a->second;
		}
		return id;
	}
#elif GAME == GAME_VC
	std::unordered_map<std::string, std::unordered_map<std::string, std::string>> textDB;
public:

	void addText(const std::string& key, const std::string& id, const std::string& text) {
		auto keyDB = textDB.insert({ key, {}}).first;
    keyDB->second.insert({ id, text });
	}

	std::string text(const std::string& key, const std::string& id) {
		auto keyDB = textDB.find(key);
    if(keyDB == textDB.end()) {
      return key + "]" + id;
    }
		auto text = keyDB->second.find(id);
		if(text != keyDB->second.end()) {
			return text->second;
		}
    return key + "[" + id;
	}
	std::string text(const std::string& id) {
    return text("MAIN", id);
  }
#endif

};

#endif
