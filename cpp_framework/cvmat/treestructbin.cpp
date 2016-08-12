#include "treestructbin.h"
#include "cvmattreestruct.h"

#include <cassert>

#include <fstream>
#include <string>

#include <opencv/cv.h>

#include <boost/lexical_cast.hpp>

namespace CppFW
{

	namespace
	{
		const uint32_t version = 1;
		const char     magic[] = "CVMatBin";


		template<typename T>
		inline void writeBin2Stream(std::ostream* stream, const T& value, std::size_t num = 1)
		{
			stream->write(reinterpret_cast<const char*>(&value), sizeof(T)*num);
		}

		inline void writeBin2Stream(std::ostream* stream, const std::string& value)
		{
			writeBin2Stream<uint32_t>(stream, value.size());
			stream->write(reinterpret_cast<const char*>(value.c_str()), value.size());
		}

		template<typename T>
		inline void writeMatBin(std::ostream* stream, const cv::Mat& mat)
		{
			int channels = mat.channels();
			for(int i = 0; i < mat.rows; i++)
			{
				const T* mi = mat.ptr<T>(i);
				for(int j = 0; j < mat.cols; j++)
				{
					for(int c = 0; c < channels; ++c)
					{
						writeBin2Stream(stream, *mi);
						++mi;
					}
				}
			}
		}

		template<typename T>
		inline void readBinStream(std::istream* stream, T& value, std::size_t num = 1)
		{
			stream->read(reinterpret_cast<char*>(&value), sizeof(T)*num);
		}

		inline std::string readBinSting(std::istream* stream)
		{
			std::string value;
			uint32_t length;
			readBinStream<uint32_t>(stream, length);
			char* buffer = new char[length];
			readBinStream<char>(stream, *buffer, static_cast<std::size_t>(length));
			value.assign(buffer, buffer+length);
			return std::move(value);
		}

		template<typename T>
		inline T readBinStream(std::istream* stream)
		{
			T value;
			stream->read(reinterpret_cast<char*>(&value), sizeof(T));
			return value;
		}

		inline void readBinStream(std::istream* stream, std::string& value)
		{
			uint32_t length;
			readBinStream<uint32_t>(stream, length);
			char* buffer = new char[length];
			readBinStream<char>(stream, *buffer, static_cast<std::size_t>(length));
			value.assign(buffer, buffer+length);
		}

		template<typename T>
		inline void readMatBin(std::istream* stream, cv::Mat& mat)
		{
			int channels = mat.channels();
			for(int i = 0; i < mat.rows; i++)
			{
				T* mi = mat.ptr<T>(i);
				for(int j = 0; j < mat.cols; j++)
				{
					for(int c = 0; c < channels; ++c)
					{
						readBinStream(stream, *mi);
						++mi;
					}
				}
			}
		}

	}


	bool CVMatTreeStructBin::writeBin(const std::string& filename, const CVMatTree& tree)
	{
		std::ofstream stream(filename, std::ios::binary | std::ios::out);
		if(!stream.good())
			return false;
		
		return writeBin(stream, tree);
	}
	
	bool CVMatTreeStructBin::writeBin(std::ostream& stream, const CVMatTree& tree)
	{
		CVMatTreeStructBin writer(stream);
		writer.writeHeader();
		writer.handleNodeWrite(tree);

		return true;
	}

	CVMatTree CVMatTreeStructBin::readBin(std::istream& stream)
	{
		CVMatTreeStructBin reader(stream);
		CVMatTree tree;

		if(reader.readHeader())
		{
			reader.handleNodeRead(tree);
		}

		return std::move(tree);
	}

	CVMatTree CVMatTreeStructBin::readBin(const std::__cxx11::string& filename)
	{
		std::ifstream stream(filename, std::ios::binary | std::ios::in);
		if(!stream.good())
			return CVMatTree();

		return std::move(readBin(stream));
	}




	void CVMatTreeStructBin::writeDir(const CVMatTree& node)
	{
		const CVMatTree::NodeDir& nodes = node.getNodeDir();

		writeBin2Stream<uint32_t>(ostream, static_cast<uint32_t>(nodes.size()));
		for(const CVMatTree::NodePair& pair : nodes)
		{
			const std::string& name  = pair.first;
			const CVMatTree* subNode = pair.second;

			writeBin2Stream(ostream, name);
			handleNodeWrite(*subNode);
		}
	}


	bool CVMatTreeStructBin::readDir(CVMatTree& node)
	{
		bool ret = true;
		uint32_t dirLength = readBinStream<uint32_t>(istream);
		for(uint32_t i=0; i<dirLength; ++i)
		{
			std::string name = readBinSting(istream);

			ret &= handleNodeRead(node.getDirNode(name));
		}
		return ret;
	}


