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

    bool is_equal(const CompiledVertexElementNoName& lhs, const CompiledVertexElementNoName& rhs)
    {
        return lhs.type == rhs.type
            && lhs.index == rhs.index
            ;
    }

    catchy::FalseString
    is_equal(const std::vector<CompiledVertexElement> lhs, const std::vector<CompiledVertexElement>& rhs)
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
    is_equal(const std::vector<CompiledVertexElementNoName> lhs, const std::vector<CompiledVertexElementNoName>& rhs)
    {
        return catchy::VectorEquals
        (
            lhs, rhs,
            [](const CompiledVertexElementNoName& f) -> std::string
            {
                return fmt::format("{} ({})", f.type, f.index);
            },
            [](const CompiledVertexElementNoName& a, const CompiledVertexElementNoName& b) -> catchy::FalseString
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
                            "{}!={} ({}!={})",
                            a.type, b.type,
                            a.index, b.index
                       )
                    );
                }
            }
        );
    }

    catchy::FalseString
    is_equal(const std::vector<VertexType> lhs, const std::vector<VertexType>& rhs)
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

    catchy::FalseString
    is_equal(const CompiledVertexLayout& lhs, const CompiledVertexLayout& rhs)
    {
        const auto same_elements = is_equal(lhs.elements, rhs.elements);
        const auto same_debug_types = is_equal(lhs.debug_types, rhs.debug_types);

        return catchy::FalseString::Combine(same_elements, same_debug_types);
    }

    catchy::FalseString
    is_equal(const CompiledMeshVertexLayout& lhs, const CompiledMeshVertexLayout& rhs)
    {
        const auto same_elements = is_equal(lhs.elements, rhs.elements);
        const auto same_debug_types = is_equal(lhs.debug_types, rhs.debug_types);

        return catchy::FalseString::Combine(same_elements, same_debug_types);
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
        const auto mesh_layout = layout_compiler.compile_mesh_layout();

        CHECK
        (
            is_equal
            (
                compiled_layout,
                {
                    {
                        {VertexType::position3, "aPos", 0},
                        {VertexType::normal3, "aNormal", 1},
                        {VertexType::color4, "aColor", 2},
                        {VertexType::texture2, "aTexCoord", 3}
                    },
                    {
                        VertexType::position3,
                        VertexType::normal3,
                        VertexType::color4,
                        VertexType::texture2
                    }
                }
            )
        );

        CHECK
        (
            is_equal
            (
                mesh_layout,
                {
                    {
                        {VertexType::position3, 0},
                        {VertexType::normal3, 1},
                        {VertexType::color4, 2},
                        {VertexType::texture2, 3}
                    },
                    {
                        VertexType::position3,
                        VertexType::normal3,
                        VertexType::color4,
                        VertexType::texture2
                    }
                }
            )
        );
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
        const auto mesh_layout = layout_compiler.compile_mesh_layout();

        CHECK
        (
            is_equal
            (
                compiled_layout_material,
                {
                    {
                        {VertexType::position3, "aPos", 0},
                        {VertexType::normal3, "aNormal", 1},
                        {VertexType::color4, "aColor", 2},
                        {VertexType::texture2, "aTexCoord", 3}
                    },
                    {
                        VertexType::position3,
                        VertexType::normal3,
                        VertexType::color4,
                        VertexType::texture2
                    }
                }
            )
        );

        CHECK
        (
            is_equal
            (
                compiled_layout_depth,
                {
                    {
                        {VertexType::position3, "aPos", 0}
                    },
                    {
                        VertexType::position3,
                        VertexType::normal3,
                        VertexType::color4,
                        VertexType::texture2
                    }
                }
            )
        );

        CHECK
        (
            is_equal
            (
                mesh_layout,
                {
                    {
                        {VertexType::position3, 0},
                        {VertexType::normal3, 1},
                        {VertexType::color4, 2},
                        {VertexType::texture2, 3}
                    },
                    {
                        VertexType::position3,
                        VertexType::normal3,
                        VertexType::color4,
                        VertexType::texture2
                    }
                }
            )
        );
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
        const auto mesh_layout = layout_compiler.compile_mesh_layout();
        
        CHECK
        (
            is_equal
            (
                compiled_layout_material,
                {
                    {
                        {VertexType::position3, "aPos", 0},
                        {VertexType::normal3, "aNormal", 1},
                        {VertexType::color4, "aColor", 2},
                        {VertexType::texture2, "aTexCoord", 3}
                    },
                    {
                        VertexType::position3,
                        VertexType::normal3,
                        VertexType::color4,
                        VertexType::texture2
                    }
                }
            )
        );
        
        CHECK
        (
            is_equal
            (
                compiled_layout_different,
                {
                    {
                        {VertexType::color4, "aColor", 2},
                        {VertexType::texture2, "aTexCoord", 3},
                        {VertexType::position3, "aPos", 0},
                        {VertexType::normal3, "aNormal", 1}
                    },
                    {
                        VertexType::position3,
                        VertexType::normal3,
                        VertexType::color4,
                        VertexType::texture2
                    }
                }
            )
        );

        CHECK
        (
            is_equal
            (
                mesh_layout,
                {
                    {
                        {VertexType::position3, 0},
                        {VertexType::normal3, 1},
                        {VertexType::color4, 2},
                        {VertexType::texture2, 3}
                    },
                    {
                        VertexType::position3,
                        VertexType::normal3,
                        VertexType::color4,
                        VertexType::texture2
                    }
                }
            )
        );
    }
    
    SECTION("crazy")
    {
        const auto layout_shader_a = VertexLayoutDescription
        {
            {VertexType::color4, "rgb"}
        };

        const auto layout_shader_b = VertexLayoutDescription
        {
            {VertexType::texture2, "uv"}
        };
        auto layout_compiler = compile
        (
            {
                layout_shader_a,
                layout_shader_b
            }
        );
        const auto compiled_layout_a = layout_compiler.compile(layout_shader_a);
        const auto compiled_layout_b = layout_compiler.compile(layout_shader_b);
        const auto mesh_layout = layout_compiler.compile_mesh_layout();
        
        CHECK
        (
            is_equal
            (
                compiled_layout_a,
                {
                    {
                        {VertexType::color4, "rgb", 0}
                    },
                    {
                        VertexType::color4,
                        VertexType::texture2
                    }
                }
            )
        );
        
        CHECK
        (
            is_equal
            (
                compiled_layout_b,
                {                
                    {
                        {VertexType::texture2, "uv", 1}
                    },
                    {
                        VertexType::color4,
                        VertexType::texture2
                    }
                }
            )
        );

        CHECK
        (
            is_equal
            (
                mesh_layout,
                {
                    {
                        {VertexType::color4, 0},
                        {VertexType::texture2, 1}
                    },
                    {
                        VertexType::color4,
                        VertexType::texture2
                    }
                }
            )
        );
    }
}
