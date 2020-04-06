#pragma once
#include <iostream>
#include <Windows.h>
#include <string>

using namespace std;

string getPathFolder(string fileName);
string getAppPath();
string getAppFolder();
string getfileName(string target);
string GetFileNameWithoutExtension(string fullname);

string GetFileExtension(string fullname);

bool isFileValid(const string& name);