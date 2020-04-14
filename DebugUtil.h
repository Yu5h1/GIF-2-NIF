#pragma once
#include <iostream>
#include <Windows.h>
#include <string>

using namespace std;

string MarshalString(System::String^ s);
void MsgBox(const char* info);
void MsgBox(string info);
void MsgBox(float info);
void MsgBox(System::String^ info);

void print(string content);
void print(float val);
void print(System::String^ info);

//string print(const char * sOne, ...);
//void MsgBox(int info);

