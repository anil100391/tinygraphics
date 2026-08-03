#ifndef _event_h_
#define _event_h_

#include <string>
#include <functional>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
enum class tgrEventType
{
    None                = 0,
    WindowClose         = 1,
    WindowResize        = 2,
    WindowFocus         = 3,
    WindowLostFocus     = 4,
    WindowMoved         = 5,
    KeyPressed          = 6,
    KeyReleased         = 7,
    MouseButtonPressed  = 8,
    MouseButtonReleased = 9,
    MouseMoved          = 10,
    MouseScrolled       = 11
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
enum tgrEventCategory
{
    None           = 0,
    EC_Application = 1 << 0,
    EC_Input       = 1 << 1,
    EC_Keyboard    = 1 << 2,
    EC_Mouse       = 1 << 3,
    EC_MouseButton = 1 << 4
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class tgrEvent
{
    friend class tgrEventDispatcher;

public:
    virtual tgrEventType GetEventType() const     = 0;
    virtual const char  *GetName() const          = 0;
    virtual int          GetCategoryFlags() const = 0;
    virtual std::string  ToString() const
    {
        return GetName();
    }

    inline bool IsInCategory( tgrEventCategory category )
    {
        return GetCategoryFlags() & category;
    }

protected:
    bool _handled = false;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class tgrEventDispatcher
{
    template <class evt>
    using EventFn = std::function<bool( evt & )>;

public:
    tgrEventDispatcher( tgrEvent &event ) : _event( event )
    {
    }

    template <class evt>
    bool Dispatch( EventFn<evt> func )
    {
        if ( _event.GetEventType() == evt::GetStaticType() )
        {
            _event._handled = func( *(evt *)&_event );
            return true;
        }
        return false;
    }

private:
    tgrEvent &_event;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
inline std::ostream &operator<<( std::ostream &os, const tgrEvent &evt )
{
    return os << evt.ToString();
}

#endif // _event_h_
