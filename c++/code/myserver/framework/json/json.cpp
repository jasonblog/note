#include "json.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "log.h"

namespace MyJson {
	Json::Json() {
	}
	Json::~Json() {
        Clear();
	}

	int Json::LoadFromFile(const std::string &filename) {
        m_sFilename = filename;

		try {
			std::fstream fp;
			fp.open(filename.c_str());
			if (!fp.is_open()) {
				LOG_ERROR("default","config file not exist: {}", filename.c_str());
				return -1;
			}
			std::string data;
			std::string linedata;

			while (!fp.eof()) {
				getline(fp, linedata, '\n');
				data += linedata;
			}
			fp.close();
            m_Obj.Parse(data.c_str());
			int ret = Parse();
			return ret;
		} catch (...) {
			return -1;
		}
	}

	int Json::Parse()
    {
		return 0;
	}

	void Json::Clear()
    {
		m_Obj.Clear();
		m_sFilename.clear();
	}
	
	int Json::Reload()
    {
        Clear();
		return LoadFromFile(m_sFilename);
	}
}