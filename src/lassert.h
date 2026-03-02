#ifndef _LASSERT_H_
#define _LASSERT_H_

#ifdef __cplusplus
extern "C"
{
#endif

    void lassert_fail(char *expr, char *file, int line);

#ifndef LASSERT
#define LASSERT(expr) \
    if (!(expr))      \
    lassert_fail(#expr, __FILE__, __LINE__)
#endif // !LASSERT

#ifdef __cplusplus
}
#endif

#endif // !_LASSERT_H_
