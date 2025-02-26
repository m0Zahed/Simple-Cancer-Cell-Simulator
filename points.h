#ifndef CADC9390_C1B8_4CE6_AFCC_9162502C338A
#define CADC9390_C1B8_4CE6_AFCC_9162502C338A

#include "shapes.h"
#include "point.h"
#include "tracker.h"
#include <tbb/parallel_for.h>
#include <tbb/blocked_range2d.h>
#include <tbb/parallel_reduce.h>

class points : private Shapes{
    private:

        enum Quadrant {
            Q1 = 1,
            Q2 = 2,
            Q3 = 3,
            Q4 = 4,
            X_AXIS,
            Y_AXIS,
            ORIGIN
        };
        int evaluate_new_pixel(point*,point*,int,int,int);   
        void set_injection_point(int*,point*,int,int,int);
        void swap_pixel_positions(point*,int,int,int,int);  
        void swap_pixel_positions(point*,int,int);  
        void swap_pixel_positions(point*,point*);  
        void evaluate_next_med_cell_position(point*, int, int);       
        int find_side_size(int);
        Quadrant determine_quadrant(int, int); 
    public:
        point* const previous_data_pointer;
        point* const next_data_pointer;
        int current_buffer;  
        points(const float*, unsigned int, unsigned int);
        void generate_next_buffer();
        void generate_next_buffer_ext(point*,point*,int,int,int,int);
        void switch_buffer(unsigned int);
        void set_point_type(int,int, int);
        void generate_medical_cells(int, int, int);
        void generate_medical_cells(point*, int, int, int*);
        int get_buffer();
};

#endif /* CADC9390_C1B8_4CE6_AFCC_9162502C338A */
