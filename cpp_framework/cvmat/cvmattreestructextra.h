#pragma once

#include "cvmattreestruct.h"

#include <opencv/cv.h>

namespace CppFW
{

	class CVMatTreeExtra
	{
	public:

		template<typename T>
		static void setCvScalar(CVMatTree& tree, T value)
		{
			static_assert(cv::DataType<T>::generic_type == false, "non generic datatype");

			cv::Mat& mat = tree.getMat();
			mat.create(1, 1, cv::DataType<T>::type);
			(*mat.ptr<T>(0)) = value;
		}

		template<typename T>
		static void setCvScalar(CVMatTree& tree, const std::string& name, T value)
		{
			setCvScalar(tree.getDirNode(name), value);
		}

		template<typename T>
		static T getCvScalar(const CVMatTree* tree, const char* name, T defaultValue, std::size_t index = 0)
		{
			if(tree)
				return getCvScalar(tree->getDirNodeOpt(name), defaultValue, index);
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

			if(static_cast<std::size_t>(mat->rows*mat->cols) < index)
				return defaultValue;

			if(mat->type() == cv::DataType<T>::type)
				return *(mat->ptr<T>(0) + index);


			switch(mat->type())
			{
#define handleType(TYPE) case cv::DataType<TYPE>::type: return cv::saturate_cast<T>(*(mat->ptr<TYPE>(0) + index));
// 				case cv::DataType<int>::type: return cv::saturate_cast<T>(*(mat->ptr<int>(0) + index));
				handleType(int8_t);
				handleType(int16_t);
				handleType(int32_t);
				handleType(uint8_t);
				handleType(uint16_t);
				handleType(uint32_t);
#undef handleType
			}

// 			cv::Mat t = (*mat)(cv::Range(0, 1), cv::Range(0, 1));
			cv::Mat conv;
			mat->convertTo(conv, cv::DataType<T>::type); // TODO: convert-funktion ersetzen
			return *(conv.ptr<T>(0) + index);
		}

		template<typename T>
		static cv::Mat convertVector2Mat(const std::vector<T>& vector)
		{
			cv::Mat out(static_cast<int>(vector.size()), 1, cv::DataType<T>::type);
			memcpy(out.data, vector.data(), vector.size()*sizeof(T));
			return out;
		}

		template<typename MatType, typename VecType>
		static void convertMat2Vec(const cv::Mat& mat, std::vector<VecType>& vector)
		{
			std::size_t size = static_cast<std::size_t>(mat.cols)*static_cast<std::size_t>(mat.rows);
			vector.resize(size);
			std::transform(mat.ptr<MatType>(), mat.ptr<MatType>()+size, vector.begin(), [](MatType val){ return static_cast<VecType>(val); });
		}


		template<typename T>
		static std::vector<T> getCvVector(const CVMatTree* tree)
		{
			std::vector<T> vec;
			if(!tree)
				return vec;

			const cv::Mat* mat = tree->getMatOpt();
			if(!mat)
				return vec;

			switch(mat->type())
			{
#define handleType(TYPE) case cv::DataType<TYPE>::type: convertMat2Vec<TYPE, T>(*mat, vec); break;
				handleType(int8_t);
				handleType(int16_t);
				handleType(int32_t);
				handleType(uint8_t);
				handleType(uint16_t);
				handleType(uint32_t);
				handleType(float);
				handleType(double);
#undef handleType
			}

			return vec;
		}


		static std::string getStringOrEmpty(const CVMatTree* tree, const char* name)
		{
			if(!tree)
				return std::string();

			const CVMatTree* node = tree->getDirNodeOpt(name);
			if(!node)
				return std::string();

			if(node->type() != CVMatTree::Type::String)
				return std::string();

			return node->getString();
		}

	};

}
