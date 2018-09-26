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

#include <stdint.h>
#include "mex.h"

template<typename T>
struct MatlabType {};

template<>
struct MatlabType<bool>
{
	constexpr static const mxClassID classID = mxLOGICAL_CLASS;
};

template<>
struct MatlabType<int8_t>
{
	constexpr static const mxClassID classID = mxINT8_CLASS;
};

template<>
struct MatlabType<uint8_t>
{
	constexpr static const mxClassID classID = mxUINT8_CLASS;
};


template<>
struct MatlabType<int16_t>
{
	constexpr static const mxClassID classID = mxINT16_CLASS;
};

template<>
struct MatlabType<uint16_t>
{
	constexpr static const mxClassID classID = mxUINT16_CLASS;
};


template<>
struct MatlabType<uint32_t>
{
	constexpr static const mxClassID classID = mxUINT32_CLASS;
};

template<>
struct MatlabType<int32_t>
{
	constexpr static const mxClassID classID = mxINT32_CLASS;
};


template<>
struct MatlabType<int64_t>
{
	constexpr static const mxClassID classID = mxINT64_CLASS;
};

template<>
struct MatlabType<uint64_t>
{
	constexpr static const mxClassID classID = mxUINT64_CLASS;
};


template<>
struct MatlabType<float>
{
	constexpr static const mxClassID classID = mxSINGLE_CLASS;
};

template<>
struct MatlabType<double>
{
	constexpr static const mxClassID classID = mxDOUBLE_CLASS;
};


