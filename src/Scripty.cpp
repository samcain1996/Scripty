/*+===================================================================
  File:      Scripty.cpp

  Summary:   Contains functions for quickly running powershell
			 scripts and obtaining their results on Windows 11

			 If using in a C++ project:
			 Scripty is a static class and is interfaced with
			 method calls

			 If using C-Calling Conventions:
			 Scripty has function calls roughly equivalent to
			 the C++ version

  Classes:   Scripty

  Origin:    Samuel Cain (samuelrcain@gmail.com)

===================================================================+*/

#include "pch.h"
#include "Scripty.h"

#ifdef __cplusplus

// Necessary additional headers
#include <iostream>
#include <fstream>
#include <random>
#include <filesystem>

// Using-directives to make following code more readable
using std::cerr;
using std::clog;
using std::pair;
using std::string;
using std::istream;
using std::ostream;
using std::stringstream;

#endif


#ifdef __cplusplus

// Class definitions

// Member variables
int Scripty::FILE_NAME_LEN;

bool Scripty::seeded;
bool Scripty::allowExecPolicyChange;

string Scripty::origExecPolicy;
string Scripty::currExecPolicy;

Scripty::LogLevel Scripty::loglevel;

// Scripty Methods

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   Scripty::Init

	  Summary:  Initializes Scripty

	  Args:     bool changeExecutionPolicyAllowed
				  Whether Scripty is allowed to change the CurrentUser's execution policy
				unsigned int seed
				  Seed for random number generator
				unsigned int fnLen
				  Length for temporary file names

	  Modifies: [ FILE_NAME_LEN, seeded, allowExecPolicyChange, origExecPolicy, currExecPolicy, loglevel ].

	  Returns:  bool
				  true... lol
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
bool Scripty::Init(bool changeExecutionPolicyAllowed, unsigned int seed, unsigned int fnLen)
{
	using namespace std;
	using namespace std::filesystem;
	FILE_NAME_LEN = 10;  // File name length

	seeded = false;     // Flag to check whether generator has been seeded

	allowExecPolicyChange = changeExecutionPolicyAllowed;  // Flag to check whether the ExecutionPolicy is allowed to be changed

	origExecPolicy = "";     // ExecutionPolicy prior to running Scripty
	currExecPolicy = "";     // Current ExecutionPolicy

	loglevel = LogLevel::NOTIFICATIONS;  // Default logging only logs notifications

	if (loglevel >= LogLevel::NOTIFICATIONS) { clog << "Initializing Scripty...\n\n"; }

	if (fnLen > 10) { FILE_NAME_LEN = fnLen; }

	if (!seeded) { SeedGenerator(seed); }  // Seed random number generator

	// Generate file name and check execution policy
	string resFN = GenerateUniqueNameInDir();
	system(string("powershell Get-ExecutionPolicy -Scope CurrentUser > " + resFN).c_str());

	// Read execution policy
	ifstream resFile(resFN);
	string result;
	getline(resFile, result);

	// Cleanup files
	resFile.close();
	remove(resFN);

	// Set original execution policy
	if (origExecPolicy.empty()) { origExecPolicy = result; }
	currExecPolicy = origExecPolicy;

	// Determine if scripts are runnable based on execution policy
	if (result != "Unrestricted" && !allowExecPolicyChange) 
	{
		if (loglevel > LogLevel::NONE)
		{
			clog << "Scripty initialization failed. The current user's execution policy ";
			clog << "does not allow for scripts to run.\nTo allow Scripty to modify the execution policy, ";
			clog << "rerun ScriptyInit with parameter 'true'\n\n";
		}
		return false; 
	}
	return true;

}

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   Scripty::ChangeLogLevel

	  Summary:  Changes the amount of logging Scripty does

	  Args:     LogLevel loglvl
				  enum representing the amount of logging to do

	  Modifies: [ logLvl ].

	  Returns:  void
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
void Scripty::ChangeLogLevel(LogLevel logLvl)
{
	loglevel = logLvl;
}

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   Scripty::SeedGenerator

  Summary:  Seeds the random number generator

  Args:     unsigned int seed
			  Seed for random number generator

  Modifies: [ seeded ].

  Returns:  void
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
void Scripty::SeedGenerator(unsigned int seed)
{
	if (loglevel >= LogLevel::DEBUG) { clog << "Seeding Scripty with seed: " << seed << "\n\n"; }
	srand(seed);
	seeded = true;
}

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   Scripty::RunScript

	  Summary:  Runs powershell script

	  Args:     std::istream& script
				  Generic input stream of script to run

	  Modifies: [ FILE_NAME_LEN, seeded, allowExecPolicyChange, origExecPolicy, currExecPolicy, loglevel ].

	  Returns:  std::stringstream
				  Output of running powershell script
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
std::stringstream Scripty::RunScript(std::istream& script)
{
	using namespace std;
	using namespace std::filesystem;

	stringstream resStream("");  // Result stream

	if (!seeded) { Init(); }  // Seed random number generator

	// Check if scripts can run
	if (currExecPolicy != "Unrestricted")
	{
		// If not, see if the execution policy can be changed
		if (allowExecPolicyChange) { ChangeExecutionPolicy(); }
		else
		{
			resStream << "Execution policy prevents scripts from running on this system.\nCall ScriptyInit(true) to rectify.";
			return resStream;
		}
	}

	// Generate unique temp file
	string scriptFN = GenerateUniqueNameInDir(".ps1");
	ofstream scriptFile(scriptFN);
	if (loglevel >= LogLevel::DEBUG) { clog << "Creating script file: " << scriptFN << "\n\n"; }
	if (loglevel >= LogLevel::NOTIFICATIONS) { clog << "Running script...\n"; }

	// Write script to file
	string line;
	while (getline(script, line))
	{
		if (loglevel >= LogLevel::NOTIFICATIONS) { clog << line << "\n"; }
		scriptFile << line << "\n";
	}

	scriptFile.close();

	// Run script
	string resFN = GenerateUniqueNameInDir();
	system(string("powershell ./" + scriptFN + " > " + resFN).c_str());

	remove(scriptFN);

	if (loglevel >= LogLevel::DEBUG)
	{
		clog << "Creating temporary results file: " << resFN << "\n";
		clog << "Deleting temporary script file: " << scriptFN << "\n\n";
	}
	if (loglevel == LogLevel::NOTIFICATIONS) { clog << "\n"; }

	// Switch execution policy back to starting value
	if (currExecPolicy != origExecPolicy) { ChangeExecutionPolicy(); }

	if (loglevel >= LogLevel::NOTIFICATIONS) { clog << "Processing results...\n"; }

	// Get results
	ifstream resFile(resFN);
	while (getline(resFile, line))
	{
		resStream << line << "\n";
	}

	resFile.close();

	if (loglevel >= LogLevel::DEBUG) { clog << "Deleting temporary results file: " << resFN << "\n\n"; }

	remove(resFN);

	return resStream;
}

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   Scripty::RunScript

	  Summary:  Runs powershell script and checks 
				if a success condition is met

	  Args:     std::istream& script
				  Generic input stream of script to run
				bool(*SuccessCondition(const std::string&&))
				  Function to check if script has succeeded

	  Modifies: [ FILE_NAME_LEN, seeded, allowExecPolicyChange, origExecPolicy, currExecPolicy, loglevel ].

	  Returns:  std::stringstream
				  Output of running powershell script
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
std::stringstream Scripty::RunScript(std::istream& script, bool(*SuccessCondition)(const std::string&&))
{
	stringstream resStream = RunScript(script);
	if (!SuccessCondition(resStream.str())) { resStream.setstate(std::ios_base::failbit); }
	return resStream;
}

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   Scripty::RunScript

	  Summary:  Runs powershell script

	  Args:     const std::string& script
				  script to run

	  Modifies: [ FILE_NAME_LEN, seeded, allowExecPolicyChange, origExecPolicy, currExecPolicy, loglevel ].

	  Returns:  std::stringstream
				  Output of running powershell script
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
std::stringstream Scripty::RunScript(const std::string& script)
{
	std::istringstream ss(script);
	return RunScript(ss);
}

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   Scripty::RandChar

  Summary:  Generates a random character between
			'a'-'z' and 'A'-'Z'

  Modifies: [ seeded ].

  Returns:  char
			  Random alphabetical character
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
char Scripty::RandChar()
{
	if (!seeded) { SeedGenerator((unsigned int)time(0)); }  // Seed random number generator

	// Generate random alphabetical character
	char letter = (char)((rand() % (LETTER_MAX - LETTER_MIN)) + LETTER_MIN + 1);

	// 50/50 chance to capitalize letter
	if (rand() % 2) { letter += CAPS_DISTANCE; }

	if (loglevel >= LogLevel::EVERYTHING) { clog << "Generated random character " << letter << "\n\n"; }

	return letter;
}

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   Scripty::GenerateUniqueNameInDir

  Summary:  Generates a unique file name
			in the current directory

  Args:     const std::string& fileType
			  Extension of file to generate

  Modifies: [ seeded ].

  Returns:  std::string
			  Randomly generated, unique file name
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
string Scripty::GenerateUniqueNameInDir(const string& fileType)
{
	using namespace std::filesystem;

	if (loglevel >= LogLevel::DEBUG) { clog << "Generating unique file name...\n"; }

	string fileName;
	bool uniqueName = false;
	do
	{
		uniqueName = true;  // Assume fileName is unqiue

		// Generate fileName
		for (size_t index = 0; index < FILE_NAME_LEN; index++) { fileName += RandChar(); }
		fileName += fileType;

		// Check if fileName is unique
		if (exists(current_path().append(fileName))) { break; }

	} while (!uniqueName);

	if (loglevel >= LogLevel::DEBUG) { clog << "Unique file name: " << fileName << "\n\n"; }

	return fileName;
}

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   Scripty::ChangeExecutionPolicy

  Summary:  Runs powershell script

  Args:     std::istream& script
			  Generic input stream of script to run

  Modifies: [ currExecPolicy ].

  Returns:  void
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
void Scripty::ChangeExecutionPolicy()
{
	if (currExecPolicy == "Unrestricted")
	{
		system(string("powershell Set-ExecutionPolicy " + origExecPolicy + " -Scope CurrentUser").c_str());
		currExecPolicy = origExecPolicy;
	}
	else
	{
		system("powershell Set-ExecutionPolicy Unrestricted -Scope CurrentUser");
		currExecPolicy = "Unrestricted";
	}
	if (loglevel >= LogLevel::DEBUG) { clog << "Changing ExecutionPolicy to " << currExecPolicy << "\n\n"; }
}

