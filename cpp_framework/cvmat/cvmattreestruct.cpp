#include "cvmattreestruct.h"


#include <fstream>
#include <string>

#include <opencv/cv.h>

#include <boost/lexical_cast.hpp>

#include <stdexcept>

namespace
{
	template<typename Type, typename IndexType, typename MapType>
	Type& getAndInsert(const IndexType& id, MapType& map)
	{
		typename MapType::iterator it = map.find(id);
		if(it == map.end())
		{
			Type* type = new Type;
			std::pair<typename MapType::iterator, bool> pit;
			try
			{
				pit = map.emplace(id, type);
			}
			catch(...)
			{
				delete type;
				throw;
			}
			if(pit.second == false)
			{
				delete type;
				throw "SubstructureTemplate pit.second == false";
			}
			return *((pit.first)->second);
		}
		return *(it->second);
	};
	
	bool matIsEqual(const cv::Mat mat1, const cv::Mat mat2) // http://dragonwood-blastevil.blogspot.de/2013/02/opencv-compare-whether-two-mat-is.html
	{
		// treat two empty mat as identical as well
		if(mat1.empty() && mat2.empty())
			return true;
		
		// if dimensionality of two mat is not identical, these two mat is not identical
		if(mat1.cols != mat2.cols || mat1.rows != mat2.rows || mat1.dims != mat2.dims)
			return false;
			
		cv::Mat diff;
		cv::compare(mat1, mat2, diff, cv::CMP_NE);
		int nz = cv::countNonZero(diff);
		return nz == 0;
	}
	
	template <typename Map>
	bool map_compare_ptr(const Map& lhs, const Map& rhs)
	{
		if(lhs.size() != rhs.size())
			return false;
		typename Map::const_iterator lhsIt = lhs.begin();
		typename Map::const_iterator rhsIt = rhs.begin();
		for(std::size_t i=0; i<lhs.size(); ++i)
		{
			if(lhsIt->first != rhsIt->first)
				return false;
			if(*(lhsIt->second) != *(rhsIt->second))
				return false;
			++lhsIt;
			++rhsIt;
		}
		return true;
	}

	template <typename Vec>
	bool vector_compare_ptr(const Vec& lhs, const Vec& rhs)
	{
		if(lhs.size() != rhs.size())
			return false;
		typename Vec::const_iterator lhsIt = lhs.begin();
		typename Vec::const_iterator rhsIt = rhs.begin();
		for(std::size_t i=0; i<lhs.size(); ++i)
		{
			if(*(*lhsIt) != *(*rhsIt))
				return false;
			++lhsIt;
			++rhsIt;
		}
		return true;
	}
}


namespace CppFW
{
	CVMatTree::~CVMatTree()
	{
		delete mat;
	}

	void CVMatTree::clear()
	{
		delete mat;
		mat = nullptr;
		nodeDir.clear();

		internalType = Type::Undef;
	}



	CVMatTree& CVMatTree::getDirNode(const std::string& name)
	{
		if(internalType == Type::Undef)
			internalType = Type::Dir;
		if(internalType != Type::Dir)
			throw std::domain_error("wrong type");
		return getAndInsert<CVMatTree, std::string, NodeDir>(name, nodeDir);
	}

	const CVMatTree::NodeDir& CVMatTree::getNodeDir() const
	{
		if(internalType != Type::Dir)
			throw std::domain_error("wrong type");
		return nodeDir;
	}


	CVMatTree& CVMatTree::getListNode(std::size_t index)
	{
		if(internalType != Type::List)
			throw std::domain_error("wrong type");
		return *(nodeList.at(index));
	}

	const CVMatTree::NodeList& CVMatTree::getNodeList() const
	{
		if(internalType != Type::Dir)
			throw std::domain_error("wrong type");
		return nodeList;

	}

	CVMatTree& CVMatTree::newListNode()
	{
		if(internalType == Type::Undef)
			internalType = Type::List;
		if(internalType != Type::List)
			throw std::domain_error("wrong type");
		CVMatTree* newNode = new CVMatTree;
		try
		{
			nodeList.push_back(newNode);
		}
		catch(...)
		{
			delete newNode;
			throw;
		}
		return *newNode;
	}


	std::size_t CVMatTree::getNumElements() const
	{
		switch(internalType)
		{
			case Type::Dir:
				return nodeDir.size();
			case Type::List:
				return nodeList.size();
			case Type::Mat:
				return 1;
			case Type::Undef:
				return 0;
		}

		return 0;
	}


	cv::Mat& CVMatTree::getMat()
	{
		if(internalType == Type::Undef)
		{
			internalType = Type::Mat;
			mat = new cv::Mat;
		}
		if(internalType != Type::Mat)
			throw "wrong type";
		return *mat;
	}
}


bool CppFW::CVMatTree::operator==(const CppFW::CVMatTree& other) const
{
	if(internalType != other.internalType)
		return false;
	
	switch(internalType)
	{
		case Type::Undef:
			return true;
		case Type::Mat:
			return matIsEqual(*mat, *(other.mat));
		case Type::List:
			return vector_compare_ptr(nodeList, other.nodeList);
		case Type::Dir:
			return map_compare_ptr(nodeDir, other.nodeDir);
	}
	
	return false;
}

void CppFW::CVMatTree::print(std::ostream& stream) const
{
	print(stream, 0);
}

void CppFW::CVMatTree::print(std::ostream& stream, int deept) const
{
	switch(internalType)
	{
		case Type::Undef:
			stream << "<>";
			break;
		case Type::Mat:
			stream << "Mat " << mat->rows << " x " << mat->cols;
			break;
		case Type::List:
			stream << "{\n";
			for(const CVMatTree* node : nodeList)
			{
				for(int i=-1; i<deept; ++i) stream << "  ";
				node->print(stream, deept+1);
			}
			for(int i=0; i<deept; ++i) stream << "  ";
			stream << '}';
			break;
		case Type::Dir:
			stream << "[\n";
			for(const NodePair& pair : nodeDir)
			{
				for(int i=-1; i<deept; ++i) stream << "  ";
				stream << pair.first << " : ";
				pair.second->print(stream, deept+1);
			}
			for(int i=0; i<deept; ++i) stream << "  ";
			stream << ']';
			break;
	}
	stream << '\n';
}

