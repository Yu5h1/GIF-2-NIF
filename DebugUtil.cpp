#include "DebugUtil.h"

string MarshalString(System::String^ s) {
	using namespace System::Runtime::InteropServices;
	const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();	
	string result = string(chars);
	Marshal::FreeHGlobal(System::IntPtr((void*)chars));
	return result;
}

void MsgBox(const char* info) { MessageBox(NULL, info, TEXT("WARNING"), MB_ICONWARNING); }
void MsgBox(string info) { MsgBox(info.c_str()); }
void MsgBox(float info) { MsgBox(std::to_string(info)); }
void MsgBox(System::String^ info) { MsgBox(MarshalString(info)); }
void print(string content) { cout << content << endl; };
void print(float val) { print(to_string(val)); };

//void MsgBox(int number) { print(to_string(number)); }

//string print(const char *sOne, ...)
//{
//	va_list arguments;
//	va_start(arguments, sOne);
//	string result = string(sOne);
//	while (sOne)
//	{
//		cout << sOne;
//		sOne = va_arg(arguments, const char *);
//		//result.append(sOne);		
//	}
//	va_end(arguments);
//	//MessageBox(NULL, result.c_str(), TEXT("WARNING"), MB_ICONWARNING);
//	return result;
//}


