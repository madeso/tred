#include "tred/shader.source.h"

#include <sstream>
#include <optional>

#include "fmt/format.h"


namespace shader
{

namespace
{
    struct Parser
    {
        std::string_view source;
        std::size_t position = 0;
        int line = 1;
        int offset = 0;

        bool has_more() const
        {
            return position < source.size();
        }

        char read()
        {
            if(has_more())
            {
                const auto r = source[position];
                position += 1;
                if(r == '\n')
                {
                    line += 1;
                    offset = 0;
                }
                return r;
            }
            else
            {
                return 0;
            }
        }

        char peek(std::size_t advance = 0) const
        {
            const auto p = position + advance;
            if(p < source.size())
            {
                return source[p];
            }
            else
            {
                return 0;
            }
        }
    };

    std::string read_line(Parser* p)
    {
        std::ostringstream ss;

        while(true)
        {
            const char c = p->read();
            if(c == '\n' || c== 0)
            {
                break;
            }
            else if(c != '\r')
            {
                ss << c;
            }
        }

        ss << '\n';

        return ss.str();
    }

    bool is_ident_char(char c, bool first)
    {
        if     ('a' <= c && c <= 'z') { return true; }
        else if('A' <= c && c <= 'Z') { return true; }
        else if('0' <= c && c <= '9') { return !first; }
        else switch(c)
        {
        case '_':
            return true;
        default:
            return false;
        }
    }

    std::optional<std::string> read_ident(Parser* p)
    {
        std::ostringstream buffer;
        bool first = true;

        while(is_ident_char(p->peek(), first))
        {
            buffer << p->read();
            first = false;
        }

        if(first)
        {
            return {};
        }
        else
        {
            return {buffer.str()};
        }
    }

    void eat_all_including(Parser* p, char c)
    {
        while(p->has_more())
        {
            char rc = p->read();
            if(rc == c)
            {
                return;
            }
        }
    }

    bool is_whitespace(char c)
    {
        switch(c)
        {
        case ' ':  case '\t':
        case '\r': case '\n': 
            return true;
        default:
            return false;
        }
    }

    void eat_whitespace(Parser* p)
    {
        while(p->has_more())
        {
            char c = p->peek();
            if(is_whitespace(c))
            {
                p->read();
                continue;
            }

            if(c == '/' && p->peek(1) == '/')
            {
                // skip to newline
                eat_all_including(p, '\n');
                continue;
            }

            if(c == '/' && p->peek(1) == '*')
            {
                // skip start
                p->read();
                p->read();

                // skip until */
                bool keep_reading = true;
                while(keep_reading && p->has_more())
                {
                    if(p->peek(0) == '*' && p->peek(1)=='/')
                    {
                        p->read();
                        p->read();
                        keep_reading = false;
                    }
                    else
                    {
                        p->read();
                    }
                }
                continue;
            }

            return;
        }
    }

    bool line_contains(const std::string& line, const std::string& what)
    {
        return line.find(what) != std::string::npos;
    }

    void log_info(ShaderLog* log, int line, const std::string& message)
    {
        log->emplace_back(ShaderMessage{ShaderMessageType::info, line, message});
    }

    void log_warning(ShaderLog* log, int line, const std::string& message)
    {
        log->emplace_back(ShaderMessage{ShaderMessageType::warning, line, message});
    }

    void log_error(ShaderLog* log, int line, const std::string& message)
    {
        log->emplace_back(ShaderMessage{ShaderMessageType::error, line, message});
    }


    void error_recovery(Parser* p, ShaderLog* log)
    {
        char pc = p->peek();
        log_info(log, p->line, "Found " + std::string(1, pc));

        eat_all_including(p, ';');
    }


    bool expect_keyword(Parser* parser, const std::string& expected, ShaderLog* log)
    {
        const auto read = read_ident(parser);
        if(read.has_value() == false)
        {
            return false;
        }

        if(*read != expected)
        {
            log_error(log, parser->line, fmt::format("Expeced keyword {} but found {}", expected, *read));
            return false;
        }

        return true;
    }


