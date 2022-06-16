#pragma once

// fyrö or fyro is a simple 2d rendering opengl playground.
// Quad or quadliteral translated to swedish is fyrhörning.
// There isn't any accepted short form fyrhörning so I picked the first four letters that made sense and sounded good.
// Fyrö also means lightouse island but that's not related but it could be the base of a logo or something :)



// standard headers


#include "tred/assert.h"

#include <numeric>
#include <functional>
#include <array>
#include <map>
#include <set>


// dependency headers
#include "stb_image.h"


// imgui
#include "tred/dependency_imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

// custom/local headers
#include "tred/dependency_sdl.h"
#include "tred/render/opengl_debug.h"
#include "tred/cint.h"
#include "tred/render/uniform.h"
#include "tred/render/shader.h"
#include "tred/log.h"
#include "tred/types.h"
#include "tred/game.h"
#include "tred/render/texture.h"
#include "tred/render/texture.load.h"

