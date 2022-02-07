// Scripty.hpp - Contains declarations of Scripty functions
#pragma once

#ifdef SCRIPTY_FUNCS
#define SCRIPTY_API __declspec(dllexport)
#else
#define SCRIPTY_API __declspec(dllimport)
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <ctime>
#include <string>
#include <filesystem>


// Enum to specify what should get logged
enum class ScriptyLogLevel { NONE, ERRORS_ONLY, NOTIFICATIONS, DEBUG, EVERYTHING };

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
	unsigned int fnLen = 10);

/**
	* @brief Modifies the ScriptyLogLevel, changing what is written to std::clog
	*
	* @param ll		Logging level, default only logs notifications
	*/
extern SCRIPTY_API void ScriptyChangeLogLevel(ScriptyLogLevel logLvl = ScriptyLogLevel::NOTIFICATIONS);

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

/**
	* @brief Runs script and returns results as stringstream
	*
	* @param script    script to run as string
	* @return          results of script as stringstream
	*/
extern SCRIPTY_API std::stringstream runScript(const std::string& script);

/**
	* @brief Runs script and returns results as stringstream
	*
	* @param script    script to run as string
	* @return          results of script as stringstream
	*/
extern SCRIPTY_API std::stringstream runScript(std::istream& script, bool(*SuccessCondition(const std::string&&)));
