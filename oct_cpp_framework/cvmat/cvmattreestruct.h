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

#include <vector>
#include <map>
#include <string>
#include <stdexcept>

namespace cv { class Mat; }


namespace CppFW
{

	class CVMatTree
	{
	public:
		class WrongType : public std::domain_error
		{
		public:
			WrongType(const char* what_arg) : domain_error(what_arg) {}
		};

		enum class Type { Undef, Dir, List, Mat, String };

		typedef std::pair<const std::string, CVMatTree*> NodePair;
		typedef std::map<std::string, CVMatTree*>        NodeDir ;
		typedef std::vector<CVMatTree*>                  NodeList;

		CVMatTree()            = default;
		CVMatTree(CVMatTree&&);
		~CVMatTree();

		Type type() const                                        { return internalType; }
		void clear();

		      CVMatTree& getDirNode(const std::string& name);
		const CVMatTree& getDirNode(const std::string& name) const;
		const CVMatTree* getDirNodeOpt(const char* name) const;
		const NodeDir  & getNodeDir() const;

		CVMatTree& getListNode(std::size_t index);
		const NodeList& getNodeList() const;
		CVMatTree& newListNode();

		std::size_t getNumElements() const;

		      cv::Mat& getMat();
		const cv::Mat& getMat() const;
		const cv::Mat* getMatOpt() const;

		      std::string& getString();
		const std::string& getString() const;
		const std::string& getStringOrEmpty() const;
		
		bool operator==(const CVMatTree& other) const;
		bool operator!=(const CVMatTree& other) const            { return !operator==(other); }

		void print(std::ostream& stream) const;
	private:
		CVMatTree(const CVMatTree&)            = delete;
		CVMatTree& operator=(const CVMatTree&) = delete;

		Type                                 internalType = Type::Undef;
		cv::Mat*                             mat          = nullptr;
		NodeDir                              nodeDir;
		NodeList                             nodeList;
		std::string                          str;
		
		void print(std::ostream& stream, int deept) const;
	};


}

inline std::ostream& operator<<(std::ostream& stream, const CppFW::CVMatTree& obj) { obj.print(stream); return stream; }
