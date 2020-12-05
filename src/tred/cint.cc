#include "tred/cint.h"


int
Csizet_to_int(std::size_t t)
{
    return static_cast<int>(t);
}


std::size_t
Cint_to_sizet(int i)
{
    return static_cast<std::size_t>(i);
}


int
Cunsigned_int_to_int(unsigned int ui)
{
    return static_cast<int>(ui);
}


GLenum
Cint_to_glenum(int i)
{
    return static_cast<GLenum>(i);
}


GLuint
Cint_to_gluint(int i)
{
    return static_cast<GLuint>(i);
}


GLsizeiptr
Csizet_to_glsizeiptr(std::size_t t)
{
    return static_cast<GLsizeiptr>(t);
}


GLsizei
Csizet_to_glsizei(std::size_t t)
{
    return static_cast<GLsizei>(t);
}
