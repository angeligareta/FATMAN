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

double get_distance_between(const Position& position_1, const Position& position_2)
{
  int x_distance_to_obstacle = abs(position_1.first - position_2.first);
  int y_distance_to_obstacle = abs(position_1.second - position_2.second);
  return sqrt(x_distance_to_obstacle*x_distance_to_obstacle + y_distance_to_obstacle*y_distance_to_obstacle);
}

Position get_next_obstacle(const Position& actual_position, const std::vector<Position>& obstacle_vector, const Area& area)
{
  Position next_obstacle_position = Position(-1, -1);
  double nearest_obstacle_distance = MAX_INT;
  
  for (int i = 0; i < obstacle_vector.size(); ++i) {
    // Check if the obstacle is after the actual x and in the area
      // 1º Check if obstacle.x is higher than actual_position.x
    if (obstacle_vector[i].first <= actual_position.first) { continue; } // TODO: IF it's equal? do another thing...
      // 2º Check if obstacle.y is higher than area.min and lower than area.max
    if ( (obstacle_vector[i].second <= area.first) || (obstacle_vector[i].second >= area.second) ) { continue; }
    
    // Return nearest obstacle in the area
    if ( nearest_obstacle_distance > get_distance_between(actual_position, obstacle_vector[i]) ) { 
      next_obstacle_position = obstacle_vector[i]; // Return nearest obstacle in the area
      nearest_obstacle_distance = get_distance_between(actual_position, obstacle_vector[i]);
    }
  }
  
  return next_obstacle_position;
}

int main () {
  int test_number = 1; // TEMP
  std::vector<Position> obstacle_vector;
  read_data(obstacle_vector);
  
  double diameter = aisle_width;
  
  Position actual_position (0, aisle_width / 2);
  Area area (0, diameter); // The area is min and max y
  
  system("clear");
  
  std::cout << "First Obstacle (" << actual_position.first << " , " << actual_position.second << ")\n";
  std::cout << "First Area (" << area.first << " , " << area.second << ")\n";
  std::cout << "First Diameter = " << diameter << "\n\n";
  
  while (actual_position != Position(-1,-1)) {
    
    actual_position = get_next_obstacle(actual_position, obstacle_vector, area);
    if (actual_position == Position(-1,-1)) { break; }
    std::cout << "Obstacle (" << actual_position.first << " , " << actual_position.second << ")\n";
  }
}