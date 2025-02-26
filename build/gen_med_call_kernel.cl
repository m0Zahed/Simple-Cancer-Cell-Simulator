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
void change_state(__global struct point* cell, float injection_x, float injection_y)
{
    cell->r = 1.0f;
    cell->g = 1.0f;
    cell->b = 0.0f;
    cell->a = 1.0f;
    cell->type = -60;
    cell->index_of_injection[0] = (int) (injection_x+1)*COLUMNS/2;
    cell->index_of_injection[1] = (int) (injection_y+1)*ROWS/2;
}

__kernel void gen_med_call_kernel(
    __global struct point* points,
    int current_buffer, 
    float x_in, 
    float y_in, 
    float radius_sqr)
{   
    //if( !( (index >= COLUMNS*ROWS && current_buffer == PREVIOUS) || (index < COLUMNS && current_buffer == NEXT) ) )
    //{ 
        float x = get_global_id(0);
        float y = get_global_id(1);
        int index = COLUMNS*y + x; 
        float norm_y = (float) y/ROWS*2.0f-1.0f;  
        float norm_x = (float) x/COLUMNS*2.0f-1.0f; 
  
        if( (norm_y-y_in)*(norm_y-y_in) + (norm_x -x_in)*(norm_x-x_in) < radius_sqr )
        {   
            change_state(&points[index], x_in, y_in);           
        }
    //}
 
}
