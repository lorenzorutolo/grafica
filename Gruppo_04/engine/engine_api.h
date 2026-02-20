#pragma once

/////////////
// VERSION //
/////////////

#ifdef _DEBUG
#   define LIB_NAME      "My Graphics Engine v0.1a (debug)"
#else
#   define LIB_NAME      "My Graphics Engine v0.1a"
#endif

#define LIB_VERSION 10


///////////////
// EXPORT API //
///////////////

#ifdef _WIN32               // ? questa macro è giusta su Windows
#ifdef ENGINE_EXPORTS
#define ENG_API __declspec(dllexport)
#else
#define ENG_API __declspec(dllimport)
#endif

#pragma warning(disable : 4251)

#else                       // ? Linux / macOS / Unix
#define ENG_API        // niente export
#endif