#endif 

// C-snterface

/*F+F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F
  Function: SeedGenerator

  Summary:  Initializes Scripty

  Args:     bool changeExecutionPolicyAllowed
			  Whether Scripty is allowed to change the CurrentUser's execution policy
			unsigned int seed
			  Seed for random number generator
			unsigned int fnLen
			  Length for temporary file names

  Modifies: [ FILE_NAME_LEN, seeded, allowExecPolicyChange, origExecPolicy, currExecPolicy, loglevel ].

  Returns:  bool
			  true... lol

F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F-F*/
int Init(int changeExecutionPolicyAllowed, unsigned int seed, unsigned int fnLen) {
	Scripty::Init(changeExecutionPolicyAllowed, seed, fnLen);
	return true;
}

/*F+F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F
  Function: SeedGenerator

  Summary:  Seeds the random number generator

  Args:     unsigned int seed
			  Seed for random number generator

  Modifies: [ seeded ].

  Returns:  void
F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F-F*/
void SeedGenerator(unsigned int seed)
{
	Scripty::SeedGenerator(seed);
}

/*F+F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F
  Function: ChangeLogLevel

  Summary:  Changes the amount of logging Scripty does

  Args:     LogLevel loglvl
			  enum representing the amount of logging to do

  Modifies: [ logLvl ].

  Returns:  void

F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F-F*/
void ChangeLogLevel(unsigned int loglvl) {
	// Check if loglvl is valid
	if (loglvl > LOG_EVERYTHING) {
		cerr << "Invalid Log Level, value must range between " << LOG_NONE <<
			"and " << LOG_EVERYTHING << "\n";
		return;
	}
	Scripty::ChangeLogLevel(Scripty::LogLevel(loglvl));
}

