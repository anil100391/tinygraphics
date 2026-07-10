#pragma once

#include <ctime>
#include <chrono>
#include <format>
#include <iomanip>
#include <iostream>
#include <sstream>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
enum class LogLevel
{
    Debug,
    Info,
    Warning,
    Error
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
inline constexpr std::string_view ToString( LogLevel lvl )
{
    // ascii escape sequence note
    // Bright/normal:
    //   Bold: \033[1m
    //   Dim:  \033[2m
    // Foreground colors:
    //   Black:   \033[30m
    //   Red:     \033[31m
    //   Green:   \033[32m
    //   Yellow:  \033[33m
    //   Blue:    \033[34m
    //   Magenta: \033[35m
    //   Cyan:    \033[36m
    //   White:   \033[37m
    switch ( lvl )
    {
    case LogLevel::Debug:
        return "\033[1mDEBUG \033[0m";
    case LogLevel::Info:
        return "\033[1;32mINFO  \033[0m";
    case LogLevel::Warning:
        return "\033[1;35mWARN  \033[0m";
    case LogLevel::Error:
        return "\033[1;31mERROR \033[0m";
    }

    return "UNKNOWN";
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
inline std::string TimeStamp()
{
    auto        now = std::chrono::system_clock::now();
    std::time_t t   = std::chrono::system_clock::to_time_t( now );

    std::tm tm{};
#if defined( _WIN32 )
    localtime_s( &tm, &t );
#else
    localtime_r( &t, &tm );
#endif
    std::stringstream ss;
    ss << "\U0001f559 " << std::put_time( &tm, "%H:%M:%S" );
    return ss.str();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <class... Args>
inline void Log( LogLevel lvl, std::string_view fmt, Args &&...args )
{
    std::ostream &out = ( lvl == LogLevel::Warning || lvl == LogLevel::Error )
                            ? std::cerr
                            : std::cout;

    out << ToString( lvl ) << TimeStamp() << " : "
        << std::vformat( fmt, std::make_format_args( args... ) ) << "\n";
}
