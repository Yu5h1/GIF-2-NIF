#include "PathUtil.h"
#include "DebugUtil.h"
#include <iostream>
#include <vector>

using namespace std;
string getFileFolder(string fileName) {
	const size_t last_slash_idx = fileName.rfind('\\');
	if (std::string::npos != last_slash_idx)
		return fileName.substr(0, last_slash_idx);
	return NULL;
}
string getAppPath()
{
	char result[MAX_PATH];
	return string(result, GetModuleFileName(NULL, result, MAX_PATH));
}
string getAppName() { return GetFileNameWithoutExtension(getAppPath()); }
string getAppFileName() { return getFileName(getAppPath()); }
string getAppFolder() { return getFileFolder(getAppPath()); }
string getFileName(string target) {
	if (target.rfind("\\") > 0)
		return target.substr(target.find_last_of("\\") + 1, target.size());
	return target;
}
string GetFileNameWithoutExtension(string fullname) {
	string fName = getFileName(fullname);
	auto lastDotPos = fName.find_last_of(".");
	if (lastDotPos == string::npos)  return fName;
	return fName.substr(0, lastDotPos);
}
string GetFileExtension(string fullname) {
	auto lastDotPos = fullname.find_last_of(".");
	if (lastDotPos == string::npos)  return "";
	return fullname.substr(lastDotPos, fullname.size());;
}


bool DoesPathOccupied(const string& path,bool prompt) {
	struct stat buffer;
	if (stat(path.c_str(), &buffer) == 0) return true;
	if (prompt) MsgBox(path + "\nInvalid file path.");
	return false;
}

string ReadAllText(string path)
{
	string result;
	if (DoesPathOccupied(path)) {
		ifstream file(path);
		result = string( istreambuf_iterator<char>(file), istreambuf_iterator<char>());
		//while (getline(file, result)) std::cout << result << "\n";
		file.close();
	}
	return result;
}

void WriteAllText(string path,string content)
{
	std::ofstream outfile(path);
	outfile << content << std::endl;
	outfile.close();
}

vector<string> ReadAllLines(string path,bool IgnoreEmpty)
{
	vector<string> results;
	if (DoesPathOccupied(path)) {
		ifstream file(path);
		string line;
		while (getline(file, line)) {
			if (!IgnoreEmpty || line != "")
				results.push_back(line);
		}
		file.close();
	}
	return results;
}

void WriteAllLine(vector<string> lines,string path)
{
	string content;
	for (auto line : lines) content += line + "\n";
	WriteAllText(path, content);
}
