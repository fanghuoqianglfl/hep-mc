#include "gtest/gtest.h"

#include <algorithm>
#include <cstddef>
//#include <cstdio>
#include <limits>
#include <vector>

#ifndef HEP_USE_MPI
#include "hep/mc.hpp"
#else
#include "hep/mc-mpi.hpp"
#endif

template <typename T>
T function(hep::mc_point<T> const& x)
{
	return T(3.0) / T(2.0) *
		(x.point()[0] * x.point()[0] + x.point()[1] * x.point()[1]);
}

template <typename T>
std::vector<T> reference_results();

template <>
std::vector<float> reference_results() {
	return {
		// PLAIN
		0xf.f7d32p-4f, 0x8.2b4edp-12f,
		// VEGAS
		0xf.f7d32p-4f, 0x8.2b4edp-12f,
		0x8.02be3p-3f, 0x9.0497bp-13f,
		0x8.00063p-3f, 0xc.e6325p-14f,
		0xf.fe5c6p-4f, 0xb.465fap-14f,
		0xf.ffadfp-4f, 0xa.c4601p-14f,
		// MULTI CHANNEL
		0xf.fca45p-4f, 0x8.2b528p-12f,
		0x8.00671p-3f, 0x8.34a28p-12f,
		0xf.fdca4p-4f, 0x8.30b93p-12f,
		0xf.ffb0dp-4f, 0x8.2c270p-12f,
		0x8.0206bp-3f, 0x8.2f97ap-12f,
	};
}

template <>
std::vector<double> reference_results() {
	return {
		// PLAIN
		0xf.ffc86404543c0p-4, 0x8.2b672f116b6c0p-12,
		// VEGAS
		0xf.ffc86404543c0p-4, 0x8.2b672f116b6c0p-12,
		0x8.01294905b4cd8p-3, 0x9.03ebf3d5a5328p-13,
		0x8.00331091e43d8p-3, 0xc.d9e34cb3fbe98p-14,
		0xf.fe107b0b3d6a0p-4, 0xb.518ce42907288p-14,
		0xf.ffd9e3eac8cf8p-4, 0xa.d9002081b1b58p-14,
		// MULTI CHANNEL
		0x8.03dfd55411338p-3, 0x8.3187b223d05d8p-12,
		0x8.0160eaaa1b870p-3, 0x8.2e3155f6b94f8p-12,
		0x8.0413e49ae60a8p-3, 0x8.3d83dfe23a5a8p-12,
		0x8.04a60eeee06f0p-3, 0x8.2e8cbae21e648p-12,
		0x8.04de1ba046340p-3, 0x8.35c33ae807200p-12,
	};
}

template <>
std::vector<long double> reference_results() {
	return {
		// PLAIN
		0xf.ffc86404543bcdbp-4l, 0x8.2b672f116b780aep-12l,
		// VEGAS
		0xf.ffc86404543bcdbp-4l, 0x8.2b672f116b780adp-12l,
		0x8.01294905b4cd55ap-3l, 0x9.03ebf3d5a522088p-13l,
		0x8.00331091e43d804p-3l, 0xc.d9e34cb3fb9ebafp-14l,
		0xf.fe107b0b3d69c68p-4l, 0xb.518ce429071b2b8p-14l,
		0xf.ffd9e3eac8cf7eap-4l, 0xa.d9002081b209a36p-14l,
		// MULTI CHANNEL
		0x8.03dfd5541133341p-3l, 0x8.3187b223d085bf1p-12l,
		0x8.0160eaaa1b86c96p-3l, 0x8.2e3155f6b942f46p-12l,
		0x8.0413e49ae60a448p-3l, 0x8.3d83dfe23a78523p-12l,
		0x8.04a60eeee06f282p-3l, 0x8.2e8cbae21e76261p-12l,
		0x8.04de1ba04633dd0p-3l, 0x8.35c33ae80726c30p-12l,
	};
}

typedef testing::Types<float, double, long double> MyT;
template <typename T> class NumericalResults : public testing::Test { };
TYPED_TEST_CASE(NumericalResults, MyT);

