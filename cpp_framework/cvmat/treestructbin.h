#pragma once

#include <fstream>


namespace cv { class Mat; }



namespace CppFW
{
	class CVMatTree;

	class CVMatTreeStructBin
	{
		std::ofstream stream;

		void writeHeader();
		void writeMatP(const cv::Mat& mat);
		void writeDir (const CVMatTree& node);
		void writeList(const CVMatTree& node);


		void handleNode(const CVMatTree& node);
	public:
		bool writeBin(const std::string& filename, const CVMatTree& tree);
		CVMatTree readBin(const std::string& filename);

		void writeMatlabReadCode(const char* filename);
	};



}
