#include "points.h"
#include "point.h"
#include "shapes.h"
#include "tracker.h"
#include "tracker.h"
#include <cmath>
#include <thread>
#include <vector>

#define COLUMN_SIZE 1024
#define ROW_SIZE 768
#define POINT_SIZE sizeof(point)
#define NEXT 1
#define PREVIOUS 0

//BUG: IN order to fix the medicince cell logic. 
//
//Idea: find arctan2() from the gl math library and then:
//Perform a switch case statement to map angle to predefined movement paths. if 
//new medicine cells are converted they are assessed based on the current quadrant they are present in. 


extern tracker observer;

/**
 * @brief Wrapper to bypass private inheritance.
 * 
 * @return int 
 */
int points::get_buffer()
{
   int buffer = Shapes::get_buffer();
   return buffer;
}

/**
 * @brief Determines the quadrant of a point.  
 * 
 * @param x 
 * @param y 
 * @return points::Quadrant 
 */
points::Quadrant points::determine_quadrant(int x, int y) {
    if (x > 0 && y > 0) return Q1;
    if (x < 0 && y > 0) return Q2;
    if (x < 0 && y < 0) return Q3;
    if (x > 0 && y < 0) return Q4;
    if (x == 0 && y == 0) return ORIGIN;
    if (x == 0) return Y_AXIS;
    if (y == 0) return X_AXIS;
    return ORIGIN;  // This line should never be reached.
}

/**
 * @brief Sets the type of the point at a given index location 
 * 
 * @param column_number Column number where the point is located
 * @param row_number row number where the point is located
 * @param type The type to be change to, options are -60(nedicated cells), 4(cancer cells), 0(regular cells)
 */
void points::set_point_type(int column_number, int row_number, int type)
{
   if((type != -60 || type != 4 || type != 0))
   {
      throw "pixel type is incorrect, recheck pixel type";
   }

   point* pixel;

   if(current_buffer == PREVIOUS)
   {
      pixel = next_data_pointer + row_number*COLUMN_SIZE + column_number;
   }
   else
   {
      pixel = previous_data_pointer + row_number*COLUMN_SIZE + column_number;
   }

   pixel->type = type;

   pixel->index_of_injection[0] = column_number;

   pixel->index_of_injection[1] = row_number;
}

/**
 * @brief Construct a new points::points object, this object inherits the shape object and extends it.
 *
 * @par The following intialises a VBO of size 2*ROW_SIZE*COLUMN_SIZE
 * 
 * 
 * @param input_ The input is a float pointer to a list of vertices. This data is passed to the data buffer. 
 * @param data_buffer_size This is the total size of the data in bytes. Typically (array length * size of one element in bytes)
 * @param point_stride Usually the length of each element. More concretely the stride lenght in order to access the next value of the attribute in the VAO.
 */
points::points(const float* input_, unsigned int data_buffer_size, unsigned int point_stride) : Shapes( input_,data_buffer_size), current_buffer(PREVIOUS),   previous_data_pointer((point*) input_),   next_data_pointer(previous_data_pointer + ROW_SIZE*COLUMN_SIZE)
{
   //set vertices
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, point_stride, (void*)0);

   glEnableVertexAttribArray(0);
    
   //set colors
   glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, point_stride, (void*)(2 * sizeof(float)));

   glEnableVertexAttribArray(1);

   glPointSize(1.0f);     
}


/**
 * @brief Swap positions: The input swaps points based on specified index position. Note: Does not check if the position exceeds the borders. 
 * 
 * @param current_point 
 * @param i_1 
 * @param j_1 
 * @param i_2 
 * @param j_2 
 */
void points::swap_pixel_positions(point* current_point, int i_1, int j_1, int i_2, int j_2)
{
   point temporary;

   temporary = *(current_point + i_1*COLUMN_SIZE + j_1);
   
   *(current_point + i_1*COLUMN_SIZE + j_1) = *(current_point + i_2*COLUMN_SIZE + j_2);

   *(current_point + i_2*COLUMN_SIZE + j_2) = temporary;
}

/**
 * @brief Swaps positions: The input is two points Note: Does not check if the position exceeds the borders. 
 * 
 * @param point_1 
 * @param point_2 
 */
