#pragma once
namespace companion::header_layout {
struct Rect {int x,y,width,height;};
// Native feature Controls/Back owns x188..320/y2..42. Stock Back owns
// x260..316/y2..42. Companion actions must leave both regions unobstructed.
inline constexpr Rect Export{8,2,68,28},Status{80,2,68,28},Remember{8,44,188,28},Forget{204,44,108,28};
constexpr bool Overlaps(Rect a,Rect b){return a.x<b.x+b.width&&a.x+a.width>b.x&&a.y<b.y+b.height&&a.y+a.height>b.y;}
}
