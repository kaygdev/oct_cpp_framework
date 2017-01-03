#pragma once

#include <iostream>


namespace cv { class Mat; }



namespace CppFW
{
	class CVMatTree;

	class CVMatTreeStructBin
	{
		std::ostream* ostream = nullptr;
		std::istream* istream = nullptr;

		// writer functions
		void writeHeader();
		void writeMatP  (const cv::Mat& mat);
		void writeDir   (const CVMatTree& node);
		void writeList  (const CVMatTree& node);
		void writeString(const std::string& str);

		void handleNodeWrite(const CVMatTree& node);
		
		// reader functions
		bool readHeader();
		bool readMatP  (cv::Mat& mat);
		bool readDir   (CVMatTree& node);
		bool readList  (CVMatTree& node);
		bool readString(std::string& str);

		bool handleNodeRead(CVMatTree& node);

		
		CVMatTreeStructBin(std::ostream& stream) : ostream(&stream) {}
		CVMatTreeStructBin(std::istream& stream) : istream(&stream) {}
		
	public:
		static bool writeBin(      std::ostream& stream , const CVMatTree& tree);
		static bool writeBin(const std::string& filename, const CVMatTree& tree);
		static bool writeBin(const std::string& filename, const cv::Mat& mat);
		
		static CVMatTree readBin(const std::string& filename);
		static CVMatTree readBin(std::istream& stream);

		static void writeMatlabReadCode (const char* filename);
		static void writeMatlabWriteCode(const char* filename);
	};

}
