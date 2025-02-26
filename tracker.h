#ifndef BACC46EE_2DE5_41BC_8121_CEB6D00C6B58
#define BACC46EE_2DE5_41BC_8121_CEB6D00C6B58

#include "value_tracker.h"
class tracker{
    private:
        value_tracker tracker_;
        
    public:
        tracker();
        enum cells {
            normal_cell,
            cancer_cell,
            medicine_cell
        };
        value_tracker get_instance();    
        void set_value(cells,int);
        void increment_value(cells);
        void decrement_value(cells);
        int get_value(cells);
};


#endif /* BACC46EE_2DE5_41BC_8121_CEB6D00C6B58 */