void points::swap_pixel_positions(point* point_1, point* point_2)
{
   point temporary = *(point_2);
   
   (point_2)->set_rgba(point_1->r, point_1->g, point_1->b, point_1->a);

   point_2->type = point_1->type;

   *(point_2->index_of_injection) = *(point_1->index_of_injection);
   
   *(point_2->index_of_injection + 1) = *(point_1->index_of_injection + 1);
   
   (point_1)->set_rgba(temporary.r, temporary.g, temporary.b, temporary.a);

   point_1->type = temporary.type;

   *(point_1->index_of_injection) = temporary.index_of_injection[0];
   
   *(point_1->index_of_injection + 1) = temporary.index_of_injection[1];
   
}

/**
 * @brief Swaps positions: The input is one specific point and another absolute position on screen.
   Does not swap with pixels in the next buffer, but swaps pixels on the current buffer.
   Note: Does not check if the position exceeds the borders. 
 * 
 * @param point_1 
 * @param i IMPORTANT THIS IS THE y coord
 * @param j IMPORTANT THIS IS THE x coord
 */
void points::swap_pixel_positions(point* point_1, int i, int j)
{  
   point* point_2;

   if(current_buffer == NEXT)
   {
      point_2 = next_data_pointer + i*COLUMN_SIZE + j;
   }
   else
   {
      point_2 = previous_data_pointer + i*COLUMN_SIZE + j;
   }

   point temporary = *(point_2);
   
   (point_2)->set_rgba(point_1->r, point_1->g, point_1->b, point_1->a);

   point_2->type = point_1->type;

   *(point_2->index_of_injection) = *(point_1->index_of_injection);
   
   *(point_2->index_of_injection + 1) = *(point_1->index_of_injection + 1);
   
   (point_1)->set_rgba(temporary.r, temporary.g, temporary.b, temporary.a);

   point_1->type = temporary.type;

   *(point_1->index_of_injection) = temporary.index_of_injection[0];
   
   *(point_1->index_of_injection + 1) = temporary.index_of_injection[1];
   
}

/**
 * @brief Searchs around the specified coord to set the injections point of the new medicine cell.
 Note: Function depracated. 
 * 
 * @param injection_point pointer to integer array of the injecton point.  
 * @param point_to_evaluate The point to be evaluated. 
 * @param i 
 * @param j 
 * @param depth 
 */
void points::set_injection_point(int* injection_point, point* point_to_evaluate, int i , int j,int depth)
{
   if((i<0) || (i>=ROW_SIZE) || (j<0) || (j>=COLUMN_SIZE)) 
   {
      return;
   }
   
   if(point_to_evaluate->type == -60)
   {
      *injection_point = *point_to_evaluate->index_of_injection;
      *(injection_point+1) = *(point_to_evaluate->index_of_injection+1);
      return;
   }

   if(depth==0)
   {
      return;
   }

   set_injection_point(injection_point,point_to_evaluate,i-1,j+1,depth-1);
   set_injection_point(injection_point,point_to_evaluate,i-1,j,depth-1);
   set_injection_point(injection_point,point_to_evaluate,i-1,j-1,depth-1);
   set_injection_point(injection_point,point_to_evaluate,i,j+1,depth-1);
   set_injection_point(injection_point,point_to_evaluate,i,j-1,depth-1);
   set_injection_point(injection_point,point_to_evaluate,i+1,j+1,depth-1);
   set_injection_point(injection_point,point_to_evaluate,i+1,j,depth-1);
   set_injection_point(injection_point,point_to_evaluate,i+1,j-1,depth-1);

}

/**
 * @brief Evaluates the next position of the medicine cell. 
 * 
 * @param med_cell 
 */
