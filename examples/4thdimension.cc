#include "tred/fyro.h"
#include "sprites/onebit.h"
#include "sprites/onebit_font.h"
#include "tred/cint.h"
#include "tred/random.h"

#include <memory>
#include <functional>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <ctime>
#include <set>
#include <iostream>

constexpr float sprite_square = 40.0f;
constexpr rect sprite_size = {sprite_square, sprite_square};

constexpr auto font = ::onebit::font{30.0f};

struct render_data
{
    rect viewport_aabb_in_worldspace;
    texture* onebit;
    sprite_batch* batch;
};

using namespace std;


constexpr int width = 800;
constexpr int height = 600;
constexpr float fade_time_intro = 2;
constexpr float fade_time_outro = 1;

void Click();

bool gAgainstComputer = true;
bool gBadAi = true;
bool gHardMode = false;

void SetGameCallbacks();
void SetMenuCallbacks();
void BuildRules();

struct Label {
    Label(const std::string& iText, float iX, float iY) : x(iX), y(iY), text(iText)
    {
    }

    void render(const render_data& data)
    {
        float gray = 0.6f;
        font.simple_text(data.batch, data.onebit, {gray, gray, gray, 1}, x, y, text, ::onebit::no_text_animation{});
    }

    std::string text;
    float x;
    float y;
};

struct Button
{
    Button(const std::string& iText, float iX, float iY)
        : text(iText)
        , x(iX)
        , y(iY)
        , over(false)
    {
    }

    void render(const render_data& data) const
    {
        constexpr glm::vec4 over_color = {0, 0, 1, 1};
        constexpr glm::vec4 default_color = {1, 1, 1, 1};
        const auto color = over ? over_color : default_color;
        font.simple_text(data.batch, data.onebit, color, x, y, text, ::onebit::no_text_animation{});
    }

    bool test(const glm::vec2& mouse)
    {
        const float w = font.get_width_of_string(text);
        const float h = font.size;
        const auto rect = ::rect{x, y, x+w, y+h};
        over = rect.is_inside_inclusive(mouse.x, mouse.y);
        return over;
    }

    std::string text;
    float x;
    float y;
    bool over;
};

struct Menu
{
    Menu()
        : state(0)
        , cursorDown(false)
        , hardAi("Human Versus Hard AI", 250, 200)
        , easyAi("Human Versus Easy AI", 250, 250)
        , noAi("Human Versus Human", 250, 300)
        , aiLabel("Select Gameplay:", 200, 100)
        , allRules("Use all rules", 150, 350)
        , allRulesDesc(" - only for 4d masters", 200, 400)
        , mostRules("Use most rules", 150, 200)
        , mostRulesDesc(" (ignores most trans-square rules)\n - recomended for beginners", 200, 250)
        , rulesLabel("Select how many rules you want to use", 100, 100)
    {
    }

    void update(const glm::vec2& mouse, bool down, float)
    {
        const auto clicked = !down && cursorDown;
        cursorDown = down;

        if( state==0 )
        {
            if( hardAi.test(mouse) && clicked )
            {
                ++state;
                gAgainstComputer = true;
                gBadAi = false;
                Click();
            }
            else if( easyAi.test(mouse) && clicked )
            {
                ++state;
                gAgainstComputer = true;
                gBadAi = true;
                Click();
            }
            else if( noAi.test(mouse) && clicked )
            {
                ++state;
                gAgainstComputer = false;
                gBadAi = false;
                Click();
            }
        }
        else if (state == 1 )
        {
            if( allRules.test(mouse) && clicked )
            {
                state++;
                Click();
                gHardMode = true;
            }
            else if( mostRules.test(mouse) && clicked )
            {
                state++;
                Click();
                gHardMode = false;
            }

        }
        else
        {
            BuildRules();
            SetGameCallbacks();
            state = 0;
        }
    }

    void render(const render_data& rd)
    {
        if( state==0 )
        {
            aiLabel.render(rd);
            hardAi.render(rd);
            easyAi.render(rd);
            noAi.render(rd);
        }
        else
        {
            allRules.render(rd);
            allRulesDesc.render(rd);
            mostRules.render(rd);
            mostRulesDesc.render(rd);
            rulesLabel.render(rd);
        }
    }

    int state;
    bool cursorDown;

    Button hardAi;
    Button easyAi;
    Button noAi;
    Label aiLabel;

    Button allRules;
    Label allRulesDesc;
    Button mostRules;
    Label mostRulesDesc;
    Label rulesLabel;
};

Menu* gMenu = 0;

void ExecuteComputerMove(Random* rand);

