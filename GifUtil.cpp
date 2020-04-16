#include "GifUtil.h"

//using namespace System::Runtime::InteropServices;
//using namespace System::Drawing;

namespace GifUtil
{
	public class ColorInt {

	public:
		ColorInt(int r, int g, int b, int a)
		{ red = r; green = g; blue = b; alpha = a; }
		int red;
		int green;
		int blue;
		int alpha;
	};
	int AlphaFromRGB(ColorInt* c) { return ((c->red + c->green + c->blue ) / 3); }
	int AlphaFromPureBlack(ColorInt* c) { return (c->red + c->green + c->blue) == 0 ? 0 : 255; }

	int AlphaFromGreen(ColorInt* c, ColorInt* filter) {
		return ((c->red + c->green + c->blue) / 3);
	}

	void SetBlackPixelToAlpha(Bitmap^ map, int func(ColorInt*))
	{		
		System::Drawing::Rectangle rect = System::Drawing::Rectangle(0, 0, map->Width, map->Height);
		auto bitmapData = map->LockBits(rect, ImageLockMode::ReadWrite, map->PixelFormat);
		int BytesPerPixel = Bitmap::GetPixelFormatSize(map->PixelFormat) / 8;
		int ByteCount = bitmapData->Stride * map->Height;
		cli::array<System::Byte>^ Pixels = gcnew cli::array<System::Byte>(ByteCount);
		
		auto PtrFirstPixel = bitmapData->Scan0;
		System::Runtime::InteropServices::Marshal::Copy(PtrFirstPixel, Pixels, 0, Pixels->Length);

		int HeightInPixels = bitmapData->Height;
		int WidthInBytes = bitmapData->Width * BytesPerPixel;
		for (int y = 0; y < HeightInPixels; y++)
		{
			int CurrentLine = y * bitmapData->Stride;
			for (int x = 0; x < WidthInBytes; x = x + BytesPerPixel)
			{
				int b = Pixels[CurrentLine + x ];
				int g = Pixels[CurrentLine + x + 1 ];
				int r = Pixels[CurrentLine + x + 2 ];
				int a = Pixels[CurrentLine + x + 3 ];
				Pixels[CurrentLine + x + 3] = (byte)func(new ColorInt(r,g,b,255));
			}
			System::Console::Write("\r Calculate alpha...{0}%   ", (y / (float)HeightInPixels) * 100);
		}											
		System::Runtime::InteropServices::Marshal::Copy(Pixels, 0, PtrFirstPixel, Pixels->Length);
		map->UnlockBits(bitmapData);
	}
	std::vector<float> ConvertToSpriteSheets(System::String^ sourcePath,
		int splitValue, int& width, int& height, float& timeLength,
		System::String^& outputPath, int& spriteDimension, int& resultTextureSize)
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

			resultTextureSize = GifUtil::GifConvertInfo::OutputTextureSize;
			if (GifUtil::GifConvertInfo::TextureSizeByDimension) {
				int powof2 = System::Math::Log(resultTextureSize, 2)+ spriteDimension;
				powof2 = powof2 <= 13 ? powof2 : 13;
				resultTextureSize = System::Math::Pow(2, powof2);
			}

			int frameSize = resultTextureSize / spriteDimension;
			float diff = gifImg->Width < gifImg->Height ? (float)gifImg->Width / frameSize : (float)gifImg->Height / frameSize;

			int frameWidth = gifImg->Width / diff;
			int frameHeight = gifImg->Height / diff;

			int extend = GifUtil::GifConvertInfo::FrameSizeExtend;
			int halfextend = extend == 0 ? 0 : extend / 2;

			if (frameWidth + extend < 0 || frameHeight + extend < 0){
				extend = 0;
				halfextend = 0;
				print("Extend Size out of range");
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
					
					auto filterColor = GifUtil::GifConvertInfo::filterColor;

					if (filterColor == "black") { SetBlackPixelToAlpha(finalSprite,AlphaFromRGB); }
					if (filterColor == "pureblack") { SetBlackPixelToAlpha(finalSprite, AlphaFromPureBlack); }

					finalSprite->Save(outputPath, ImageFormat::Png);
				}
				finally{ delete canvas; }
			}
			finally{ delete finalSprite; }

		}
		if (timeLength == 0) {
			for (size_t i = 0; i < results.size(); i++)
			{ results[i] = GifUtil::GifConvertInfo::averageFrameRate; }
		}
		System::Console::Write("\r" + targetName + "...{0}%\n", 100);
		return results;
	}
}
