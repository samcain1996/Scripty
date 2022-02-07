#include "pch.h"
#include "Scripty.hpp"

using std::clog;
using std::pair;
using std::string;
using std::istream;
using std::ostream;
using std::stringstream;

//static char RandChar()
//{
//	if (!seeded) { SeedGenerator((unsigned int)time(0)); }  // Seed random number generator
//
//	char letter = (char)((rand() % (LETTER_MAX - LETTER_MIN)) + LETTER_MIN + 1);
//
//	// 50/50 chance to capitalize letter
//	if (rand() % 2) { letter += CAPS_DISTANCE; }
//
//	if (logLevel >= ScriptyLogLevel::EVERYTHING) { clog << "Generated random character " << letter << "\n\n"; }
//
//	return letter;
//}

//static void ChangeExecutionPolicy()
//{
//	if (currExecPolicy == "Unrestricted")
//	{
//		system(string("powershell Set-ExecutionPolicy " + origExecPolicy + " - Scope CurrentUser").c_str());
//		currExecPolicy = origExecPolicy;
//	}
//	else
//	{
//		system("powershell Set-ExecutionPolicy Unrestricted -Scope CurrentUser");
//		currExecPolicy = "Unrestricted";
//	}
//	if (logLevel >= ScriptyLogLevel::DEBUG) { clog << "Changing ExecutionPolicy to " << currExecPolicy << "\n\n"; }
//}

//static string GenerateUniqueNameInDir(const string& fileType = ".txt")
//{
//	using namespace std::filesystem;
//
//	if (logLevel >= LogLevel::DEBUG) { clog << "Generating unique file name...\n"; }
//
//	string fileName;
//	bool uniqueName = false;
//	do
//	{
//		uniqueName = true;  // Assume fileName is unqiue
//
//		// Generate fileName
//		for (size_t index = 0; index < FILE_NAME_LEN; index++) { fileName += RandChar(); }
//		fileName += fileType;
//
//		// Check if fileName is unique
//		if (exists(current_path().append(fileName))) { break; }
//
//	} while (!uniqueName);
//
//	if (logLevel >= LogLevel::DEBUG) { clog << "Unique file name: " << fileName << "\n\n"; }
//
//	return fileName;
//}


//void ChangeLogLevel(LogLevel ll) { logLevel = ll; }

///**
// * @brief Seeds random number generator
// *
// * @param seed
// */
//void SeedGenerator(unsigned int seed)
//{
//	if (logLevel >= ScriptyLogLevel::DEBUG) { clog << "Seeding Scripty with seed: " << seed << "\n\n"; }
//	srand(seed);
//	seeded = true;
//}

/**
 * @brief Initializes Scripty
 *
 * @param changeExecutionPolicyAllowed		Defines if Scripty is allowed to modify
 *											the current user's ExecutionPolicy
 * @param seed								Seed for random number generator
 * @param FN_LEN							Length of temporary file names
 * @return									Whether Scripty successfully initialized
 */
//bool ScriptyInit(bool changeExecutionPolicyAllowed, unsigned int seed, unsigned int fnLen)
//{
//	using namespace std;
//	using namespace std::filesystem;
//
//	if (logLevel >= ScriptyLogLevel::NOTIFICATIONS) { clog << "Initializing Scripty...\n\n"; }
//
//	if (fnLen > 10) { FILE_NAME_LEN = fnLen; }
//	allowExecPolicyChange = changeExecutionPolicyAllowed;
//
//	if (!seeded) { SeedGenerator(seed); }  // Seed random number generator
//
//	// Generate file name and check execution policy
//	string resFN = GenerateUniqueNameInDir();
//	system(string("powershell Get-ExecutionPolicy -Scope CurrentUser > " + resFN).c_str());
//
//	// Read execution policy
//	ifstream resFile(resFN);
//	string result;
//	getline(resFile, result);
//
//	// Cleanup files
//	resFile.close();
//	remove(resFN);
//
//	// Set original execution policy
//	if (origExecPolicy.empty()) { origExecPolicy = result; }
//	currExecPolicy = origExecPolicy;
//
//	// Determine if scripts are runnable based on execution policy
//	if (result != "Unrestricted" && !allowExecPolicyChange) 
//	{
//		if (logLevel > ScriptyLogLevel::NONE)
//		{
//			clog << "Scripty initialization failed. The current user's execution policy ";
//			clog << "does not allow for scripts to run.\nTo allow Scripty to modify the execution policy, ";
//			clog << "rerun ScriptyInit with parameter 'true'\n\n";
//		}
//		return false; 
//	}
//	return true;
//}

