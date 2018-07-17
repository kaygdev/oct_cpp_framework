#pragma once

#include<string>
#include<vector>

typedef void* zipFile;

namespace CppFW
{
class UnzipCpp
{
	zipFile file = nullptr;

	public:
		UnzipCpp(const std::string& filename);
		~UnzipCpp();

		UnzipCpp(const UnzipCpp& other) = delete;
		UnzipCpp& operator=(const UnzipCpp& other) = delete;

// 		void addFile(const std::string& zipPath, const char* buff, std::size_t bufflen, bool compress = true);
// 		void addFile(const std::string& zipPath, const unsigned char* buff, std::size_t bufflen, bool compress = true)
// 		                                                               { addFile(zipPath, reinterpret_cast<const char*>(buff), bufflen, compress); }
		std::vector<char> readFile(const std::string& zipPath);
};


}
