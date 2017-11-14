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

bool debug = true;

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
      if (debug) std::cout << "\nObstacle (" << actual_position_.first << " , " << actual_position_.second << ")\n";
      
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
      // std::cout << "Last Obstacle (" << last_obstacle_position.first << " , " << last_obstacle_position.second << ")\n"; // (1,3)
      
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
      if ( check_obstacle_in_line(actual_position_, 0, aisle_width) ) { return; }
      
      create_way(actual_position_, actual_position_.second, aisle_width);
      create_way(actual_position_, 0, actual_position_.second);
      
      /* double distance_from_top = aisle_width - actual_position_.second;
      // double distance_from_bottom = actual_position_.second;
      
      // // Divide cases
      // double diameter_1 = diameter_;
      // double diameter_2 = diameter_;
      
      // if ( diameter_1 > distance_from_top) { diameter_1 = distance_from_top; } // If excedes limit
      // if ( diameter_2 > distance_from_bottom) { diameter_2 = distance_from_bottom; } // If excedes limit
      
      // Area area_1 = Area(actual_position_.second, actual_position_.second + diameter_1);
      // if (debug) std::cout << "Area1 (" << area_1.first << " , " << area_1.second << ")\n";
      // Area area_2 = Area(actual_position_.second - diameter_2, actual_position_.second);
      // if (debug) std::cout << "Area2 (" << area_2.first << " , " << area_2.second << ")\n";
      
      // // Adjust diameter
      
      // diameter_1 = get_correct_diameter(diameter_1, area_1);
      // if (debug) std::cout << "Diameter1 = " << diameter_1 << "\n";
      // diameter_2 = get_correct_diameter(diameter_2, area_2);
      // if (debug) std::cout << "Diameter2 = " << diameter_2 << "\n";
      
      // Way way_1 (actual_position_, diameter_1, area_1);
      // Way way_2 (actual_position_, diameter_2, area_2);
      
      // possible_way_set.insert(way_1);
      // possible_way_set.insert(way_2);
      */
    }
    
    bool check_obstacle_in_line(Position actual_position, double bottom, double top)
    {
      bool obstacles_in_line = false;
      
      checked_obstacles.push_back(actual_position); // Start with actual_position
      
      Position nearest_obstacle;
      int nearest_obstacle_distance = MAX_INT;
      
      for (int i = 0; i < obstacle_vector.size(); ++i) {
        if ( (obstacle_vector[i] == actual_position) || (obstacle_vector[i].first != actual_position.first) ) { // Si no están en línea
          continue;
        }
        if (std::count (checked_obstacles.begin(), checked_obstacles.end(), obstacle_vector[i]) != 0) { // Si ya lo hemos marcado
          continue;
        }
        if (obstacle_vector[i].second > top && obstacle_vector[i].second < bottom) { // Si no está entre los límites (siempre buscar al mismo lado)
          continue;
        }
        // If it's the NEAREST
        if ( nearest_obstacle_distance > get_distance_between(actual_position, nearest_obstacle) ) {
          nearest_obstacle = obstacle_vector[i];
          nearest_obstacle_distance = get_distance_between(actual_position, obstacle_vector[i]);
          
          obstacles_in_line = true;
        }
      }
      
      if (obstacles_in_line) {
        divide_ways_with_obstacle(nearest_obstacle, actual_position.second, top);
        divide_ways_with_obstacle(nearest_obstacle, bottom, actual_position.second);
      }

      return obstacles_in_line;
    }
    
    void divide_ways_with_obstacle (Position actual_position, double bottom, double top)
    {
      // Si no hay mas, divido las dos áreas que quedan
      if (!check_obstacle_in_line(actual_position, bottom, top)) {
        if (actual_position.second <= top && bottom <= actual_position.second) { 
          create_way(actual_position, actual_position.second, top); 
          create_way(actual_position, bottom, actual_position.second);
          return;
        }
      }
      create_way(actual_position, bottom, top);
    }
    
    void create_way (Position actual_position, double bottom, double top)
    {
      // Si ya no hay mas hacia el lado que esté yendo calculo esa área
      double diameter = diameter_;
      
      if ( diameter > top - bottom) { diameter = top - bottom; } // If excedes current area width
      
      Area area = Area(bottom, top);
      if (debug) std::cout << "Area (" << area.first << " , " << area.second << ")\n";
      
      diameter = get_correct_diameter(diameter, area);
      if (debug) std::cout << "Diameter = " << diameter << "\n";
      
      Way way (actual_position, diameter, area);
      
      possible_way_set.insert(way);
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
    
    if (debug) system("clear");
    
    if (debug) std::cout << "\n\nSTART\n\n";
    if (debug) std::cout << "First Obstacle (" << actual_position.first << " , " << actual_position.second << ")\n";
    if (debug) std::cout << "First Area (" << area.first << " , " << area.second << ")\n";
    if (debug) std::cout << "First Diameter = " << diameter << "\n\n";
  
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