struct Object
{
    Object()
        : mShouldKill(false)
    {
    }

    virtual ~Object() = default;

    virtual void render(const render_data&)
    {
    }

    // float delta, bool interact, const glm::vec2& mouse, bool down
    virtual void update(float, bool, const glm::vec2&, bool)
    {
    }

    void kill()
    {
        mShouldKill = true;
    }

    bool shouldKill() const
    {
        return mShouldKill;
    }
private:
    bool mShouldKill;
};

struct Background : Object
{
    Background(const std::string&)
    {
    }

    void render(const render_data& rd)
    {
        // sprite->RenderStretch(0, 0, WIDTH, HEIGHT);
        rd.batch->quad({}, rd.viewport_aabb_in_worldspace, {}, {0.8, 0.8, 0.8, 1.0f});
    }
};

struct FullscreenColorSprite : Object
{
    FullscreenColorSprite(const glm::vec4& iColor) : color(iColor)
    {
    }

    void render(const render_data& data)
    {
        data.batch->quad({}, {width, height}, {}, color);
    }

    glm::vec4 color;
};

struct FadeFromBlack : FullscreenColorSprite
{
    FadeFromBlack(float iTime)
        : FullscreenColorSprite({0,0,0, 1})
        , time(iTime)
    {
    }

    void update(float delta, bool, const glm::vec2&, bool) override
    {
        color.a -= delta/time;
        if( color.a <= 0.0f )
        {
            kill();
        }
        color.a = std::max(color.a, 0.0f);
    }
    const float time;
};

void QuitGame();

struct FadeToBlackAndExit : FullscreenColorSprite
{
    FadeToBlackAndExit(float iTime)
        : FullscreenColorSprite({0,0,0, 0})
        , time(iTime)
    {
    }
    void update(float delta, bool, const glm::vec2&, bool) override
    {
        color.a += delta/time;
        if( color.a > 1.0f )
        {
            QuitGame();
        }
        color.a = std::max(color.a, 0.0f);
    }
    const float time;
};

typedef shared_ptr<Object> ObjectPtr;

bool IsRemoveable(const ObjectPtr& object)
{
    return object->shouldKill();
}

struct SuggestedLocation
{
    SuggestedLocation()
    {
        clear();
    }

    void set(float x, float y)
    {
        valid = true;
        mx = x;
        my = y;
    }

    void clear()
    {
        valid = false;
    }

    bool get(float* x, float* y)
    {
        if( valid )
        {
            float dx = 0;
            float dy = 0;
            *x = mx + dx;
            *y = my + dy;
            return true;
        }
        return false;
    }

private:
    float mx;
    float my;
    bool valid;
};

SuggestedLocation gSuggestedLocation;

enum CrossOrCircle
{
    COC_NEITHER,
    COC_CROSS,
    COC_CIRCLE,
    COC_AI
};

CrossOrCircle gCurrentCursorState = COC_CROSS;

void SetCompleteCursor()
{
    gCurrentCursorState = COC_NEITHER;
}

void SetStartCursor()
{
    gCurrentCursorState = COC_CROSS;
}

void flipCurrentCursorState()
{
    if( gCurrentCursorState == COC_CROSS )
    {
        if( gAgainstComputer )
        {
            gCurrentCursorState = COC_AI;
        }
        else
        {
            gCurrentCursorState = COC_CIRCLE;
        }
    }
    else
    {
        gCurrentCursorState = COC_CROSS;
    }
}

glm::vec2 mouse_cursor;

struct IconPlacer : Object {
    IconPlacer()
    {
    }

    void render(const render_data& data)
    {
        std::optional<recti> cursor = ::onebit::cross;

        if( gCurrentCursorState == COC_CIRCLE ) {
            cursor = ::onebit::circle;
        }

        if( gCurrentCursorState == COC_NEITHER ) {
            cursor = {};
        }

        if( gCurrentCursorState == COC_AI ) {
            cursor = {}; // hourglass ?
        }

        float mx = 0;
        float my = 0;
        if( cursor && gSuggestedLocation.get(&mx, &my) )
        {
            // if a location isn't suggested, the data isn't modified
            data.batch->quad(data.onebit, sprite_size.translate(mx, my), *cursor);
        }
    }
};

struct Index
{
    Index()
        : mCube(-1)
        , mCol(-1)
        , mRow(-1)
    {
    }

    Index(int cube, int col, int row)
        : mCube(cube)
        , mCol(col)
        , mRow(row)
    {
    }

    bool operator==(const Index& i) const
    {
        return mCube == i.mCube &&
            mCol == i.mCol &&
            mRow == i.mRow;
    }

