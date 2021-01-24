#pragma once
#include <iostream>
#include <Windows.h>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

string getAppPath();
string getAppName();
string getAppFileName();
string getAppFolder();

string getFileName(string target);
string GetFileNameWithoutExtension(string fullname);
string getFileFolder(string fileName);


string GetFileExtension(string fullname);

bool DoesPathOccupied(const string& name,bool prompt = false);

string ReadAllText(string path);
void WriteAllText(string path,string content);

vector<string> ReadAllLines(string path,bool IgnoreEmpty = false);
void WriteAllLine(vector<string> lines,string path);
