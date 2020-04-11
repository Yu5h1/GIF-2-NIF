#include "GifUtil.h"


namespace GifUtil
{
	void UseBlackAsAlpha(Bitmap^ map)
	{
		for (int x = 0; x < map->Width; x++)
		{
			for (int y = 0; y < map->Height; y++)
			{
				Color pixel = map->GetPixel(x, y);
				pixel = Color::FromArgb(((pixel.R + pixel.G + pixel.B) / 3), pixel.R, pixel.G, pixel.B);
				map->SetPixel(x, y, pixel);
			}
		}
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

			int frameWidth = gifImg->Width;
			int frameHeight = gifImg->Height;

			int finalWidth = spriteDimension * (frameWidth + 2);
			int finalHeight = spriteDimension * (frameHeight + 2);

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
							canvas->DrawImage(frame, col * (frameWidth + 2) + 1, row * (frameHeight + 2) + 1);
						}
						finally{ delete frame; }
						System::Console::Write("\r" + targetName + "...{0}%   ", (i / (float)frameCountPerSplit) * 100);
					}
					canvas->Save();
					if (outputPath == System::String::Empty)
						outputPath = sourcePath->Substring(0, sourcePath->ToCharArray()->Length - 4) + ".png";
					
					if (GifUtil::GifConvertInfo::filterColor == "black") {
						UseBlackAsAlpha(finalSprite);
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
