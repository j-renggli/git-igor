#ifdef __linux__
#ifndef backend_EXPORT
#define backend_EXPORT __attribute__((visibility("default")))
#endif
#else
#ifndef backend_EXPORT
#ifdef DLL_BACKEND
#define backend_EXPORT __declspec(dllexport)
#else
#define backend_EXPORT __declspec(dllimport)
#endif
#endif
#endif