TYPED_TEST(NumericalResults, CheckPlainIntegration)
{
	using T = TypeParam;

	std::size_t const calls = 100000;

#ifndef HEP_USE_MPI
	auto const result = hep::plain<T>(
#else
	auto const result = hep::mpi_plain<T>(
		MPI_COMM_WORLD,
#endif
		2,
		calls,
		function<T>
	);
	auto const reference = reference_results<T>();

#ifndef HEP_USE_MPI
	T const value_abs_error = T();
	T const error_abs_error = T();
#else
	T const value_abs_error = T(4.0) * std::numeric_limits<T>::epsilon();
	T const error_abs_error = T(4.0) * std::numeric_limits<T>::epsilon();
#endif

	EXPECT_NEAR( result.value() , reference[0] , value_abs_error );
	EXPECT_NEAR( result.error() , reference[1] , error_abs_error );

	EXPECT_EQ( result.calls() , calls );

//	std::printf("%La, %La,\n", static_cast <long double> (result.value()),
//			static_cast <long double> (result.error()));
}

TYPED_TEST(NumericalResults, CheckVegasIntegration)
{
	using T = TypeParam;

	std::size_t const calls = 100000;
	std::size_t const iterations = 5;

#ifndef HEP_USE_MPI
	auto const results = hep::vegas<T>(
#else
	auto const results = hep::mpi_vegas<T>(
		MPI_COMM_WORLD,
#endif
		2,
		std::vector<std::size_t>(iterations, calls),
		function<T>
	);
	auto const reference = reference_results<T>();

#ifndef HEP_USE_MPI
	T const value_abs_error = T();
	T const error_abs_error = T();
#else
	T const value_abs_error = T(4.0) * std::numeric_limits<T>::epsilon();
	T const error_abs_error = T(4.0) * std::numeric_limits<T>::epsilon();
#endif

	ASSERT_EQ( results.size() , iterations );

	EXPECT_NEAR( results[0].value() , reference[ 2] , value_abs_error );
	EXPECT_NEAR( results[0].error() , reference[ 3] , error_abs_error );
	EXPECT_NEAR( results[1].value() , reference[ 4] , value_abs_error );
	EXPECT_NEAR( results[1].error() , reference[ 5] , error_abs_error );
	EXPECT_NEAR( results[2].value() , reference[ 6] , value_abs_error );
	EXPECT_NEAR( results[2].error() , reference[ 7] , error_abs_error );
	EXPECT_NEAR( results[3].value() , reference[ 8] , value_abs_error );
	EXPECT_NEAR( results[3].error() , reference[ 9] , error_abs_error );
	EXPECT_NEAR( results[4].value() , reference[10] , value_abs_error );
	EXPECT_NEAR( results[4].error() , reference[11] , error_abs_error );

	EXPECT_EQ( results[0].calls() , calls );
	EXPECT_EQ( results[1].calls() , calls );
	EXPECT_EQ( results[2].calls() , calls );
	EXPECT_EQ( results[3].calls() , calls );
	EXPECT_EQ( results[4].calls() , calls );

//	for (auto i : results)
//	{
//		std::printf("%La, %La,\n", static_cast <long double> (i.value()),
//			static_cast <long double> (i.error()));
//	}
}

TYPED_TEST(NumericalResults, CheckMultiChannelIntegration)
{
	using T = TypeParam;

	std::size_t const calls = 100000;
	std::size_t const iterations = 5;

	auto unit_densities = [](std::size_t, std::vector<T> const& random_numbers,
		std::vector<T>& coordinates, std::vector<T>& channel_densities) {

		std::copy(
			random_numbers.begin(),
			random_numbers.end(),
			coordinates.begin()
		);

		std::fill(channel_densities.begin(), channel_densities.end(), T(1.0));

		return T(1.0);
	};

#ifndef HEP_USE_MPI
	auto const results = hep::multi_channel<T>(
#else
	auto const results = hep::mpi_multi_channel<T>(
		MPI_COMM_WORLD,
#endif
		2,
		2,
		std::vector<std::size_t>(iterations, calls),
		function<T>,
		1,
		unit_densities
	);
	auto const reference = reference_results<T>();

#ifndef HEP_USE_MPI
	T const value_abs_error = T();
	T const error_abs_error = T();
#else
	T const value_abs_error = T(4.0) * std::numeric_limits<T>::epsilon();
	T const error_abs_error = T(4.0) * std::numeric_limits<T>::epsilon();
#endif

	ASSERT_EQ( results.size() , iterations );

	EXPECT_NEAR( results[0].value() , reference[12] , value_abs_error );
	EXPECT_NEAR( results[0].error() , reference[13] , error_abs_error );
	EXPECT_NEAR( results[1].value() , reference[14] , value_abs_error );
	EXPECT_NEAR( results[1].error() , reference[15] , error_abs_error );
	EXPECT_NEAR( results[2].value() , reference[16] , value_abs_error );
	EXPECT_NEAR( results[2].error() , reference[17] , error_abs_error );
	EXPECT_NEAR( results[3].value() , reference[18] , value_abs_error );
	EXPECT_NEAR( results[3].error() , reference[19] , error_abs_error );
	EXPECT_NEAR( results[4].value() , reference[20] , value_abs_error );
	EXPECT_NEAR( results[4].error() , reference[21] , error_abs_error );

	EXPECT_EQ( results[0].calls() , calls );
	EXPECT_EQ( results[1].calls() , calls );
	EXPECT_EQ( results[2].calls() , calls );
	EXPECT_EQ( results[3].calls() , calls );
	EXPECT_EQ( results[4].calls() , calls );

//	for (auto i : results)
//	{
//		std::printf("%La, %La,\n", static_cast <long double> (i.value()),
//			static_cast <long double> (i.error()));
//	}
}
