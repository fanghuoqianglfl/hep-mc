#ifndef HEP_MC_MULTI_CHANNEL_CALLBACK_HPP
#define HEP_MC_MULTI_CHANNEL_CALLBACK_HPP

/*
 * hep-mc - A Template Library for Monte Carlo Integration
 * Copyright (C) 2015  Christopher Schwan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "hep/mc/multi_channel_result.hpp"
#include "hep/mc/multi_channel_max_difference.hpp"

#include <cmath>
#include <functional>
#include <iostream>
#include <vector>

namespace hep
{

/// \addtogroup callbacks
/// @{

/// The default callback function. This function does nothing and always returns
/// `true`.
///
/// \see \ref multi_channel_callback
template <typename T>
inline bool multi_channel_default_callback(
	std::vector<multi_channel_result<T>> const&
) {
	return true;
}

/// Callback function that prints a detailed summary about every iteration
/// performed so far. This function always returns `true`.
///
/// \see \ref multi_channel_callback
template <typename T>
inline bool multi_channel_verbose_callback(
	std::vector<multi_channel_result<T>> const& results
) {
	std::cout << "iteration " << (results.size() - 1) << " finished.\n";

	T const relative_error_percent = (T(100.0) * results.back().error() /
		std::fabs(results.back().value()));

	T const max_difference = multi_channel_max_difference(results.back());

	// print result for this iteration
	std::cout << "this iteration: N=" << results.back().calls() << " E="
		<< results.back().value() << " +- " << results.back().error() << " ("
		<< relative_error_percent << "%) D=" << max_difference << "\n";

	// compute cumulative results
	auto const result = cumulative_result0(results.begin(), results.end());
	T const chi = chi_square_dof0(results.begin(), results.end());

	T const relative_error_percent_all = (T(100.0) * result.error() /
		std::fabs(result.value()));

	// print the combined result
	std::cout << "all iterations: N=" << result.calls() << " E="
		<< result.value() << " +- " << result.error() << " ("
		<< relative_error_percent_all << "%) chi^2/dof=" << chi << "\n\n";

	std::cout.flush();

	return true;
}

/// The type of callback function that can be set by the user with
/// \ref multi_channel_callback.
template <typename T>
using multi_channel_callback_type =
	std::function<bool(std::vector<multi_channel_result<T>>)>;

/// Sets the multi channel  `callback` function and returns it. This function is
/// called after each iteration performed by \ref multi_channel. The default
/// callback is \ref multi_channel_default_callback. The function can e.g. be
/// set to \ref multi_channel_verbose_callback which prints after each
/// iteration. If the callback function returns `false` the integration is
/// stopped.
///
/// If this function is called without any argument, the previous function is
/// retained.
template <typename T>
inline multi_channel_callback_type<T> multi_channel_callback(
	multi_channel_callback_type<T> callback = nullptr
) {
	static multi_channel_callback_type<T> object =
		multi_channel_default_callback<T>;

	if (callback != nullptr)
	{
		object = callback;
	}

	return object;
}

/// @}

}

#endif