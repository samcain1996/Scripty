// Scripty.h - Contains declarations of Scripty functions
#pragma once

#ifdef SCRIPTY_FUNCS
#define SCRIPTY_API __declspec(dllexport)
#else
#define SCRIPTY_API __declspec(dllimport)
#endif

#include <fstream>
#include <sstream>
#include <random>
#include <ctime>
#include <string>
#include <filesystem>

extern SCRIPTY_API std::stringstream runScript(std::istream& script);

extern SCRIPTY_API void SeedGenerator(unsigned int seed);