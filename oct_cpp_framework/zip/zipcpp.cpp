#include "zipcpp.h"

#include<minizip/zip.h>


#ifdef WITH_ZLIB
namespace CppFW
{

	ZipCpp::ZipCpp(const std::string& filename)
	{
		file = zipOpen(filename.c_str(), APPEND_STATUS_CREATE);
	}


	ZipCpp::~ZipCpp()
	{
		zipClose(file, nullptr);
	}


	void ZipCpp::addFile(const std::string& zipPath, const char* buff, std::size_t bufflen, bool compress)
	{
		zip_fileinfo zinfo{};

		/*int code = */zipOpenNewFileInZip(file,
		                       zipPath.c_str(),
		                       &zinfo,
		                       nullptr,
		                       0,
		                       nullptr,
		                       0,
		                       nullptr,
		                       compress?Z_DEFLATED:0,
		                       Z_DEFAULT_COMPRESSION);

		zipWriteInFileInZip(file, buff, static_cast<unsigned>(bufflen));
		zipCloseFileInZip(file);
	}
}
#else
namespace CppFW
{
	ZipCpp::ZipCpp(const std::string& /*filename*/)
	{
		throw("not build with zlib");
	}
	ZipCpp::~ZipCpp() {}

	void ZipCpp::addFile(const std::string& /*zipPath*/, const char* /*buff*/, std::size_t /*bufflen*/, bool /*compress*/) {}
}

#endif
