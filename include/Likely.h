#ifndef _TPOOL_LIKELY_H_
#define _TPOOL_LIKELY_H_


#if defined(__GNUC__)
#define TPOOL_LIKELY(x)   (__builtin_expect((x), 1))
#define TPOOL_UNLIKELY(x) (__builtin_expect((x), 0))
#else
#define TPOOL_LIKELY(x) (x)
#define TPOOL_UNLIKELY(x) (x)
#endif

#endif /* _TPOOL_LIKELY_H_ */
