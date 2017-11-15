#include <iostream>
#include <sstream>
#include <iterator>

#include <vector>
#include <set>
#include <string>

#include <cstdlib> //abs
#include <cmath> //sqrt
#include <algorithm> //count

using Position = std::pair<int,int>;
using Area = std::pair<double,double>;
class Way;

const int MAX_INT = 101*101; // Used for distance
int aisle_length; // Initialized in read_data
int aisle_width; // Initialized in read_data

std::vector<Position> obstacle_vector;
std::set<Way> possible_way_set; // Ways that can have the max diameter
std::set<Way> finished_way_set; // Ways that have finished

class Way {
  
  private:
    Position actual_position_;
    double diameter_;
    Area area_;
    std::vector<Position> checked_obstacles;
    
  public:
    Way(const Position& actual_position, double diameter, Area area):
    actual_position_(actual_position),
    diameter_(diameter),
    area_(area),
    checked_obstacles(std::vector<Position>())
    {}
    
    Way(const Way& way)
    {
      *this = way;
    }
    
    Way& operator= (const Way& way)
    {
      actual_position_ = way.actual_position_;
      diameter_ = way.diameter_;
      area_ = way.area_;
      
      return *this;
    }
    
    // Order the set by distance
    friend bool operator< (const Way& way_1, const Way& way_2)
    {
      return way_1.diameter_ < way_2.diameter_;
    }
    
    void get_next_ways () // Hace todo el proceso hasta que seleccione área y divida los casos, introduciendolos en possible_way
    {
      actual_position_ = get_next_obstacle(area_);
      
      // If way has finished
      if (actual_position_ == Position(-1,-1)) { finished_way_set.insert(*this); }
      else {
        divide_ways();
      }
    }
    
    double get_diameter () const
    {
      return diameter_;
    }
  
  private:
    
    double get_distance_between(const Position& position_1, const Position& position_2) const
    {
      int x_distance_to_obstacle = abs(position_1.first - position_2.first);
      int y_distance_to_obstacle = abs(position_1.second - position_2.second);
      return sqrt(x_distance_to_obstacle*x_distance_to_obstacle + y_distance_to_obstacle*y_distance_to_obstacle);
    }

    Position get_next_obstacle(const Area& area) const
    {
      Position next_obstacle_position = Position(-1, -1);
      double nearest_obstacle_distance = MAX_INT;
      
      for (int i = 0; i < obstacle_vector.size(); ++i) {
        // Check if the obstacle is after the actual x and in the area
          // 1º Check if obstacle.x is higher than actual_position_.x
        if (obstacle_vector[i].first <= actual_position_.first) { continue; } // TODO: IF it's equal? do another thing...
          // 2º Check if obstacle.y is higher than area.min and lower than area.max
        if ( (obstacle_vector[i].second <= area.first) || (obstacle_vector[i].second >= area.second) ) { continue; }
        
        // Return nearest obstacle in the area
        if ( nearest_obstacle_distance > get_distance_between(actual_position_, obstacle_vector[i]) ) { 
          next_obstacle_position = obstacle_vector[i]; // Return nearest obstacle in the area
          nearest_obstacle_distance = get_distance_between(actual_position_, obstacle_vector[i]);
        }
      }
      
      return next_obstacle_position;
    }

    Position get_last_obstacle(const Area& area) const
    {
      Position last_obstacle_position = Position(-1, -1);
      double nearest_obstacle_distance = MAX_INT;
      
      for (int i = 0; i < obstacle_vector.size(); ++i) {
        // Check if the obstacle is before the actual x and in the area
          // 1º Check if obstacle.x is lower than actual_position_.x
        if (obstacle_vector[i].first >= actual_position_.first) { continue; } // TODO: IF it's equal? do another thing...
          // 2º Check if obstacle.y is higher than area.min and lower than area.max
        if ( (obstacle_vector[i].second <= area.first) || (obstacle_vector[i].second >= area.second) ) { continue; }
        
        // Return nearest obstacle in the area
        if ( nearest_obstacle_distance > get_distance_between(actual_position_, obstacle_vector[i]) ) {
          last_obstacle_position = obstacle_vector[i]; // Return nearest obstacle in the area
          nearest_obstacle_distance = get_distance_between(actual_position_, obstacle_vector[i]);
        } 
      }
      
      return last_obstacle_position;
    }

    double get_correct_diameter(const double& diameter, const Area& area) const
    {
      double correct_diameter = diameter;
      Position last_obstacle_position = get_last_obstacle(area);
      
      // Check if there is one before which distance is lower than the diameter
      while (last_obstacle_position != Position(-1, -1)) {
        
        double distance_to_obstacle = get_distance_between(actual_position_, last_obstacle_position);
        
        if ( distance_to_obstacle < diameter) { // We adjust
          correct_diameter = distance_to_obstacle;
        }
        last_obstacle_position = Position(-1, -1); // There would be no one else
      }
      
      return correct_diameter;
    }
    
    void divide_ways ()
    {
      // Miramos si por arriba hay obstáculos
      if (!check_obstacle_in_line(actual_position_, actual_position_.second, aisle_width)) {
        Way way_1 = create_way(actual_position_, actual_position_.second, aisle_width);
        possible_way_set.insert(way_1);
      }
      
      // Miramos si por abajo hay obstáculos
      if (!check_obstacle_in_line(actual_position_, 0, actual_position_.second)) {
        Way way_2 = create_way(actual_position_, 0, actual_position_.second);
        possible_way_set.insert(way_2);
      }
    }
    
