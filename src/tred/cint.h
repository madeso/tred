#pragma once

#include "tred/opengl.h"


// include size_t
#include <vector>


int
Csizet_to_int(std::size_t t);


std::size_t
Cint_to_sizet(int i);


int
Cunsigned_int_to_int(unsigned int ui);


GLenum
Cint_to_glenum(int i);


GLuint
Cint_to_gluint(int i);


GLsizeiptr
Csizet_to_glsizeiptr(std::size_t t);


GLsizei
Csizet_to_glsizei(std::size_t t);

float
Cint_to_float(int i);
