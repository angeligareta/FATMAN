#include <iostream>
#include <vector>
#include <set>

#include <iomanip> // setprecision
#include <sstream> // io
#include <iterator> // io
#include <string> // io

#include <cstdlib> //abs
#include <cmath> //sqrt
#include <algorithm> //count

#include <ctime>

using Position = std::pair<int,int>; /**< Pair of int used to store the x and y of a position. */
using Area = std::pair<double,double>; /**< Pair of double used to store the bottom and top of the area in a way. */
class Way;

const int MAX_INT = 101*101; /**< Variable used to calculate distance. */
int aisle_length; /**< Length of the aisle in a test. */
int aisle_width; /**< Width of the aisle in a test. */
bool first;

std::vector<Position> obstacle_vector; /**< Vector of obstacles in a test.*/
std::set<Way> possible_way_set; /**< Ways that can have the max diameter in a test. */
std::set<Way> finished_way_set; /**< Ways that have finished in a test. */

/**
 * @file main.cpp
 * @brief Program to solve the fatman problem of UVA.
 * @author Ángel Luis Igareta Herráiz.
 */

/**
 * @class Way
 * @brief Represent a way in the test.
 * Each time that we see an obstacle, instead of going to the bigger side, we store both ways. 
 * Doing that if we discover that the way we went was not the best one 
 * (comparing it to all the possible ways) we can simply go back and check another way.
 * Each way has a position, diameter and area, to allow us to keep moving from it.
 */
class Way {
  
  private:
  Position actual_position_; /**< Actual position of the way (the obstacle where it is). */
  Position last_position_;
  double diameter_; /**< Diameter of the fat person if we use this way. */
  Area area_; /**< Area that we are analyzing in that way. There will be a way for any different area. */
  std::vector<Position> checked_obstacles; /**< Obstacles that we have seen (used in @see check_obstacle_in_line()).*/
    
  public:
/**
  * Constructor of a way with the neccesary parameters to continue looking for the best path.
  * @param[in] actual_position Actual position of the way (the obstacle where it is).
  * @param[in] diameter Diameter of the fat person if we use this way.
  * @param[in] area Area that we are analyzing in that way. There will be a way for any different area.
  */
  Way(const Position& actual_position, const double& diameter, const Area& area):
  actual_position_(actual_position),
  diameter_(diameter),
  area_(area),
  checked_obstacles(std::vector<Position>())
  {}

/**
  * Copy constructor where we use the '=' overload. Used by set.
  * @param[in] way Way we want to copy.
  */ 
  Way(const Way& way)
  {
    *this = way;
  }

/**
  * Overload of the operator '='.
  * @param[in] way Way we want to copy.
  */ 
  Way& operator= (const Way& way)
  {
    actual_position_ = way.actual_position_;
    diameter_ = way.diameter_;
    area_ = way.area_;
    
    return *this;
  }
  
/**
  * Used to compare two ways.
  * @param[in] way_1 First way.
  * @param[in] way_2 Second way.
  */
  friend bool operator< (const Way& way_1, const Way& way_2)
  {
    // !(a<b) && !(b<a)
    if (way_1.diameter_ == way_2.diameter_) {
      if (way_1.area_ == way_2.area_) {
        if (way_1.actual_position_ == way_2.actual_position_) {
          return false;
        }
        return (way_1.actual_position_ < way_2.actual_position_);
      }
      return (way_1.area_ < way_2.area_);
    }
    return (way_1.diameter_ < way_2.diameter_);
  }

/**
  * Method used to know in which ways we can divide our problems.
  *   - First we look for the next obstacle in our way (attending to the area).
  *   - If there are no more obstacles we insert this one in the finished_way_set.
  *   - If there are more obstacles we diviide our way in divide_ways().
  */
  void get_next_ways ()
  {
    last_position_ = actual_position_;
    actual_position_ = get_next_obstacle(area_);
    
    if (actual_position_ == Position(-1,-1)) { 
      finished_way_set.insert(*this); 
    }
    else {
      divide_ways();
    }
  }

/**
  * Diameter getter.
  * @return diameter_ Diameter of the fat person if we use this way.
  */
  double get_diameter () const
  {
    return diameter_;
  }
  
