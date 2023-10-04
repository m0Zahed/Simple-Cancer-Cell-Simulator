#include "shapes.h"
#include <cstddef>

/**
 * @brief Construct a new Shapes:: Shapes object
 * 
 * @param vertices_ pointer to the beginning of the data
 * @param data_buffer_size Total buffer size, typically NUMBER_OF_ELEMENTS*SIZE_OF_ONE_ELEMENT
 */
Shapes::Shapes(const float* vertices_,unsigned int data_buffer_size) : vertices(vertices_), current_data_buffer_size(data_buffer_size)
{
    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, data_buffer_size, vertices_, GL_DYNAMIC_DRAW);

    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);
}

