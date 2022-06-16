#include "tred/render/shader.source.h"

#include <sstream>

#include "catch.hpp"

using namespace shader;


std::string GetLogAsString(const ShaderLog& log)
{
    std::ostringstream ss;

    for(const auto& e: log)
    {
        switch(e.type)
        {
            case ShaderMessageType::info: ss << "info"; break;
            case ShaderMessageType::warning: ss << "warning"; break;
            case ShaderMessageType::error: ss << "error"; break;
            default: ss << "???"; break;
        }
        ss << "(" << e.line << ")"<< ": " << e.message << "\n";
    }

    return ss.str();
}


TEST_CASE("shadersource-error-missing-version", "[shadersource]")
{
    const auto result = parse_shader_source
    (
        "foobar\n"
        "#shader vertex\n"
        "hello vertex\n"
        "#shader pixel\n"
        "hello pixel\n"
    );

    INFO(GetLogAsString(result.log));
    REQUIRE_FALSE(result.source);
    // error(-1): vertex and/or fragment doesn't contain a line statement and is invalid
}

TEST_CASE("shadersource-pixel-fragment-bad-frontmatter", "[shadersource]")
{
    const auto result = parse_shader_source
    (
        "foobar\n"
        "#shader vertex\n"
        "#version 42\n"
        "hello vertex\n"
        "#shader pixel\n"
        "#version 42\n"
        "version hello pixel\n"
    );

    INFO(GetLogAsString(result.log));
    REQUIRE_FALSE(result.source);
    // error(1): Invalid type foobar
    // error(-1): failed to parse frontmatter
}


TEST_CASE("shadersource-pixel-fragment", "[shadersource]")
{
    const auto result = parse_shader_source
    (
        "attribute position3 pos;\n"
        "#shader vertex\n"
        "#version 24\n"
        "hello vertex\n"
        "#shader pixel\n"
        "#version 42\n"
        "hello pixel\n"
    );

    INFO(GetLogAsString(result.log));
    REQUIRE(result.source);
    const auto& source = *result.source;
    CHECK(source.vertex == "#version 24\n#line 4\nhello vertex\n");
    CHECK(source.fragment == "#version 42\n#line 7\nhello pixel\n");
    //   error(1): Invalid type foobar
  // error(-1): failed to parse frontmatter
}
