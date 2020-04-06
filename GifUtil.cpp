#include "GifUtil.h"


namespace GifUtil
{


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

			int finalWidth = spriteDimension * (gifImg->Width + 2);
			int finalHeight = spriteDimension * (gifImg->Height + 2);


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

						Bitmap^ frame = gcnew Bitmap(gifImg);
						try
						{
							canvas->DrawImage(frame, col * (gifImg->Width + 2) + 1, row * (gifImg->Height + 2) + 1);
						}
						finally{ delete frame; }
						System::Console::Write("\r" + targetName + "...{0}%   ", (i / (float)frameCountPerSplit) * 100);
					}
					canvas->Save();
					if (outputPath == System::String::Empty)
						outputPath = sourcePath->Substring(0, sourcePath->ToCharArray()->Length - 4) + ".png";
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
