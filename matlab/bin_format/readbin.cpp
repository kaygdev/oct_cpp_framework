#include"mex.h"


#include "../helper/matlab_helper.h"
#include "../helper/matlab_types.h"

#include<fstream>


template<typename T>
inline void readBinStream(std::istream& stream, T* value, std::size_t num = 1)
{
	stream.read(reinterpret_cast<char*>(value), sizeof(T)*num);
	if(!stream.good())
		mexErrMsgIdAndTxt("MexCPP", "error while reading: stream not good");
}


inline void readString(std::istream& stream, std::string& value, std::size_t length)
{
	value.resize(length);
	readBinStream<char>(stream, const_cast<std::string::value_type*>(value.data()), static_cast<std::size_t>(length)); // TODO: remove const_cast in C++17
}


inline void readBinStream(std::istream& stream, std::string& value)
{
	uint32_t length;
	readBinStream<uint32_t>(stream, &length);
	readString(stream, value, length);
}

inline std::string readBinSting(std::istream& stream)
{
	std::string value;
	readBinStream(stream, value);
	return value;
}

template<typename T>
inline T readBinStream(std::istream& stream)
{
	T value;
	readBinStream(stream, &value, 1);
	return value;
}



mxArray* readDir   (std::istream& stream);
mxArray* readList  (std::istream& stream);
mxArray* readMat   (std::istream& stream);
mxArray* readString(std::istream& stream);


mxArray* readNode(std::istream& stream)
{
	const uint32_t type = readBinStream<uint32_t>(stream);
	switch(type)
	{
		case 0:
			// node not written because unhandled type
			return nullptr;
		case 1: return readDir   (stream);
		case 2: return readList  (stream);
		case 3: return readMat   (stream);
		case 4: return readString(stream);
		default:
			mexWarnMsgIdAndTxt("MexParsing", "unknown node type %d\n", type);
			return nullptr;
	}
}

mxArray* readDir(std::istream& stream)
{
	struct NameAndMxArray
	{
		mxArray* array;
		std::string name;
	};

	// TODO: remove leak when exception

	// read dir elements
	const uint32_t dirLength = readBinStream<uint32_t>(stream);
	std::vector<NameAndMxArray> nameAndArray;
	nameAndArray.reserve(dirLength);
	for(uint32_t i = 0; i < dirLength; ++i)
	{
		NameAndMxArray data;
		data.name  = readBinSting(stream);
		data.array = readNode(stream);

		nameAndArray.push_back(data);
	}

	// extract names
	std::vector<const char*> namePtrList;
	namePtrList .reserve(dirLength);
	for(const NameAndMxArray& item : nameAndArray)
		namePtrList .push_back(item.name.data());

	// fill mx structure
	mxArray* mxDir = mxCreateStructMatrix(1, 1, static_cast<int>(namePtrList.size()), namePtrList.data());
	for(std::size_t i = 0; i<nameAndArray.size(); ++i)
		mxSetFieldByNumber(mxDir, 0, static_cast<int>(i), nameAndArray[i].array);

	return mxDir;
}

mxArray* readList(std::istream& stream)
{
	const uint32_t dirLength = readBinStream<uint32_t>(stream);

	mxArray* mxarr = mxCreateCellMatrix(1, dirLength);
	for(uint32_t i = 0; i<dirLength; ++i)
		mxSetCell(mxarr, i, readNode(stream));

	return mxarr;
}

mxArray* readString(std::istream& stream)
{
	std::string str = readBinSting(stream);
	return mxCreateString(str.c_str());
}


template<typename T>
mxArray* readMat(std::istream& stream, uint32_t rows, uint32_t cols)
{
	mxArray* matlabMat = mxCreateNumericMatrix(cols, rows, MatlabType<T>::classID, mxREAL);

	if(!matlabMat)
		return nullptr;

	T* matlabPtr = reinterpret_cast<T*>(mxGetPr(matlabMat));
	readBinStream(stream, matlabPtr, rows*cols);

	transposeMatlabMatrix<T>(matlabMat);
	return matlabMat;
}


mxArray* readMat(std::istream& stream)
{
	const uint32_t type     = readBinStream<uint32_t>(stream);
	/*const uint32_t channels =*/ readBinStream<uint32_t>(stream);
	const uint32_t rows     = readBinStream<uint32_t>(stream);
	const uint32_t cols     = readBinStream<uint32_t>(stream);
	                          readBinStream<uint32_t>(stream);
	                          readBinStream<uint32_t>(stream);
	                          readBinStream<uint32_t>(stream);
	                          readBinStream<uint32_t>(stream);

	switch(type)
	{
		case 0: return readMat<uint8_t >(stream, rows, cols);
		case 2: return readMat<uint16_t>(stream, rows, cols);
		case 7: return readMat<uint32_t>(stream, rows, cols);
		case 1: return readMat<int8_t  >(stream, rows, cols);
		case 3: return readMat<int16_t >(stream, rows, cols);
		case 4: return readMat<int32_t >(stream, rows, cols);
		case 5: return readMat<float   >(stream, rows, cols);
		case 6: return readMat<double  >(stream, rows, cols);
		default:
			mexWarnMsgIdAndTxt("MexParsing", "unknown matrix data type %d\n", type);
			return nullptr;
	}
}

void mexFunction(int            nlhs  ,
                 mxArray*       plhs[],
                 int            nrhs  ,
                 const mxArray* prhs[]
                 )
{

	if(nrhs != 1)
	{
		mexErrMsgIdAndTxt("MATLAB:mexcpp:nargin", "MEXCPP requires 1 input arguments");
		return;
	}
	if(nlhs > 1)
	{
		mexErrMsgIdAndTxt("MATLAB:mexcpp:nargout", "MEXCPP requires only one output argument.");
		return;
	}

	if(!mxIsChar(prhs[0]))
	{
		mexErrMsgIdAndTxt("MATLAB:mexcpp:nargin", "requires filename");
		return;
	}
	// Filename
	mxChar* fnPtr = (mxChar*) mxGetPr(prhs[0]);
	std::size_t fnLength = mxGetN(prhs[0]);
	std::string filename(fnPtr, fnPtr+fnLength);


	try
	{
		std::ifstream stream(filename, std::ios::binary);
		if(!stream)
		{
			mexErrMsgIdAndTxt("MATLAB:mexcpp:nargin", "cant open file: %s", filename.c_str());
			return;
		}

		std::string binFormatIdentifier;
		readString(stream, binFormatIdentifier, 8);
		if(binFormatIdentifier != "CVMatBin")
		{
			mexErrMsgIdAndTxt("MATLAB:mexcpp:nargin", "no bin file: %s", binFormatIdentifier.c_str());
			return;
		}

	    uint32_t version = readBinStream<uint32_t>(stream);
		if(version != 1)
		{
			mexErrMsgIdAndTxt("MATLAB:mexcpp:nargin", "wrong bin format version %d", version);
			return;
		}

	                       readBinStream<uint32_t>(stream);
	                       readBinStream<uint32_t>(stream);
	                       readBinStream<uint32_t>(stream);
	                       readBinStream<uint32_t>(stream);

		plhs[0] = readNode(stream);
	}
	catch(...)
	{
		mexErrMsgIdAndTxt("MATLAB:mexcpp:nargin", "error while reading file");
	}


}
