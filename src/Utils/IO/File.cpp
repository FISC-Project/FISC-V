#include <fvm/Utils/IO/File.h>
#include <fvm/Utils/String.h>
#include <sstream>
#include <vector>

File::File(std::string fileName, unsigned int mode)
{
	create(fileName, mode);
}

bool File::create(std::string fileName, unsigned int mode)
{
	if(fileName == NULLSTR) {
		this->fileName = NULLSTR;
		this->mode = 0;
		this->buffer.clear();
		valid = false;
		return valid;
	}

	if (fileExists(fileName.c_str())) {
		this->fileName = fileName;
		this->mode = mode;
		this->buffer = "";
		valid = true;
	} else {
		this->fileName = NULLSTR;
		this->mode = 0;
		this->buffer.clear();
		valid = false;
	}
	return valid;
}

bool File::isOpen()
{
	return fileHandle.is_open();
}

void File::open()
{
	fileHandle.open(fileName, mode);
}

void File::close()
{
	fileHandle.close();
}

unsigned int File::fileSize()
{
	unsigned int curPos = fileHandle.tellg();
	fileHandle.seekg(0, fileHandle.end);
	unsigned int size = fileHandle.tellg();
	fileHandle.seekg(curPos);
	return size;
}

std::string & File::read(unsigned int offset, unsigned int bytecount)
{
	buffer = "";
	bool wasClosed = false;

	if (!isOpen() && valid) {
		wasClosed = true;
		open();
	}

	if(offset)
		fileHandle.seekg(offset);

	switch (((signed int)bytecount)) {
	case FILEOP_READ_ALL: {
		std::vector<char> buffer_char(fileSize());
		if (offset)
			fileHandle.seekg(offset);
		else
			fileHandle.seekg(0, fileHandle.beg);
		if (fileHandle.read(buffer_char.data(), buffer_char.size()))
			buffer = std::string(buffer_char.begin(), buffer_char.end());
		break;
	}
	case FILEOP_READ_LINE: {
		std::string line;
		if(getline(fileHandle, line))
			buffer = line;
		else
			buffer = "";
		break;
	}
	default: {
		std::vector<char> buffer_char(bytecount);
		if (fileHandle.read(buffer_char.data(), buffer_char.size()))
			buffer = std::string(buffer_char.begin(), buffer_char.end());
	}
	}

	if(wasClosed)
		close();

	return buffer;
}

std::string & File::read(unsigned int bytecount)
{
	return read(0, bytecount);
}

bool File::write(std::string data)
{
	bool wasClosed = false;

	if (!isOpen() && valid) {
		wasClosed = true;
		open();
	}
	
	fileHandle << data;
	
	if (wasClosed)
		close();

	return true;
}

std::ostream& operator<<(std::ostream & os, File & file)
{
	if (file.isOpen()) {
		os << file.read(FILEOP_READ_ALL);
		file.close();
	}		
	return os;
}

std::string& operator<<(File & file, std::string & str)
{
	if(!file.isOpen())
		file.open();
	file.write(str);
	return str;
}