    /*
    void operator=(const Index& i)
    {
        mCube = i.mCube;
        mCol = i.mCol;
        mRow = i.mRow;
    }
    */

    void operator+=(const Index& i)
    {
        mCube += i.mCube;
        mCol += i.mCol;
        mRow += i.mRow;
    }

    void clear()
    {
        mCube = -1;
        mCol = -1;
        mRow = -1;
    }

    int mCube;
    int mCol;
    int mRow;
};

struct GameWorld;
struct WinningCombination
{
    Index combination[4];
    CrossOrCircle test(const GameWorld& iWorld) const;
};

typedef std::function<WinningCombination ()> TestWinningConditionFunction;

struct GameWorld
{
public:
    GameWorld()
        : mIsModified(false)
    {
        clear();
    }

    CrossOrCircle getState(int cube, int col, int row) const
    {
        return mState[cube][col][row];
    }

    CrossOrCircle getState(const Index& index) const
    {
        return mState[index.mCube][index.mCol][index.mRow];
    }

    void setState(int cube, int col, int row, CrossOrCircle state)
    {
        mState[cube][col][row] = state;
        mIsModified = true;
    }

    void clear()
    {
        for(int i=0; i<4; ++i)
        {
            for(int j=0; j<4; ++j)
            {
                for(int k=0; k<4; ++k)
                {
                    mState[i][j][k] = COC_NEITHER;
                }
            }
        }
        mIsModified = true;
    }

