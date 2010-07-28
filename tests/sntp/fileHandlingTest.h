#include "sntptest.h"

class fileHandlingTest : public sntptest {
protected:
	enum DirectoryType {
		INPUT_DIR = 0,
		OUTPUT_DIR = 1
	};

	std::string CreatePath(const char* filename, DirectoryType argument) {
		std::string path;

		if (m_params.size() >= argument + 1) {
			path = m_params[argument];
		}

		if (path[path.size()-1] != DIR_SEP && !path.empty()) {
			path.append(1, DIR_SEP);
		}
		path.append(filename);

		return path;
	}
};
