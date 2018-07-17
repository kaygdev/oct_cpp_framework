#include "unzipcpp.h"

#ifdef WITH_ZLIB
#include<minizip/unzip.h>


namespace CppFW
{
	UnzipCpp::UnzipCpp(const std::string& filename)
	{
		file = unzOpen(filename.c_str());
	}

	UnzipCpp::~UnzipCpp()
	{
		unzClose(file);
	}

	std::vector<char> UnzipCpp::readFile(const std::string& zipPath)
	{
		/*int result = */ unzLocateFile(file, zipPath.c_str(), 1);

		unz_file_info pfile_info;
		unzGetCurrentFileInfo(file, &pfile_info, nullptr, 0, nullptr, 0, nullptr, 0);

		std::size_t filebuffersize = pfile_info.uncompressed_size;
		std::vector<char> fileBuffer(filebuffersize);

		unzOpenCurrentFile(file);
		unzReadCurrentFile(file, fileBuffer.data(), fileBuffer.size());
		unzCloseCurrentFile(file);

		return fileBuffer;
	}
}
#else
namespace CppFW
{
	UnzipCpp::UnzipCpp(const std::string& /*filename*/)
	{
		throw("not build with zlib");
	}

	UnzipCpp::~UnzipCpp() {}

	std::vector<char> UnzipCpp::readFile(const std::string& /*zipPath*/) { return std::vector<char>(); }
}
#endif

