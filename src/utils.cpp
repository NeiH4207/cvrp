/**
* @file utils.cpp
* @author vss2sn
* @brief Contains the structs, classes and functions used for the set up of the problem aand solution as well as some functions that aid in debugging.
*/

#include "utils.hpp"

void Node::PrintStatus(){
  std::cout << "Node Status" << std::endl;
  std::cout << "ID    : " << id_ << std::endl;
  std::cout << "X     : " << x_ << std::endl;
  std::cout << "Y     : " << y_ << std::endl;
  std::cout << "Demand: " << demand_ << std::endl;
  std::cout << std::endl;
}

void Route::PrintStatus(){
  std::cout << "Route Status" << std::endl;
  std::cout << "Cost        : " << cost_ << std::endl;
  std::cout << "Path        : "   ;
  // the nodes_.size()-1 limit is only added to ensure that there isnt a --->
  // after the last node, which is always the depot, ie node 0.
  for(size_t i = 0; i < nodes_.size()-1; ++i) std::cout << nodes_[i] << " ---> ";
  std::cout << "0";
  std::cout << std::endl << std::endl;
}

void Route::PrintRoute(){
  std::cout << "Path        : "   ;
  // the nodes_.size()-1 limit is only added to ensure that there isnt a --->
  // after the last node, which is always the depot, ie node 0.
  for(size_t i = 0; i < nodes_.size()-1; ++i) std::cout << nodes_[i] << " ---> ";
  std::cout << "0";
  std::cout << std::endl << std::endl;
}

void Route::CalculateCost(std::vector<std::vector<double>> distanceMatrix){
  cost_ = 0;
  for(size_t i=0;i<nodes_.size()-1;i++) cost_+=distanceMatrix[nodes_[i]][nodes_[i+1]];
}

void Vehicle::PrintStatus(){
  std::cout << "Vehicle Status" << std::endl;
  std::cout << "Cost        : " << cost_ << std::endl;
  std::cout << "ID          : " << id_ << std::endl;
  std::cout << "Load        : " << load_ << std::endl;
  std::cout << "Capacity    : " << capacity_ << std::endl;
  std::cout << "Distance    : " << distance_ << std::endl;
  std::cout << "Max Distance: " << max_distance_ << std::endl;
  std::cout << "Path        : "   ;
  // the nodes_.size()-1 limit is only added to ensure that there isnt a --->
  // after the last node, which is always the depot, ie node 0.
  for(size_t i = 0; i < nodes_.size()-1; ++i) std::cout << nodes_[i] << " ---> ";
  std::cout << "0";
  std::cout << std::endl << std::endl;
}

Solution::Solution(std::vector<Node> nodes, std::vector<Vehicle> vehicles, std::vector<std::vector<double>> distanceMatrix)
  :nodes_(nodes), vehicles_(vehicles), distanceMatrix_(distanceMatrix){
  depot_ = nodes_[0];
  capacity_ = vehicles[0].load_;
  max_distance_ = vehicles[0].max_distance_;
}

Solution::Solution(Problem p){
  nodes_ = p.nodes_;
  vehicles_ = p.vehicles_;
  distanceMatrix_ = p.distanceMatrix_;
  depot_ = nodes_[0];
  capacity_ = p.capacity_;
  max_distance_ = p.max_distance_;
}

void Solution::CreateInitialSolution(){
  for(auto& v:vehicles_){
    while(true){
      Node closest_node = find_closest(v);
      if(closest_node.id_!=-1
        && v.load_ - closest_node.demand_ >=0
        && (v.distance_ - distanceMatrix_[v.nodes_.back()][closest_node.id_] - distanceMatrix_[closest_node.id_][depot_.id_]>= 0)){//}.2*capacity){
        v.load_ -= closest_node.demand_;
        v.distance_-=distanceMatrix_[v.nodes_.back()][closest_node.id_];
        v.cost_ += distanceMatrix_[v.nodes_.back()][closest_node.id_];
        v.nodes_.push_back(closest_node.id_);
        nodes_[closest_node.id_].is_routed_ = true;
      }
      else{
        v.cost_ += distanceMatrix_[v.nodes_.back()][depot_.id_];
        v.distance_ -= distanceMatrix_[v.nodes_.back()][depot_.id_];
        v.nodes_.push_back(depot_.id_);
        break;
      }
    }
  }
}

void Solution::Solve(){}

