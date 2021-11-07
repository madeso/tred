// design or mockup of engine api... this is not compiled nor part of tred code but might be soon...

/*

Design philosopy?

// todo(Gustav): Should the engine be bothered with loading meshes, shaders, textures, levels etc or
let the user handle that? I kinda lean towards "no support" but with a clean api to easily add
assimp or something else

Pro (in favor of no loading supported)
 - easier to run (? currently embeded images are loaded)
 - easier to integrate into existing frameworks
 - examples will be easier to read (? loadMesh("cube.obj") vs. createCubeMesh())

Con (in favor of loading supported)
 - simple meshes are easy to generate but (good) skinned meshes will be hard
 - do we care about ease of use since the target codebase is euphoria?


// todo(Gustav): Should the engine be flexible enough to handle different mesh attributes?

Fully dynamic:
 - Easy to add new rendering techniques with custom mesh data

Fixed setup
 - Easy to have a general shader (flat color) and less memory is used
 - Is custom per-vertex mesh data common or is it more a new texture

*/

// std things
struct string {};
template<typename T> using unique_ptr = T*;
template <typename T> struct vector {};
template <typename T> struct optional {};
template <typename K, typename V> struct map {};
template <typename L, typename R> struct pair {};

// math things
struct vec2f{}; struct vec3f{}; struct ray3f{};
struct rectf{}; struct recti{}; struct aabb{}; struct ray2f{};
struct rgba{}; struct rgb;
struct Angle{}; struct quatf {};
struct sizei
{
    sizei(int w, int h);
};

// also known as sausage body and stadium
struct Capsule2
{
    vec2f start;
    vec2f end;
    float radius;

    // converts stadium to a "aa stadium" and rotates point to the 'aa stadium' coordinate space
    bool Contains(vec2f) const;
};



// render things
struct render3
{
};

struct render2
{
    void quad();
};

struct Layout {float width; float height;};

struct RenderCommand
{
    render2 begin_2d(Layout, optional<Camera2>);
    render3 begin_3d(Layout, Camera3);
};

void render(RenderCommand rc)
{
    World3 w;
    Camera3 c3;
    {
        auto r3 = rc.begin_3d({800, 600}, c3);
        w.render(r3);
    }
    {
        auto r2 = rc.begin_2d({800, 600}, {});
        r2.quad();
    }
}

// engine things

struct Mesh {};
struct CompiledMesh {};

struct Pose
{
};

struct Animation
{
    float length;
    Pose GetPose(float time) const;
};

struct ActorDef
{
    map<string, Animation> animations;
    // definition of a mesh
};


struct World3;
struct PlacementInWorld;

struct Actor
{
    optional<rgb> outline;

    // position, rotation(?) and animation pose(?) might change how the world culls the actor
    // so we need to reevaluate how to render it, for example to move to another room
    // in a portal setup or "another" cube in a quadtree
    PlacementInWorld* placement = 0;

    // position
    // rotation
    void SetPosition(const vec3f& pos);
    void SetRotation(const quatf& rot);
    const vec3f& GetPosition() const;
    const quatf& GetRotation() const;

    // idea: replace position and rotation with a matrix since that's what is used
    // by the rendering or keep position and rotation as they are easier to tween?

    // current animation pose
    // ActorDef

    vec3f WorldToObjectSpace(vec3f) const;
    vec3f ObjectToWorldSpace(vec3f) const;
};


// frustum(perspective) or a box(ortho)
struct ViewVolume
{
    // contains the 6 planes
    // function to test if aabb, sphere, etc are inside

    // might want to make it 6 or less (ie dynamic)
    // so when culling, we can drop planes we know children in a hierarchical layout surely will pass
    // this might give a speed increase since we are doing less calculations
};

// how to handle different sizes
enum class ViewportType
{
    // exact pixel match, no black bars, no stretch
    Screen,

    // fills, keeps the same virtual size, stretches
    // wants width and height
    Stretch,

    // keeps aspect ratio, might be black bars, no stretch
    // wants width and height
    Fit,

    // keeps the aspect ration but might crop the view, no stretch
    // wants width and height
    Fill
};

// where to render on screen or texture
struct Viewport
{
    recti Calculate(const recti& available) const;

    ViewportType type;
    float virtual_width;
    float virtual_height;
};

// todo(Gustav): how to handle animating viewports like 24 or Hulk?

struct Camera3
{
    // virtual width & height or send as a argument when needed
    // world position
    // near and far plane
    // aspect ratio

    // move to the compiled camera or a util function that compiles executes and discards? or dont bother compiling?
    // or get a dirty flag so we don't calculate inverse matrix and multiplications all the time and make the api a bit nicer?
    ViewVolume GetViewVolume();
    ray3f CameraToRay(vec2f) const;
    vec2f WorldToCameraSpace(vec3f) const;
    rectf WorldToCameraSpace(aabb) const;
    ray2f WorldToCameraSpace(ray3f) const;
    pair<vec2f, vec2f> WorldToCameraSpaceClipped(ray3f) const;
};




// text rendering should support animation text:
// https://github.com/mdechatech/CharTweener



// fullscreen effects are handled elsewhere, not sure yet how to approach this


struct Camera2
{
    vec2f center;
    Angle rotation;
};

struct Sprite
{
    // Texture texture
    rectf subsection; // uv
    rgba tint;
    vec2f center;
    vec2f size;
    Angle rotation;
};

struct SubSection
{
    vector<rectf> subsections;

    // one "constructor" should map how tiled tiles work
    // border support?
    static SubSection FullImage();
    static SubSection FromPixels(int image_width, int image_height, int tile_width, int tile_height); // divide image in tiles, might skip if "overflows"
    static SubSection FromGrid(int image_width, int image_height, int columns, int rows); // divide image evenly - needed?
};

