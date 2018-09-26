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
