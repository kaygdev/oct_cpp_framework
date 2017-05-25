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