Node Solution::find_closest(Vehicle& v){
    double cost = INT_MAX;
    int id = -1;
    for(size_t j=0; j < distanceMatrix_[0].size(); j++){
      if(!nodes_[j].is_routed_
        && nodes_[j].demand_ <= v.load_
        && (distanceMatrix_[v.nodes_.back()][j] + distanceMatrix_[j][depot_.id_]) <= v.distance_
        && distanceMatrix_[v.nodes_.back()][j] < cost){
        cost = distanceMatrix_[v.nodes_.back()][j];
        id = j;
      }
    }
    if(id!=-1) return nodes_[id];
    else return Node(0,0,-1,0);
}

bool Solution::CheckSolutionValid(){
  // double cost = 0;
  std::cout << "max: " << max_distance_ << std::endl;
  std::vector<bool> check_nodes(nodes_.size(), false);
  check_nodes[0]=true;
  for(auto&v:vehicles_){
    int load = capacity_;
    for(auto&n:v.nodes_){
      load-=nodes_[n].demand_;
      check_nodes[n] = true;
    }
    if(load<0) return false;
    double distance = max_distance_;
    for(size_t i=0;i<v.nodes_.size()-1;i++){
      distance -= distanceMatrix_[v.nodes_[i]][v.nodes_[i+1]];
    }
    if(distance<0.000001){
      std::cout << "Max distance exceeded" << std::endl;
      return false;
    }
  }
  for(auto b:check_nodes){
    if(!b) return false;
  }
  return true;
}

Problem::Problem(int noc, int demand_range, int nov, int capacity, int grid_range, std::string distribution, int n_clusters, int cluster_range, double max_distance){

  std::random_device rd; // obtain a random number from hardware
  std::mt19937 eng(rd()); // seed the generator
  std::uniform_int_distribution<int> ran(-grid_range,grid_range); // define the range
  std::uniform_int_distribution<int> ran_d(0,demand_range); // define the range
  std::uniform_int_distribution<int> ran_c(-cluster_range,cluster_range);
  Node depot(0, 0, 0, 0, true);
  this->capacity_ = capacity;
  this->max_distance_ = max_distance;
  nodes_.push_back(depot);

  if(distribution != "uniform" && distribution != "cluster") distribution = "uniform";
  if(distribution == "uniform") for(int i = 1; i <=noc; ++i) nodes_.emplace_back(ran(eng), ran(eng), i, ran_d(eng), false);
  else if(distribution == "cluster"){
    int id = 1;
    int n_p_c = noc/n_clusters;
    int remain = noc%n_clusters;
    for(int i=0;i<n_clusters;i++){
      int x = ran(eng);
      int y = ran(eng);
      for(int j=0;j<n_p_c;j++){
        nodes_.emplace_back(x + ran_c(eng), y + ran_c(eng), id, ran_d(eng), false);
        // nodes_.back().PrintStatus();
        id++;
      }
    }
    int x = ran(eng);
    int y = ran(eng);
    for(int j=0;j<remain;j++){
      nodes_.emplace_back(x + ran_c(eng), y + ran_c(eng), id, ran_d(eng), false);
      id++;
    }
  }

  // for(auto& n:nodes) n.PrintStatus();
  std::vector<double> tmp(nodes_.size());
  for(size_t i=0; i<nodes_.size(); ++i) distanceMatrix_.push_back(tmp);
  for(size_t i=0; i<nodes_.size(); ++i){
    for(size_t j=i; j < nodes_.size(); ++j){
      distanceMatrix_[i][j] = sqrt(double(pow((nodes_[i].x_ - nodes_[j].x_),2)
                                       + pow((nodes_[i].  y_ - nodes_[j].y_),2)));
      distanceMatrix_[j][i] = distanceMatrix_[i][j];
    }
  }

  // int load = capacity_;
  for(int i=0; i<nov; ++i){
    vehicles_.emplace_back(i, capacity_, capacity_);
    vehicles_[i].nodes_.push_back(0);
  }
}

void Solution::PrintSolution(const std::string& option){
  int status;
  char * demangled = abi::__cxa_demangle(typeid(*this).name(),0,0,&status);
  std::cout << demangled << ":" << std::endl;
  double total_cost = 0;
  for(auto& v:vehicles_){
    total_cost+=v.cost_;
    if(option=="status"){
      v.PrintStatus();
    }
    else if(option=="route"){
      std::cout << "Vehicle ID: " << v.id_ << " | ";
      v.PrintRoute();
    }
  }
  bool valid = CheckSolutionValid();
  std::cout << "Total solution cost: " << total_cost << std::endl;
  std::cout << "Solution validity  : " << valid << std::endl;
  if(!valid){
    for(auto& i:nodes_){
      if(!i.is_routed_){
        std::cout << "Unreached node: " << std::endl;
        i.PrintStatus();
      }
    }
  }
}
