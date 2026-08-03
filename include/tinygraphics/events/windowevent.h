#ifndef _window_event_h_
#define _window_event_h_

#include <events/event.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class tgrWindowResizeEvent : public tgrEvent
{
public:
    virtual int GetCategoryFlags() const override
    {
        return EC_Application | EC_Input;
    }

    virtual tgrEventType GetEventType() const override
    {
        return tgrEventType::WindowResize;
    }

    virtual const char *GetName() const override
    {
        return "Window Resize";
    }

    tgrWindowResizeEvent( int w, int h ) : _w( w ), _h( h )
    {
    }

    int Width() const
    {
        return _w;
    }

    int Height() const
    {
        return _h;
    }

private:
    int _w;
    int _h;
};

#endif // _window_event_h_
