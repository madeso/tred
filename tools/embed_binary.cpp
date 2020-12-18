// imgui/misc/fonts
#define CUSTOM_BINARY_TO_COMPRESSED_C custom_binary_to_compressed_c
#include "binary_to_compressed_c.cpp"

bool custom_binary_to_compressed_c(const char* filename, const char* symbol, bool use_base85_encoding, bool use_compression)
{
    FILE* out = stdout;

    fprintf(out, "#pragma once\n");
    fprintf(out, "\n");
    fprintf(out, "#include \"tred/embedded_types.h\"\n");
    fprintf(out, "\n");

    const auto result = binary_to_compressed_c(filename, symbol, use_base85_encoding, use_compression);
    if(!result) {return result;}

    if(use_base85_encoding == false)
    {
        const char* compressed_type = use_compression ? "CompressedBinary" : "EmbeddedBinary";
        const char* compressed_str = use_compression ? "compressed_" : "";
        fprintf(out, "constexpr %s %s = %s{%s_%sdata, %s_%ssize};\n",
            compressed_type, symbol,
            compressed_type,
            symbol, compressed_str,
            symbol, compressed_str
        );
    }
    else
    {
        // todo(Gustav): support base85 encodings
        fprintf(out, "// base85 encoding currently not supported\n");
    }
    fprintf(out, "\n");

    return result;
}
