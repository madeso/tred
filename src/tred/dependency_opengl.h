#pragma once

// #include "SDL_opengl.h"
#include "tred/dependency_glad.h"

// include size_t
#include <vector>


GLenum
Cint_to_glenum(int i);


GLuint
Cint_to_gluint(int i);


GLsizeiptr
Csizet_to_glsizeiptr(std::size_t t);


GLsizei
Csizet_to_glsizei(std::size_t t);

