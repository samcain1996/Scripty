#include "pch.h"
#include "Scripty.h"

using std::string;
using std::istream;
using std::stringstream;

static const int LETTER_MIN		= 65;  // Decimal representation of 'a'
static const int LETTER_MAX		= 90;  // Deciaml representation of 'z'
static const int CAPS_DISTANCE	= 32;  // Distance between lower case and upper case counterpart

static int FILE_NAME_LEN = 20;  // File name length

static bool seeded = false;     // Flag to check whether generator has been seeded

static bool allowExecPolicyChange = false;  // Flag to check whether the ExecutionPolicy is allowed to be changed

static string origExecPolicy = "";     // ExecutionPolicy prior to running Scripty
static string currExecPolicy = "";     // Current ExecutionPolicy

/**
 * @brief  Return a random alphanumeric character
 *
 * @return char alphanumeric character
 */
char RandChar()
{
	if (!seeded) { SeedGenerator((unsigned int)time(0)); }  // Seed random number generator

	char letter = (char)((rand() % (LETTER_MAX - LETTER_MIN)) + LETTER_MIN + 1);

	// 50/50 chance to capitalize letter
	if (rand() % 2) { letter += CAPS_DISTANCE; }

	return letter;
}

/**
 * @brief Change execution policy between "Restricted" and "Unrestricted",
 *		  if allowed by allowExecPolicyChange
 */
void ChangeExecutionPolicy()
{
	if (currExecPolicy == "Unrestricted")
	{
		system(string("powershell Set-ExecutionPolicy " + origExecPolicy + " - Scope CurrentUser").c_str());
		currExecPolicy = origExecPolicy;
	}
	else
	{
		system("powershell Set-ExecutionPolicy Unrestricted -Scope CurrentUser");
		currExecPolicy = "Unrestricted";
	}
}

/**
 * @brief Generate unique file name in current directory
 *
 * @param fileType 		extension of file name (.txt, .csv, etc...)
 * @return std::string  file name
 */
string GenerateUniqueNameInDir(const string& fileType = ".txt")
{
	using namespace std::filesystem;

	string fileName;
	bool uniqueName = false;
	do
	{
		uniqueName = true;  // Assume fileName is unqiue

		// Generate fileName
		for (size_t index = 0; index < FILE_NAME_LEN; index++) { fileName += RandChar(); }
		fileName += fileType;

		// Check if fileName is unique
		if (exists(current_path().append(fileName)))
		{
			uniqueName = false;
			break;
		}

	} while (!uniqueName);

	return fileName;
}

/**
 * @brief Seeds random number generator
 * 
 * @param seed 
 */
void SeedGenerator(unsigned int seed)
{
	srand(seed);
	seeded = true;
}

bool ScriptyInit(bool changeExecutionPolicyAllowed, unsigned int seed, unsigned int FN_LEN)
{
	using namespace std;
	using namespace std::filesystem;

	if (FN_LEN >= 10) { FILE_NAME_LEN = FN_LEN; }
	allowExecPolicyChange = changeExecutionPolicyAllowed;

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
	if (result != "Unrestricted" && !allowExecPolicyChange) { return false; }
	return true;
}

/**
 * @brief Runs script and returns results as stringstream
 * 
 * @param script    script to run as istream
 * @return          results of script as stringstream
 */
stringstream runScript(istream& script)
{
	using namespace std;
	using namespace std::filesystem;

	stringstream resStream;  // Result stream

	if (!seeded) { ScriptyInit(); }  // Seed random number generator

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

	// Write script to file
	string line;
	while (getline(script, line))
	{
		scriptFile << line << "\n";
	}

	scriptFile.close();

	// Run script
	string resFN = GenerateUniqueNameInDir();
	system(string("powershell ./" + scriptFN + " > " + resFN).c_str());

	remove(scriptFN);

	// Switch execution policy back to starting value
	if (currExecPolicy != origExecPolicy) { ChangeExecutionPolicy(); }

	// Get results
	ifstream resFile(resFN);
	while (getline(resFile, line))
	{
		resStream << line << "\n";
	}

	resFile.close();
	remove(resFN);

	return resStream;
}