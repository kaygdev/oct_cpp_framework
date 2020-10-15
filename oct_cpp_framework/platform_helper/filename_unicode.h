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

#include <string>
#include<filesystem>

#include <boost/predef.h>



#if BOOST_OS_WINDOWS
	#include <codecvt>

	typedef std::wstring FileString;

	inline std::wstring filenameConv(const std::string& utf_str)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.from_bytes(utf_str);
	}

	inline std::wstring filepathConv(const std::filesystem::path& file)
	{
		return file.generic_wstring();
	}
#else

	typedef std::string FileString;

	inline const std::string& filenameConv(const std::string& utf_str) { return utf_str; }
	inline const std::string filepathConv(const std::filesystem::path& file)
	{
		return file.generic_string();
	}

#endif

