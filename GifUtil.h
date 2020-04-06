#pragma once
#using <System.dll>
#using <system.drawing.dll>

#include <vector>

using namespace std;

using namespace System::Drawing;
using namespace System::Drawing::Imaging;
using namespace System::Diagnostics;


namespace GifUtil
{
	std::vector<float> ConvertToSpriteSheets(System::String ^ sourcePath,
		int splitValue, int & width, int & height, float & timeLength,
		System::String^& outputPath, int& spriteDimension);

	//void GenerateGifAssets(std::string gifImagePath, bool IsSpecialEdition,
	//	string GameDataFolderPath, float averageFrameRate, int textureSize);
}