// immediate mode, no advanced culling
// could also be called scenerenderer (though that may be confusing with the world)
// spritebatch name from libGDX
struct SpriteBatch
{
    SpriteBatch(const Camera2& camera);

    void Add(const Sprite& sprite);

    void Render();
};

// baseclass (?) with more advanced culling (octree/ tiling?)
// 2d world should be able to handle:
//  - tile renderer (with sloped tiles)
//  - liero/worms painted world
//  - beziercurve based worlds like soldat/elastomania
struct World2
{
    // layer support

    // should be able to spline/drgonbone meshes
    // simple access to sprites
    // easily modifyable sprite deformations like blobs
    // fluid dynamics like water, lava and smoke
    void AddActor();

    void Render(const Camera2& camera);
};


enum class TextureRef {};

struct Textures
{
    TextureRef Load(const string& path);
    TextureRef Create();
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// how to handle lights?

// open gl shader
struct ShaderProgram;

// defined by "world"
//  exoses a 'calculate_light()' function
struct LightShaderUniforms {};
struct LightShader
{
    void set_shader_uniforms(ShaderProgram*, LightShaderUniforms* uniforms, Light*);
    LightShaderUniforms* get_uniforms(ShaderProgram*);
};

// defined by "script"
//  exposes a 'calculate_color()' function
struct MaterialShaderUniforms {};
struct MaterialShader
{
    void set_shader_uniforms(ShaderProgram*, MaterialShaderUniforms* uniforms, MaterialProperties*);
    MaterialShaderUniforms* get_uniforms(ShaderProgram*);
};

// defined by "mesh"
//   exposes position, normal and whatever else for the material and light
struct TransformShaderUniforms {};
struct TransformShader
{
    void set_shader_uniforms(ShaderProgram*, TransformShaderUniforms* uniforms, MeshInstance* mesh);
    TransformShaderUniforms* get_uniforms(ShaderProgram*);
};

struct Shader
{
    ShaderProgram* program;

    LightShader* light_shader;
    MaterialShader* material_shader;
    TransformShader* transform_shader;

    LightShaderUniforms* light_shader_uniforms;
    MaterialShaderUniforms* material_shader_uniforms;
    TransformShaderUniforms transform_shader_uniforms;
};

struct Look
{
    Shader* shader;
    MaterialProperties* properties;
};

struct Material
{
    // look defines the "look" of a mesh
    // useful vor vision modes like normal, thermal, xray
    map<string, Look> looks;
};


// known shader variables like diffuse and color
struct ShaderName {string name;};

struct DefinedShaderProperties
{
    std::vector<ShaderName> textures;
};

// assigned shader variable, like diffuse=diffuse.png, color=red;
struct AssignedShaderProperties
{
    vector<TextureRef> textures;
};

struct MaterialProperties
{
    AssignedShaderProperties properties;
};
struct Light
{
};

struct Renderer
{
    virtual void RenderWorld(World3* world) = 0;
};

// specific light algorithm

struct ForwardLight : public Light { };

struct ForwardRenderer : public Renderer
{
};

// deferred
// other/toon

///////////////////////////////////////////////////////////////////////////////////////////////////
// "working" main function

enum class ActorDefHandle {};

struct Engine
{
    ActorDefHandle CreateActorDef(const CompiledMesh& mesh);
};

struct Windows
{
    unique_ptr<Engine> engine;

    template<typename F>
    void AddWindow(const sizei&, F&& on_render);

    void Render();

    void PumpEvents();
};
unique_ptr<Windows> Setup();

enum class ActorHandle {};

// WindowRenderTarget, TextureRenderTarget...
struct RenderTarget {};

// Technique or a Renderer handles shader creation
// creates a "super" shader from a light shader (based on the lighting technique)
//   and a material shader

struct Technique
{
    virtual void Render(const vector<Actor>& culled); // pure virtual
};

struct TechniqueWireframe : public Technique {};
struct TechniqueSingleShader : public Technique {};
struct TechniqueUsingLook : public Technique {};

// look here refers to the look of the meshes
// a Material can have one to many Looks
// a look is a shader reference, texture references, colors...


// 'Game Engine Architecture' calls this structure a SceneGraph, but I think
// that name implies some form of hierarchy and my World does not have that
struct World3
{
    // how to handle memory?
    // RemoveX function are needed
    void AddActor(Actor);
    void AddLight();
    void AddParticleSystem();

    virtual void Render(const Camera3& camera, Technique* technique) const = 0;

    virtual void OnActorMoved(Actor*, PlacementInWorld*) =0;
};

unique_ptr<World3> CreateWorld();



template<typename F>
int MainLoop(Windows* windows, F&&);

int main()
{
    auto windows = Setup();

    auto world = CreateWorld();
    auto def = windows->engine->CreateActorDef(CompiledMesh{});
    auto actor = world->CreateActor(def);

    // todo(Gustav): how does full screen effects work
    // todo(Gustav): how does custom material shaders work?
    // todo(Gustav): access vfs from engine object and add?

    windows->AddWindow
    (
        {800, 600},
        [&](const recti& size)
        {
            // todo(Gustav): camera? save or create here inline?
            // how to specify camera? 3d=aspect ratio + screen rect, 2d=world rect + screen rect
            // first arg is "constant" but screen rect needs some form of layout system
            // probably also want some immediate api for atleast 2d but perhaps also 3d
            world->Render(size, TechniqueFullyLit{});
        }
    );

    return MainLoop
    (
        windows,
        [&](float delta)
        {
            // todo(Gustav): how to handle input? register inputs or "pump" events in MainLoop?
            world->PlaceActor(actor, vec3f{});
        }
    );
}
