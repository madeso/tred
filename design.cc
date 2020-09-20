// design or mockup of engine api... this is not compiled nor part of tred code but might be soon...

// std things?
struct string {};
template <typename T> struct vector {};
template <typename K, typename V> struct map {};
template <typename L, typename R> struct pair {};

// math things
struct vec2f{}; struct vec3f{}; struct ray3f{};
struct rect{}; struct aabb{}; struct ray2f{};
struct rgba{}; struct rgb;
struct Angle{};

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

struct Actor
{
    // position
    // rotation
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
};

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
    rect WorldToCameraSpace(aabb) const;
    ray2f WorldToCameraSpace(ray3f) const;
    pair<vec2f, vec2f> WorldToCameraSpaceClipped(ray3f) const;
};

struct World
{
    void AddActor(Actor);
};
void Render(const World& world, const Camera& camera);


// fullscreen effects are handled elsewhere, not sure yet how to approach this


struct Camera2d
{
    vec2f center;
    Angle rotation;
};

struct Sprite
{
    // Texture texture
    rect subsection; // uv
    rgba tint;
    vec2f center;
    vec2f size;
    Angle rotation;
};

struct SubSection
{
    vector<rect> subsections;

    // one "constructor" should map how tiled tiles work
    // border support?
    static SubSection FullImage();
    static SubSection FromPixels(int image_width, int image_height, int tile_width, int tile_height); // divide image in tiles, might skip if "overflows"
    static SubSection FromGrid(int image_width, int image_height, int columns, int rows); // divide image evenly - needed?
};

// immediate mode, no advanced culling
struct SceneRenderer
{
    SceneRenderer(const Camera2d& camera);

    void Add(const Sprite& sprite);

    void Render();
};

// baseclass (?) with more advanced culling (octree/ tiling?)
// 2d world should be able to handle:
//  - tile renderer (with sloped tiles)
//  - liero/worms painted world
//  - beziercurve based worlds like soldat/elastomania
struct World
{
    // should be able to spline/drgonbone meshes
    // simple access to sprites
    // easily modifyable sprite deformations like blobs
    void AddActor();

    void Render(const Camera2d& camera);
};

