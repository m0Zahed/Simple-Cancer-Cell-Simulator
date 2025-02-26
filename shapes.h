#ifndef C721A9BD_735C_4777_A53E_099E323BA9DB
#define C721A9BD_735C_4777_A53E_099E323BA9DB

#include <glad/glad.h>
#include <iostream>
#include <string>

class Shapes
{   
    protected:
        unsigned int VBO, VBO2, VAO;
        int current_data_buffer_size;
        const float* vertices;
    public:
        Shapes(const float*,unsigned int);
        void extend_buffer();
        int get_buffer();
};

#endif /* C721A9BD_735C_4777_A53E_099E323BA9DB */


