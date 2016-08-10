#pragma once

#include <vector>
#include <map>
#include <string>

namespace cv { class Mat; }


namespace CppFW
{

	class CVMatTree
	{
	public:
		enum class Type { Undef, Dir, List, Mat };

		typedef std::pair<const std::string, CVMatTree*> NodePair;
		typedef std::map<std::string, CVMatTree*>        NodeDir ;
		typedef std::vector<CVMatTree*>                  NodeList;

		CVMatTree()            = default;
		CVMatTree(CVMatTree&&) = default;
		~CVMatTree();

		Type type() const                                        { return internalType; }
		void clear();

		CVMatTree& getDirNode(const std::string& name);
		const NodeDir& getNodeDir() const;

		CVMatTree& getListNode(std::size_t index);
		const NodeList& getNodeList() const;
		CVMatTree& newListNode();

		std::size_t getNumElements() const;

		cv::Mat& getMat();
		const cv::Mat& getMat() const;


	private:
		CVMatTree(const CVMatTree&)            = delete;
		CVMatTree& operator=(const CVMatTree&) = delete;

		Type                                 internalType = Type::Undef;
		cv::Mat*                             mat          = nullptr;
		NodeDir                              nodeDir;
		NodeList                             nodeList;
	};


}