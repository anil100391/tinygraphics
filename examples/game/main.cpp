#include <filesystem>
#include <game.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int main(int argc, const char* argv[])
{
    WindowProperties wprops;
    wprops._width = 1920 / 2;
    wprops._height = 1920 / 2;
    wprops._maximized = false;
    wprops._notitle = false;
    wprops._title = "tinygraphics game";

    Game game(wprops, std::filesystem::absolute(argv[0]).parent_path());
    game.Run();

    return 0;
}