    // std::optional<ShaderVertexAttributes> parse_layout(const std::string& source, ShaderLog* log)
    std::optional<ShaderVertexAttributes> parse_layout(std::string_view source, ShaderLog* log)
    {
        auto parser = Parser{source};
        eat_whitespace(&parser);

        bool ok = true;

        ShaderVertexAttributes r;

        while(parser.has_more())
        {
            eat_whitespace(&parser);

            if(false == expect_keyword(&parser, "attribute", log))
            {
                ok = false;
                log_error(log, parser.line, "Expected attribute keyword");
                error_recovery(&parser, log);
                continue;
            }

            eat_whitespace(&parser);
            const auto type_name = read_ident(&parser);
            if(type_name.has_value() == false)
            {
                ok = false;
                log_error(log, parser.line, "Missing type");
                error_recovery(&parser, log);
                continue;
            }

            const auto type = parse_vertex_type(*type_name);
            if(type.has_value() == false)
            {
                ok = false;
                log_error(log, parser.line, "Invalid type " + *type_name);
                error_recovery(&parser, log);
                continue;
            }

            eat_whitespace(&parser);
            const auto name = read_ident(&parser);
            if(name.has_value() == false)
            {
                ok = false;
                log_error(log, parser.line, "Missing name");
                error_recovery(&parser, log);
                continue;
            }

            eat_whitespace(&parser);
            if(parser.peek() == ';')
            {
                parser.read();
            }
            else
            {
                ok = false;
                log_error(log, parser.line, "Missing ;");
                error_recovery(&parser, log);
                continue;
            }

            eat_whitespace(&parser);
            r.emplace_back(VertexElementDescription{*type, *name});
        }

        if(ok) { return r; }
        else {return {};}
    }
}



ShaderResult
parse_shader_source(std::string_view source)
{
    enum Type { FRONTMATTER, VERTEX, FRAGMENT };

    Type type = FRONTMATTER;
    std::ostringstream sources[3];
    bool injected_line[3] = {true, false, false};
    auto parser = Parser{source};
    auto log = ShaderLog{};

    int index_num = 0;
    while(parser.has_more())
    {
        index_num += 1;
        const auto line_text = read_line(&parser);

        if(line_contains(line_text, "#shader"))
        {
            const auto has_vertex = line_contains(line_text, "vertex");
            const auto has_fragment = line_contains(line_text, "fragment") || line_contains(line_text, "pixel");

            if(has_vertex && !has_fragment)
            {
                type = VERTEX;
            }
            else if (has_fragment && !has_vertex)
            {
                type = FRAGMENT;
            }
            else
            {
                log_warning(&log, index_num, "Invalid shader line: " + line_text);
            }
        }
        else
        {
            sources[type] << line_text;
            if(injected_line[type] == false)
            {
                const auto was_version_line = line_contains(line_text, "#version");
                if(was_version_line)
                {
                    sources[type] << "#line " << (index_num + 1) << "\n";
                    injected_line[type] = true;
                }
            }
        }
    }

    if(injected_line[VERTEX] == false || injected_line[FRAGMENT] == false)
    {
        log_error(&log, -1, "vertex and/or fragment doesn't contain a line statement and is invalid");

        if(injected_line[VERTEX] == false)
        {
            log_info(&log, -1, "vertex is missing a line statement and is invalid");
        }
        if(injected_line[FRAGMENT] == false)
        {
            log_info(&log, -1, "fragment is missing line statement and is invalid");
        }

        return {{}, log};
    }

    const auto layout = parse_layout(sources[FRONTMATTER].str(), &log);

    if(layout)
    {
        return
        {
            ShaderSource{*layout, sources[VERTEX].str(), sources[FRAGMENT].str()},
            log
        };
    }
    else
    {
        log_error(&log, -1, "failed to parse frontmatter");
        return {{}, log};
    }
}

}