    bool check_obstacle_in_line(Position actual_position, double bottom, double top)
    {
      bool obstacles_in_line = false;
      
      checked_obstacles.push_back(actual_position); // Start with actual_position
      
      Position nearest_obstacle;
      int nearest_obstacle_distance = MAX_INT;
      
      // Get nearest_obstacle
      for (int i = 0; i < obstacle_vector.size(); ++i) {
        if ( (obstacle_vector[i] == actual_position) || (obstacle_vector[i].first != actual_position.first) ) { // Si no están en línea
          continue;
        }
        if (std::count (checked_obstacles.begin(), checked_obstacles.end(), obstacle_vector[i]) != 0) { // Si ya lo hemos marcado
          continue;
        }
        if (obstacle_vector[i].second > top || obstacle_vector[i].second < bottom) { // Si no está entre los límites (siempre buscar al mismo lado)
          continue;
        }
        // If it's the NEAREST
        if ( nearest_obstacle_distance > get_distance_between(actual_position, obstacle_vector[i]) ) {
          nearest_obstacle = obstacle_vector[i];
          nearest_obstacle_distance = get_distance_between(actual_position, obstacle_vector[i]);
          
          obstacles_in_line = true;
        }
      }
      
      if (obstacles_in_line) {
        if (actual_position.second <= nearest_obstacle.second) {// Si ésta por arriba
          divide_ways_with_obstacle(nearest_obstacle, nearest_obstacle.second, top);
          
          if (!check_obstacle_in_line(actual_position, bottom, nearest_obstacle.second)) { // Miro por abajo también
              Way way_1 = create_way(actual_position, bottom, nearest_obstacle.second);
              possible_way_set.insert(way_1);
          }
        }
        else { // Si ésta por abajo
          divide_ways_with_obstacle(nearest_obstacle, bottom, nearest_obstacle.second);
          
          if (!check_obstacle_in_line(actual_position, nearest_obstacle.second, top)) { // Miro por arriba también
            Way way_2 = create_way(actual_position, nearest_obstacle.second, top);
            possible_way_set.insert(way_2);
          }
        }
      }
      
      return obstacles_in_line;
    }
    
    void divide_ways_with_obstacle (Position actual_position, double bottom, double top)
    {
      // Si no hay mas, divido las dos áreas que quedan
      if (!check_obstacle_in_line(actual_position, bottom, top)) {
        if (actual_position.second < top && bottom < actual_position.second) { 
          Way way_1 = create_way(actual_position, actual_position.second, top); 
          Way way_2 = create_way(actual_position, bottom, actual_position.second);
          
          possible_way_set.insert(way_1);
          possible_way_set.insert(way_2);
        }
        else {
          Way way_1 = create_way(actual_position, bottom, top);
          possible_way_set.insert(way_1);
        }
      }
    }
    
    Way create_way (Position actual_position, double bottom, double top)
    {
      // Si ya no hay mas hacia el lado que esté yendo calculo esa área
      double diameter = diameter_;
      
      if ( diameter > top - bottom) { diameter = top - bottom; } // If excedes current area width
      
      Area area = Area(bottom, top);
      
      diameter = get_correct_diameter(diameter, area);
      
      return Way (actual_position, diameter, area);
    }
  
};

void read_position(const std::string& line, int& x, int& y)
{
  std::istringstream buffer(line);
  std::istream_iterator<std::string> begin(buffer), end;

  std::vector<std::string> string_array(begin, end); // done!
  
  x = stoi(string_array[0]);
  y = stoi(string_array[1]);
}

void read_data () 
{
  std::string line;
  std::getline(std::cin,line);
  
  read_position(line, aisle_length, aisle_width);
  
  if ( (aisle_length < 0) || (aisle_length > 100) ) { throw; }
  if ( (aisle_width < 0) || (aisle_width > 100) ) { throw; }
  
  int obstacle_number = 0; std::cin >> obstacle_number; std::cin.ignore();
  
  if ( (obstacle_number < 0) || (obstacle_number > 100) ) { throw; }
  
  for (int i = 0; i < obstacle_number; i++) {
    std::getline(std::cin,line);
    
    int x, y;
    read_position(line, x, y);
    
    if ( (x < 0) || (x > aisle_length) ) { throw; }
    if ( (y < 0) || (y > aisle_width) ) { throw; }
    
    obstacle_vector.push_back(Position(x, y));
  }
}

int main () {
  int test_number; std::cin >> test_number; std::cin.ignore();
  
  for (int i = 1; i <= test_number; i++) {
    obstacle_vector.clear();
    possible_way_set.clear(); // Ways that can have the max diameter
    finished_way_set.clear(); // Ways that have finished
    
    read_data();
    
    Position actual_position (0, aisle_width / 2);
    double diameter = aisle_width;
    Area area (0, diameter); // The area is min and max y
  
    possible_way_set.insert(Way(actual_position, diameter, area));
    
    while ( !possible_way_set.empty() ) {
      // Extract the possible way with maximum diameter
      Way actual_way = *possible_way_set.rbegin(); 
      possible_way_set.erase(actual_way);
      
      actual_way.get_next_ways();
      
      double max_diameter_possible_way = (possible_way_set.empty()) ? 0 : (*possible_way_set.rbegin()).get_diameter();
      double max_diameter_finished_way = (finished_way_set.empty()) ? 0 : (*finished_way_set.rbegin()).get_diameter();
      
      // If the finished way's diameter is higuer than all the others don't continue
      if (max_diameter_finished_way >= max_diameter_possible_way) {
        possible_way_set.clear();
      }
    }
    
    // Take the finished way with maximum diameter
    diameter = (*finished_way_set.rbegin()).get_diameter(); 
    
    //Round diameter to 4 decimals
    diameter = round(diameter * 10000) / 10000;
    std::cout << "Maximum size in test case " << i << " is " << diameter << ".\n";
  }
}