#include <cstdlib>

#include <iostream>
#include <fstream>

#include <string>

using namespace std;

const string FILENAME = "./test/data.txt";

int main ()
{
   const int test_number = 75;
   const int max_obstacle_number = 15;
   const int max_aisle_length = 11;
   const int max_aisle_width = 11;
   
   srand (time(NULL));
   
   ofstream data_file (FILENAME, ios::out | ios::trunc);
   data_file << test_number << "\n";
   
   for (int i = 0; i < test_number; ++i) { // Por cada test
      int aisle_length = rand() % max_aisle_length;
      int aisle_width = rand() % max_aisle_width;
      data_file << aisle_length << " " << aisle_width << "\n";
      
      int obstacle_number = rand() % max_obstacle_number;
      data_file << obstacle_number << "\n";
      
      // std::cout << aisle_length << aisle_width << obstacle_number;
      // std::cin.get();
      
      if (obstacle_number == 0) { continue; }
      for (int j = 0; j < obstacle_number; ++j) { // Generamos obstáculos
         int x = rand() % (aisle_length+1);
         int y = rand() % (aisle_width+1);
         
         data_file << x << " " << y;
         if ( (i != test_number - 1) || (j != obstacle_number - 1) ) { data_file << "\n"; } // Menos al final
      }
   }
   
   data_file.close();
}