	void CVMatTreeStructBin::writeList(const CVMatTree& node)
	{
		const CVMatTree::NodeList& nodes = node.getNodeList();

		writeBin2Stream<uint32_t>(ostream, static_cast<uint32_t>(nodes.size()));
		for(const CVMatTree* subNode : nodes)
		{
			handleNodeWrite(*subNode);
		}
	}

	bool CVMatTreeStructBin::readList(CVMatTree& node)
	{
		bool ret = true;
		uint32_t listLength = readBinStream<uint32_t>(istream);
		for(uint32_t i=0; i<listLength; ++i)
		{
			ret &= handleNodeRead(node.newListNode());
		}
		return ret;

	}



	void CVMatTreeStructBin::handleNodeWrite(const CVMatTree& node)
	{
		writeBin2Stream<uint32_t>(ostream, static_cast<uint32_t>(node.type()));
		switch(node.type())
		{
			case CVMatTree::Type::Undef:
				break;
			case CVMatTree::Type::Dir:
				writeDir(node);
				break;
			case CVMatTree::Type::List:
				writeList(node);
				break;
			case CVMatTree::Type::Mat:
				writeMatP(node.getMat());
				break;
		}
	}
	

	bool CVMatTreeStructBin::handleNodeRead(CVMatTree& node)
	{
		assert(node.type() == CVMatTree::Type::Undef);
		
		uint32_t type = readBinStream<uint32_t>(istream);
		switch(static_cast<CVMatTree::Type>(type))
		{
			case CVMatTree::Type::Undef:
				return true;
			case CVMatTree::Type::Dir:
				return readDir(node);
			case CVMatTree::Type::List:
				return readList(node);
			case CVMatTree::Type::Mat:
				return readMatP(node.getMat());
		}
		return false;
	}



	void CVMatTreeStructBin::writeHeader()
	{
		ostream->write(magic, sizeof(magic)-1);
		writeBin2Stream<uint32_t>(ostream, version);

		writeBin2Stream<uint32_t>(ostream, 0);
		writeBin2Stream<uint32_t>(ostream, 0);
		writeBin2Stream<uint32_t>(ostream, 0);
		writeBin2Stream<uint32_t>(ostream, 0);
	}
	
	bool CVMatTreeStructBin::readHeader()
	{
		char readmagic[sizeof(magic)-1];
		istream->read(readmagic, sizeof(magic)-1);
		if(std::memcmp(magic, readmagic, sizeof(magic)-1) != 0)
			return false;

		uint32_t readedVersion = readBinStream<uint32_t>(istream);
		if(version != readedVersion)
			return false;
		
		uint32_t tmp;
		readBinStream<uint32_t>(istream, tmp);
		readBinStream<uint32_t>(istream, tmp);
		readBinStream<uint32_t>(istream, tmp);
		readBinStream<uint32_t>(istream, tmp);
		return true;
	}


	void CVMatTreeStructBin::writeMatP(const cv::Mat& mat)
	{
		writeBin2Stream<uint32_t>(ostream, mat.type());
		writeBin2Stream<uint32_t>(ostream, mat.channels());

		writeBin2Stream<uint32_t>(ostream, mat.rows);
		writeBin2Stream<uint32_t>(ostream, mat.cols);

		writeBin2Stream<uint32_t>(ostream, 0);
		writeBin2Stream<uint32_t>(ostream, 0);
		writeBin2Stream<uint32_t>(ostream, 0);
		writeBin2Stream<uint32_t>(ostream, 0);

	#define HandleType(X) case cv::DataType<X>::type: writeMatBin<X>(ostream, mat); break;
		switch(mat.type())
		{
			HandleType(uint8_t)
			HandleType(uint16_t)
			HandleType(uint32_t)
			HandleType(int8_t)
			HandleType(int16_t)
			HandleType(int32_t)
			HandleType(float)
			HandleType(double)
			default:
				std::cerr << "writeMatP: Unhandled Mat-Type";
		}
	#undef HandleType
	}


	bool CVMatTreeStructBin::readMatP(cv::Mat& mat)
	{
		uint32_t type     = readBinStream<uint32_t>(istream);
		uint32_t channels = readBinStream<uint32_t>(istream);

		uint32_t rows     = readBinStream<uint32_t>(istream);
		uint32_t cols     = readBinStream<uint32_t>(istream);

		readBinStream<uint32_t>(istream);
		readBinStream<uint32_t>(istream);
		readBinStream<uint32_t>(istream);
		readBinStream<uint32_t>(istream);

	#define HandleType(X) case cv::DataType<X>::type: mat.create(rows, cols, CV_MAKETYPE(cv::DataType<X>::depth, channels)); readMatBin<X>(istream, mat); break;
		switch(type)
		{
			HandleType(uint8_t)
			HandleType(uint16_t)
			HandleType(uint32_t)
			HandleType(int8_t)
			HandleType(int16_t)
			HandleType(int32_t)
			HandleType(float)
			HandleType(double)
			default:
				std::cerr << "readMatP: Unhandled Mat-Type";
				return false;
		}
	#undef HandleType
		return true;
	}



