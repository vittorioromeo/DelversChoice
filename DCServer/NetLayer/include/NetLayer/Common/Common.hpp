#pragma once

#include "./API.hpp"
#include "./Deps.hpp"
#include "./Typedefs.hpp"

// TODO: improve
#if(1)

#define NL_DEBUGLO() ::ssvu::lo() << "\nDEBUG: "

#else

#define NL_DEBUGLO() ::ssvu::loNull()

#endif
