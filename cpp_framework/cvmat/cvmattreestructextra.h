#pragma once

#include "cvmattreestruct.h"

#include <opencv/cv.h>

namespace CppFW
{

	class CVMatTreeExtra
	{
	public:


		template<typename T>
		static T getCvScalar(const CVMatTree* tree, const char* name, T defaultValue)
		{
			if(tree)
				return getCvScalar(tree->getDirNodeOpt(name), defaultValue);
			return defaultValue;
		}


		template<typename T>
		static T getCvScalar(const CVMatTree* tree, T defaultValue, std::size_t index = 0)
		{
			if(!tree)
				return defaultValue;

			const cv::Mat* mat = tree->getMatOpt();
			if(!mat)
				return defaultValue;

			if(mat->type() == cv::DataType<T>::type)
				return *(mat->ptr<T>(0) + index);

// 			cv::Mat t = (*mat)(cv::Range(0, 1), cv::Range(0, 1));
			cv::Mat conv;
			mat->convertTo(conv, cv::DataType<T>::type); // TODO: convert-funktion ersetzen
			return *(conv.ptr<T>(0) + index);
		}

	};

}