/**
 * @brief Runs script and returns results as stringstream
 *
 * @param script    script to run as istream
 * @return          results of script as stringstream
 */
//stringstream runScript(istream& script)
//{
//	using namespace std;
//	using namespace std::filesystem;
//
//	stringstream resStream("");  // Result stream
//
//	if (!seeded) { ScriptyInit(); }  // Seed random number generator
//
//	// Check if scripts can run
//	if (currExecPolicy != "Unrestricted")
//	{
//		// If not, see if the execution policy can be changed
//		if (allowExecPolicyChange) { ChangeExecutionPolicy(); }
//		else
//		{
//			resStream << "Execution policy prevents scripts from running on this system.\nCall ScriptyInit(true) to rectify.";
//			return resStream;
//		}
//	}
//
//	// Generate unique temp file
//	string scriptFN = GenerateUniqueNameInDir(".ps1");
//	ofstream scriptFile(scriptFN);
//	if (logLevel >= ScriptyLogLevel::DEBUG) { clog << "Creating script file: " << scriptFN << "\n\n"; }
//	if (logLevel >= ScriptyLogLevel::NOTIFICATIONS) { clog << "Running script...\n"; }
//
//	// Write script to file
//	string line;
//	while (getline(script, line))
//	{
//		if (logLevel >= ScriptyLogLevel::NOTIFICATIONS) { clog << line << "\n"; }
//		scriptFile << line << "\n";
//	}
//
//	scriptFile.close();
//
//	// Run script
//	string resFN = GenerateUniqueNameInDir();
//	system(string("powershell ./" + scriptFN + " > " + resFN).c_str());
//
//	remove(scriptFN);
//
//	if (logLevel >= ScriptyLogLevel::DEBUG) 
//	{ 
//		clog << "Creating temporary results file: " << resFN << "\n";
//		clog << "Deleting temporary script file: " << scriptFN << "\n\n"; 
//	}
//	if (logLevel == ScriptyLogLevel::NOTIFICATIONS) { clog << "\n"; }
//
//	// Switch execution policy back to starting value
//	if (currExecPolicy != origExecPolicy) { ChangeExecutionPolicy(); }
//
//	if (logLevel >= ScriptyLogLevel::NOTIFICATIONS) { clog << "Processing results...\n"; }
//	
//	// Get results
//	ifstream resFile(resFN);
//	while (getline(resFile, line))
//	{
//		resStream << line << "\n";
//	}
//
//	resFile.close();
//
//	if (logLevel >= ScriptyLogLevel::DEBUG) { clog << "Deleting temporary results file: " << resFN << "\n\n"; }
//
//	remove(resFN);
//
//	return resStream;
//}

//stringstream runScript(istream& script, bool (*SuccessCondition)(string&&))
//{
//	stringstream resStream = runScript(script);
//	if (!SuccessCondition(resStream.str())) { resStream.setstate(std::ios_base::failbit); }
//	return resStream;
//}

//stringstream runScript(const string& script)
//{
//	std::istringstream ss(script);
//	return runScript(ss);
//}


// class

int Scripty::FILE_NAME_LEN;
bool Scripty::seeded;
bool Scripty::allowExecPolicyChange;
string Scripty::origExecPolicy;
string Scripty::currExecPolicy;
Scripty::LogLevel Scripty::logLevel;

