//obtained from [William Swanson's GitHub](https://github.com/swansontec/map-macro/blob/ce95c50f32b3c9fed2f9780948010dccbe2b75a3/map.h) and slightly modified

#ifndef MCSL_MAP_H
#define MCSL_MAP_H

#define __MCSL_EVAL0(...) __VA_ARGS__
#define __MCSL_EVAL1(...) __MCSL_EVAL0(__MCSL_EVAL0(__MCSL_EVAL0(__VA_ARGS__)))
#define __MCSL_EVAL2(...) __MCSL_EVAL1(__MCSL_EVAL1(__MCSL_EVAL1(__VA_ARGS__)))
#define __MCSL_EVAL3(...) __MCSL_EVAL2(__MCSL_EVAL2(__MCSL_EVAL2(__VA_ARGS__)))
#define __MCSL_EVAL4(...) __MCSL_EVAL3(__MCSL_EVAL3(__MCSL_EVAL3(__VA_ARGS__)))
#define __MCSL_EVAL(...)  __MCSL_EVAL4(__MCSL_EVAL4(__MCSL_EVAL4(__VA_ARGS__)))

#define __MCSL_MAP_END(...)
#define __MCSL_MAP_OUT
#define __MCSL_MAP_COMMA ,

#define __MCSL_MAP_GET_END2() 0, __MCSL_MAP_END
#define __MCSL_MAP_GET_END1(...) __MCSL_MAP_GET_END2
#define __MCSL_MAP_GET_END(...) __MCSL_MAP_GET_END1
#define __MCSL_MAP_NEXT0(test, next, ...) next __MCSL_MAP_OUT
#define __MCSL_MAP_NEXT1(test, next) __MCSL_MAP_NEXT0(test, next, 0)
#define __MCSL_MAP_NEXT(test, next)  __MCSL_MAP_NEXT1(__MCSL_MAP_GET_END test, next)

#define __MCSL_MAP0(f, x, peek, ...) f(x) __MCSL_MAP_NEXT(peek, __MCSL_MAP1)(f, peek, __VA_ARGS__)
#define __MCSL_MAP1(f, x, peek, ...) f(x) __MCSL_MAP_NEXT(peek, __MCSL_MAP0)(f, peek, __VA_ARGS__)

#define __MCSL_MAP_LIST_NEXT1(test, next) __MCSL_MAP_NEXT0(test, __MCSL_MAP_COMMA next, 0)
#define __MCSL_MAP_LIST_NEXT(test, next)  __MCSL_MAP_LIST_NEXT1(__MCSL_MAP_GET_END test, next)

#define __MCSL_MAP_LIST0(f, x, peek, ...) f(x) __MCSL_MAP_LIST_NEXT(peek, __MCSL_MAP_LIST1)(f, peek, __VA_ARGS__)
#define __MCSL_MAP_LIST1(f, x, peek, ...) f(x) __MCSL_MAP_LIST_NEXT(peek, __MCSL_MAP_LIST0)(f, peek, __VA_ARGS__)

//Applies the function macro `f` to each of the remaining parameters
#define MCSL_MAP(f, ...) __MCSL_EVAL(__MCSL_MAP1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

//Applies the function macro `f` to each of the remaining parameters and inserts commas between the results
#define MCSL_MAP_LIST(f, ...) __MCSL_EVAL(__MCSL_MAP_LIST1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

#endif //MCSL_MAP_H