#include <cvmat/cvmattreestruct.h>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(CVMatTreeBin)

BOOST_AUTO_TEST_CASE( CVMatTree_undef_on_start )
{
	CppFW::CVMatTree tree;
	BOOST_CHECK( tree.type() == CppFW::CVMatTree::Type::Undef );
	BOOST_CHECK( tree.getNumElements() == 0 );
}

BOOST_AUTO_TEST_SUITE_END()