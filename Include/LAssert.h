#pragma once

#include <assert.h>

#ifndef CHECK_BOOL
#define CHECK_BOOL(Condition) \
    if (!(Condition)) {       \
        return false;         \
    }
#endif

#ifndef CHECK_HRESULT
#define CHECK_HRESULT(hr) \
    if (!SUCCEEDED(hr)) { \
        return false;     \
    }
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(p)    \
    if ((p)) {          \
        free((p));      \
        (p) = nullptr;  \
    }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) \
    if ((p)) {         \
        delete (p);    \
        (p) = nullptr; \
    }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) \
    if ((p)) {               \
        delete[] (p);        \
        (p) = nullptr;       \
    }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) \
    if ((p)) {          \
        (p)->Release(); \
        (p) = nullptr;  \
    }
#endif

#ifndef SAFE_CLOSE_HANDLE
#define SAFE_CLOSE_HANDLE(h)    \
    if ((h)) {                  \
        CloseHandle((h));       \
        (h) = nullptr;          \
    }
#endif

#define BOOL_ERROR_BREAK(Condition)   \
    if (!(Condition)) {               \
        break;                        \
    }

#define BOOL_ERROR_EXIT(Condition)   \
    if (!(Condition)) {               \
        goto Exit0;                        \
    }