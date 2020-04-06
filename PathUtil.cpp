#include "PathUtil.h"

string getPathFolder(string fileName) {
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
string getAppFolder() { return getPathFolder(getAppPath()); }
string getfileName(string target) {
	if (target.rfind("\\") > 0)
		return target.substr(target.find_last_of("\\") + 1, target.size());
	return target;
}
string GetFileNameWithoutExtension(string fullname) {
	string fName = getfileName(fullname);
	auto lastDotPos = fName.find_last_of(".");
	if (lastDotPos == string::npos)  return fName;
	return fName.substr(0, lastDotPos);
}
string GetFileExtension(string fullname) {
	auto lastDotPos = fullname.find_last_of(".");
	if (lastDotPos == string::npos)  return "";
	return fullname.substr(lastDotPos, fullname.size());;
}


bool isFileValid(const string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}