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

#include "simplematcompress.h"

#include"../cvmat/cvmattreestruct.h"

#include<opencv/cv.hpp>
#include <cvmat/cvmattreestructextra.h>

namespace CppFW
{

	SimpleMatCompress::SimpleMatCompress(int rows, int cols, uint8_t initValue)
	: rows(rows)
	, cols(cols)
	{
		addSegment(rows*cols, initValue);
	}

	bool SimpleMatCompress::readFromMat(const uint8_t* mat, int rows, int cols)
	{
		this->rows = rows;
		this->cols = cols;
		segmentsChange.clear();

		sumSegments = 0;

		if(mat == nullptr)
			return false;

		int     actSegmentLengt = 0;
		uint8_t actSegmentValue = *mat;

		const uint8_t* dataPtr = mat;
		for(int row = 0; row < rows; ++row)
		{
			for(int col = 0; col < cols; ++col)
			{
				if(actSegmentValue != *dataPtr)
				{
					addSegment(actSegmentLengt, actSegmentValue);
					actSegmentValue = *dataPtr;
					actSegmentLengt = 1;
				}
				else
					++actSegmentLengt;

				++dataPtr;
			}
		}
		addSegment(actSegmentLengt, actSegmentValue);

		assert(sumSegments == rows*cols);
		return true;
	}

	bool SimpleMatCompress::writeToMat(uint8_t* mat, int rows, int cols) const
	{
		return writeToMatConvert(mat, rows, cols);
	}

	template<typename T>
	bool SimpleMatCompress::writeToMatConvert(T* mat, int rows, int cols) const
	{
		if(this->rows != rows || this->cols != cols || mat == nullptr)
			return false;

		for(const MatSegment& segment : segmentsChange)
		{
			uint8_t segmentValue = segment.value;
			for(int i=0; i<segment.length; ++i, ++mat)
				*mat = segmentValue;
		}
		return true;
	}


	template bool SimpleMatCompress::writeToMatConvert(float*, int, int) const;


	inline void SimpleMatCompress::addSegment(int length, uint8_t value)
	{
		segmentsChange.push_back(MatSegment(length, value));
		sumSegments += length;
	}

	bool SimpleMatCompress::isEmpty(uint8_t defaultValue) const
	{
		if(segmentsChange.empty())
			return true;
		else if(segmentsChange.size() == 1)
			return segmentsChange[0].value == defaultValue;
		return false;
	}

	bool SimpleMatCompress::isEqual(const uint8_t* mat, int rows, int cols) const
	{
		if(this->rows != rows || this->cols != cols || mat == nullptr)
			return false;

		for(const MatSegment& segment : segmentsChange)
		{
			uint8_t segmentValue = segment.value;
			for(int i=0; i<segment.length; ++i, ++mat)
				if(*mat != segmentValue)
					return false;
		}
		return true;
	}

	bool SimpleMatCompress::operator==(const SimpleMatCompress& other) const
	{
		return rows           == other.rows
		    && cols           == other.cols
		    && segmentsChange == other.segmentsChange;
	}




	bool SimpleMatCompress::fromCVMatTree(const CppFW::CVMatTree& imgCompressNode)
	{
		int imageHeight = CppFW::CVMatTreeExtra::getCvScalar(&imgCompressNode, "height", uint32_t());
		int imageWidth  = CppFW::CVMatTreeExtra::getCvScalar(&imgCompressNode, "width" , uint32_t());
		const cv::Mat& compressSymbols   = imgCompressNode.getDirNode("compressSymbols")  .getMat();
		const cv::Mat& compressRunLength = imgCompressNode.getDirNode("compressRunLength").getMat();


		if(cv::DataType<uint8_t> ::type != compressSymbols  .type()
		|| cv::DataType<uint32_t>::type != compressRunLength.type())
			return false;

		if(compressSymbols.rows*compressSymbols.cols != compressRunLength.rows*compressRunLength.cols)
			return false;

		const int compDataLength = compressSymbols.rows*compressSymbols.cols;

		const uint8_t * compSymbolPtr = compressSymbols  .ptr<uint8_t >();
		const uint32_t* compRunLenPtr = compressRunLength.ptr<uint32_t>();

		segmentsChange.clear();
		segmentsChange.reserve(compDataLength);
		for(int i = 0; i < compDataLength; ++i)
		{
			uint8_t  symbol = *compSymbolPtr;
			uint32_t number = *compRunLenPtr;

			segmentsChange.emplace_back(number, symbol);

			++compSymbolPtr;
			++compRunLenPtr;
		}

		rows = imageHeight;
		cols = imageWidth ;

		return true;
	}

	void SimpleMatCompress::toCVMatTree(CppFW::CVMatTree& imgCompressNode) const
	{
		cv::Mat compressSymbols   = cv::Mat(1, segmentsChange.size(), cv::DataType<uint8_t> ::type);
		cv::Mat compressRunLength = cv::Mat(1, segmentsChange.size(), cv::DataType<uint32_t>::type);

		uint8_t * compSymbolPtr = compressSymbols  .ptr<uint8_t >();
		uint32_t* compRunLenPtr = compressRunLength.ptr<uint32_t>();
		for(const MatSegment& segment : segmentsChange)
		{
			*compSymbolPtr = segment.value;
			*compRunLenPtr = segment.length;

			++compSymbolPtr;
			++compRunLenPtr;
		}

		imgCompressNode.clear();
		CppFW::CVMatTreeExtra::setCvScalar(imgCompressNode, "height", rows);
		CppFW::CVMatTreeExtra::setCvScalar(imgCompressNode, "width" , cols);
		imgCompressNode.getDirNode("compressSymbols")  .getMat() = compressSymbols  ;
		imgCompressNode.getDirNode("compressRunLength").getMat() = compressRunLength;
	}
}
