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

#include "zipcpp.h"



#ifdef WITH_ZLIB
#include<minizip/zip.h>


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
