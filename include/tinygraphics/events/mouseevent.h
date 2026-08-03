#ifndef _mouse_event_h_
#define _mouse_event_h_

#include <events/event.h>
#include <sstream>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class tgrMouseEvent : public tgrEvent
{
public:
    enum class Button
    {
        LEFT,
        RIGHT,
        MIDDLE
    };

    inline int X() const
    {
        return _x;
    }

    inline int Y() const
    {
        return _y;
    }

    virtual int GetCategoryFlags() const override
    {
        return EC_Mouse | EC_Input;
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << GetName() << ": " << _x << ", " << _y;
        return ss.str();
    }

protected:
    tgrMouseEvent( int x, int y ) : _x( x ), _y( y )
    {
    }

    int _x;
    int _y;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class tgrMouseMoveEvent : public tgrMouseEvent
{
public:
    tgrMouseMoveEvent( int x, int y ) : tgrMouseEvent( x, y )
    {
    }

    tgrEventType GetEventType() const override
    {
        return tgrEventType::MouseMoved;
    }

    static tgrEventType GetStaticEventType()
    {
        return tgrEventType::MouseMoved;
    }

    const char *GetName() const override
    {
        return "MouseMovedEvent";
    }
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class tgrMousePressedEvent : public tgrMouseEvent
{
public:
    tgrMousePressedEvent( int x, int y, tgrMouseEvent::Button button )
        : tgrMouseEvent( x, y ), _button( button )
    {
    }

    tgrEventType GetEventType() const override
    {
        return tgrEventType::MouseButtonPressed;
    }

    static tgrEventType GetStaticEventType()
    {
        return tgrEventType::MouseButtonPressed;
    }

    const char *GetName() const override
    {
        return "MousePressedEvent";
    }

    virtual int GetCategoryFlags() const override
    {
        return EC_Mouse | EC_MouseButton | EC_Input;
    }

    [[nodiscard]] tgrMouseEvent::Button GetButton() const noexcept
    {
        return _button;
    }

private:
    tgrMouseEvent::Button _button;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class tgrMouseReleasedEvent : public tgrMouseEvent
{
public:
    tgrMouseReleasedEvent( int x, int y, tgrMouseEvent::Button button )
        : tgrMouseEvent( x, y ), _button( button )
    {
    }

    tgrEventType GetEventType() const override
    {
        return tgrEventType::MouseButtonReleased;
    }

    static tgrEventType GetStaticEventType()
    {
        return tgrEventType::MouseButtonReleased;
    }

    const char *GetName() const override
    {
        return "MouseReleasedEvent";
    }

    virtual int GetCategoryFlags() const override
    {
        return EC_Mouse | EC_MouseButton | EC_Input;
    }

    [[nodiscard]] tgrMouseEvent::Button GetButton() const noexcept
    {
        return _button;
    }

private:
    tgrMouseEvent::Button _button;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class tgrMouseScrollEvent : public tgrMouseEvent
{
public:
    tgrMouseScrollEvent( int x, int y, double xoffset, double yoffset )
        : tgrMouseEvent( x, y ), _xoffset( xoffset ), _yoffset( yoffset )
    {
    }

    tgrEventType GetEventType() const override
    {
        return tgrEventType::MouseScrolled;
    }

    static tgrEventType GetStaticEventType()
    {
        return tgrEventType::MouseScrolled;
    }

    const char *GetName() const override
    {
        return "MouseScrolledEvent";
    }

    virtual int GetCategoryFlags() const override
    {
        return EC_Mouse | EC_MouseButton | EC_Input;
    }

    double XOffset() const noexcept
    {
        return _xoffset;
    }

    double YOffset() const noexcept
    {
        return _yoffset;
    }

private:
    double _xoffset;
    double _yoffset;
};

#endif // _mouse_event_h_
