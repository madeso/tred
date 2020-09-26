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

*/

// std things
struct string {};
template <typename T> struct vector {};
template <typename K, typename V> struct map {};
template <typename L, typename R> struct pair {};

// math things
struct vec2f{}; struct vec3f{}; struct ray3f{};
struct rectf{}; struct recti{}; struct aabb{}; struct ray2f{};
struct rgba{}; struct rgb;
struct Angle{}; struct quatf {};

// also known as sausage body and capsule2d
struct Stadium
{
    vec2f start;
    vec2f end;
    float radius;

    // converts stadium to a "aa stadium" and rotates point to the 'aa stadium' coordinate space
    bool Contains(vec2f) const;
};

// engine things

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


struct World;
struct PlacementInWorld;

struct Actor
{

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

    // might want to make it 6 less (ie dynamic)
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

    float virtual_width;
    float virtual_height;
};

// todo(Gustav): how to handle animating viewports like 24 or Hulk?

struct Camera
{
    // virtual width & height or send as a argument when needed
    // world position
    // near and far plane

    // move to the compiled camera or a util function that compiles executes and discards? or dont bother compiling?
    // or get a dirty flag so we don't calculate inverse matrix and multiplications all the time and make the api a bit nicer?
    ViewVolume GetViewVolume();
    ray3f CameraToRay(vec2f) const;
    vec2f WorldToCameraSpace(vec3f) const;
    rectf WorldToCameraSpace(aabb) const;
    ray2f WorldToCameraSpace(ray3f) const;
    pair<vec2f, vec2f> WorldToCameraSpaceClipped(ray3f) const;
};

struct World
{
    // how to handle memory?
    // RemoveX function are needed
    void AddActor(Actor);
    void AddLight();
    void AddParticleSystem();

    virtual void Render(const Camera& camera) const = 0;

    virtual void OnActorMoved(Actor*, PlacementInWorld*) =0;
};



// fullscreen effects are handled elsewhere, not sure yet how to approach this


struct Camera2d
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
    SpriteBatch(const Camera2d& camera);

    void Add(const Sprite& sprite);

    void Render();
};

// baseclass (?) with more advanced culling (octree/ tiling?)
// 2d world should be able to handle:
//  - tile renderer (with sloped tiles)
//  - liero/worms painted world
//  - beziercurve based worlds like soldat/elastomania
struct World2d
{
    // layer support

    // should be able to spline/drgonbone meshes
    // simple access to sprites
    // easily modifyable sprite deformations like blobs
    // fluid dynamics like water, lava and smoke
    void AddActor();

    void Render(const Camera2d& camera);
};

