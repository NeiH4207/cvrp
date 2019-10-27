/**
* @file local_search_intra.hpp
* @author vss2sn
* @brief Contains the LocalSearchIntraSolution class (Local search restricted to within individual vehicles)
*/

#ifndef LSI_HPP
#define LSI_HPP

#include "utils.hpp"

class LocalSearchIntraSolution : public Solution{
public:
  /**
  * @brief Constructor
  * @param nodes Vector of nodes
  * @param vehicles Vector of vehicles
  * @param distanceMatrix Matrix containing distance between each pair of nodes
  * @return No return parameter
  * @details Constructor for initial setup of problem, and solution using Local Search applied to the routes of each of the vehicles separately
  */
  LocalSearchIntraSolution(std::vector<Node> nodes, std::vector<Vehicle> vehicles, std::vector<std::vector<double>> distanceMatrix);

  /**
  * @brief Constructor
  * @param p Instance of problem class defining the problem parameters
  * @return No return parameter
  * @details Constructor for initial setup of problem, and solution using Local Search applied to the routes of each of the vehicles separately
  */
  LocalSearchIntraSolution(Problem p);

  /**
  * @brief Constructor
  * @param s Instance of Solution class containing a valid solution and problem parameters
  * @return No return parameter
  * @details Constructor for initial setup of problem, and solution using Local Search applied to the routes of each of the vehicles separately
  */
  LocalSearchIntraSolution(Solution s);
  /**
  * @brief Function called to solve the given problem using a local search algorithm
  * @return void
  * @details Generates random iniitial solutions. Applies selected algorithm. Prints cost of best solution, and its validity.
  */
  void Solve();
};

#endif
