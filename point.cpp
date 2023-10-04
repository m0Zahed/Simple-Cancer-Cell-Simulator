#include "point.h"

point::point()
{
    x = 0.0f;

    y = 0.0f;
}

/**
 * @brief Construct a new point::point object
 * 
 * @param a X-axis coord
 * @param b Y-axis coord
 */
point::point(float a, float b) : x(a), y(b)
{
            
}

/**
 * @brief Setter for point color
 * 
 * @param r_ 
 * @param g_ 
 * @param b_ 
 * @param a_ 
 */
void point::set_rgba(float r_, float g_, float b_, float a_)
{
    r = r_;
    g = g_;
    b = b_;
    a = a_;
}