void points::evaluate_next_med_cell_position(point* data_pointer, int y_, int x_)
{

   point* med_cell = data_pointer + y_*COLUMN_SIZE + x_;

   Quadrant q = determine_quadrant(x_-*(med_cell->index_of_injection), y_-*(med_cell->index_of_injection+1));   

   int x, y;

   switch(q) {
         case Q1: 
            x = 1; y = 1; break;

         case Q2: 
            x = -1; y = -1; break;            
         
         case Q3: 
            x = -1; y = -1; break;
         
         case Q4: 
            x = 1; y = -1; break;
         
         case X_AXIS: 
            x = 1; y = 0; break;
         
         case Y_AXIS: 
            x = 0; y = 1; break;
         
         case ORIGIN: 
            x = 0; y = 0;  break;
      }
   
   //I'll proceed to jump out the window, if this shit does not work. xd. 
   //Gotta refactor all code that talks about positions on the screen to x and y instead of i and j, Use i and j only for indices. 

   int new_x = x_ + x; 

   int new_y = y_ + y;

   if(new_x<0 || new_x>=COLUMN_SIZE || new_y<0 || new_y>=ROW_SIZE)
   {
      swap_pixel_positions(med_cell, med_cell);
   }
   else
   {
      swap_pixel_positions(med_cell, y_+y, x_+x);  
   }

   return;
}

/**
 * @brief Generates the medicine cells around the specified injection point. 
 * 
 * @param x_pos 
 * @param y_pos 
 * @param number_of_cells 
 */
void points::generate_medical_cells(int x_pos, int y_pos, int number_of_cells)
{  
   int* num_cells = &number_of_cells;

   if(current_buffer == NEXT)
   {
         generate_medical_cells(next_data_pointer, x_pos, y_pos, num_cells);
   }
   else
   {
         generate_medical_cells(previous_data_pointer, x_pos, y_pos, num_cells); 
   } 
}

/**
 * @brief The generate medical cells generates medical cells in the following manner-
 * 
 * @parblock 
 * The idea is to group the cells in a square where they are to be generated. 
   The square of the number of cells is evaluated. Then a cube of that square sized is generated, 
   if we enecounter the borders of the simulation, the rest of the 
   We have an offset because shift the beginning positions of where the loop starts by (side/2,side/2). So that 
   the injection point is in the center. 
 * @endparblock
 *
 * 
 * @param current_pixel 
 * @param x_pos 
 * @param y_pos 
 * @param number_of_cells 
 * @param i 
 * @param j 
  */
void points::generate_medical_cells(point* current_pixel,int x_pos, int y_pos, int* number_of_cells)
{
   int side = find_side_size(*number_of_cells);

   int offset = side%2 == 0 ? side/2 : (side+1)/2;
   
   int begin_y = (y_pos-offset)%ROW_SIZE;

   int begin_x = (x_pos-offset)%COLUMN_SIZE;

   int end_y = (y_pos+offset)>=ROW_SIZE ? ROW_SIZE-1 : y_pos+offset;
   
   int end_x = (x_pos+offset)>=COLUMN_SIZE ? COLUMN_SIZE-1 : x_pos+offset;

   for(int i = begin_y; i < end_y; i++)
   {
      for(int j = begin_x; j < end_x; j++)
      {  
         if(*number_of_cells==0) break;

         point* pixel_to_be_evaluated = current_pixel + i*COLUMN_SIZE + j;

         pixel_to_be_evaluated->type=-60;

         pixel_to_be_evaluated->set_rgba(1.0f, 1.0f, 0.0f, 1.0);

         *pixel_to_be_evaluated->index_of_injection = x_pos; 

         *(pixel_to_be_evaluated->index_of_injection+1) = y_pos; 

         *(number_of_cells)-=1;
      }

      if(*number_of_cells==0) break;

   }
   
}

/**
 * @brief 
 * 
 * @param no_cells 
 */
int points::find_side_size(int no_cells)
{
   int prelim_side_size = (int) sqrt(no_cells)+1;

   return prelim_side_size;
}

/**
 * @brief The following function calculates the next state of each pixel on a new buffer swaps it with the 
 *        old buffer. 
 *
 * @par How this works
 * There are two buffers - PREVIOUS_BUFFER and NEXT_BUFFER(An array containing the points/pixel. They are completely Identical in shape and size)
   On initialisation the PREVIOUS_BUFFER contains the initial data. Next for each pixel in the NEXT_BUFFER we evaluate the surrounding pixels of the 
   corresponding pixel in the PREVIOUS_BUFFER and evaluate its type(Cancer/Medicated/Normal).  
 * 
 *  
 */