  void set_diameter (const double& diameter)
  {
    diameter_ = diameter;
  }

  private:
/**
  * Returns the distance between two position of our way using the euclidian algorithm.
  * @param[in] position_1 First position.
  * @param[in] position_2 Second position.
  * @return Euclidian distance.
  */
  double get_distance_between(const Position& position_1, const Position& position_2) const
  {
    double x_distance_to_obstacle = abs(position_1.first - position_2.first);
    double y_distance_to_obstacle = abs(position_1.second - position_2.second);
    return sqrt(x_distance_to_obstacle*x_distance_to_obstacle + y_distance_to_obstacle*y_distance_to_obstacle);
  }

/**
  * Return the next obstacle to our actual_position in the way limited by an specific area.
  * We have to look all the obstacles and:
  *   - First check if it's fore the actual position. If obstacle.x is higher than actual_position_.x
  *   - Second check if the obstacle is in the area. If obstacle.y is higher than area.min and lower than area.max.
  *   - Third calculate nearest obstacle in the area.
  * Finally return it.
  * @param[in] area Pair of the top and bottom where we want to search the obstacle.
  * @return next_obstacle_position Next obstacle position.
  */
  Position get_next_obstacle(const Area& area) const
  {
    Position next_obstacle_position = Position(-1, -1);
    int nearest_obstacle_x = aisle_length + 1;
    double nearest_obstacle_y_distance = MAX_INT;
    
    for (int i = 0; i < obstacle_vector.size(); ++i) {
      // If First see limits
      if (first) {
        if ((obstacle_vector[i].second < area.first) || (obstacle_vector[i].second > area.second)) { continue; }
        if (obstacle_vector[i].first < actual_position_.first) { continue; }
      }
      else {
        if ((obstacle_vector[i].second < area.first) || (obstacle_vector[i].second > area.second)) { continue; }
        if (obstacle_vector[i].first <= actual_position_.first) { continue; }
      } 
      
      // Calcular siguiente x en el área.
      if (nearest_obstacle_x >= obstacle_vector[i].first) { // Busco 'x' mas cerca y si son iguales buscara la 'y' mas cercana
        nearest_obstacle_x = obstacle_vector[i].first;
        if ((next_obstacle_position.first != obstacle_vector[i].first) || 
            (nearest_obstacle_y_distance > abs(actual_position_.second - obstacle_vector[i].second))) {
          next_obstacle_position = obstacle_vector[i];
          nearest_obstacle_y_distance = abs(actual_position_.second - obstacle_vector[i].second);
        }
      }
    }
    if (first) { first = false; }
    return next_obstacle_position;
  }

/**
  * Return the last obstacle to our actual_position in the way limited by an specific area.
  * We have to look all the obstacles and:
  *   - First check if it's behind the actual position. If obstacle.x is lower than actual_position_.x
  *   - Second check if the obstacle is in the area. If obstacle.y is higher than area.min and lower than area.max
  *   - Third calculate nearest obstacle in the area
  * Finally return it.
  * @param[in] area Pair of the top and bottom where we want to search the obstacle.
  * @return last_obstacle_position Last obstacle position.
  */
  Position get_last_obstacle(const std::vector<Position>& last_obstacle_vector, bool look_on_line = false) const
  {
    Position last_obstacle_position = Position(-1, -1);
    int nearest_obstacle_x = -1;
    double nearest_obstacle_y_distance = MAX_INT;
    
    for (int i = 0; i < obstacle_vector.size(); ++i) {
      if ((obstacle_vector[i] == actual_position_) || (obstacle_vector[i].first >= actual_position_.first)) { continue; }
      if (std::count (last_obstacle_vector.begin(), last_obstacle_vector.end(), obstacle_vector[i]) != 0) { continue; }
      
      if (look_on_line) {
        if ((obstacle_vector[i].second < area_.first) || (obstacle_vector[i].second > area_.second)) { continue; }
      }
      else { 
        if ((obstacle_vector[i].second <= area_.first) || (obstacle_vector[i].second >= area_.second)) { continue; }
      }
      
      // Calcular siguiente x en el área.
      if (nearest_obstacle_x <= obstacle_vector[i].first) { // Busco 'x' mas cerca y si son iguales buscara la 'y' mas cercana
        nearest_obstacle_x = obstacle_vector[i].first;
        if ((last_obstacle_position.first != obstacle_vector[i].first) || 
            (nearest_obstacle_y_distance > abs(actual_position_.second - obstacle_vector[i].second))) {
          last_obstacle_position = obstacle_vector[i];
          nearest_obstacle_y_distance = abs(actual_position_.second - obstacle_vector[i].second);
        }
      }
    }
    
    return last_obstacle_position;
  }

/**
  * Reduce the current diameter if it is higuer than the distance to an obstacle at the back. 
  * Because we have to pass through the obstacles.
  * @param[in] diameter Diameter of the fat person if we use this way.
  * @param[in] area Area that we are analyzing in that way.
  * @return correct_diameter Diameter adjusted if neccesary.
  */
  double get_correct_diameter(bool area_changed) const
  {
    double correct_diameter = diameter_;
    std::vector<Position> last_obstacle_vector;
    
    // 1º Adjust to limits
    if ( correct_diameter > (area_.second - area_.first)) { correct_diameter = area_.second - area_.first; }
    
    // 2º Adjust to distance and if area changed
    // TODO: Can be optimized if we stop looking afer a distance... Order obstacles by distance?
    Position last_obstacle_position = get_last_obstacle(last_obstacle_vector, area_changed);
    
    while (last_obstacle_position != Position(-1, -1)) {
      double distance_to_obstacle = get_distance_between(actual_position_, last_obstacle_position);
      
      if (distance_to_obstacle < correct_diameter) { correct_diameter = distance_to_obstacle; }
      
      last_obstacle_vector.push_back(last_obstacle_position); // Marked
      last_obstacle_position = get_last_obstacle(last_obstacle_vector, area_changed);
    }
    
    return correct_diameter;
  }
  
