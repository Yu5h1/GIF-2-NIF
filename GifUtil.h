#pragma once
#using <System.dll>
#using <system.drawing.dll>
#include <DebugUtil.h>

#include <vector>

//using namespace std;

using namespace System::Drawing;
using namespace System::Drawing::Imaging;
using namespace System::Diagnostics;


namespace GifUtil
{
	class GifConvertInfo {
	public:
		static std::string filterColor;
		static int OutputTextureSize;
	private:
	};

	std::vector<float> ConvertToSpriteSheets(System::String ^ sourcePath,
		int splitValue, int & width, int & height, float & timeLength,
		System::String^& outputPath, int& spriteDimension);
}
