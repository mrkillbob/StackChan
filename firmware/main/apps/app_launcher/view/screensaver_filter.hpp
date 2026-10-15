#pragma once
#include <array>
namespace view::saver {
// Persisted IDs remain unchanged: DVD=0, upstream catalog index+1.
// Remove whole light/flashing and red/blue full-background modes, including ChessModels.
// Stable IDs remain tombstones; intentional neutral gray and colored foregrounds remain.
constexpr std::array<unsigned,125> ExcludedSlides={4,5,6,13,14,17,18,19,21,22,23,24,25,26,27,28,29,30,31,32,33,35,36,37,40,41,43,45,46,48,49,51,52,53,54,56,57,58,59,60,61,63,65,68,76,81,95,97,107,112,113,122,124,125,126,127,130,135,139,144,146,147,148,150,151,156,175,179,181,192,195,197,199,201,203,204,206,208,209,210,213,214,217,221,229,231,233,238,239,241,243,244,246,250,251,256,271,280,285,288,290,293,294,302,303,304,306,308,309,310,312,313,314,316,317,318,319,323,326,327,328,330,331,334,341};
constexpr bool Allowed(unsigned id) {
    if(id>=342)return false;
    for(auto excluded:ExcludedSlides)if(id==excluded)return false;
    return true;
}
constexpr unsigned AllowedCount=342-ExcludedSlides.size();
inline unsigned AllowedOrdinal(unsigned id){unsigned n=0;for(unsigned i=0;i<=id&&i<342;++i)n+=Allowed(i);return n;}
inline unsigned AdjacentAllowed(unsigned id,bool forward){for(unsigned n=0;n<342;++n){id=(id+(forward?1:341))%342;if(Allowed(id))return id;}return 0;}
}