  void adjust_area ()
  {
    //Optimization: Recalculate area depending on the diameter
    if (actual_position_.second == area_.first) { // If the area is above the actual_position
      area_ = Area(area_.first, area_.first + diameter_);
    }
    else if (actual_position_.second == area_.second) { // If the area is under the actual_position
      area_ = Area(area_.second - diameter_, area_.second);
    }  
  }
  
/**
  * Method that creates a way with the actual_position, bottom and top and 
  * adjusting the diameter if it's above the limits and calling get_correct_diameter().
  * @param[in] actual_position Actual position of the way (the obstacle where it is).
  * @param[in] bottom Bottom limit for searching the obstacle.
  * @param[in] top Top limit for searching the obstacle.
  * @return Way The created way.
  */  
  Way create_way (const Position& actual_position, const double& bottom, const double& top)
  {
    bool area_changed = false;
    
    // If it changes from bottom to top or viceversa, it means it has changed of area, we adjust to the nearest in line.
    // Error in detecting Area not solved.
    if ((area_.second == bottom) || (area_.first == top)) {
      area_changed = true;
    }
    // if ((last_position_.second > top) || (last_position_.second < bottom)) {
    //   area_changed = true;
    // }
    
    Way way = Way (actual_position, diameter_, Area(bottom, top));
    way.set_diameter(way.get_correct_diameter(area_changed));
    way.adjust_area();
    
    if (area_changed) { diameter_ = way.diameter_; }
  
    return way;
  }

/**
  * Divides the actual way in two of them checking first if there are more obstacles in the line.
  * It checks first up and then down. If there are obstacles in that side, the method check_obstacle_in_line
  * will be the attendant.
  */  
  void divide_ways ()
  {
    // Only if there are not more obstacles, insert in possible way. If there are, it would insert them.
    double diameter_backup = diameter_; // If the diameter changes, recover the last one.
    double top = aisle_width, bottom = 0;
    
    // Look up
    if (actual_position_.second != aisle_width) {
      Way way = create_way(actual_position_, actual_position_.second, aisle_width);
      if (!way.check_obstacle_in_line()) { possible_way_set.insert(way); }
    }
    
    diameter_ = diameter_backup;
    
    // Look down
    if (actual_position_.second != 0) {
      Way way = create_way(actual_position_, 0, actual_position_.second);
      if (!way.check_obstacle_in_line()) { possible_way_set.insert(way); }
    }
  }

/**
  * Check if there are more obstacles between the botom and the top of the current position. 
  *   - First it check the obstacle as this method is recursive for preventing an infinite loop.
  *   - Second it get's the nearest obstacle that is in line, marked and in the area.
  *   - If there have been obstacles we look if it's above our position or under it and call check_obstacle_in_line(). This is neccesary
  *     because the algorithm is recursive.
  * @param[in] actual_position Actual position of the way (the obstacle where it is).
  * @param[in] bottom Bottom limit for searching the obstacle.
  * @param[in] top Top limit for searching the obstacle.
  * @return obstacles_in_line Bool that indicates if the method found obstacles between botom and top of that line.
  */  
  bool check_obstacle_in_line()
  {
    bool obstacles_in_line = false;
    checked_obstacles.push_back(actual_position_);
    
    Position nearest_obstacle;
    int nearest_obstacle_distance = MAX_INT;
    
    // Get nearest obstacle in line
    for (int i = 0; i < obstacle_vector.size(); ++i) {
      if ((obstacle_vector[i] == actual_position_) || (obstacle_vector[i].first != actual_position_.first)) { continue; }
      if (std::count (checked_obstacles.begin(), checked_obstacles.end(), obstacle_vector[i]) != 0) { continue; }
      if (obstacle_vector[i].second > area_.second || obstacle_vector[i].second < area_.first) {continue; }
      
      if (nearest_obstacle_distance > get_distance_between(actual_position_, obstacle_vector[i])) {
        nearest_obstacle = obstacle_vector[i];
        nearest_obstacle_distance = get_distance_between(actual_position_, obstacle_vector[i]);
        obstacles_in_line = true;
      }
    }
    
    if (obstacles_in_line) {
      if (actual_position_.second <= nearest_obstacle.second) {  // Look up
        // Create way between obstacles
        Way way_1 = create_way(actual_position_, actual_position_.second, nearest_obstacle.second);
        possible_way_set.insert(way_1);
      
        if (nearest_obstacle.second != aisle_width) {
          double top = nearest_obstacle.second + diameter_;
          if (top > aisle_width) { top = aisle_width; }
          
          Way way_2 = create_way(nearest_obstacle, nearest_obstacle.second, top);
          if (!way_2.check_obstacle_in_line()) { possible_way_set.insert(way_2); }
        }
      }
      else { // Look down
        // Create way between obstacles
        Way way_1 = create_way(actual_position_, nearest_obstacle.second, actual_position_.second);
        possible_way_set.insert(way_1);
        
        if (nearest_obstacle.second != 0) {
          double bottom = nearest_obstacle.second - diameter_;
          if (bottom < 0) { bottom = 0; }
          
          Way way_2 = create_way(nearest_obstacle, bottom, nearest_obstacle.second);
          if (!way_2.check_obstacle_in_line()) { possible_way_set.insert(way_2); } 
        }
      }
    }
    
    return obstacles_in_line;
  }
};