    bool canPlacemarker() const
    {
        for(int i=0; i<4; ++i)
        {
            for(int j=0; j<4; ++j)
            {
                for(int k=0; k<4; ++k)
                {
                    if( mState[i][j][k] == COC_NEITHER )
                    {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    bool hasBeenModifiedSinceLastCall()
    {
        bool modified = mIsModified;
        mIsModified = false;
        return modified;
    }

    CrossOrCircle testWinningConditions(WinningCombination* oCombo) const
    {
        const std::size_t count = mWinningConditions.size();
        for(std::size_t i=0; i<count; ++i)
        {
            const WinningCombination combo = mWinningConditions[i]();
            const CrossOrCircle result = combo.test(*this);
            if( result != COC_NEITHER )
            {
                *oCombo = combo;
                return result;
            }
        }

        return COC_NEITHER;
    }

    void addWinningCondition(TestWinningConditionFunction condition)
    {
        mWinningConditions.push_back(condition);
    }

    size_t getWinningComditions( vector<WinningCombination>* conditions)
    {
        const std::size_t count = mWinningConditions.size();
        for(std::size_t i=0; i<count; ++i)
        {
            const WinningCombination combo = mWinningConditions[i]();
            conditions->push_back(combo);
        }
        return count;
    }

    bool isPlaceFree(int cube, int col, int row) const
    {
        return mState[cube][col][row] == COC_NEITHER;
    }

    bool isPlaceFree(Index index) const
    {
        return isPlaceFree(index.mCube, index.mCol, index.mRow);
    }

private:
    CrossOrCircle mState[4][4][4];
    std::vector<TestWinningConditionFunction> mWinningConditions;
    bool mIsModified;
};



Index gCurrent;

CrossOrCircle WinningCombination::test(const GameWorld& iWorld) const
{
    CrossOrCircle first = iWorld.getState(combination[0]);
    for(int i=1; i<4; ++i)
    {
        if( first != iWorld.getState(combination[i]) )
        {
            return COC_NEITHER;
        }
    }

    return first;
}

int gWinPulsatingIndex = 0;


void PlaceMarker(GameWorld* world, int cube, int col, int row);

struct Part
{
    Part()
        : extraScale(0)
        , animateIntro(true)
        , direction(1)
    {
    }

    void setup(GameWorld* iWorld, float* iox, float* ioy, int icube, int r, int c)
    {
        world = iWorld;
        ox = iox;
        oy = ioy;
        cube = icube;
        row = r;
        col = c;
        extraScale = 0;
    }

    void start()
    {
        animateIntro = true;
    }

    void render(const render_data& rd)
    {
        CrossOrCircle state = world->getState(cube, col, row);
        // if( state != COC_NEITHER )
        {
            const auto sprite = state == COC_NEITHER
                ? ::onebit::box
                :
                (
                    state == COC_CROSS
                    ? ::onebit::cross
                    : ::onebit::circle
                )
                ;

            float dx = 0;
            float dy = 0;

            const auto extra_size = sprite_size.get_width() * (1+extraScale) - sprite_size.get_width();

            rd.batch->quad
            (
                rd.onebit, sprite_size.translate
                (
                    *ox + Cint_to_float(col) * sprite_size.get_width() + dx,
                    *oy + Cint_to_float(row) * sprite_size.get_height() + dy
                ).extend(extra_size), sprite
            );
        }
    }

    const recti* getSprite()
    {
        CrossOrCircle state = world->getState(cube, col, row);
        if( state != COC_NEITHER )
        {
            if( state == COC_CROSS )
            {
                return &::onebit::cross;
            }
            else
            {
                return &::onebit::circle;
            }
        }
        else
        {
            return nullptr;
        }
    }

    void update(WinningCombination* combo, float delta)
    {
        const float speed = 5.0f;

        if( combo )
        {
            bool scale = false;
            if( combo )
            {
                Index me(cube, col, row);
                if( combo->combination[gWinPulsatingIndex] == me )
                {
                    scale = true;
                }
            }

            if( !scale )
            {
                return;
            }

            constexpr float min = 0.0f;
            constexpr float max = 2.0f;

            extraScale += delta * max * speed * direction * 0.7f;
            if( extraScale > max )
            {
                extraScale = max;
                direction = -1;
            }
            if( extraScale < min )
            {
                extraScale = min;
                direction = 1;
                gWinPulsatingIndex += 1;
                if( gWinPulsatingIndex==4 )
                {
                    gWinPulsatingIndex=0;
                }
            }
        }
        else
        {
            const auto* sprite = getSprite();
            if( sprite && animateIntro )
            {
                extraScale = 4;
                animateIntro = false;
            }

            if( extraScale > 0 )
            {
                extraScale -= delta * 4*speed;
                if( extraScale < 0 )
                {
                    extraScale = 0;
                }
            }
        }
    }

    void testPlacements(float mx, float my, bool mouse)
    {
        CrossOrCircle state = world->getState(cube, col, row);
        if( state == COC_NEITHER )
        {
            const float w = sprite_size.get_width();
            const float h = sprite_size.get_height();
            const float rx = *ox + Cint_to_float(col) * w;
            const float ry = *oy + Cint_to_float(row) * h;
            auto rect = ::rect{rx, ry, rx+w, ry+h};
            const bool over = rect.is_inside_inclusive(mx, my);
            Index me(cube, col, row);

            if( mouse && over )
            {
                gCurrent = me;
            }

            if( !mouse && over && me==gCurrent )
            {
                PlaceMarker(world, cube, col, row);
            }

            if( over )
            {
                gSuggestedLocation.set(rx, ry);
            }
        }
    }

    int cube;
    int row;
    int col;

    float* ox;
    float* oy;
    GameWorld* world;

    float extraScale;
    float direction;
    bool animateIntro;
};

void PlaceMarker(GameWorld* world, int cube, int col, int row)
{
    if( gCurrentCursorState == COC_CIRCLE || gCurrentCursorState == COC_CROSS )
    {
        world->setState(cube, col, row, gCurrentCursorState);
        flipCurrentCursorState();
        gCurrent.clear();
        Click();
    }
}

struct Cube : Object
{
    Cube(float ix, float iy, GameWorld& iWorld, int iCube, WinningCombination** icombo)
        : x(ix)
        , y(iy)
        , world(iWorld)
        , cube(iCube)
        , combo(icombo)
    {
        for(int col=0; col<4; ++col)
        {
            for(int row=0; row<4; ++row)
            {
                parts[col][row].setup(&world, &x, &y, cube, row, col);
            }
        }
    }

    void render(const render_data& rd) override
    {
        // todo(Gustav): render cube
        // cubeSprite->Render(x,y);
        constexpr float g = 0.5f;
        constexpr auto s = sprite_size.get_width() * 4.0f;
        constexpr auto back = rect{s , s};
        rd.batch->quad({}, back.translate(x, y), {}, {g, g, g, 1.0f});
        for(int col=0; col<4; ++col)
        {
            for(int row=0; row<4; ++row)
            {
                parts[col][row].render(rd);
            }
        }
    }

    void start()
    {
        for(int col=0; col<4; ++col)
        {
            for(int row=0; row<4; ++row)
            {
                parts[col][row].start();
            }
        }
    }

    void update(float delta, bool interact, const glm::vec2& mouse, bool down) override
    {
        for(int col=0; col<4; ++col)
        {
            for(int row=0; row<4; ++row)
            {
                parts[col][row].update(*combo, delta);
            }
        }
        if( interact )
        {
            testPlacements(mouse.x, mouse.y, down);
        }
    }

    void testPlacements(float mx, float my, bool mouse)
    {
        for(int col=0; col<4; ++col)
        {
            for(int row=0; row<4; ++row)
            {
                parts[col][row].testPlacements(mx, my, mouse);
            }
        }
    }

private:
    float x;
    float y;
    GameWorld& world;
    const int cube;

    WinningCombination** combo;
    Part parts[4][4];
};

struct StartStep
{
    StartStep(int iStart, int iStep)
        : start(iStart)
        , step(iStep)
    {
    }

    StartStep(int iStep)
        : start(0)
        , step(iStep)
    {
        if( iStep < 0 )
        {
            start = 3;
        }
    }

    int start;
    int step;
};

struct WinningConditionModular
{
    WinningConditionModular(const StartStep& cube, const StartStep& col, const StartStep& row)
        : start(cube.start, col.start, row.start)
        , step(cube.step, col.step, row.step)
    {
    }

    WinningCombination operator()()
    {
        WinningCombination combo;
        Index index = start;
        for(int i=0; i<4; ++i)
        {
            combo.combination[i] = index;
            index+= step;
        }

        return combo;
    }

    Index start;
    Index step;
};

struct PressKeyToContinue : Object
{
    PressKeyToContinue()
        : time(0)
    {
    }

    void update(float delta, bool iInteract, const glm::vec2&, bool) override
    {
        interact = !iInteract;
        if( interact )
        {
            const float PI = 3.14159265f;
            const float max = 2 * PI;
            const float speed = 4.0f;
            time += delta * speed;
            while( time > max )
            {
                time -= max;
            }
        }
    }

    void render(const render_data& rd) override
    {
        if( interact )
        {
            // todo(Gustav): make center
            font.simple_text(rd.batch, rd.onebit, {}, width / 2, height - 35, "Click To Play Again", ::onebit::no_text_animation{});
        }
    }

    float time;
    bool interact;
};

struct Game;
void AddStartNewGameFader(Game& iGame);

struct AiPlacerObject : Object
{
    float thinkTime;
    Random* rand;

    AiPlacerObject(Random* r)
        : thinkTime(0.35f)
        , rand(r)
    {
    }

    void update(float delta, bool interact, const glm::vec2&, bool) override
    {
        if( interact )
        {
            thinkTime -= delta;
            if( thinkTime < 0 )
            {
                kill();
                ExecuteComputerMove(rand);
            }
        }
    }
};

struct Game
{
    Game()
        : quit(false)
        , quiting(false)
        , hasWon(false)
        , combo(0)
        , interactive(true)
        , aiHasMoved(false)
    {
        add( new Background("BackgroundSprite") );
        for(int i=0; i<4; ++i)
        {
            add
            (
                cube[i] = new Cube
                (
                    Cint_to_float(25 + i*193),
                    Cint_to_float(48 + i*126),
                    world, i, &combo
                )
            );
        }
        add( new PressKeyToContinue() );
        add( new IconPlacer() );
        add( new FadeFromBlack(fade_time_intro) );
    }

    void buildRules()
    {
        for(int cube_index=0; cube_index<4; ++cube_index)
        {
            world.addWinningCondition( WinningConditionModular(StartStep(cube_index, 0), StartStep(0, 1), StartStep(0,1)) );
            world.addWinningCondition( WinningConditionModular(StartStep(cube_index, 0), StartStep(0, 1), StartStep(3,-1)) );
            for(int i=0; i<4; ++i)
            {
                world.addWinningCondition( WinningConditionModular(StartStep(cube_index, 0), StartStep(i, 0), StartStep(0,1)) );
                world.addWinningCondition( WinningConditionModular(StartStep(cube_index, 0), StartStep(0, 1), StartStep(i,0)) );
            }
        }
        if( gHardMode )
        {
            world.addWinningCondition( WinningConditionModular(StartStep(0, 1), StartStep(0, 1), StartStep(0,1)) );
            world.addWinningCondition( WinningConditionModular(StartStep(0, 1), StartStep(0, 1), StartStep(3,-1)) );

            world.addWinningCondition( WinningConditionModular(StartStep(3, -1), StartStep(0, 1), StartStep(0,1)) );
            world.addWinningCondition( WinningConditionModular(StartStep(3, -1), StartStep(0, 1), StartStep(3,-1)) );
            for(int i=0; i<4; ++i)
            {
                world.addWinningCondition( WinningConditionModular(StartStep(0, 1), StartStep(i, 0), StartStep(0,1)) );
                world.addWinningCondition( WinningConditionModular(StartStep(0, 1), StartStep(0, 1), StartStep(i,0)) );

                world.addWinningCondition( WinningConditionModular(StartStep(0, 1), StartStep(i, 0), StartStep(3,-1)) );
                world.addWinningCondition( WinningConditionModular(StartStep(0, 1), StartStep(3, -1), StartStep(i,0)) );
            }
        }

        for(int row=0; row<4; ++row)
        {
            for(int col=0; col<4; ++col)
            {
                world.addWinningCondition( WinningConditionModular(StartStep(0, 1), StartStep(col, 0), StartStep(row,0)) );
            }
        }
    }

    void render(const render_data& rd)
    {
        const std::size_t length = mObjects.size();
        for(std::size_t i=0; i<length; ++i)
        {
            mObjects[i]->render(rd);
        }
    }

    void clearBoard()
    {
        world.clear();
        hasWon = false;
        combo = 0;
        for(int c=0; c<4; ++c)
        {
            cube[c]->start();
        }
    }

    void newGame()
    {
        SetStartCursor();
        interactive = true;
    }

    void click()
    {
        fmt::print("sfx: click\n");
    }

    void cheer()
    {
        fmt::print("sfx: cheer\n");
    }

    void on_escape_key()
    {
        if( !quiting )
        {
            add( new FadeToBlackAndExit(fade_time_outro) );
            quiting = true;
        }
    }

    void update(Random* rand, bool mouse, bool oldMouse, float delta, const glm::vec2& mouse_position, bool enter_state)
    {
        gSuggestedLocation.clear();

        if( interactive && hasWon && mouse==false&& mouse != oldMouse )
        {
            AddStartNewGameFader(*this);
            interactive = false;
        }
        const bool interact = !hasWon && interactive;
        const std::size_t length = mObjects.size();

        for(std::size_t i=0; i<length; ++i)
        {
            mObjects[i]->update(delta, interact, mouse_position, mouse);
        }
        mObjects.erase(std::remove_if(mObjects.begin(), mObjects.end(), IsRemoveable), mObjects.end() );

        if( interactive && enter_state )
        {
            displayNoWinner();
        }

        if( !mouse )
        {
            gCurrent.clear();
        }

        oldMouse = mouse;

        if( world.hasBeenModifiedSinceLastCall() )
        {
            WinningCombination winning_combo;
            CrossOrCircle result = world.testWinningConditions(&winning_combo);
            if( result != COC_NEITHER )
            {
                displayWinner(winning_combo);
            }
            else if( !world.canPlacemarker() )
            {
                displayNoWinner();
            }
        }

        if( interactive && gAgainstComputer )
        {
            if( gCurrentCursorState == COC_AI && !aiHasMoved )
            {
                add( new AiPlacerObject(rand) );
                aiHasMoved = true;
            }
            else if( gCurrentCursorState != COC_AI )
            {
                aiHasMoved = false;
            }
        }
    }

    void displayNoWinner()
    {
        hasWon = true;
        mWinningCombo = WinningCombination();
        SetCompleteCursor();
        gWinPulsatingIndex = 0;
        combo = nullptr;
    }

    void displayWinner(const WinningCombination& new_combo)
    {
        hasWon = true;
        mWinningCombo = new_combo;
        SetCompleteCursor();
        gWinPulsatingIndex = 0;
        combo = &mWinningCombo;
        cheer();
    }

    void add(Object* iObject)
    {
        ObjectPtr object(iObject);
        mObjects.push_back(object);
    }

    vector< ObjectPtr> mObjects;
    bool quit;
    bool quiting;
    GameWorld world;
    Cube* cube[4];
    bool hasWon;
    WinningCombination mWinningCombo;
    WinningCombination* combo;
    bool interactive;
    bool aiHasMoved;
};

Game* gGame = 0;

void ComputerPlaceMarker(int cube, int col, int row)
{
    gAgainstComputer = false;
    gCurrentCursorState = COC_CIRCLE;
    const bool free = gGame->world.isPlaceFree(cube, col, row);
    assert( free );
    PlaceMarker(&gGame->world, cube, col, row);
    gAgainstComputer = true;
}

float GetWinFactorPlacement(const WinningCombination& combo)
{
    float value = 0;
    for(int i=0; i<4; ++i)
    {
        switch( gGame->world.getState(combo.combination[i]) )
        {
        case COC_CIRCLE:
            value += 1;
            break;
        case COC_NEITHER:
            break;
        case COC_CROSS:
            return 0;
        default:
            return -1;
        }
    }

    return value;
}

float GetStopFactorPlacement(const WinningCombination& combo)
{
    float value = 1;
    for(int i=0; i<4; ++i)
    {
        switch( gGame->world.getState(combo.combination[i]) )
        {
        case COC_CIRCLE:
            return 0;
            break;
        case COC_CROSS:
            value += 1;
            break;
        case COC_NEITHER:
            break;
        default:
            return -1;
        }
    }

    if( value > 1 ) return value;
    else return 0;
}

bool ExecutePlaceWin(Random* rand)
{
    vector<WinningCombination> combinations;
    const size_t count = gGame->world.getWinningComditions(&combinations);
    WinningCombination bestCombo;
    float bestFactor = -1;
    for(size_t i=0; i<count; ++i)
    {
        auto FREE = [&](int j) -> bool
        {
            return gGame->world.isPlaceFree(combinations[i].combination[j]);
        };

        if( FREE(0) || FREE(1) || FREE(2) || FREE(3) )
        {
            float factor = GetWinFactorPlacement(combinations[i]) + GetStopFactorPlacement(combinations[i]) ;
            if( bestFactor < 0 || (factor > bestFactor && rand->get_excluding(100) < 73) )
            {
                bestFactor = factor;
                bestCombo = combinations[i];
            }
        }
    }

    if( bestFactor > 0 )
    {
        int index = 0;
        do
        {
            index = rand->get_excluding(4);
        } while( !gGame->world.isPlaceFree(bestCombo.combination[index]) );
        Index place = bestCombo.combination[index];
        ComputerPlaceMarker(place.mCube, place.mCol, place.mRow);
        return true;
    }

    return false;
}

int GenerateRandomPlace(Random* r)
{
    return r->get_excluding(4);
}

void ExecuteRandomPlacement(Random* r)
{
    int cube = -1;
    int col = -1;
    int row = -1;
    do
    {
        cube = GenerateRandomPlace(r);
        col = GenerateRandomPlace(r);
        row = GenerateRandomPlace(r);
    } while( !gGame->world.isPlaceFree(cube, col, row) );
    ComputerPlaceMarker(cube, col, row);
}

typedef int Factor;

Factor GetPlacingFactor(const Index& i, const vector<WinningCombination>& rules, size_t count)
{
    Factor factor = 0;
    for(size_t ruleIndex=0; ruleIndex<count; ++ruleIndex)
    {
        bool hasMe = false;
        bool hasFree = false;
        for(int placementIndex=0; placementIndex<4; ++placementIndex)
        {
            if( gGame->world.isPlaceFree(rules[ruleIndex].combination[placementIndex]) )
            {
                hasFree = true;
            }
            if( rules[ruleIndex].combination[placementIndex] == i )
            {
                hasMe = true;
            }
        }
        if( hasMe && hasFree )
        {
            // this place is worth taking a deeper look into
            int crosses = 0;
            int circles = 0;
            for(int placementIndex=0; placementIndex<4; ++placementIndex)
            {
                const Index at = rules[ruleIndex].combination[placementIndex];
                switch( gGame->world.getState(at) )
                {
                case COC_CIRCLE:
                    circles += 1;
                    break;
                case COC_CROSS:
                    crosses += 1;
                    break;
                default:
                    break;
                }
            }

            Factor aloneCircles = 0;
            Factor aloneCrosses = 0;
            if( circles == 0 )
            {
                aloneCrosses = crosses;
            }
            if( crosses == 0 )
            {
                aloneCircles = circles;
            }
            if( aloneCrosses == 3 )
            {
                aloneCrosses *= 500;
            }
            if( aloneCrosses >=2 )
            {
                aloneCrosses *= aloneCrosses;
            }
            factor += aloneCircles*aloneCircles + aloneCrosses;
        }
    }

    return factor;
}

struct Placement
{
    Placement(const vector<WinningCombination>& rules, size_t count, const Index& i)
        : index(i)
        , factor(0)
    {
        if( gGame->world.isPlaceFree(index) )
        {
            factor = GetPlacingFactor(index, rules, count);
        }
        else
        {
            factor = -1;
        }
    }

    Index index;
    Factor factor;

    bool operator<(const Placement& i) const
    {
        return factor < i.factor;
    }
};

void ExecuteComputerMoveTest(Random* rand)
{
    multiset<Placement> placements;
    vector<WinningCombination> rules;
    const size_t count = gGame->world.getWinningComditions(&rules);

    for(int cube=0; cube<4; ++cube)
    {
        for(int col=0; col<4; ++col)
        {
            for(int row=0; row<4; ++row)
            {
                Placement placement(rules, count, Index(cube, col, row));
                if( placement.factor >= 0 )
                {
                    placements.insert(placement);
                }
            }
        }
    }

    // this would indicate that there is nowhere to place and that is handled elsewhere
    assert( !placements.empty() );

    vector<Index> bestPlacements;
    const int bestValue = placements.rbegin()->factor;
    for(multiset<Placement>::iterator i=placements.begin(); i != placements.end(); ++i)
    {
        if( i->factor == bestValue )
        {
            bestPlacements.push_back(i->index);
        }
    }

    const int index = rand->get_excluding(Csizet_to_int(bestPlacements.size()));
    Index placeThis = bestPlacements[Cint_to_sizet(index)];
    ComputerPlaceMarker(placeThis.mCube, placeThis.mCol, placeThis.mRow);
}

void ExecuteComputerMove(Random* rand)
{
    if( gBadAi )
    {
        if( ExecutePlaceWin(rand) ) return;
        ExecuteRandomPlacement(rand);
    }
    else
    {
        ExecuteComputerMoveTest(rand);
    }
}

void Click()
{
    gGame->click();
}

struct StartNewGameFader : FullscreenColorSprite
{
    StartNewGameFader()
        : FullscreenColorSprite({0,0,0,0})
        , direction(1)
    {
    }

    void update(float delta, bool, const glm::vec2&, bool) override
    {
        color.a += delta * direction;

        if( color.a > 1 )
        {
            gGame->clearBoard();
            direction = -1;
        }
        if( color.a < 0 )
        {
            gGame->newGame();
            kill();
        }

    }

    float direction;
};

void AddStartNewGameFader(Game& iGame)
{
    iGame.add( new StartNewGameFader() );
}

void QuitGame()
{
    gGame->quit = true;
}

bool FrameFunc(Random* rand, bool mouse, bool oldMouse, float delta, const glm::vec2& mouse_position, bool enter_state)
{
    gGame->update(rand, mouse, oldMouse, delta, mouse_position, enter_state);
    return gGame->quit;
}

void RenderFunc(const render_data& rd)
{
    gGame->render(rd);
}

bool MenuFrameFunc(const glm::vec2& mouse, bool down, float dt)
{
    gMenu->update(mouse, down, dt);
    return false;
}



void MenuRenderFunc(const render_data& rd)
{
    gMenu->render(rd);
}

bool FrameFuncNull()
{
    return false;
}

void RenderFuncNull(const render_data&)
{
}

void BuildRules()
{
    gGame->buildRules();
}

enum struct game_state {empty, game, menu};
game_state state = game_state::menu;

void Clear()
{
    state = game_state::empty;
}

void SetGameCallbacks()
{
    state = game_state::game;
}

void SetMenuCallbacks()
{
    state = game_state::menu;
}


struct fourthd_game : game
{
    glm::vec2 mouse;
    texture onebit;

    bool mouse_button = false;
    bool old_mouse_button = false;
    bool enter_state = false;
    Random random;

    Game game;
    Menu menu;

    void on_mouse_button(const command2&, input::mouse_button button, bool down) override
    {
        if(button != input::mouse_button::left) { return; }

        mouse_button = down;
    }


    fourthd_game()
        : onebit
        (
            ::onebit::load_texture()
        )
        , mouse(0, 0)
    {
        gGame = &game;
        gMenu = &menu;
    }

    bool
    on_update(float dt) override
    {
        switch(state)
        {
        case game_state::empty:
            FrameFuncNull();
            break;
        case game_state::game:
            FrameFunc(&random, mouse_button, old_mouse_button, dt, mouse, enter_state);
            break;
        case game_state::menu:
            MenuFrameFunc(mouse, mouse_button, dt);
            break;
        default:
            assert(false);
            break;
        }

        old_mouse_button = mouse_button;
        return true;
    }

    constexpr static auto layout = layout_data{viewport_style::black_bars, width, height, glm::mat4(1.0f)};

    void
    on_render(const render_command2& rc) override
    {
        auto r = with_layer(rc, layout);

        auto rd = render_data{r.viewport_aabb_in_worldspace, &onebit, r.batch};

        switch(state)
        {
        case game_state::empty:
            RenderFuncNull(rd);
            break;
        case game_state::game:
            RenderFunc(rd);
            break;
        case game_state::menu:
            MenuRenderFunc(rd);
            break;
        default:
            assert(false);
            break;
        }
    }

    void on_mouse_position(const command2& c, const glm::ivec2& p) override
    {
        mouse = with_layer(c, layout).mouse_to_world({p.x, p.y});
    }
};

int
main(int, char**)
{
    return run_game
    (
        "4th Dimension", glm::ivec2{800, 600}, false, []()
        {
            return std::make_shared<fourthd_game>();
        }
    );
}
