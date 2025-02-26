#include <math.h>

#define NEXT 1
#define PREVIOUS 0
#define POINT_SIZE 36
#define COLUMNS 1024
#define ROWS 768

struct point {
    float x, y;
    float r, g, b, a;
    int type; // Cancerous: 4, Non-Cancerous: 0, Medicated Cels: -60 
    int index_of_injection[2];
};

/**
* @brief Changes state of cell to medicine 
*/
void change_state(__global point* cell, __global float injection_x, __global float injection_y)
{
    cell->r = 1.0f;
    cell->g = 1.0f;
    cell->b = 0.0f;
    cell->a = 1.0f;

    cell->index_of_injection[0] = injection_x;
    cell->index_of_injection[1] = injection_y;
}

__kernel void gen_med_call_kernel(
    __global point* points,
    __global int current_buffer, 
    __global float x_in, 
    __global float y_in, 
    __global float radius_sqr)
{   
    //if( !( (index >= COLUMNS*ROWS && current_buffer == PREVIOUS) || (index < COLUMNS && current_buffer == NEXT) ) )
    //{ 
        int index = get_global_id(0);
        point* current_cell = &points[index]; 

        int y = index/COLUMNS + 1;
        int x = index%COLUMNS;

        float norm_y = y/COLUMNS*2.0f-1.0f;  
        float norm_x = x/ROWS*2.0f-1.0f; 
  
        // if( pow((norm_y- y_in), 2) + pow((norm_x- y_sqr), 2) < radius_sqr )
        // {   
            change_state(current_cell, x_in, y_in);           
        // }
    //}
 
}