/** 
  * @fn read_position
  * @brief Method for reading a Position. 
  */
void read_position(const std::string& line, int& x, int& y)
{
  std::istringstream buffer(line);
  std::istream_iterator<std::string> begin(buffer), end;
  std::vector<std::string> string_array(begin, end); // done!
  
  if (string_array.size() != 2) { throw std::runtime_error(""); }
  
  x = stoi(string_array[0]);
  y = stoi(string_array[1]);
}

bool equal (Position position_1, Position position_2) {
  return (position_1 == position_2);
}

/** 
  * @fn read_data
  * @brief Method that read data from input in each test and modify the aisle_length, aisle_width and obstacle_vector.
  */
void read_data () 
{
  std::string line;
  std::getline(std::cin,line);
  
  try { read_position(line, aisle_length, aisle_width); }
  catch (...) { throw; }
  
  if ( (aisle_length < 0) || (aisle_length > 100) ) { throw std::runtime_error(""); }
  if ( (aisle_width < 0) || (aisle_width > 100) ) { throw std::runtime_error(""); }
  
  int obstacle_number = 0; std::cin >> obstacle_number; std::cin.ignore();
  
  if ( (obstacle_number < 0) || (obstacle_number > 100) ) { throw std::runtime_error(""); }
  
  if (obstacle_number != 0) {
    for (int i = 0; i < obstacle_number; i++) {
      std::getline(std::cin,line);
      
      int x, y;
      try { read_position(line, x, y); }
      catch (...) { throw; }
      
      if ( (x < 0) || (x > aisle_length) ) { throw std::runtime_error(""); }
      if ( (y < 0) || (y > aisle_width) ) { throw std::runtime_error(""); }
      
      obstacle_vector.push_back(Position(x, y));
    }
    std::sort(obstacle_vector.begin(), obstacle_vector.begin());
    std::unique (obstacle_vector.begin(), obstacle_vector.end(), equal);
  }
}

