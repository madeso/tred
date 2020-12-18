#pragma once

struct EmbeddedBinary
{
    const unsigned int* data;
    unsigned int size;

    constexpr EmbeddedBinary(const unsigned int* d, unsigned int s) : data(d), size(s) {}
};


// todo(Gustav): add CompressedBinary