void points::generate_next_buffer()
{  
   point* data_pointer_1, *data_pointer_2;

   if(current_buffer == NEXT)
   {  
      data_pointer_1 = next_data_pointer;

      data_pointer_2 = previous_data_pointer;
      
      switch_buffer(PREVIOUS);
   }
   else
   {
      data_pointer_1 = previous_data_pointer;

      data_pointer_2 = next_data_pointer;

      switch_buffer(NEXT);
   }
   int grid_length = 3;
   int row_unit = ROW_SIZE / grid_length;
   int column_unit = COLUMN_SIZE / grid_length;

   // Using blocked_range2d to iterate over 2D data
   tbb::parallel_for(
      tbb::blocked_range2d<int, int>(0, ROW_SIZE, row_unit, 0, COLUMN_SIZE, column_unit),
      [this, &data_pointer_1, &data_pointer_2](const tbb::blocked_range2d<int, int>& r) {
         points::generate_next_buffer_ext( data_pointer_1, data_pointer_2, r.rows().begin(), r.rows().end(), r.cols().begin(), r.cols().end());
      }
   );

   std::cout << "Parallel for completed." << std::endl;

   /* The follow code implements multi-threading using the standard threads library. */

   // int grid_lenght = 3;

   // int row_unit = ROW_SIZE/grid_lenght; int column_unit = COLUMN_SIZE/grid_lenght;

   // std::vector<std::thread> thread_list;

   // for(int base_i = 0; base_i<=ROW_SIZE; base_i+=row_unit)
   // {
   //    for(int base_j = 0; base_j<=COLUMN_SIZE; base_j+=column_unit)
   //    {
   //       std::thread t1(&points::generate_next_buffer_,this,data_pointer_1, data_pointer_2, base_i, base_i+row_unit, base_j, base_j+column_unit);

   //       thread_list.push_back(std::move(t1));
   //    }
   // }

   // for(auto &func : thread_list)
   // {
   //    func.join();
   // }
}

/**
 * @brief An extension of calling function generate_next_buffer()
 * 
 * @param data_pointer pointer to the beginning of the data. 
 */
void points::generate_next_buffer_ext(point* data_pointer_1, point* data_pointer_2, int start_i, int end_i, int start_j, int end_j)
{
   for(int i = start_i; i < end_i; i++)
   {
      for(int j = start_j; j < end_j; j++)
      {
         evaluate_new_pixel(data_pointer_1,data_pointer_2,i,j,1);
      }
   }
}

/**
 * @brief Evaluates the color of a new pixel by evaluating the probability that it may be 
 *        transformed into  cancer or medicated cell, recursively. 
 *
 * @parblock 
 *        The types are enumerated as following, (cancer=4) (medicated=-1)(regular=0)
 *        Consider the operation n1*type1 + n2*type2 + n3*type3 = result
 *        An example- 4*6 + 2*-60 + 0*0 = -94. There should be a 
 *        one to one mapping from (n1,type1,n2,type2,n3type3) --> (result).
 *        Thus, selecting (type 1,2 and 3) as (4,-1 and 0) gives us a one to one mapping.
 * @endparblock
 * 
 * 
 * @param data_pointer_1 A pointer of type :point* to the pixels on the data buffer 1
 * @param data_pointer_2 A pointer of type :point* to the pixels on the data buffer 2
 * @param i 
 * @param j 
 * @param depth the depth to which the function traverses the node. It is to be initialised to 1.
 */