int main () {
  int test_number; std::cin >> test_number; std::cin.ignore();
  if ( (test_number < 0) || (test_number > 100) ) { return 0; }
  
  std::vector<double> solution_vector(test_number);
  
  const clock_t begin_time = clock();
  // do something
  
  for (int i = 0; i < test_number; i++) {
    // Clear the vectors before using
    obstacle_vector.clear();
    possible_way_set.clear();
    finished_way_set.clear();
    first = true;
    
    try { read_data(); }
    catch (...) { return 0; }
    
    Position actual_position (0, aisle_width / 2);
    double diameter = aisle_width;
    Area area (0, diameter); // The area is min and max y
    possible_way_set.insert(Way(actual_position, diameter, area));
    
    while ( !possible_way_set.empty() ) {
      // Extract the possible way with maximum diameter
      Way actual_way = *possible_way_set.rbegin(); 
      possible_way_set.erase(actual_way);
      
      // Create new ways for that way (when finding an obstacle)
      actual_way.get_next_ways();
      
      double max_diameter_possible_way = (possible_way_set.empty()) ? 0 : (*possible_way_set.rbegin()).get_diameter();
      double max_diameter_finished_way = (finished_way_set.empty()) ? 0 : (*finished_way_set.rbegin()).get_diameter();
      
      // If the finished way's diameter is higher than all the others don't continue
      if (max_diameter_finished_way >= max_diameter_possible_way) {
        possible_way_set.clear();
      }
    }
    
    // Take the finished way with maximum diameter
    diameter = (finished_way_set.empty()) ? 0 : (*finished_way_set.rbegin()).get_diameter();
    
    //Round diameter to 4 decimals
    solution_vector[i] = round(diameter * 10000) / 10000;
  }
  
  // Show results
  for (int i = 0; i < test_number; i++) {
    std::cout << "Maximum size in test case " << i+1 << " is " << std::setprecision(4) << std::fixed << solution_vector[i] << ".\n";
  }
  
  // Runtime
  // double elapsed_time = float(clock () - begin_time) /  CLOCKS_PER_SEC;
  // std::cout << "Average Runtime per test: " << (elapsed_time / test_number)  << ".\n";
  // std::cout << "Total Runtime: " << elapsed_time << ".\n";
}