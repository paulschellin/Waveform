

#include <iostream>


#include <typeinfo>
#include <functional>

#include <vector>
#include <IdentityTransform.hpp>

#include <boost/range.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/algorithm_ext.hpp>

#include <gtest/gtest.h>




namespace {


class IdentityTest : public ::testing::Test {
  protected:

	IdentityTest()
	{

	}


	virtual
	~IdentityTest()
	{

	}


	virtual
	void
	SetUp()
	{
		
		reference_vec.resize(ref_vec_len);

		boost::iota (reference_vec, 0);

	}


	virtual
	void
	TearDown()
	{

	}

	const std::size_t ref_vec_len = 512;
	//std::vector<int> reference_vec (ref_vec_len);
	//std::vector<int> reference_vec (512);
	std::vector<int> reference_vec;
};


typedef std::vector<int> VT;

typedef  Waveform::Transform::Identity<typename VT::iterator> IdentityT;
//typedef  Waveform::Transform::Identity<std::vector<int>> IdentityT;



TEST_F(IdentityTest, CTor1)
{
	//	Create the first "domain" array for the transform
	//std::vector<int> vec (512, 0);
	VT vec (reference_vec);

	//	Fill the array with incremental values
	//boost::iota (vec, 0);

	//	Create the second "domain" array with the same length as the first
	VT vec2 (vec.size());
	//std::vector<int> vec2 (vec.size());
	
	//	Create the transform from the two domain arrays
	IdentityT idT (vec.begin(), vec.end(), vec2.begin());

	//	Compare the values against the reference values
	for (std::size_t i = 0; i < vec.size(); ++i)
	{
		EXPECT_EQ( reference_vec.at(i), vec.at(i) );
	}
}



TEST_F(IdentityTest, CTor2)
{
	//	Create the first "domain" array for the transform
//	std::vector<int> vec (512, 0);
	VT vec (reference_vec);

	//	Fill the array with incremental values
//	boost::iota (vec, 0);

	//	Create the second "domain" array with the same length as the first
//	std::vector<int> vec2 (vec.size());
	VT vec2 (vec.size());

	//	Create the transform from the two domain arrays
	IdentityT idT (vec,vec2);

	//	Compare the values against the reference values
	for (std::size_t i = 0; i < vec.size(); ++i)
	{
		EXPECT_EQ( reference_vec.at(i), vec.at(i) );
	}

}

TEST_F(IdentityTest, SeeTypes)
{
	VT vec (reference_vec);
	VT vec2 (vec.size());
	IdentityT idT (vec,vec2);

	
	std::cout << "Transform Type: " << typeid(decltype(idT)::transform_type).name() << std::endl;


}


TEST_F(IdentityTest, FwTransWithIters)
{
	//	Create the first "domain" array for the transform
//	std::vector<int> vec (512, 0);
	VT vec (reference_vec);

	//	Fill the array with incremental values
//	boost::iota (vec, 0);

	//	Create the second "domain" array with the same length as the first
	//std::vector<int> vec2 (vec.size());
	VT vec2 (vec.size());

	//	Create the transform from the two domain arrays
	//IdentityT idT (vec, vec2);
	IdentityT idT (vec.begin(), vec.end(), vec2.begin());

	//	Execute the transform
	idT.exec_transform();

	//	Compare the values against the reference values
	for (std::size_t i = 0; i < vec.size(); ++i)
	{
		EXPECT_EQ( reference_vec.at(i), vec.at(i) );
	}

	//	Compare the values against the reference values
	for (std::size_t i = 0; i < vec.size(); ++i)
	{
		EXPECT_EQ( reference_vec.at(i), vec2.at(i) );
	}
}


TEST_F(IdentityTest, FwTransWithRange)
{
	//	Create the first "domain" array for the transform
//	std::vector<int> vec (512, 0);
	VT vec (reference_vec);

	//	Fill the array with incremental values
//	boost::iota (vec, 0);

	//	Create the second "domain" array with the same length as the first
	//std::vector<int> vec2 (vec.size());
	VT vec2 (vec.size());

	//	Create the transform from the two domain arrays
	IdentityT idT (vec, vec2);
	//IdentityT idT (vec.begin(), vec.end(), vec2.begin());

	//	Execute the transform
	idT.exec_transform();

	//	Compare the values against the reference values
	for (std::size_t i = 0; i < vec.size(); ++i)
	{
		EXPECT_EQ( reference_vec.at(i), vec.at(i) );
	}

	//	Compare the values against the reference values
	for (std::size_t i = 0; i < vec.size(); ++i)
	{
		EXPECT_EQ( reference_vec.at(i), vec2.at(i) );
	}

	EXPECT_EQ( vec.begin(), boost::begin(idT.range1_));
	EXPECT_EQ( vec.end(), boost::end(idT.range1_));
	EXPECT_EQ( vec2.begin(), boost::begin(idT.range2_));
	EXPECT_EQ( vec2.end(), boost::end(idT.range2_));
}


TEST_F(IdentityTest, InvTrans)
{
	//	Create the first "domain" array for the transform
	//std::vector<int> vec (512, 0);
	VT vec (reference_vec);

	//	Create the second "domain" array with the same length as the first
	//std::vector<int> vec2 (vec.size());
	VT vec2 (vec.size());

	//	Fill the array with incremental values
	//boost::iota (vec2, 0);

	//	Create the transform from the two domain arrays
	//IdentityT idT (vec, vec2);
	IdentityT idT (vec2.begin(), vec2.end(), vec.begin());

	//	Execute the inverse transform
	idT.exec_inverse_transform();

	//	Compare the values against the reference values
	for (std::size_t i = 0; i < vec.size(); ++i)
	{
		EXPECT_EQ( reference_vec.at(i), vec.at(i) );
	}

	//	Compare the values against the reference values
	for (std::size_t i = 0; i < vec.size(); ++i)
	{
		EXPECT_EQ( reference_vec.at(i), vec2.at(i) );
	}

}






}	//	namespace


int
main (int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
