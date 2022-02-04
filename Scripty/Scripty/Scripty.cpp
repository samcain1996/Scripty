#include "pch.h"
#include "Scripty.h"

static const int LETTER_MIN		= 65;
static const int LETTER_MAX		= 90;
static const int CAPS_DISTANCE	= 32;

static int FILE_NAME_LEN = 20;

static bool seeded = false;

char RandChar()
{
	if (!seeded) { SeedGenerator(time(0)); }

	char letter = (char)((rand() % (LETTER_MAX - LETTER_MIN)) + LETTER_MIN + 1);

	if (rand() % 2) { letter += CAPS_DISTANCE; }

	return letter;
}



void SeedGenerator(unsigned int seed)
{
	srand(seed);
	seeded = true;
}

std::string GenerateUniqueNameInDir(const std::string& fileType = ".txt")
{
	using namespace std;
	using namespace std::filesystem;

	string fileName;
	bool uniqueName = false;
	do
	{
		uniqueName = true;  // Assume fileName is unqiue

		// Generate fileName
		for (size_t index = 0; index < FILE_NAME_LEN; index++) { fileName += RandChar(); }

		// Check if fileName is unique
		for (const auto& file : directory_iterator{ current_path() })
		{
			if (!file.is_directory() && file.path().filename() == fileName)
			{
				uniqueName = false;
				break;
			}
		}

	} while (!uniqueName);

	return string(fileName + fileType);
}

std::stringstream runScript(std::istream& script)
{
	using namespace std;
	using namespace std::filesystem;

	if (!seeded) { SeedGenerator(time(0)); }

	// Generate unique temp file name
	string scriptFN = GenerateUniqueNameInDir(".ps1");

	ofstream scriptFile(scriptFN);

	string line;
	while (getline(script, line))
	{
		scriptFile << line;
	}

	scriptFile.close();

	// Run script
	string resFN = GenerateUniqueNameInDir();
	system(string("powershell ./" + scriptFN + " > " + resFN).c_str());

	remove(scriptFN);

	// Get results
	ifstream resFile(resFN);

	stringstream resStream;
	while (getline(resFile, line))
	{
		resStream << line << "\n";
	}

	resFile.close();

	remove(resFN);

	return resStream;
}