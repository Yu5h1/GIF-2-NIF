#include "GifUtil.h"

//using namespace System::Runtime::InteropServices;

namespace GifUtil
{
	//void SetPixel(cli::array<System::Int32>^ Bits,int Width,int x, int y, Color colour)
	//{
	//	int index = x + (y * Width);
	//	int col = colour.ToArgb();

	//	Bits[index] = col;
	//}

	//Color GetPixel(cli::array<System::Int32>^ Bits, int Width,int x, int y)
	//{
	//	int index = x + (y * Width);
	//	int col = Bits[index];
	//	Color result = Color::FromArgb(col);
	//	return result;
	//}

	//void qq() {
	//	cli::array<System::Int32>^ Bits;
	//	//System::Int32 Bits[];
	//	GCHandle BitsHandle = GCHandle::Alloc(Bits, GCHandleType::Pinned);
	//	//Bitmap = new Bitmap(width, height, width * 4, PixelFormat::Format32bppPArgb, BitsHandle.AddrOfPinnedObject());
	//	BitsHandle.Free();
	//}

	void SetBlackPixelToAlpha(Bitmap^ map)
	{		
		/*System::Drawing::Rectangle rect = System::Drawing::Rectangle(0, 0, map->Width, map->Height);
		auto data = map->LockBits(rect, System::Drawing::Imaging::ImageLockMode::ReadWrite, map->PixelFormat);*/
		for (int x = 0; x < map->Width; x++)
		{
			for (int y = 0; y < map->Height; y++)
			{
				Color pixel = map->GetPixel(x, y);
				pixel = Color::FromArgb(((pixel.R + pixel.G + pixel.B) / 3), pixel.R, pixel.G, pixel.B);
				map->SetPixel(x, y, pixel);
			}
			System::Console::Write("\r ErasPixel...{0}%", (x / (float)map->Width) * 100);
		}
		//map->UnlockBits(data);
	}

	std::vector<float> ConvertToSpriteSheets(System::String^ sourcePath,
		int splitValue, int& width, int& height, float& timeLength,
		System::String^& outputPath, int& spriteDimension)
	{
		System::String^ targetName = System::IO::Path::GetFileName(sourcePath);
		Image^ gifImg = Image::FromFile(sourcePath);
		
		FrameDimension^ dimension = gcnew FrameDimension(gifImg->FrameDimensionsList[0]);
		int frameCount = gifImg->GetFrameCount(dimension);
		cli::array<unsigned char>^ times = gifImg->GetPropertyItem(0x5100)->Value;

		width = gifImg->Width;
		height = gifImg->Height;

		std::vector<float> results;
		timeLength = 0;
		int gifDuration = 0;
		for (int i = 0; i < frameCount; i++)
		{

			int bitTime = System::BitConverter::ToInt32(times, i * 4);
			float curTime = bitTime * 0.01f;
			results.push_back(curTime);
			timeLength += curTime;
			gifDuration += bitTime;
		}
		int frameCountPerSplit = frameCount / splitValue;
		int extraFrame = frameCount % splitValue;

		for (int k = 0; k < splitValue; k++)
		{
			if (k < extraFrame)
			{
				frameCountPerSplit++;
			}
			spriteDimension = (int)(System::Math::Ceiling(System::Math::Sqrt(frameCountPerSplit)));


			int frameSize = GifUtil::GifConvertInfo::OutputTextureSize / spriteDimension;
			float diff = gifImg->Width < gifImg->Height ? (float)gifImg->Width / frameSize : (float)gifImg->Height / frameSize;

			int frameWidth = gifImg->Width / diff;
			int frameHeight = gifImg->Height / diff;

			int extend = 0;
			int halfextend = extend / 2;

			if (extend < 2) {
				extend = 0;
				halfextend = 0; 
			}

			int finalWidth = spriteDimension * (frameWidth + extend);
			int finalHeight = spriteDimension * (frameHeight + extend);

			Bitmap^ finalSprite = gcnew Bitmap(finalWidth, finalHeight);
			
			try
			{
				Graphics^ canvas = Graphics::FromImage(finalSprite);
				try {
					int row = 0;
					int col = 0;
					for (int i = 0; i < frameCountPerSplit; i++)
					{
						row = i / spriteDimension;
						col = i - row * spriteDimension;
						gifImg->SelectActiveFrame(dimension, i + k * frameCountPerSplit + (k >= extraFrame ? extraFrame : 0));

						Bitmap^ frame = gcnew Bitmap(gifImg, frameWidth, frameHeight);
						try
						{
							canvas->DrawImage(frame, col * (frameWidth + extend) + halfextend, row * (frameHeight + extend) + halfextend);
						}
						finally{ delete frame; }
						System::Console::Write("\r" + targetName + "...{0}%   ", (i / (float)frameCountPerSplit) * 100);
					}
					canvas->Save();
					if (outputPath == System::String::Empty)
						outputPath = sourcePath->Substring(0, sourcePath->ToCharArray()->Length - 4) + ".png";
					
					if (GifUtil::GifConvertInfo::filterColor == "black") {
						SetBlackPixelToAlpha(finalSprite);
					}
					finalSprite->Save(outputPath, ImageFormat::Png);
				}
				finally{ delete canvas; }
			}
			finally{ delete finalSprite; }

		}
		
		System::Console::Write("\rGenerating " + targetName + "...{0}%\n", 100);
		return results;
	}
}
