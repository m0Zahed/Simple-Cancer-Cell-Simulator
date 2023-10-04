#pragma once
#include "Rectangle.h"

Rectangle::Rectangle(float* data, int* Indices) : Shapes(data), indices(Indices)
{
    glGenBuffers(1,&EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}