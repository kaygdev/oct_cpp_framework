#include "cvmattreestruct.h"


#include <fstream>
#include <string>

#include <opencv/cv.h>

#include <boost/lexical_cast.hpp>


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
			throw "wrong type";
		return getAndInsert<CVMatTree, std::string, NodeDir>(name, nodeDir);
	}

	const CVMatTree::NodeDir& CVMatTree::getNodeDir() const
	{
		if(internalType != Type::Dir)
			throw "wrong type";
		return nodeDir;
	}


	CVMatTree& CVMatTree::getListNode(std::size_t index)
	{
		if(internalType == Type::Undef)
			internalType = Type::List;
		if(internalType != Type::Dir)
			throw "wrong type";
		return *(nodeList.at(index));
	}

	const CVMatTree::NodeList& CVMatTree::getNodeList() const
	{
		if(internalType != Type::Dir)
			throw "wrong type";
		return nodeList;

	}

	CVMatTree& CVMatTree::newListNode()
	{
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