	void CVMatTreeStructBin::writeMatlabReadCode(const char* filename)
	{
		const char* help = "% [Mats] = readbin(filename)\n\
% read a bin file writen by C++ class CVMatTreeStructBin\n\
% Input  : a bin file\n% Output : a cellarray with the read matrics\n\n\
% this file is generated by CVMatTreeStructBin::writeMatlabCode\n";


		std::ofstream stream(filename, std::ios::out);

		std::string matlabCode;

		stream << "function [ s ] = readbin(filename)\n";
		stream << help;
		stream << "\tfileID = fopen(filename);\n";
		stream << "\tif fileID == -1\n";
		stream << "\t	error(['cant open file ' filename]);\n";
		stream << "\tend\n\n";

		stream << "\tmagic    = fread(fileID, " << boost::lexical_cast<std::string>(sizeof(magic)-1) << ", 'char');\n";
		stream << std::string("\tif strcmp(convertChar2String(magic), '") << magic << "') == false\n";
		stream << "\t	error('not a bin file')\n";
		stream << "\tend\n";
		stream << "\tversion  = fread(fileID, 1, 'uint32');\n";
		stream << "\tassert(version == 1);\n";
		stream << "\t           fread(fileID, 4, 'uint32');\n";

		stream << "\ts  = readNode(fileID);\n";
		stream << "\tfclose(fileID);\n";
		stream << "end\n\n\n";


		stream << "function [node] = readNode(fileID)\n";
		stream << "\ttype  = fread(fileID, 1, 'uint32');\n";
		stream << "\tswitch(type)\n";
		stream << "\t\tcase " << static_cast<uint32_t>(CVMatTree::Type::Dir ) << '\n';
		stream << "\t\t\tnode = readDir(fileID);\n";
		stream << "\t\tcase " << static_cast<uint32_t>(CVMatTree::Type::Mat ) << '\n';
		stream << "\t\t\tnode = readMat(fileID);\n";
		stream << "\t\tcase " << static_cast<uint32_t>(CVMatTree::Type::List) << '\n';
		stream << "\t\t\tnode = readList(fileID);\n";
		stream << "\tend\n";
		stream << "end\n\n";


		stream << "function [node] = readDir(fileID)\n";
		stream << "\tdirLength  = fread(fileID, 1, 'uint32');\n";
		stream << "\tfor i=1:dirLength\n";
		stream << "\t\tname        = readString(fileID);\n";
		stream << "\t\tnode.(name) = readNode(fileID);\n";
		stream << "\tend\n";
		stream << "end\n\n";


		stream << "function [node] = readList(fileID)\n";
		stream << "\tnode = {};\n";
		stream << "\tdirLength  = fread(fileID, 1, 'uint32');\n";
		stream << "\tfor i=1:dirLength\n";
		stream << "\t\tnode{i} = readNode(fileID);\n";
		stream << "\tend\n";
		stream << "end\n\n";


		stream << "function [string] = readString(fileID)\n";
		stream << "\tstringLength  = fread(fileID, 1, 'uint32');\n";
		stream << "\tstr    = fread(fileID, stringLength, 'char');\n";
		stream << "\tstring = convertChar2String(str);\n";
		stream << "end\n\n";


		stream << "function [string] = convertChar2String(str)\n";
		stream << "\tstring = sprintf('%s', str);\n";
		stream << "end\n\n";


		stream << "function [mat] = readMat(fileID)\n";
		stream << "\ttype     = fread(fileID, 1, 'uint32');\n";
		stream << "\tchannels = fread(fileID, 1, 'uint32');\n";
		stream << "\trows     = fread(fileID, 1, 'uint32');\n";
		stream << "\tcols     = fread(fileID, 1, 'uint32');\n";
		stream << "\t           fread(fileID, 4, 'uint32');\n";

	#define MatlabSwtichType(X, Y) 	stream << "		case " << boost::lexical_cast<std::string>(cv::DataType<X>::type) << " % OpenCV type for "#X"\n\t\tmat = fread(fileID, [cols rows], '"#Y"')';\n";
		stream << "	switch type\n";
		MatlabSwtichType(uint8_t , uint8 )
		MatlabSwtichType(uint16_t, uint16)
		MatlabSwtichType(uint32_t, uint32)
		MatlabSwtichType( int8_t , int8  )
		MatlabSwtichType( int16_t, int16 )
		MatlabSwtichType( int32_t, int32 )
		MatlabSwtichType( float  , single)
		MatlabSwtichType( double , double)

		stream << "\tend\n";
		stream << "end\n";
	}



}
