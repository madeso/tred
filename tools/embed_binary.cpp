// imgui/misc/fonts
#define CUSTOM_BINARY_TO_COMPRESSED_C custom_binary_to_compressed_c
#include "binary_to_compressed_c.cpp"

bool custom_binary_to_compressed_c(const char* filename, const char* symbol, bool use_base85_encoding, bool use_compression)
{
    FILE* out = stdout;

    fprintf(out, "#pragma once\n");

    const auto result = binary_to_compressed_c(filename, symbol, use_base85_encoding, use_compression);
    if(!result) {return result;}

    fprintf(out, "// post imgui output of %s\n\n", symbol);

    return result;
}
