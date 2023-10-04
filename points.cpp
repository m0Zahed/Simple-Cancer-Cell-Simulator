#include "points.h"
#include "point.h"
#include "shapes.h"
#include <cmath>
#include <thread>
#include <vector>

#define COLUMN_SIZE 1024
#define ROW_SIZE 768
#define POINT_SIZE sizeof(point)
#define NEXT 1
#define PREVIOUS 0

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
points::points(const float* input_, unsigned int data_buffer_size, unsigned int point_stride) : Shapes(input_,data_buffer_size), current_buffer(PREVIOUS)
{
   previous_data_pointer = (point*) input_;

   next_data_pointer = (previous_data_pointer + ROW_SIZE*COLUMN_SIZE);

   //set vertices
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, point_stride, (void*)0);

   glEnableVertexAttribArray(0);
    
   //set colors
   glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, point_stride, (void*)(2 * sizeof(float)));

   glEnableVertexAttribArray(1);

   glPointSize(1.0f);     
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
      int* index_of_injection = current_next_pixel->index_of_injection;

      int injection_column_index = (int) *index_of_injection;

      int injection_row_index = (int) *(index_of_injection+1); 

      int old_column_index = j;

      int old_row_index = i;

      int vector_param1 = old_column_index-injection_column_index;
      
      int vector_param2 = old_row_index-injection_row_index;

      float vector_magnitude = sqrt(vector_param1*vector_param1 + vector_param2*vector_param2);

      // vector_param1 = (int) vector_param1/vector_magnitude;

      // vector_param2 = (int) vector_param2/vector_magnitude;

      if( !((i+vector_param2 >= ROW_SIZE) 
         || (j+vector_param1 >= COLUMN_SIZE) 
         || (i+vector_param2 < 0) 
         || (j+vector_param1 < 0)) )
      {
         swap_pixel_positions(data_pointer_1, i, j, i+vector_param2, j+vector_param1);
      }
   }

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
      
      current_next_pixel->set_rgba(0.949f, 0.6f, 0.59f, 1.0f);
   }  
   else if((result<=-352) && (result>=-480) && (current_previous_pixel->type == 4))
   {
      current_next_pixel->type=-60;

      set_injection_point(current_next_pixel->index_of_injection, current_previous_pixel,i,j,1);

      current_next_pixel->set_rgba(1.0f, 1.0f, 0.0f, 1.0f);
   }

   return 0;
}

/**
 * @brief Swaps pixel on screen 
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
 * @brief Searchs around the specified coord to set the injections point of the new medicine cell. 
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
      *(injection_point+1) = *point_to_evaluate->index_of_injection+1;
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
         generate_medical_cells(next_data_pointer, x_pos, y_pos, num_cells,1);
   }
   else
   {
         generate_medical_cells(previous_data_pointer, x_pos, y_pos, num_cells,1); 
   } 
}

/**
 * @brief 
 * 
 * @param current_pixel 
 * @param x_pos 
 * @param y_pos 
 * @param number_of_cells 
 * @param i 
 * @param j 
 * @param depth Hard-Coded in for now, Terminates when number of cells reaches 1.
 */
void points::generate_medical_cells(point* current_pixel,int x_pos, int y_pos,int* number_of_cells, int depth)
{
   if((depth == 0)||(y_pos<0) || (y_pos>=ROW_SIZE) || (x_pos<0) || (x_pos>=COLUMN_SIZE) || (*number_of_cells==0))
   {
      return; 
   }

   point* current_pixel_ = current_pixel+y_pos*COLUMN_SIZE+x_pos;

   if(current_pixel_->type == -60)
   {
      return;
   }

   current_pixel->set_rgba(1.0f, 0.9f, 0.0f, 1.0f);

   *current_pixel->index_of_injection = x_pos;

   *(current_pixel->index_of_injection+1) = y_pos;

   *number_of_cells = *number_of_cells - 1;
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

   int grid_lenght = 3;

   int row_unit = ROW_SIZE/grid_lenght; int column_unit = COLUMN_SIZE/grid_lenght;

   std::vector<std::thread> thread_list;

   for(int base_i = 0; base_i<=ROW_SIZE; base_i+=row_unit)
   {
      for(int base_j = 0; base_j<=COLUMN_SIZE; base_j+=column_unit)
      {
         std::thread t1(&points::generate_next_buffer_,this,data_pointer_1, data_pointer_2, base_i, base_i+row_unit, base_j, base_j+column_unit);

         thread_list.push_back(std::move(t1));
      }
   }

   for(auto &func : thread_list)
   {
      func.join();
   }
}

/**
 * @brief An extension of the previous function
 * 
 * @param data_pointer pointer to the beginning of the data. 
 */
void points::generate_next_buffer_(point* data_pointer_1, point* data_pointer_2, int start_i, int end_i, int start_j, int end_j)
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


