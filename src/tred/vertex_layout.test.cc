#include "fmt/format.h"

#include "catch.hpp"
#include "catchy/vectorequals.h"

#include "tred/vertex_layout.h"


template<> struct fmt::formatter<VertexType>: formatter<string_view>
{
    template <typename FormatContext>
    auto format(VertexType c, FormatContext& ctx)
    {
        string_view name = "<unknown>";
        switch (c)
        {
            case VertexType::position2: name = "position2"; break;
            case VertexType::position3: name = "position3"; break;
            case VertexType::normal3: name = "normal3"; break;
            case VertexType::color4: name = "color4"; break;
            case VertexType::texture2: name = "texture2"; break;
        }
        return formatter<string_view>::format(name, ctx);
    }
};


namespace
{
    bool is_equal(const CompiledVertexElement& lhs, const CompiledVertexElement& rhs)
    {
        return lhs.type == rhs.type
            && lhs.name == rhs.name
            && lhs.index == rhs.index
            ;
    }

    catchy::FalseString
    CheckEquals(const std::vector<CompiledVertexElement> lhs, const std::vector<CompiledVertexElement>& rhs)
    {
        return catchy::VectorEquals
        (
            lhs, rhs,
            [](const CompiledVertexElement& f) -> std::string
            {
                return fmt::format("{} {} ({})", f.type, f.name, f.index);
            },
            [](const CompiledVertexElement& a, const CompiledVertexElement& b) -> catchy::FalseString
            {
                if(is_equal(a, b))
                {
                    return catchy::FalseString::True();
                }
                else
                {
                    return catchy::FalseString::False
                    (
                        fmt::format
                        (
                            "{}!={} {}!={} ({}!={})",
                            a.type, b.type,
                            a.name, b.name,
                            a.index, b.index
                       )
                    );
                }
            }
        );
    }

    catchy::FalseString
    CheckEquals(const std::vector<VertexType> lhs, const std::vector<VertexType>& rhs)
    {
        return catchy::VectorEquals
        (
            lhs, rhs,
            [](const VertexType& f) -> std::string
            {
                return fmt::format("{}", f);
            },
            [](const VertexType& a, const VertexType& b) -> catchy::FalseString
            {
                if(a == b)
                {
                    return catchy::FalseString::True();
                }
                else
                {
                    return catchy::FalseString::False
                    (
                        fmt::format("{} != {}", a, b)
                    );
                }
            }
        );
    }
}


TEST_CASE("vertex_layout_test", "[vertex_layout]")
{
    const auto layout_shader_material = VertexLayoutDescription
    {
        {VertexType::position3, "aPos"},
        {VertexType::normal3, "aNormal"},
        {VertexType::color4, "aColor"},
        {VertexType::texture2, "aTexCoord"}
    };

    const auto layout_shader_depth = VertexLayoutDescription
    {
        {VertexType::position3, "aPos"}
    };

    const auto layout_shader_different = VertexLayoutDescription
    {
        {VertexType::color4, "aColor"},
        {VertexType::texture2, "aTexCoord"},
        {VertexType::position3, "aPos"},
        {VertexType::normal3, "aNormal"}
    };

    SECTION("simple")
    {
        auto layout_compiler = compile
        (
            {
                layout_shader_material
            }
        );
        const auto compiled_layout = layout_compiler.compile(layout_shader_material);

        const auto same_elements = CheckEquals
        (
            compiled_layout.elements,
            {
                {VertexType::position3, "aPos", 0},
                {VertexType::normal3, "aNormal", 1},
                {VertexType::color4, "aColor", 2},
                {VertexType::texture2, "aTexCoord", 3}
            }
        );
        const auto same_types = CheckEquals
        (
            compiled_layout.types,
            {
                VertexType::position3,
                VertexType::normal3,
                VertexType::color4,
                VertexType::texture2
            }
        );
        CHECK(same_elements);
        CHECK(same_types);
    }

    SECTION("material + depth")
    {
        auto layout_compiler = compile
        (
            {
                layout_shader_material,
                layout_shader_depth
            }
        );
        const auto compiled_layout_material = layout_compiler.compile(layout_shader_material);
        const auto compiled_layout_depth = layout_compiler.compile(layout_shader_depth);

        SECTION("check material")
        {
            const auto same_elements = CheckEquals
            (
                compiled_layout_material.elements,
                {
                    {VertexType::position3, "aPos", 0},
                    {VertexType::normal3, "aNormal", 1},
                    {VertexType::color4, "aColor", 2},
                    {VertexType::texture2, "aTexCoord", 3}
                }
            );
            const auto same_types = CheckEquals
            (
                compiled_layout_material.types,
                {
                    VertexType::position3,
                    VertexType::normal3,
                    VertexType::color4,
                    VertexType::texture2
                }
            );
            CHECK(same_elements);
            CHECK(same_types);
        }

        SECTION("check depth")
        {
            const auto same_elements = CheckEquals
            (
                compiled_layout_depth.elements,
                {
                    {VertexType::position3, "aPos", 0}
                }
            );
            const auto same_types = CheckEquals
            (
                compiled_layout_depth.types,
                {
                    VertexType::position3,
                    VertexType::normal3,
                    VertexType::color4,
                    VertexType::texture2
                }
            );
            CHECK(same_elements);
            CHECK(same_types);
        }
    }
    
    SECTION("material + different")
    {
        auto layout_compiler = compile
        (
            {
                layout_shader_different,
                layout_shader_material
            }
        );
        const auto compiled_layout_material = layout_compiler.compile(layout_shader_material);
        const auto compiled_layout_different = layout_compiler.compile(layout_shader_different);

        SECTION("check material")
        {
            const auto same_elements = CheckEquals
            (
                compiled_layout_material.elements,
                {
                    {VertexType::position3, "aPos", 0},
                    {VertexType::normal3, "aNormal", 1},
                    {VertexType::color4, "aColor", 2},
                    {VertexType::texture2, "aTexCoord", 3}
                }
            );
            const auto same_types = CheckEquals
            (
                compiled_layout_material.types,
                {
                    VertexType::position3,
                    VertexType::normal3,
                    VertexType::color4,
                    VertexType::texture2
                }
            );
            CHECK(same_elements);
            CHECK(same_types);
        }

        SECTION("check different")
        {
            const auto same_elements = CheckEquals
            (
                compiled_layout_different.elements,
                {
                    {VertexType::color4, "aColor", 2},
                    {VertexType::texture2, "aTexCoord", 3},
                    {VertexType::position3, "aPos", 0},
                    {VertexType::normal3, "aNormal", 1}
                }
            );
            const auto same_types = CheckEquals
            (
                compiled_layout_different.types,
                {
                    VertexType::position3,
                    VertexType::normal3,
                    VertexType::color4,
                    VertexType::texture2
                }
            );
            CHECK(same_elements);
            CHECK(same_types);
        }
    }
}

// todo(Gustav): is the data correct when we compile more than one material
// todo(Gustav): get data that isn't for shader from the compiler to the mesh
