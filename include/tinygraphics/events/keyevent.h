#ifndef _key_event_h_
#define _key_event_h_

#include <events/event.h>
#include <sstream>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class tgrKeyEvent : public tgrEvent
{
public:
    inline int GetKeyCode() const
    {
        return _keyCode;
    }

    virtual int GetCategoryFlags() const
    {
        return EC_Keyboard | EC_Input;
    }

    virtual tgrEventType GetEventType() const = 0;

protected:
    tgrKeyEvent( int keycode ) : _keyCode( keycode )
    {
    }

    int _keyCode;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class tgrKeyPressedEvent : public tgrKeyEvent
{
public:
    tgrKeyPressedEvent( int keycode, int repeatcount )
        : tgrKeyEvent( keycode ), _repeatCount( repeatcount )
    {
    }

    inline int GetRepeatCount() const
    {
        return _repeatCount;
    }

    tgrEventType GetEventType() const override
    {
        return tgrEventType::KeyPressed;
    }

    static tgrEventType GetStaticEventType()
    {
        return tgrEventType::KeyPressed;
    }

    const char *GetName() const override
    {
        return "KeyPressedEvent";
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "KeyPressedEvent: " << _keyCode << " (" << _repeatCount
           << " repeats)";
        return ss.str();
    }

private:
    int _repeatCount = 0;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class tgrKeyReleasedEvent : public tgrKeyEvent
{
public:
    tgrKeyReleasedEvent( int keycode ) : tgrKeyEvent( keycode )
    {
    }

    tgrEventType GetEventType() const override
    {
        return tgrEventType::KeyReleased;
    }

    static tgrEventType GetStaticEventType()
    {
        return tgrEventType::KeyReleased;
    }

    const char *GetName() const override
    {
        return "KeyReleasedEvent";
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "KeyReleased: " << _keyCode;
        return ss.str();
    }
};

#endif // _key_event_h_
