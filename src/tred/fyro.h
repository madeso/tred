#pragma once

// fyrö or fyro is a simple 2d rendering opengl playground.
// Quad or quadliteral translated to swedish is fyrhörning.
// There isn't any accepted short form fyrhörning so I picked the first four letters that made sense and sounded good.
// Fyrö also means lightouse island but that's not related but it could be the base of a logo or something :)



// standard headers
#include <string>
#include <string_view>
#include <cassert>
#include <vector>
#include <numeric>
#include <functional>
#include <array>
#include <map>
#include <set>

#include <fmt/core.h>

// dependency headers
#include "glad/glad.h"
#include "stb_image.h"
#include "glm/glm.hpp"

// imgui
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

// custom/local headers
#include "tred/dependency_sdl.h"
#include "tred/opengl.h"
#include "tred/opengl.debug.h"
#include "tred/cint.h"
#include "tred/uniform.h"
#include "tred/shader.h"
#include "tred/log.h"
#include "tred/types.h"
#include "tred/game.h"
#include "tred/texture.h"
#include "tred/image.h"
