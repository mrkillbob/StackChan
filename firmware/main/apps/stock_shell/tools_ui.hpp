#pragma once
#include <array>
#include <string_view>
namespace stock_tools::ui {
enum class Screen { Home, Infrared, Learn, Send, Nfc, Edit, Emulate };
enum Action {
    Back         = 900,
    OpenInfrared = 101,
    OpenNfc      = 102,
    OpenLearn    = 103,
    OpenSend     = 104,
    OpenEdit     = 105,
    StartLearn   = 1,
    SendOnce     = 2,
    StartEmulate = 4,
    Stop         = 5,
    NextKeys     = 210,
    Erase        = 211,
    Save         = 212,
    Character    = 300
};
inline Screen Parent(Screen screen)
{
    switch (screen) {
        case Screen::Learn:
        case Screen::Send:
            return Screen::Infrared;
        case Screen::Edit:
        case Screen::Emulate:
            return Screen::Nfc;
        default:
            return Screen::Home;
    }
}
inline bool Allowed(Screen screen, int action)
{
    if (action == Back) return true;
    switch (screen) {
        case Screen::Home:
            return action == OpenInfrared || action == OpenNfc;
        case Screen::Infrared:
            return action == OpenLearn || action == OpenSend;
        case Screen::Learn:
            return action == StartLearn || action == Stop || action == OpenSend;
        case Screen::Send:
            return action == SendOnce;
        case Screen::Nfc:
            return action == OpenEdit || action == StartEmulate;
        case Screen::Edit:
            return action == NextKeys || action == Erase || action == Save ||
                   (action >= Character + 33 && action <= Character + 126);
        case Screen::Emulate:
            return action == StartEmulate || action == Stop;
    }
    return false;
}
inline constexpr std::array<std::string_view, 8> KeyPages = {"abcdefghij", "klmnopqrst", "uvwxyz./:-", "0123456789",
                                                             "ABCDEFGHIJ", "KLMNOPQRST", "UVWXYZ?=&_", "%#~+!$()*;"};
struct Rect {
    int x, y, w, h;
};
inline constexpr Rect BackButton{8, 4, 84, 44};
inline constexpr Rect RowOne{8, 64, 304, 64}, RowTwo{8, 140, 304, 64};
inline constexpr Rect EmulateDetail{8, 152, 304, 20};
inline constexpr Rect Primary{8, 172, 304, 60};
inline constexpr Rect EditButton{8, 104, 304, 56}, EmulateButton{8, 176, 304, 56};
inline constexpr Rect UrlInput{8, 52, 304, 44};
inline constexpr std::array<Rect, 3> EditorFooter = {Rect{8, 192, 88, 44}, Rect{100, 192, 64, 44},
                                                     Rect{168, 192, 144, 44}};
inline Rect KeyRect(int index)
{
    return {8 + (index % 5) * 60, 100 + (index / 5) * 46, 56, 44};
}
}  // namespace stock_tools::ui
