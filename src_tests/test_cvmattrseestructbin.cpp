#include <cvmat/cvmattreestruct.h>
#include <cvmat/treestructbin.h>

#include <boost/test/unit_test.hpp>

#include <opencv2/opencv.hpp>
#include <sstream>

namespace
{
	template<typename T>
	void createMat(cv::Mat& mat, int rows, int cols)
	{
		mat.create(rows, cols, cv::DataType<T>::type);

		int count = 0;
		for(int r = 0; r<rows; ++r)
		{
			for(int c = 0; c<cols; ++c)
			{
				mat.at<T>(r, c) = count;
				++count;
			}
		}

	}
}



BOOST_AUTO_TEST_SUITE(CVMatTreeBin)

	BOOST_AUTO_TEST_CASE( CVMatTreeBin_rw_empty )
	{
		CppFW::CVMatTree tree1;

		std::stringstream sstream;
		CppFW::CVMatTreeStructBin::writeBin(sstream, tree1);

		CppFW::CVMatTree tree2 = CppFW::CVMatTreeStructBin::readBin(sstream);

		BOOST_CHECK( tree1 == tree2 );
	}

	BOOST_AUTO_TEST_CASE( CVMatTreeBin_rw_singel_empty_map )
	{
		CppFW::CVMatTree tree1;

		tree1.getMat();

		std::stringstream sstream;
		CppFW::CVMatTreeStructBin::writeBin(sstream, tree1);

		CppFW::CVMatTree tree2 = CppFW::CVMatTreeStructBin::readBin(sstream);

		BOOST_CHECK( tree1 == tree2 );
	}

	BOOST_AUTO_TEST_CASE( CVMatTreeBin_rw_singel_map )
	{
		CppFW::CVMatTree tree1;

		createMat<double>(tree1.getMat(),4,3);

		std::stringstream sstream;
		CppFW::CVMatTreeStructBin::writeBin(sstream, tree1);

		CppFW::CVMatTree tree2 = CppFW::CVMatTreeStructBin::readBin(sstream);
		// std::cout << tree1 << std::endl << tree2 << std::endl;

		BOOST_CHECK( tree1 == tree2 );
	}


	BOOST_AUTO_TEST_CASE( CVMatTreeBin_rw_more_complex_trees )
	{
		CppFW::CVMatTree tree1;

		CppFW::CVMatTree& tree1bla  = tree1.getDirNode("bla");
		CppFW::CVMatTree& tree1blub = tree1.getDirNode("blub");

		CppFW::CVMatTree& tree1bla1 = tree1bla.newListNode();
		createMat<double>(tree1bla1.getMat(), 5 ,10);
		CppFW::CVMatTree& tree1bla2 = tree1bla.newListNode();
		createMat<int>(tree1bla2.getMat(), 5 ,5);
		CppFW::CVMatTree& tree1bla3hm = tree1bla.newListNode().getDirNode("hm");
		createMat<float>(tree1bla3hm.getMat(), 5 ,3);

		tree1blub.newListNode().getMat();


		std::stringstream sstream;
		CppFW::CVMatTreeStructBin::writeBin(sstream, tree1);


		CppFW::CVMatTree tree2 = CppFW::CVMatTreeStructBin::readBin(sstream);
		// std::cout << tree1 << std::endl << tree2 << std::endl;

		BOOST_CHECK( tree1 == tree2 );
	}

/*
	BOOST_AUTO_TEST_CASE( CVMatTreeBin_save_more_complex_trees )
	{

		CppFW::CVMatTree tree1;

		CppFW::CVMatTree& tree1bla  = tree1.getDirNode("bla");
		CppFW::CVMatTree& tree1blub = tree1.getDirNode("blub");

		CppFW::CVMatTree& tree1bla1 = tree1bla.newListNode();
		createMat<double>(tree1bla1.getMat(), 5 ,10);
		CppFW::CVMatTree& tree1bla2 = tree1bla.newListNode();
		createMat<int>(tree1bla2.getMat(), 5 ,5);
		CppFW::CVMatTree& tree1bla3hm = tree1bla.newListNode().getDirNode("hm");
		createMat<float>(tree1bla3hm.getMat(), 5 ,3);

		tree1blub.newListNode().getMat();


		std::stringstream sstream;
		CppFW::CVMatTreeStructBin::writeBin("test.bin", tree1);


		CppFW::CVMatTreeStructBin::writeMatlabReadCode("readbin.m");
		std::cout << tree1 << std::endl;
	}
	*/

BOOST_AUTO_TEST_SUITE_END()

