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

#include "cvmattreestruct.h"


#include <fstream>
#include <string>

#include <opencv2/opencv.hpp>

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
		if(mat1.cols != mat2.cols || mat1.rows != mat2.rows || mat1.dims != mat2.dims || mat1.type() != mat2.type())
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
		for(CVMatTree* obj : nodeList)
			delete obj;
		for(NodePair& pair : nodeDir)
			delete pair.second;
	}

	void CVMatTree::clear()
	{
		delete mat;
		mat = nullptr;

		for(CVMatTree* obj : nodeList)
			delete obj;
		nodeList.clear();
		for(NodePair& pair : nodeDir)
			delete pair.second;
		nodeDir.clear();

		str.clear();

		internalType = Type::Undef;
	}



	CVMatTree& CVMatTree::getDirNode(const std::string& name)
	{
		if(internalType == Type::Undef)
			internalType = Type::Dir;
		if(internalType != Type::Dir)
			throw WrongType("CVMatTree::getDirNode()");
		return getAndInsert<CVMatTree, std::string, NodeDir>(name, nodeDir);
	}

	const CppFW::CVMatTree& CVMatTree::getDirNode(const std::string& name) const
	{
		if(internalType != Type::Dir)
			throw WrongType("CVMatTree::getDirNode() const");

		const NodeDir::const_iterator it = nodeDir.find(name);
		if(it == nodeDir.end())
			throw std::out_of_range("CVMatTree::getDirNode() const: node not found");

		return *(it->second);
	}


	const CppFW::CVMatTree* CVMatTree::getDirNodeOpt(const char* name) const
	{
		if(internalType != Type::Dir)
			return nullptr;

		NodeDir::const_iterator it = nodeDir.find(name);
		if(it == nodeDir.end())
			return nullptr;

		return it->second;
	}


	const CVMatTree::NodeDir& CVMatTree::getNodeDir() const
	{
		if(internalType != Type::Dir)
			throw WrongType("CVMatTree::getNodeDir() const");
		return nodeDir;
	}


	CVMatTree& CVMatTree::getListNode(std::size_t index)
	{
		if(internalType != Type::List)
			throw WrongType("VMatTree::getListNode()");
		return *(nodeList.at(index));
	}

	const CVMatTree::NodeList& CVMatTree::getNodeList() const
	{
		if(internalType != Type::List)
			throw WrongType("CVMatTree::getNodeList() const");
		return nodeList;

	}

	CVMatTree& CVMatTree::newListNode()
	{
		if(internalType == Type::Undef)
			internalType = Type::List;
		if(internalType != Type::List)
			throw WrongType("CVMatTree::newListNode()");
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
			case Type::String:
				return 1;
			case Type::Undef:
				return 0;
		}

		return 0;
	}

	const cv::Mat& CVMatTree::getMat() const
	{
		if(internalType != Type::Mat)
			throw WrongType("CVMatTree::getMat() const");
		return *mat;
	}

	const cv::Mat* CVMatTree::getMatOpt() const
	{
		if(internalType != Type::Mat)
			return nullptr;
		return mat;
	}

	cv::Mat& CVMatTree::getMat()
	{
		if(internalType == Type::Undef)
		{
			internalType = Type::Mat;
			mat = new cv::Mat;
		}
		if(internalType != Type::Mat)
			throw WrongType("CVMatTree::getMat()");
		return *mat;
	}

	const std::string& CVMatTree::getString() const
	{
		if(internalType != Type::String)
			throw WrongType("CVMatTree::getString() const");
		return str;
	}

	std::string& CVMatTree::getString()
	{
		if(internalType == Type::Undef)
			internalType = Type::String;
		if(internalType != Type::String)
			throw WrongType("CVMatTree::getString()");
		return str;
	}

	const std::string& CVMatTree::getStringOrEmpty() const
	{
		return str; // str is empty when other datatype is set
	}



	bool CVMatTree::operator==(const CppFW::CVMatTree& other) const
	{
		if(internalType != other.internalType)
			return false;

		switch(internalType)
		{
			case Type::Undef:
				return true;
			case Type::String:
				return str == other.str;
			case Type::Mat:
				return matIsEqual(*mat, *(other.mat));
			case Type::List:
				return vector_compare_ptr(nodeList, other.nodeList);
			case Type::Dir:
				return map_compare_ptr(nodeDir, other.nodeDir);
		}

		return false;
	}

	CVMatTree::CVMatTree(CVMatTree&& other)
	{
		std::swap(mat         , other.mat         );
		std::swap(internalType, other.internalType);

		nodeDir .swap(other.nodeDir );
		nodeList.swap(other.nodeList);
	}


	void CVMatTree::print(std::ostream& stream) const
	{
		print(stream, 0);
	}

	void CVMatTree::print(std::ostream& stream, int deept) const
	{
		switch(internalType)
		{
			case Type::Undef:
				stream << "<>";
				break;
			case Type::String:
				stream << "Str: " << str;
				break;
			case Type::Mat:
				stream << "Mat " << mat->rows << " x " << mat->cols << " | type: " << mat->type() << " | depth: " << mat->depth() << " | channels: " << mat->channels() << '\n';
				stream << *mat;
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

}
