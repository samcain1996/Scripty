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

/**
 * @brief Initializes Scripty
 *
 * @param changeExecutionPolicyAllowed		Defines if Scripty is allowed to modify
 *											the current user's ExecutionPolicy
 * @param seed								Seed for random number generator
 * @param FN_LEN							Length of temporary file names
 * @return									Whether Scripty successfully initialized
 */
 extern SCRIPTY_API bool ScriptyInit(bool changeExecutionPolicyAllowed = false, unsigned int seed = time(0),
	 unsigned int FN_LEN = 20);

 /**
  * @brief          Seeds random number generator for file generation
  *
  * @param seed     number to seed generator with
  */
 extern SCRIPTY_API void SeedGenerator(unsigned int seed);

 /**
  * @brief Runs script and returns results as stringstream
  *
  * @param script    script to run as istream
  * @return          results of script as stringstream
  */
 extern SCRIPTY_API std::stringstream runScript(std::istream& script);
