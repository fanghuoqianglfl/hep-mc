#include "hep/mc.hpp"

double arctan(hep::mc_point<double> const& point)
{
	double const projection = 10.0 * point.point()[0] - 5.0;
	return std::atan(projection);
}

void bin_projector_function(
	hep::mc_point<double> const& point,
	hep::bin_projector<double>& projector,
	hep::function_value<double> value
) {
	double const projection = 10.0 * point.point()[0] - 5.0;
	projector.add(0, projection, value.value());
}

int main()
{
	// create the integrand: We want to integrate the function `arctan` which
	// has a single dimension, and furthermore we want to generate a
	// differential distribution of this function using 100 bins in the interval
	// [-5,+5]
	auto integrand = hep::make_integrand<double>(
		arctan,
		1,
		bin_projector_function,
		hep::distribution_parameters<double>(100, -5.0, 5.0)
	);

	// now integrate and record the differential distributions
	auto const result = hep::plain<double>(integrand, 1000000);

	// integral is zero
	std::cout << "integral is I = " << result.value() << " +- "
		<< result.error() << "\n\n";

	auto const& distribution = result.distributions()[0];

	// print the distribution - compare this with the plot of the function
	// 'atan(x)/10.0'
	for (std::size_t i = 0; i != distribution.mid_points().size(); ++i)
	{
		std::cout << distribution.mid_points()[i] << "\t"
			<< distribution.results()[i].value() << "\t"
			<< distribution.results()[i].error() << "\n";
	}
}