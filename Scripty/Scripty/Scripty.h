/*+===================================================================
  File:      Scripty.h

  Summary:   Contains functions for quickly running powershell
			 scripts and obtaining their results on Windows 11

			 If using in a C++ project:
			 Scripty is a static class and is interfaced with
			 method calls

			 If using C-Calling Conventions:
			 Scripty has function calls roughly equivalent to
			 the C++ version

  Classes:   Scripty

  Functions: Functions exported (in source files).

  Origin:    Samuel Cain (samuelrcain@gmail.com)
			 
===================================================================+*/

#pragma once

#ifdef SCRIPTY_EXPORT
	#define SCRIPTY_API __declspec(dllexport)
	#define SCRIPTYC_API "C" __declspec(dllexport)
	#define LOG_NONE 0
	#define LOG_NOTIFICATIONS 1
	#define LOG_DEBUG 2
	#define LOG_EVERYTHING 3
#else
	#ifdef __cplusplus
		#define SCRIPTY_API __declspec(dllimport)
		#define SCRIPTYC_API "C" __declspec(dllimport)
	#else
		#define LOG_NONE 0
		#define LOG_NOTIFICATIONS 1
		#define LOG_DEBUG 2
		#define LOG_EVERYTHING 3
		#define SCRIPTYC_API __declspec(dllimport)
	#endif
#endif

#ifdef SCRIPTY_EXPORT

#include <string>		// Needed for non-barbaric strings
#include <ctime>		// Needed for random number seeding

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Class:    Scripty

  Summary:  Provides an easy way to run powershell scripts

  Methods:  Init
			  Intializes Scripty, seeds random number generator
			SeedGenerator
			  Seeds random number generator
			ChangeLogLevel
			  Adjusts the amount of logging
			RunScript
			  Runs powershell script
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
class Scripty
{
public:
	// Enum to specify what should get logged
	enum class LogLevel { NONE, ERRORS_ONLY, NOTIFICATIONS, DEBUG, EVERYTHING };
private:
	static const int LETTER_MIN		= 65;	// Decimal representation of 'a'
	static const int LETTER_MAX		= 90;	// Deciaml representation of 'z'
	static const int CAPS_DISTANCE	= 32;	// Distance between lower case and upper case counterpart

	static int FILE_NAME_LEN;				// File name length

	static bool seeded;						// Flag to check whether generator has been seeded
	static bool allowExecPolicyChange;		// Flag to check whether the ExecutionPolicy is allowed to be changed

	static std::string origExecPolicy;		// ExecutionPolicy prior to running Scripty
	static std::string currExecPolicy;		// Current ExecutionPolicy

	static LogLevel loglevel;				// Default logging only logs notifications

public:

	static char RandChar();

	static std::string GenerateUniqueNameInDir(const std::string& fileType = ".txt");

	static void ChangeExecutionPolicy();

public:

	static SCRIPTY_API bool Init(bool changeExecutionPolicyAllowed = false, unsigned int seed = time(0),
		unsigned int fnLen = 10);

	static SCRIPTY_API void ChangeLogLevel(LogLevel logLvl = LogLevel::NOTIFICATIONS);

	static SCRIPTY_API void SeedGenerator(unsigned int seed);

	static SCRIPTY_API std::stringstream RunScript(std::istream& script);

	static SCRIPTY_API std::stringstream RunScript(std::istream& script, 
		bool (*SuccessCondition)(const std::string&&));

	static SCRIPTY_API std::stringstream RunScript(const std::string& script);
};

#endif

extern SCRIPTYC_API int Init(int changeExecutionPolicyAllowed, unsigned int seed,
	unsigned int fnLen);

extern SCRIPTYC_API void SeedGenerator(unsigned int seed);

extern SCRIPTYC_API void ChangeLogLevel(unsigned int loglvl);

extern SCRIPTYC_API long RunScript(char* script, char* res);

extern SCRIPTYC_API long RunScript_p(char* script, char* res);

extern SCRIPTYC_API long RunScriptCond(char* script, char* res, int (*SuccessCondition(char* str, const int len)));