int points::evaluate_new_pixel(point* data_pointer_1, point* data_pointer_2, int i, int j, int depth)
{
   if((i<0) || (i>=ROW_SIZE) || (j<0) || (j>=COLUMN_SIZE)) 
   {
      return 0;
   }

   point *current_previous_pixel, *current_next_pixel;

   current_previous_pixel = data_pointer_1 + i*COLUMN_SIZE + j;

   current_next_pixel = data_pointer_2 + i*COLUMN_SIZE + j;
   
   if(depth==0)
   {
      return current_previous_pixel->type;
   }

   if(current_previous_pixel->type==-60)
   {
      evaluate_next_med_cell_position(data_pointer_1, i, j);
   }
    
//   static const int di[8] = {-1, -1, -1,  0, 0,  1,  1,  1};
//   static const int dj[8] = {-1,  0,  1, -1, 1, -1,  0,  1};

//   int result = tbb::parallel_reduce(
//       tbb::blocked_range<size_t>(0, 8),
//       0,  // Initial result value
//       [&](const tbb::blocked_range<size_t>& r, int init_sum) -> int {
//           for (size_t k = r.begin(); k != r.end(); ++k) {
//               init_sum += evaluate_new_pixel(data_pointer_1, data_pointer_2, i + di[k], j + dj[k], depth-1);
//           }
//           return init_sum;
//       },
//       [](int x, int y) -> int {  // Combine the results
//           return x + y;
//       }
//   );

  int result = evaluate_new_pixel(data_pointer_1,data_pointer_2, i-1, j-1, depth-1)
              +evaluate_new_pixel(data_pointer_1,data_pointer_2, i-1, j, depth-1)
              +evaluate_new_pixel(data_pointer_1,data_pointer_2, i-1, j+1, depth-1)
              +evaluate_new_pixel(data_pointer_1,data_pointer_2, i, j-1, depth-1)
              +evaluate_new_pixel(data_pointer_1,data_pointer_2, i, j+1, depth-1)
              +evaluate_new_pixel(data_pointer_1,data_pointer_2, i+1, j-1, depth-1)
              +evaluate_new_pixel(data_pointer_1,data_pointer_2, i+1, j, depth-1)
              +evaluate_new_pixel(data_pointer_1,data_pointer_2, i+1, j+1, depth-1);
  

   if((result==-96)||(result==-32)||((result>=24)&&(result<=32)))
   {
      current_next_pixel->type=4;
      
      current_next_pixel->set_rgba(1.0f, 0.0f, 0.0f, 1.0f);

      if(current_previous_pixel->type == 0)
      {
         observer.decrement_value(observer.normal_cell);

         observer.increment_value(observer.cancer_cell);
      }
      else if( current_previous_pixel->type == -60)
      {
         observer.decrement_value(observer.medicine_cell);

         observer.increment_value(observer.cancer_cell);
      }

   }  
   else if((result<=-352)&& (result>=-480) && (current_previous_pixel->type == 4))
   {
      current_next_pixel->type=-60;

      set_injection_point(current_next_pixel->index_of_injection, current_previous_pixel,i,j,1);

      current_next_pixel->set_rgba(1.0f, 1.0f, 0.0f, 1.0f);

      observer.decrement_value(observer.cancer_cell);

      observer.increment_value(observer.medicine_cell);
   }
   else
   {
      current_next_pixel->type = current_previous_pixel->type;

      float red = current_previous_pixel->r;

      float green = current_previous_pixel->g;

      float blue = current_previous_pixel->b;

      float alpha = current_previous_pixel->a;

      current_next_pixel->set_rgba(red, green, blue, alpha);
   }  

   return 0;
}

/**
 * @brief All the operation to switch a buffer are contained below. 
 * 
 * @param buffer Switches the buffer to the specified buffer. 
 */
void points::switch_buffer(unsigned int buffer)
{  

   size_t position_offset, color_offset;

   if(buffer == PREVIOUS)
   {  
      position_offset = 0;

      color_offset = 2 * sizeof(float);

      glBufferSubData(GL_ARRAY_BUFFER, position_offset,ROW_SIZE*COLUMN_SIZE*POINT_SIZE, previous_data_pointer);

      //set vertices
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, POINT_SIZE, (void*) position_offset);

      glEnableVertexAttribArray(0);
      
      //set colors
      glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, POINT_SIZE, (void*) color_offset);

      glEnableVertexAttribArray(1);
   }
   else
   {  
      position_offset = ROW_SIZE*COLUMN_SIZE*POINT_SIZE;

      color_offset = position_offset + (2*sizeof(float));

      glBufferSubData(GL_ARRAY_BUFFER, position_offset, ROW_SIZE*COLUMN_SIZE*POINT_SIZE, next_data_pointer);

      //set vertices
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, POINT_SIZE, (void*) position_offset);

      glEnableVertexAttribArray(0);
      
      //set colors
      glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, POINT_SIZE, (void*) color_offset);

      glEnableVertexAttribArray(1);
   }

   current_buffer = buffer;
}


