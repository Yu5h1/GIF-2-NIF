#include "StringUtil.h"

using namespace std;

string StringTolower(string txt) {
	for (int i = 0; i < txt.size(); i++) txt[i] = tolower(txt[i]);
	return txt;
}
bool StringContain(string txt, string arg) {
	if (arg == "") return true;
	txt = StringTolower(txt);
	arg = StringTolower(arg);
	return	txt.find(arg) != string::npos;
}
bool StringContainAll(string txt, vector<string> args) {
	if (args.size() == 0) return true;
	for (auto arg : args)
		if (!StringContain(txt, arg)) return false;
	return true;
}
bool StringContainAny(string txt, vector<string> args) {
	if (args.size() == 0) return true;
	for (auto arg : args)
		if (StringContain(txt, arg)) return true;
	return false;
}
string StringReplace(string txt, const string from, const string to) {
	size_t start_pos = txt.find(from);
	// Null pos
	if (start_pos != string::npos) {
		txt = txt.replace(start_pos, from.length(), to);
	}
	string str;
	return txt;
}

vector<string> StringSplit(string txt, const string separator)
{
	vector<string> results;
	int next = txt.find(separator);
	while (next >= 0)
	{
		results.push_back(txt.substr(0, next));
		txt = txt.replace(0, next + 1, "");
		next = txt.find(separator);
	}
	results.push_back(txt);
	return results;
}

string TrimLeft(string txt,string arg, int offset)
{
	int index = txt.find(arg);
	if (index > -1 && index < txt.size()) {
		index += offset;
		if (index > -1 && index < txt.size()) txt = txt.erase(0, index);
	} 
	return txt;
}

string TrimRight(string txt, string arg, int offset)
{
	int index = txt.find(arg);
	if (index > -1 && index < txt.size()) {
		index += offset+1;
		if (index > -1 && index < txt.size()) txt = txt.erase(index, txt.size());
	}
	return txt;
}