/*
 * Copyright (c) 2018 Kay Gawlik <kaydev@amarunet.de> <kay.gawlik@beuth-hochschule.de> <kay.gawlik@charite.de>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

