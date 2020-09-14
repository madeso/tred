// design or mockup of engine api... this is not compiled nor part of tred code but might be soon...

// std things?
struct string {};
template <typename T> struct vector {};
template <typename K, typename V> struct map {};

// math things
struct vec2f{}; struct vec3f{}; struct ray3f{};
struct rect{}; struct aabb{}; struct ray2f{};

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
    // virtual width & height
    // world position
    // near and far plane

    // move to the compiled camera or a util function that compiles executes and discards? or dont bother compiling?
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

// todo(Gustav): figure out how to handle figure out how to handle 2d rendering both immediate modeish and tiled/world rendering
