#ifndef TME_EXPORT_HPP
#define TME_EXPORT_HPP

#if defined(_WIN32) || defined(_WIN64)
    #ifdef TME_EXPORTS
        #define TME_API __declspec(dllexport)
    #else
        #define TME_API __declspec(dllimport)
    #endif
#else
    #define TME_API __attribute__((visibility("default")))
#endif

#endif