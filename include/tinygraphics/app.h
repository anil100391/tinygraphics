#ifndef _app_h_
#define _app_h_

#include <string>

#include <events/keyevent.h>
#include <events/mouseevent.h>
#include <events/windowevent.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
struct GLFWwindow;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
struct tgrWindowProperties
{
    tgrWindowProperties() = default;
    tgrWindowProperties( unsigned int w,
                         unsigned int h,
                         const char  *title,
                         bool         maximized );
    ~tgrWindowProperties() = default;

    unsigned int _width     = 960u;
    unsigned int _height    = 540u;
    std::string  _title     = "tinygraphics";
    bool         _maximized = false;
    bool         _notitle   = false;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class tgrApplication
{
public:
    tgrApplication( const tgrWindowProperties &wprops = tgrWindowProperties(),
                    bool                       initDearImGui = true );

    virtual ~tgrApplication();

    void Run();

    virtual void Update();

    void  GetWindowSize( int &width, int &height ) const noexcept;
    float GetCurrentTime() const noexcept;
    void  GetCursorPosition( double &x, double &y ) const noexcept;

    virtual bool OnEvent( tgrEvent &evt );

protected:
    tgrWindowProperties _windPros;
    GLFWwindow         *_window = nullptr;

private:
    bool _initDearImGui = true;
};

#endif // _app_h_