bool Scripty::Init(bool changeExecutionPolicyAllowed, unsigned int seed, unsigned int fnLen)
{
	using namespace std;
	using namespace std::filesystem;
	FILE_NAME_LEN = 10;  // File name length

	seeded = false;     // Flag to check whether generator has been seeded

	allowExecPolicyChange = changeExecutionPolicyAllowed;  // Flag to check whether the ExecutionPolicy is allowed to be changed

	origExecPolicy = "";     // ExecutionPolicy prior to running Scripty
	currExecPolicy = "";     // Current ExecutionPolicy

	logLevel = LogLevel::NOTIFICATIONS;  // Default logging only logs notifications

	if (logLevel >= LogLevel::NOTIFICATIONS) { clog << "Initializing Scripty...\n\n"; }

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
		if (logLevel > LogLevel::NONE)
		{
			clog << "Scripty initialization failed. The current user's execution policy ";
			clog << "does not allow for scripts to run.\nTo allow Scripty to modify the execution policy, ";
			clog << "rerun ScriptyInit with parameter 'true'\n\n";
		}
		return false; 
	}
	return true;

}

void Scripty::ChangeLogLevel(LogLevel logLvl)
{
	logLevel = logLvl;
}

/**
 * @brief  Return a random alphanumeric character
 *
 * @return char alphanumeric character
 */
char Scripty::RandChar()
{
	if (!seeded) { SeedGenerator((unsigned int)time(0)); }  // Seed random number generator

	char letter = (char)((rand() % (LETTER_MAX - LETTER_MIN)) + LETTER_MIN + 1);

	// 50/50 chance to capitalize letter
	if (rand() % 2) { letter += CAPS_DISTANCE; }

	if (logLevel >= LogLevel::EVERYTHING) { clog << "Generated random character " << letter << "\n\n"; }

	return letter;
}
/**
 * @brief			Generate unique file name in current directory
 *
 * @param fileType	Extension of file name (.txt, .csv, etc...)
 * @return string	File name
 */
string Scripty::GenerateUniqueNameInDir(const string& fileType)
{
	using namespace std::filesystem;

	if (logLevel >= LogLevel::DEBUG) { clog << "Generating unique file name...\n"; }

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

	if (logLevel >= LogLevel::DEBUG) { clog << "Unique file name: " << fileName << "\n\n"; }

	return fileName;
}

void Scripty::SeedGenerator(unsigned int seed)
{
	if (logLevel >= LogLevel::DEBUG) { clog << "Seeding Scripty with seed: " << seed << "\n\n"; }
	srand(seed);
	seeded = true;
}

/**
 * @brief Change execution policy between "Restricted" and "Unrestricted",
 *		  if allowed by allowExecPolicyChange
 */
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
	if (logLevel >= LogLevel::DEBUG) { clog << "Changing ExecutionPolicy to " << currExecPolicy << "\n\n"; }
}

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
	if (logLevel >= LogLevel::DEBUG) { clog << "Creating script file: " << scriptFN << "\n\n"; }
	if (logLevel >= LogLevel::NOTIFICATIONS) { clog << "Running script...\n"; }

	// Write script to file
	string line;
	while (getline(script, line))
	{
		if (logLevel >= LogLevel::NOTIFICATIONS) { clog << line << "\n"; }
		scriptFile << line << "\n";
	}

	scriptFile.close();

	// Run script
	string resFN = GenerateUniqueNameInDir();
	system(string("powershell ./" + scriptFN + " > " + resFN).c_str());

	remove(scriptFN);

	if (logLevel >= LogLevel::DEBUG)
	{
		clog << "Creating temporary results file: " << resFN << "\n";
		clog << "Deleting temporary script file: " << scriptFN << "\n\n";
	}
	if (logLevel == LogLevel::NOTIFICATIONS) { clog << "\n"; }

	// Switch execution policy back to starting value
	if (currExecPolicy != origExecPolicy) { ChangeExecutionPolicy(); }

	if (logLevel >= LogLevel::NOTIFICATIONS) { clog << "Processing results...\n"; }

	// Get results
	ifstream resFile(resFN);
	while (getline(resFile, line))
	{
		resStream << line << "\n";
	}

	resFile.close();

	if (logLevel >= LogLevel::DEBUG) { clog << "Deleting temporary results file: " << resFN << "\n\n"; }

	remove(resFN);

	return resStream;
}

std::stringstream Scripty::RunScript(std::istream& script, bool(*SuccessCondition)(std::string&&))
{
	stringstream resStream = RunScript(script);
	if (!SuccessCondition(resStream.str())) { resStream.setstate(std::ios_base::failbit); }
	return resStream;
}

std::stringstream Scripty::RunScript(const std::string& script)
{
	std::istringstream ss(script);
	return RunScript(ss);
}
