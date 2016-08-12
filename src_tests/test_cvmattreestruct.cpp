#include <cvmat/cvmattreestruct.h>

#include <boost/test/unit_test.hpp>
#include <opencv2/opencv.hpp>

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

BOOST_AUTO_TEST_SUITE(CVMatTree)

BOOST_AUTO_TEST_CASE( CVMatTree_undef_on_start )
{
	CppFW::CVMatTree tree;
	BOOST_CHECK( tree.type() == CppFW::CVMatTree::Type::Undef );
	BOOST_CHECK( tree.getNumElements() == 0 );
}


BOOST_AUTO_TEST_CASE( CVMatTree_becomes_mat )
{
	CppFW::CVMatTree tree;
	BOOST_CHECK_NO_THROW(tree.getMat());
	BOOST_CHECK( tree.type() == CppFW::CVMatTree::Type::Mat );
	BOOST_CHECK( tree.getNumElements() == 1 );
}


BOOST_AUTO_TEST_CASE( CVMatTree_becomes_dir )
{
	CppFW::CVMatTree tree;
	BOOST_CHECK_NO_THROW(tree.getDirNode("test"));
	BOOST_CHECK( tree.type() == CppFW::CVMatTree::Type::Dir );
	BOOST_CHECK( tree.getNumElements() == 1 );
}


BOOST_AUTO_TEST_CASE( CVMatTree_becomes_list )
{
	CppFW::CVMatTree tree;
	BOOST_CHECK_NO_THROW(tree.newListNode());
	BOOST_CHECK( tree.type() == CppFW::CVMatTree::Type::List );
	BOOST_CHECK( tree.getNumElements() == 1 );
}


BOOST_AUTO_TEST_CASE( CVMatTree_list_throws_by_wrong_access )
{
	CppFW::CVMatTree tree;
	BOOST_CHECK_THROW(tree.getListNode(0), std::exception);
	
	
	BOOST_CHECK_NO_THROW(tree.newListNode());
	BOOST_REQUIRE( tree.type() == CppFW::CVMatTree::Type::List );
	BOOST_REQUIRE( tree.getNumElements() == 1 );
	
	BOOST_CHECK_NO_THROW(tree.getListNode(0));
	BOOST_CHECK_THROW(tree.getListNode(1), std::exception);
}


BOOST_AUTO_TEST_CASE( CVMatTree_list_grows )
{
	CppFW::CVMatTree tree;
	BOOST_CHECK_THROW(tree.getListNode(0), std::exception);
	
	
	BOOST_CHECK_NO_THROW(tree.newListNode());
	BOOST_REQUIRE( tree.type() == CppFW::CVMatTree::Type::List );
	BOOST_CHECK( tree.getNumElements() == 1 );
	
	BOOST_CHECK_NO_THROW(tree.newListNode());
	BOOST_CHECK( tree.getNumElements() == 2 );
}


