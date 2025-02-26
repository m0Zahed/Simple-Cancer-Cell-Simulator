#include "tracker.h"
#include "value_tracker.h"

/**
 * @brief Construct a new tracker singleton::tracker singleton object
 * 
 */
tracker::tracker() : tracker_(value_tracker(0,0,0))
{
    
}

/**
 * @brief The following sets a value for a specified cell 
 * 
 * @param c 
 * @param value 
 * @return * void 
 */
void tracker::set_value(cells c, int value)
{
    switch (c) {
        case normal_cell: 
            break;
        case cancer_cell:
            break;
        case medicine_cell:
            break;
    
    }
}

value_tracker tracker::get_instance()
{
    return tracker_;
}

void tracker::increment_value(cells c)
{
    switch (c) {
        case normal_cell:
            tracker_.no_of_normal_cells++; 
            break;
        case cancer_cell:
            tracker_.no_of_cancer_cells++;
            break;
        case medicine_cell:
            tracker_.no_of_medicine_cells++;
            break;
    }
}

/**
 * @brief decrements the value of the cells specified 
 * 
 * @param c Specified Cell
 */
void tracker::decrement_value(cells c)
{
    switch (c) {
        case normal_cell:
            tracker_.no_of_normal_cells--; 
            break;
        case cancer_cell:
            tracker_.no_of_cancer_cells--;
            break;
        case medicine_cell:
            tracker_.no_of_medicine_cells--;
            break;
    }
}

int tracker::get_value(cells c)
{
    switch (c) {
            case normal_cell: 
                return tracker_.no_of_normal_cells;
                break;
            case cancer_cell:
                return tracker_.no_of_cancer_cells; 
                break;
            case medicine_cell:
                return tracker_.no_of_medicine_cells;
                break;
        }
}