#pragma once

#include "cvmattreestruct.h"
#include"cvmattreestructextra.h"

#include <opencv/cv.h>

namespace CppFW
{
	class SetToCVMatTree
	{
		CVMatTree& tree;
	public:
		SetToCVMatTree(CVMatTree& tree) : tree(tree) {}

		template<typename T>
		void operator()(const std::string& name, const T& value)
		{
			CVMatTree& node = tree.getDirNode(name);
			CVMatTreeExtra::setCvScalar(node, value);
		}

		template<typename T>
		void operator()(const std::string& name, const std::vector<T>& value)
		{
			CVMatTree& node = tree.getDirNode(name);
			node.getMat() = CVMatTreeExtra::convertVector2Mat<T>(value);
		}

		void operator()(const std::string& name, const std::string& value)
		{
			if(value.empty())
				return;
			CVMatTree& node = tree.getDirNode(name);
			node.getString() = value;
		}

		SetToCVMatTree subSet(const std::string& name)
		{
			return SetToCVMatTree(tree.getDirNode(name));
		}
	};

	class GetFromCVMatTree
	{
		const CVMatTree* tree;

		template<typename T>
		void setValue(const CVMatTree& t, T& value)
		{
			value = CVMatTreeExtra::getCvScalar(&t, value);
		}

		template<typename T>
		void setValue(const CVMatTree& t, std::vector<T>& value)
		{
			value = CVMatTreeExtra::getCvVector<T>(&t);
		}

		void setValue(const CVMatTree& t, std::string& value)
		{
			value = t.getString();
		}
	public:
		GetFromCVMatTree(const CVMatTree* tree) : tree( tree) {}
		GetFromCVMatTree(const CVMatTree& tree) : tree(&tree) {}

		template<typename T>
		void operator()(const std::string& name, T& value)
		{
			if(tree)
			{
				const CVMatTree* node = tree->getDirNodeOpt(name.c_str());
				if(node)
					setValue(*node, value);
			}
		}


		GetFromCVMatTree subSet(const std::string& name)
		{
			if(tree)
				return GetFromCVMatTree(tree->getDirNodeOpt(name.c_str()));
			return GetFromCVMatTree(nullptr);
		}

	};
}