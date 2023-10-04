#ifndef BB994DC9_80B7_402E_9770_145808894DA5
#define BB994DC9_80B7_402E_9770_145808894DA5

#include "shapes.h"
class Rectangle : Shapes
{   
    private:
        int *indices;
        unsigned int EBO;
    public:
        Rectangle(float*,int*);
};

#endif /* BB994DC9_80B7_402E_9770_145808894DA5 */