/*F+F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F
  Function: RunScript

  Summary:  Runs powershell script

  Args:     char* script
			  character pointer to script
			char* res
			  character array to store results

  Modifies: [ FILE_NAME_LEN, seeded, allowExecPolicyChange, origExecPolicy, currExecPolicy, loglevel ].

  Returns:  long
			  Length of script output
F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F-F*/
long RunScript(char* script, char* res)
{
	// Run script and store results
	string resStr = Scripty::RunScript(string(script)).str();

	// Copy c_str
	rsize_t size = sizeof(char) * resStr.size();
	strcpy_s(res, size, resStr.c_str());
	return (int)resStr.size();
}

/*F+F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F
  Function: RunScript

  Summary:  Runs powershell script

  Args:     char* script
			  character pointer to script
			char* res
			  character pointer to store results

  Modifies: [ FILE_NAME_LEN, seeded, allowExecPolicyChange, origExecPolicy, currExecPolicy, loglevel ].

  Returns:  long
			  Length of script output
F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F-F*/
long RunScript_p(char* script, char* res)
{
	// Run script and store results
	string resStr = Scripty::RunScript(string(script)).str();

	// Copy result as pointer
	rsize_t size = sizeof(char) * resStr.size();
	memcpy(res, &resStr, size);
	return (int)resStr.size();
}

/*F+F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F
  Function: RunScript

  Summary:  Runs powershell script

  Args:     char* script
			  character pointer to script
			char* res
			  character array to store results
			int (*SuccessCondition(char* str, const int len))
			  Function to check if script ran successfully

  Modifies: [ FILE_NAME_LEN, seeded, allowExecPolicyChange, origExecPolicy, currExecPolicy, loglevel ].

  Returns:  long
			  Length of script output
F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F-F*/
long RunScriptCond(char* script, char* res, int (*SuccessCondition)(char* str, const int len)) {
	// Run script and obtain results
	string resStr = Scripty::RunScript(string(script)).str();

	// Copy results to c_str
	rsize_t size = sizeof(char) * resStr.size();
	strcpy_s(res, size, resStr.c_str());

	// Check if successful
	if (!SuccessCondition(res, (const int)resStr.size())) { return -1; }
	return (long)resStr.size();
}