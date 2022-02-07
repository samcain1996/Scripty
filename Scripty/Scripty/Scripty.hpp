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

class Scripty
{
public:
	// Enum to specify what should get logged
	SCRIPTY_API enum class LogLevel { NONE, ERRORS_ONLY, NOTIFICATIONS, DEBUG, EVERYTHING };
private:
	static const int LETTER_MIN = 65;  // Decimal representation of 'a'
	static const int LETTER_MAX = 90;  // Deciaml representation of 'z'
	static const int CAPS_DISTANCE = 32;  // Distance between lower case and upper case counterpart

	static int FILE_NAME_LEN;  // File name length

	static bool seeded;     // Flag to check whether generator has been seeded

	static bool allowExecPolicyChange;  // Flag to check whether the ExecutionPolicy is allowed to be changed

	static std::string origExecPolicy;     // ExecutionPolicy prior to running Scripty
	static std::string currExecPolicy;     // Current ExecutionPolicy

	static LogLevel logLevel;  // Default logging only logs notifications

private:

	static char RandChar();
	static std::string GenerateUniqueNameInDir(const std::string& fileType = ".txt");
	static void ChangeExecutionPolicy();

public:

	static SCRIPTY_API bool Init(bool changeExecutionPolicyAllowed = false, unsigned int seed = time(0),
		unsigned int fnLen = 10);

	static SCRIPTY_API void ChangeLogLevel(LogLevel logLvl = LogLevel::NOTIFICATIONS);

	static SCRIPTY_API void SeedGenerator(unsigned int seed);

	static SCRIPTY_API std::stringstream RunScript(std::istream& script);

	static SCRIPTY_API std::stringstream RunScript(std::istream& script, bool (*SuccessCondition)(std::string&&));

	static SCRIPTY_API std::stringstream RunScript(const std::string& script);
};

/**
	 * @brief Initializes Scripty
	 *
	 * @param changeExecutionPolicyAllowed		Defines if Scripty is allowed to modify
	 *											the current user's ExecutionPolicy
	 * @param seed								Seed for random number generator
	 * @param FN_LEN							Length of temporary file names
	 * @return									Whether Scripty successfully initialized
	 */

/**
	* @brief Modifies the ScriptyLogLevel, changing what is written to std::clog
	*
	* @param ll		Logging level, default only logs notifications
	*/

/**
	 * @brief          Seeds random number generator for file generation
	 *
	 * @param seed     number to seed generator with
	 */

/**
	 * @brief Runs script and returns results as stringstream
	 *
	 * @param script    script to run as istream
	 * @return          results of script as stringstream
	 */

/**
	 * @brief Runs script and returns results as stringstream
	 *
	 * @param script    script to run as istream
	 * @return          results of script as stringstream
	 */

/**
	* @brief Runs script and returns results as stringstream
	*
	* @param script    script to run as string
	* @return          results of script as stringstream
	*/

