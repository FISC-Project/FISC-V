#ifndef FILE_H_
#define FILE_H_

#include <string>
#include <fstream>
#include <iostream>

enum FILEOP {
	FILEOP_NULL,
	FILEOP_READ_ALL = -1,
	FILEOP_READ_LINE = -2,
	FILEOP_COUNT = 4
};

class File {
public:
	File(std::string fileName, unsigned int mode);
	bool create(std::string fileName, unsigned int mode);
	std::string & read(unsigned int bytecount);
	std::string & read(unsigned int offset, unsigned int bytecount);
	bool write(std::string data);

	bool isOpen();
	void open();
	void close();

	unsigned int fileSize();

	friend std::ostream& operator<<(std::ostream& os, File & str);
	friend std::string& operator<<(File & file, std::string & str);

private:
	bool valid;
	unsigned int mode;
	std::string fileName;
	std::fstream fileHandle;
	std::string buffer;
};

inline bool fileExists(const char *fileName)
{
	return std::ifstream(fileName).good();
}

#endif