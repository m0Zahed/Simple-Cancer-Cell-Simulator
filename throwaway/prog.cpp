#include <cstdlib>
#include <initializer_list>
#include <iostream>
#include <ostream>
#include <string>
#include <typeinfo>
#include <vector>
#include <random>

#define ROW_SIZE 768
#define COLUMN_SIZE 1024

void test_func(void* something)
{
  std::cout<< sizeof(something) << std::endl;  
}

void test_func_2(float* p)
{
  p = new float[2]; // A new pointer is assigned to p, this is only reflected in this scope. 
  *(p) = 0.0;
  *(p+1) = 1.0;
}

void test_func_3(float* &p)
{
  p = new float[2]; //The value assigned to the pointer is reflected even outside the function.  
  *(p) = 0.0;
  *(p+1) = 1.0;
}

class point{
  public:
      float x, y;
      unsigned char r, g, b, a;
      point() {x=0.0f;y=0.0f;}
      point(float a, float b) : x(a), y(b) {}
};

void initialise_grid(std::vector<std::vector<point>>& grid )
{   
    int i = 0, j = 0;
    while(i < 2)
    {   
        std::vector<point> columns;

        float y_coord;
        y_coord = i/2.0f*2.0f-1.0f;

        while(j < 2)
        {   
            float x_coord;
            x_coord = j/2.0f*2.0f-1.0f;
            columns.push_back(point(x_coord, y_coord));
            j++;   
        }
        
        grid.push_back(columns);
        i++;
    }
}

inline void initialise_grid_2(point* &p)
{ 
  std::mt19937 engine(std::random_device{}());
  std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

  p = new point[786432]; 
  for(int i = 0; i < ROW_SIZE; i++)
  {
    for(int j = 0; j < COLUMN_SIZE; j++)
    {
      (p + i*COLUMN_SIZE + j)->x = dist(engine);
      (p + i*COLUMN_SIZE + j)->y = dist(engine);  
    }
  }
}

int main(int argc, char* argv[])
{
//   struct point point_grid[2][3];

//   point_grid[0][0].x = 1.0f; 
//   point_grid[0][0].y = 1.0f;

//   point_grid[0][1].x = -1.0f; 
//   point_grid[0][1].y = 1.0f; 

//   point_grid[1][0].x = -1.0f; 
//   point_grid[1][0].y = -1.0f; 

//   point_grid[1][1].x = 1.0f; 
//   point_grid[1][1].y = -1.0f; 
  
//   cout << &point_grid << endl;
//   cout << &point_grid[0][0].x << endl;
//   cout << &point_grid[0][1].x << endl;
//   cout << &point_grid[1][0].x << endl;
//   cout << &point_grid[1][1].x << endl;
//   cout << "Size of points grid: " << sizeof(point_grid) << endl;
//   cout << sizeof(point_grid[0][0].x) << endl;

//   float* item = &point_grid[0][0].x;
//   cout << sizeof(item) << endl;
//   cout << sizeof(point) << endl;
//   test_func(&point_grid[0][0]);
//   cout << typeid(point_grid).name() <<endl;

point a(1.0,1.0);

std::cout << "The size of the whole point Object: "<< sizeof(a) << std::endl;
std::cout << "The address of a: "<< &a << std::endl;
std::cout << "The address of the a.x: "<< &a.x << std::endl;
std::cout << "The address of a.y: "<< &a.y << std::endl;

point* test_points;
initialise_grid_2(test_points);


std::cout << "The position of the vector: " << test_points << std::endl;
std::cout << "The position of the first element: " << test_points+0*COLUMN_SIZE+0%ROW_SIZE << std::endl;
std::cout << "The position of the second element: " << test_points+0*COLUMN_SIZE+1%ROW_SIZE << std::endl;
std::cout << "The position of the second row first element: " << test_points+1*COLUMN_SIZE+0%ROW_SIZE << std::endl;
std::cout << "The position of the second row second element: " << test_points+1*COLUMN_SIZE+1%ROW_SIZE << std::endl;
std::cout << "The position of the second row second element x: " << (test_points+1*COLUMN_SIZE+1)->x<< std::endl;

delete [] test_points;

//testing some stuff for obsidian notes
std::cout << "Passing a pointer as a reference and value\n";
std::cout << "Whenever you pass a pointer as a reference,\n this allows you to change the address where the address is pointer is pointing \n to even outisde the scope.\n";

float test_float = 10.0f;
float* f_pointer;

f_pointer = &test_float;

//function with pointer pass by value
std::cout << "float pointer after pass by value\n" << std::endl;
std::cout << "Address where pointer is pointing before operation" << f_pointer <<std::endl;

test_func_2(f_pointer);

std::cout << "Address where pointer is pointing after operation" <<  f_pointer << std::endl;

//function with pointer as pass by reference

std::cout << "float pointer after pass by reference\n" << std::endl;
std::cout << "Address where pointer is pointing before operation" << f_pointer <<std::endl;

test_func_3(f_pointer);

std::cout << "Address where pointer is pointing after operation" <<  f_pointer << std::endl;

}