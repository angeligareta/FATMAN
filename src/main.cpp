#include <iostream>
#include <cstdlib> //abs
#include <vector>
#include <cmath> //sqrt
#include <algorithm>

using Position = std::pair<int,int>;
using Area = std::pair<double,double>;

const int MAX_INT = 101*101; // Used for distance
int aisle_length; // Initialized in read_data
int aisle_width; // Initialized in read_data

void read_data (std::vector<Position> &obstacle_vector) 
{
  // TEMP
  aisle_length = 8; aisle_width = 5;
  int obstacle_number = 8;
  
  obstacle_vector.resize(obstacle_number);
  obstacle_vector[0] = Position(2, 1);
  obstacle_vector[1] = Position(1, 3);
  obstacle_vector[2] = Position(3, 2);
  obstacle_vector[3] = Position(4, 4);
  obstacle_vector[4] = Position(5, 3);
  obstacle_vector[5] = Position(6, 4);
  obstacle_vector[6] = Position(7, 2);
  obstacle_vector[7] = Position(7, 1);
  
  //obstacle_vector.push_back(Position(3, 3));
}

int main () {
  int test_number = 1; // TEMP
  std::vector<Position> obstacle_vector;
  read_data(obstacle_vector);
}