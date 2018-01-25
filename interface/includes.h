#ifdef __linux__
#ifndef interface_EXPORT
#define interface_EXPORT __attribute__((visibility("default")))
#endif
#else
#ifndef interface_EXPORT
#ifdef DLL_BACKEND
#define interface_EXPORT __declspec(dllexport)
#else
#define interface_EXPORT __declspec(dllimport)
#endif
#endif
#endif