BOOST_AUTO_TEST_CASE( CVMatTree_clear_becoms_undef )
{
	CppFW::CVMatTree tree;
	BOOST_CHECK_NO_THROW(tree.newListNode());
	BOOST_REQUIRE( tree.type() == CppFW::CVMatTree::Type::List );
	
	tree.clear();
	BOOST_REQUIRE( tree.type() == CppFW::CVMatTree::Type::Undef );
	BOOST_CHECK( tree.getNumElements() == 0 );
	
	
	BOOST_CHECK_NO_THROW(tree.getDirNode("test"));
	BOOST_CHECK( tree.type() == CppFW::CVMatTree::Type::Dir );
	BOOST_CHECK( tree.getNumElements() == 1 );
		
	tree.clear();
	BOOST_REQUIRE( tree.type() == CppFW::CVMatTree::Type::Undef );
	BOOST_CHECK( tree.getNumElements() == 0 );
	
	
	BOOST_CHECK_NO_THROW(tree.getMat());
	BOOST_CHECK( tree.type() == CppFW::CVMatTree::Type::Mat );
	BOOST_CHECK( tree.getNumElements() == 1 );
	
	tree.clear();
	BOOST_REQUIRE( tree.type() == CppFW::CVMatTree::Type::Undef );
	BOOST_CHECK( tree.getNumElements() == 0 );
}

	BOOST_AUTO_TEST_SUITE(Compare)

		BOOST_AUTO_TEST_CASE( empty_trees_are_equale )
		{
			CppFW::CVMatTree tree1;
			CppFW::CVMatTree tree2;
			
			BOOST_CHECK( tree1 == tree2 );
		}
		
		BOOST_AUTO_TEST_CASE( empty_trees_not_equale_with_tree_mat )
		{
			CppFW::CVMatTree tree1;
			CppFW::CVMatTree tree2;
			
			tree1.getMat();
			
			BOOST_CHECK( tree1 != tree2 );
		}
		
		BOOST_AUTO_TEST_CASE( mat_trees_compare_mats )
		{
			CppFW::CVMatTree tree1;
			CppFW::CVMatTree tree2;
			
			BOOST_CHECK( tree1 == tree2 );
			
			cv::Mat& mat1 = tree1.getMat();
			mat1.create(2, 1, cv::DataType<uint8_t>::type);
			mat1.at<uint8_t>(0, 0) = 1;
			mat1.at<uint8_t>(1, 0) = 2;
			
			BOOST_CHECK( tree1 != tree2 );
			
			cv::Mat& mat2 = tree2.getMat();
			mat2.create(2, 1, cv::DataType<uint8_t>::type);
			mat2.at<uint8_t>(0, 0) = 1;
			mat2.at<uint8_t>(1, 0) = 1;
			BOOST_CHECK( tree1 != tree2 );
			
			mat2.at<uint8_t>(1, 0) = 2;
			BOOST_CHECK( tree1 == tree2 );
			
			mat2.create(2, 2, cv::DataType<uint8_t>::type);
			BOOST_CHECK( tree1 != tree2 );
			
		}
		
		
		BOOST_AUTO_TEST_CASE( mat_trees_compare_more_complex_trees )
		{
			CppFW::CVMatTree tree1;
			CppFW::CVMatTree tree2;
			
			BOOST_CHECK( tree1 == tree2 );
			
			CppFW::CVMatTree& tree1bla  = tree1.getDirNode("bla");
			BOOST_CHECK( tree1 != tree2 );
			CppFW::CVMatTree& tree1blub = tree1.getDirNode("blub");
			BOOST_CHECK( tree1 != tree2 );
			
			CppFW::CVMatTree& tree2bla  = tree2.getDirNode("bla");
			BOOST_CHECK( tree1 != tree2 );
			CppFW::CVMatTree& tree2blub = tree2.getDirNode("blub");
			BOOST_CHECK( tree1 == tree2 );
			
			
			CppFW::CVMatTree& tree1bla1 = tree1bla.newListNode();
			createMat<double>(tree1bla1.getMat(), 5 ,10);
			CppFW::CVMatTree& tree1bla2 = tree1bla.newListNode();
			createMat<double>(tree1bla2.getMat(), 5 ,5);
			CppFW::CVMatTree& tree1bla3hm = tree1bla.newListNode().getDirNode("hm");
			createMat<double>(tree1bla3hm.getMat(), 5 ,3);
			
			tree1blub.newListNode().getMat();
			
			BOOST_CHECK( tree1 != tree2 );
			
			// std::cout << tree1 << std::endl;
			
			
			CppFW::CVMatTree& tree2bla1 = tree2bla.newListNode();
			BOOST_CHECK( tree1 != tree2 );
			createMat<double>(tree2bla1.getMat(), 5 ,10);
			BOOST_CHECK( tree1 != tree2 );
			CppFW::CVMatTree& tree2bla2 = tree2bla.newListNode();
			BOOST_CHECK( tree1 != tree2 );
			createMat<double>(tree2bla2.getMat(), 5 ,5);
			BOOST_CHECK( tree1 != tree2 );
			CppFW::CVMatTree& tree2bla3hm = tree2bla.newListNode().getDirNode("hm");
			BOOST_CHECK( tree1 != tree2 );
			createMat<double>(tree2bla3hm.getMat(), 5 ,3);
			
			BOOST_CHECK( tree1 != tree2 );
			tree2blub.newListNode().getMat();
			
			BOOST_CHECK( tree1 == tree2 );
			
			tree2bla2.getMat().at<double>(0,0) = 3.14;
			BOOST_CHECK( tree1 != tree2 );
		}

	BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
