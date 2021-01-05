#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;


string StringTolower(string txt);
bool StringContain(string txt, string arg);
bool StringContainAll(string txt, vector<string> args);
bool StringContainAny(string txt, vector<string> args);
string StringReplace(string txt, const string from, const string to);
vector<string> StringSplit(string txt, const string separator);

//template<typename T>
//string Join(vector<T> objs, string separator = ",");
template<typename T>
string Join(vector<T> objs, string separator = ",")
{
	ostringstream stream;
	int size = objs.size();
	if (size > 0) {
		stream << objs[0];
		if (size > 1) {
			for (int i = 1; i < size; i++) {
				stream << separator;
				stream << objs[i];
			}
		}
	}
	return stream.str();
}

template<typename T>
string JoinBy(string GetString(T obj),vector<T> list, string separator) {
	vector<string> results;
	for (auto item : list) results.push_back(GetString(item));
	return Join(results, separator);
}
template <typename T>
string Join(string separator, T obj) {
	ostringstream stream;
	stream << obj;
	return stream.str();
}
template<typename T, typename... Args>
string Join(string separator, T obj, Args... args) // recursive variadic function
{
	ostringstream stream;
	stream << obj;
	string result = stream.str();

	result += separator + Join(separator,args...) ;
	return result;
}

//template <typename T>
//vector<string> collect(T obj) {
//	ostringstream stream;
//	stream << obj;
//	return vector<string>(stream.str());
//}
//template<typename T, typename... Args>
//vector<string> collect(T obj, Args... args) // recursive variadic function
//{
//	ostringstream stream;
//	stream << obj;
//	vector<string> results = vector<string>(stream.str());
//
//	collect(args...);
//
//	return results;
//}