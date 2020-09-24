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

#include "cvmattreestruct.h"
#include"cvmattreestructextra.h"

#include<opencv2/opencv.hpp>

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
