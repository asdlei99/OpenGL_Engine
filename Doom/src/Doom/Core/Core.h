#pragma once

#ifdef DOOM_ENGINE
#define DOOM_API __declspec(dllexport)
#else
#define DOOM_API __declspec(dllimport)
#endif