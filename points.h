#ifndef CADC9390_C1B8_4CE6_AFCC_9162502C338A
#define CADC9390_C1B8_4CE6_AFCC_9162502C338A

#include "shapes.h"
#include "point.h"
class points : private Shapes{
    private:
        point* previous_data_pointer;
        point* next_data_pointer;
        int evaluate_new_pixel(point*,point*,int,int,int);   
        void set_injection_point(int*,point*,int,int,int);
        void swap_pixel_positions(point*,int,int,int,int);  
    public:
        points(const float*, unsigned int, unsigned int);
        void generate_next_buffer();
        void generate_next_buffer_(point*,point*,int,int,int,int);
        void switch_buffer(unsigned int);
        int current_buffer;  
        void set_point_type(int,int, int);
        void generate_medical_cells(int, int, int);
        void generate_medical_cells(point*, int, int, int*, int);
};

#endif /* CADC9390_C1B8_4CE6_AFCC_9162502C338A */
