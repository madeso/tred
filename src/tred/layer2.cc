#include "tred/layer2.h"

#include "tred/render2.h"
#include "tred/spritebatch.h"
#include "tred/dependency_opengl.h"
#include "tred/viewportdef.h"


void set_gl_viewport(const Recti& r)
{
    glViewport(r.left, r.bottom, r.get_width(), r.get_height());
}


RenderLayer2::~RenderLayer2()
{
    batch->submit();
}


RenderLayer2::RenderLayer2(Layer2&& l, SpriteBatch* b)
    : Layer2(l)
    , batch(b)
{
}


Layer2 create_layer(const ViewportDef& vp)
{
    return {{vp.virtual_width, vp.virtual_height}, vp.screen_rect};
}


RenderLayer2 create_layer(const RenderCommand2& rc, const ViewportDef& vp, const glm::mat4 camera)
{
    set_gl_viewport(vp.screen_rect);

    const auto projection = glm::ortho(0.0f, vp.virtual_width, 0.0f, vp.virtual_height);

    rc.render->quad_shader.use();
    rc.render->quad_shader.set_mat(rc.render->view_projection_uniform, projection);
    rc.render->quad_shader.set_mat(rc.render->transform_uniform, camera);

    // todo(Gustav): transform viewport according to the camera
    return RenderLayer2{create_layer(vp), &rc.render->batch};
}


glm::vec2 Layer2::mouse_to_world(const glm::vec2& p) const
{
    // transform from mouse pixels to window 0-1
    const auto n = Cint_to_float(screen).to01(p);
    return viewport_aabb_in_worldspace.from01(n);
}


ViewportDef create_viewport(const LayoutData& ld, const glm::ivec2& size)
{
    if(ld.style==ViewportStyle::black_bars)
    {
        return ViewportDef::fit_with_black_bars(ld.requested_width, ld.requested_height, size.x, size.y);
    }
    else
    {
        return ViewportDef::extend(ld.requested_width, ld.requested_height, size.x, size.y);
    }
}

RenderLayer2 with_layer(const RenderCommand2& rc, const LayoutData& ld)
{
    const auto vp = create_viewport(ld, rc.size);
    return create_layer(rc, vp, ld.camera);
}


Layer2 with_layer(const Command2& rc, const LayoutData& ld)
{
    const auto vp = create_viewport(ld, rc.size);
    return create_layer(vp);
}
