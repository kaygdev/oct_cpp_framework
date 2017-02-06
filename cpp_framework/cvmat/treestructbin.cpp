#include "treestructbin.h"
#include "cvmattreestruct.h"

#include <cassert>

#include <fstream>
#include <string>

#include <opencv/cv.h>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

namespace bfs = boost::filesystem;

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
			return value;
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

	bool CVMatTreeStructBin::writeBin(const std::string& filename, const cv::Mat& mat)
	{
		std::ofstream stream(filename, std::ios::binary | std::ios::out);
		if(!stream.good())
			return false;

		CVMatTree tree;
		tree.getMat() = mat;

		return writeBin(stream, tree);
	}


	CVMatTree CVMatTreeStructBin::readBin(std::istream& stream)
	{
		CVMatTreeStructBin reader(stream);
		CVMatTree tree;

		if(reader.readHeader())
		{
			reader.handleNodeRead(tree);
		}

		return tree;
	}

	CVMatTree CVMatTreeStructBin::readBin(const std::string& filename)
	{
		std::ifstream stream(filename, std::ios::binary | std::ios::in);
		if(!stream.good())
			return CVMatTree();

		return readBin(stream);
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
			case CVMatTree::Type::String:
				writeString(node.getString());
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
			case CVMatTree::Type::String:
				return readString(node.getString());
		}
		return false;
	}

	bool CVMatTreeStructBin::readString(std::string& str)
	{
		str = readBinSting(istream);
		return true;
	}


	void CVMatTreeStructBin::writeString(const std::string& string)
	{
		writeBin2Stream(ostream, string);
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
		bfs::path file(filename);

		const std::string basename = file.stem().generic_string();

		std::ofstream stream(filename, std::ios::out);

		stream << "function [ s ] = " << basename << "(filename)\n";

		stream << "% [ s ] = " << basename << "(filename)\n\
% read a bin file writen by C++ class CVMatTreeStructBin\n\
% Input  : a bin file\n\
% Output : a data structure contains strings, scalare, 2D matrics, cellarrays, structures\n\n\
% this file is generated by CVMatTreeStructBin::writeMatlabReadCode\n";


		stream << "\tfileID = fopen(filename);\n";
		stream << "\tif fileID == -1\n";
		stream << "\t	error(['cant open file ' filename]);\n";
		stream << "\tend\n\n";

		stream << "\tmagic    = fread(fileID, " << sizeof(magic)-1 << ", 'char');\n";
		stream << std::string("\tif strcmp(convertChar2String(magic), '") << magic << "') == false\n";
		stream << "\t	error('not a bin file')\n";
		stream << "\tend\n";
		stream << "\tversion  = fread(fileID, 1, 'uint32');\n";
		stream << "\tassert(version == " << version << ");\n";
		stream << "\t           fread(fileID, 4, 'uint32');\n";

		stream << "\ts  = readNode(fileID);\n";
		stream << "\tfclose(fileID);\n";
		stream << "end\n\n\n";


		stream << "function [node] = readNode(fileID)\n";
		stream << "\ttype  = fread(fileID, 1, 'uint32');\n";
		stream << "\tswitch(type)\n";
		stream << "\t\tcase " << static_cast<uint32_t>(CVMatTree::Type::Dir   ) << '\n';
		stream << "\t\t\tnode = readDir(fileID);\n";
		stream << "\t\tcase " << static_cast<uint32_t>(CVMatTree::Type::Mat   ) << '\n';
		stream << "\t\t\tnode = readMat(fileID);\n";
		stream << "\t\tcase " << static_cast<uint32_t>(CVMatTree::Type::List  ) << '\n';
		stream << "\t\t\tnode = readList(fileID);\n";
		stream << "\t\tcase " << static_cast<uint32_t>(CVMatTree::Type::String) << '\n';
		stream << "\t\t\tnode = readString(fileID);\n";
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

#define MatlabSwtichType(X, Y) 	stream << "		case " << boost::lexical_cast<std::string>(cv::DataType<X>::type) << " % OpenCV type for "#X"\n\t\t\tmat = fread(fileID, [cols rows], '"#Y"=>"#Y"')';\n";
		stream << "	switch type\n";
		MatlabSwtichType(uint8_t , uint8 )
		MatlabSwtichType(uint16_t, uint16)
		MatlabSwtichType(uint32_t, uint32)
		MatlabSwtichType( int8_t , int8  )
		MatlabSwtichType( int16_t, int16 )
		MatlabSwtichType( int32_t, int32 )
		MatlabSwtichType(float   , single)
		MatlabSwtichType(double  , double)
#undef MatlabSwtichType

		stream << "\tend\n";
		stream << "end\n";
	}


	void CVMatTreeStructBin::writeMatlabWriteCode(const char* filename)
	{
		bfs::path file(filename);

		const std::string basename = file.stem().generic_string();

		std::ofstream stream(filename, std::ios::out);

		stream << "function [] = " << basename << "(filename, s)\n";

		stream << "% [] = " << basename << "(filename, s)\n\
% read a bin file writen by C++ class CVMatTreeStructBin\n\
% Input  : filename   a bin file name\n\
%        : s          a data structure contains strings, scalare, 2D matrics, cellarrays, structures\n\n\
% this file is generated by CVMatTreeStructBin::writeMatlabWriteCode\n";


		stream << "\tfileID = fopen(filename, 'w');\n";
		stream << "\tif fileID == -1\n";
		stream << "\t	error(['cant open file ' filename]);\n";
		stream << "\tend\n\n";

		stream << "\tfwrite(fileID, '" << magic << "', 'char');\n";
		stream << "\tfwrite(fileID, " << version << ", 'uint32');\n";
		stream << "\tfwrite(fileID, [0,0,0,0], 'uint32');\n";

		stream << "\twriteNode(fileID, s);\n";
		stream << "\tfclose(fileID);\n";
		stream << "end\n\n\n";


		stream << "function [] = writeNode(fileID, node)\n";
		stream << "\tif isstruct(node)\n";
		stream << "\t\tfwrite(fileID, " << static_cast<uint32_t>(CVMatTree::Type::Dir  ) << ", 'uint32');\n";
		stream << "\t\twriteDir(fileID, node);\n";
		stream << "\tend\n";

		stream << "\tif isnumeric(node)\n";
		stream << "\t\tfwrite(fileID, " << static_cast<uint32_t>(CVMatTree::Type::Mat  ) << ", 'uint32');\n";
		stream << "\t\twriteMat(fileID, node);\n";
		stream << "\tend\n";

		stream << "\tif iscell(node)\n";
		stream << "\t\tfwrite(fileID, " << static_cast<uint32_t>(CVMatTree::Type::List  ) << ", 'uint32');\n";
		stream << "\t\twriteList(fileID, node);\n";
		stream << "\tend\n";

		stream << "\tif ischar(node)\n";
		stream << "\t\tfwrite(fileID, " << static_cast<uint32_t>(CVMatTree::Type::String  ) << ", 'uint32');\n";
		stream << "\t\twriteString(fileID, node);\n";
		stream << "\tend\n";

		stream << "end\n\n";


		stream << "function [] = writeDir(fileID, node)\n";
		stream << "\tdirNames   = fieldnames(node);\n";
		stream << "\tdirLength  = length(dirNames);\n";
		stream << "\tfwrite(fileID, dirLength, 'uint32');\n";
		stream << "\tfor i=1:dirLength\n";
		stream << "\t\tname        = dirNames{i};\n";
		stream << "\t\twriteString(fileID, name);\n";
		stream << "\t\twriteNode(fileID, node.(name));\n";
		stream << "\tend\n";
		stream << "end\n\n";


		stream << "function [] = writeList(fileID, node)\n";
		stream << "\tdirLength = length(node);\n";
		stream << "\tfwrite(fileID, dirLength, 'uint32');\n";
		stream << "\tfor i=1:dirLength\n";
		stream << "\t\twriteNode(fileID, node{i});\n";
		stream << "\tend\n";
		stream << "end\n\n";


		stream << "function [] = writeString(fileID, str)\n";
		stream << "\tstringLength  = length(str);\n";
		stream << "\tfwrite(fileID, stringLength, 'uint32');\n";
		stream << "\tfwrite(fileID, str, 'char');\n";
		stream << "end\n\n";

		stream << "function [] = writeMat(fileID, mat)\n";


#define MatlabSwtichType(X, Y) 	stream << "	if isa(mat, '"#Y"')\n\t\ttype = " << boost::lexical_cast<std::string>(cv::DataType<X>::type) << "';\n\tend\n";
		MatlabSwtichType(uint8_t , uint8 )
		MatlabSwtichType(uint16_t, uint16)
		MatlabSwtichType(uint32_t, uint32)
		MatlabSwtichType( int8_t , int8  )
		MatlabSwtichType( int16_t, int16 )
		MatlabSwtichType( int32_t, int32 )
		MatlabSwtichType(float   , single)
		MatlabSwtichType(double  , double)
#undef MatlabSwtichType

		stream << "\t[rows, cols] = size(mat);\n";

		stream << "\tfwrite(fileID, type, 'uint32');\n";
		stream << "\tfwrite(fileID, 1   , 'uint32');\n";
		stream << "\tfwrite(fileID, rows, 'uint32');\n"; // transpose!
		stream << "\tfwrite(fileID, cols, 'uint32');\n";
		stream << "\tfwrite(fileID, [0,0,0,0], 'uint32');\n";

#define MatlabSwtichType(X) stream << "	if isa(mat, '"#X"')\n\t\tfwrite(fileID, mat', '"#X"')';\n\tend\n";
		MatlabSwtichType(uint8 )
		MatlabSwtichType(uint16)
		MatlabSwtichType(uint32)
		MatlabSwtichType(int8  )
		MatlabSwtichType(int16 )
		MatlabSwtichType(int32 )
		MatlabSwtichType(single)
		MatlabSwtichType(double)
#undef MatlabSwtichType

		stream << "end\n";
	}


}
