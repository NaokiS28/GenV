#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>

/* -------------------------------------------------------------------------
 * String helpers
 * ------------------------------------------------------------------------- */

inline static bool _charIsPrintable(const char c)
{
    if (c <= '~' && c >= ' ') return true;
    return false;
}

inline static bool _charIsLetter(const char c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return true;
    return false;
}

#ifdef __cplusplus
}
#endif
