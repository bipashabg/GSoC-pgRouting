/*PGR-GNU*****************************************************************
File: optimize.hpp

Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

------

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

 ********************************************************************PGR-GNU*/

/*! @file */

#ifndef INCLUDE_VRP_OPTIMIZE_HPP_
#define INCLUDE_VRP_OPTIMIZE_HPP_
#pragma once

#include "vrp/solution.hpp"
#include "vrp/pickDeliver.hpp"

namespace pgrouting {
namespace vrp {


class Optimize : public Solution {
 public:
     explicit Optimize(const Solution &solution);
     Optimize(const Solution &solution, size_t times);

     /* @brief decrease_truck
      *
      * Optimization by decreasing trucks
      */
     void decrease_truck();
     void inter_swap(size_t times);
     Solution best_solution;

 private:
     bool decrease_truck(size_t);
     void sort_by_duration();
     void sort_by_size();
     void delete_empty_truck();

     bool swap_worse(Vehicle_pickDeliver &from, Vehicle_pickDeliver &to);
     bool move_reduce_cost(Vehicle_pickDeliver &from, Vehicle_pickDeliver &to);
     bool inter_swap();

     bool move_order(
             Order order,
             Vehicle_pickDeliver &from_truck,
             Vehicle_pickDeliver &to_truck);
     void save_if_best();
};

}  //  namespace vrp
}  //  namespace pgrouting

#endif  // INCLUDE_VRP_OPTIMIZE_HPP_
