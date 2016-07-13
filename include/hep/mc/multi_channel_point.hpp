#ifndef HEP_MC_MULTI_CHANNEL_POINT_HPP
#define HEP_MC_MULTI_CHANNEL_POINT_HPP

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

#include "hep/mc/mc_point.hpp"

#include <cstddef>
#include <vector>

namespace hep
{

/// \addtogroup integrands
/// @{

/// Point in the unit-hypercube for Multi Channel Monte Carlo integration.
template <typename T>
class multi_channel_point : public mc_point<T>
{
public:
	/// Constructor.
	multi_channel_point(
		std::vector<T> const& point,
		std::vector<T> const& coordinates,
		std::size_t channel,
		T total_density
	)
		: mc_point<T>(point, T(1.0) / total_density)
		, channel_(channel)
		, coordinates_(coordinates)
	{
	}

	/// The selected channel for this point.
	std::size_t channel() const
	{
		return channel_;
	}

	/// The point in the hypercube transformed by the current \ref channel.
	std::vector<T> const& coordinates() const
	{
		return coordinates_;
	}

private:
	std::size_t channel_;
	std::vector<T> const& coordinates_;
};

/// Point in the unit-hypercube for multi-channel Monte Carlo integration. This
/// type also captures the map that is used to generate `coordinates`.
template <typename T, typename M>
class multi_channel_point2 : public multi_channel_point<T>
{
public:
	/// Constructor.
	multi_channel_point2(
		std::vector<T> const& point,
		std::vector<T> const& coordinates,
		std::size_t channel,
		T total_density,
		M const& map
	)
		: multi_channel_point<T>(point, coordinates, channel, total_density)
		, map_(map)
	{
	}

	/// The map function that constructed this point. See
	/// \ref multi_channel_iteration for reference on the signature of this
	/// function.
	M const& map() const
	{
		return map_;
	}

private:
	M const& map_;
};

/// @}

}

#endif
