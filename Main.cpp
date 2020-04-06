// Gif2Nif.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <sstream>
#include <NifFile.h>
#include <GifUtil.h>
#include <DebugUtil.h>
#include <PathUtil.h>
#include <NifVersionConverter.h>
#include <Resources.h>

using namespace std;

string GifMeshTemplate() { return (getAppFolder() + "\\GifMeshTemplate.nif"); }
bool DoesMeshTemplateExists() {
	return System::IO::File::Exists(gcnew System::String(GifMeshTemplate().c_str()));
}

void GenerateGifAssets(std::string gifImagePath, bool IsSpecialEdition = false,
	string GameDataFolderPath = "",
	float averageFrameRate = 0.1f,
	int textureSize = 1024)
{
	bool useGameFolder = GameDataFolderPath != "";
	if (System::IO::File::Exists(gcnew System::String(gifImagePath.c_str())) == false) {
		MsgBox(gifImagePath + "\n does not exists.");
		return;
	}

	string appFolder = getAppFolder();
	string gifName = GetFileNameWithoutExtension(gifImagePath);

	System::String^ GameDataFolder = gcnew System::String(GameDataFolderPath.c_str());

	string MeshOutputFolder = appFolder;
	string TextureOutputFolder = appFolder;

	string textureGifDir = "Textures\\Gif";

	if (useGameFolder) {
		if (System::IO::Directory::Exists(GameDataFolder) == false) {
			MsgBox(GameDataFolder + "\n GameDataFolder does not exists.");
			return;
		}
		System::String^ MOF = GameDataFolder + "\\meshes\\GIF";
		System::String^ TOF = GameDataFolder + "\\" + gcnew System::String(textureGifDir.c_str());
		if (System::IO::Directory::Exists(MOF) == false) { System::IO::Directory::CreateDirectory(MOF); }
		if (System::IO::Directory::Exists(TOF) == false) { System::IO::Directory::CreateDirectory(TOF); }
		MeshOutputFolder = MarshalString(MOF);
		TextureOutputFolder = MarshalString(TOF);
	}

	System::String^ MeshTemplatePath = gcnew System::String(GifMeshTemplate().c_str());
	
	string nifoutput = MeshOutputFolder + "\\" + gifName + ".nif";

	NifFile target;

	if (DoesMeshTemplateExists()) {
		target.Load(MarshalString(MeshTemplatePath));
	}
	else {
		GenerateGifMesh(gcnew System::String(nifoutput.c_str()));
		target.Load(nifoutput);
	}

	

	float timeLength = 0;
	int gifWidth, gifHeight;

	System::String^ outputPng = gcnew System::String((TextureOutputFolder + "\\" + gifName + ".png").c_str());
	int spriteDimension = 0;

	vector<float> gifKeysTimes = GifUtil::ConvertToSpriteSheets(gcnew System::String(gifImagePath.c_str()), 1,
		gifWidth, gifHeight, timeLength, outputPng, spriteDimension);

	if (spriteDimension < 2) return;
	if (System::IO::File::Exists(outputPng)) {
		auto texconv = gcnew System::String((appFolder + "\\texconv.exe").c_str());
		System::String^ textureSizeStr = gcnew System::String(to_string(textureSize).c_str());
		if (System::IO::File::Exists(texconv)) {
			Process^ convertddsProcess = gcnew Process();
			convertddsProcess->StartInfo->FileName = texconv;
			convertddsProcess->StartInfo->Arguments = " -o \"" + gcnew System::String(TextureOutputFolder.c_str()) + "\" -y -w " + textureSizeStr + " -h " + textureSizeStr + " -f BC1_UNORM " +
				"\"" + outputPng + "\"";
			convertddsProcess->Start();
			convertddsProcess->WaitForExit();
			System::IO::File::Delete(outputPng);
		}
		else {
			MsgBox("texconv.exe\n does not exist.\n failed convert to DDS");
		}
	}
	int halfWidth = gifWidth / 2;
	int halfHeight = gifHeight / 2;

	int biggerLength = halfWidth;
	if (halfWidth < halfHeight) biggerLength = halfHeight;
	float diff = 0;
	if (biggerLength > 50) {
		diff = ((float)biggerLength) / 50.0f;
		halfHeight = halfHeight / diff;
		halfWidth = halfWidth / diff;
	}

	float uvInterval = 1.0f / spriteDimension;
	for (auto &refShape : target.GetShapes()) {

		NiShader* shader = target.GetShader(refShape);

		string gifdds = string(textureGifDir + "\\" + gifName + ".dds");
		target.SetTextureSlot(shader, gifdds, 0);//Set texture

		auto geomData = target.GetHeader().GetBlock<NiGeometryData>(refShape->GetDataRef());

		geomData->uvSets[0][0] = Vector2(uvInterval, 0);
		geomData->uvSets[0][1] = Vector2(0, 0);
		geomData->uvSets[0][2] = Vector2(0, uvInterval);
		geomData->uvSets[0][3] = Vector2(uvInterval, uvInterval);

		geomData->vertices[0] = Vector3(-halfWidth, 0, halfHeight);
		geomData->vertices[1] = Vector3(halfWidth, 0, halfHeight);
		geomData->vertices[2] = Vector3(halfWidth, 0, -halfHeight);
		geomData->vertices[3] = Vector3(-halfWidth, 0, -halfHeight);

		int frameCount = spriteDimension * spriteDimension;

		vector<float> keyFrames;
		keyFrames.push_back(0);
		if (timeLength == 0) {
			for (size_t i = 0; i < frameCount; i++) keyFrames.push_back(averageFrameRate);
		}
		else
		{
			for (size_t i = 0; i < gifKeysTimes.size(); i++) keyFrames.push_back(gifKeysTimes[i]);
		}


		auto columnsController = target.GetHeader().GetBlock<BSLightingShaderPropertyFloatController>(shader->GetControllerRef());
		auto rowsController = target.GetHeader().GetBlock<BSLightingShaderPropertyFloatController>(columnsController->GetNextControllerRef());

		columnsController->startTime = 0;
		auto columnsInterpolator = target.GetHeader().GetBlock<NiFloatInterpolator>(columnsController->GetInterpolatorRef());
		auto columnsFloatData = target.GetHeader().GetBlock<NiFloatData>(columnsInterpolator->GetDataRef());
		columnsFloatData->data.ClearKeys();
		rowsController->startTime = 0;
		auto rowsInterpolator = target.GetHeader().GetBlock<NiFloatInterpolator>(rowsController->GetInterpolatorRef());
		auto rowsFloatData = target.GetHeader().GetBlock<NiFloatData>(rowsInterpolator->GetDataRef());
		rowsFloatData->data.ClearKeys();
		float accumulator = 0;

		for (size_t i = 0; i < spriteDimension; i++)
		{
			for (size_t o = 0; o < spriteDimension; o++)
			{
				auto k = Key<float>();
				int index = i * spriteDimension + o;
				if (index < keyFrames.size()) {
					accumulator += keyFrames[index];
					k.time = accumulator;
					k.value = uvInterval * o;
					columnsFloatData->data.AddKey(k);
					if (o == 0) {
						auto rk = Key<float>();
						rk.value = uvInterval * i;
						rk.time = accumulator;
						rowsFloatData->data.AddKey(rk);
					}
				}
			}
		}

		columnsController->stopTime = accumulator;
		rowsController->stopTime = accumulator;

	}
	target.Save(nifoutput);

	if (IsSpecialEdition) {
		ConvertFileByVersion(nifoutput.c_str(), "SE");
	}
}

int main(int argc, char* argv[], char* const envp[])
{
	bool IsSpecialEdition = true;
	string GameDataFolderPath = "";
	float averageFrameRate = 0.1f;
	int textureSize = 1024;
	auto appName = System::IO::Path::GetFileNameWithoutExtension(gcnew System::String(getAppPath().c_str()));
	auto config = gcnew System::String(getAppFolder().c_str()) + "\\" + appName + ".config";
	if (System::IO::File::Exists(config)) {
		auto lines = System::IO::File::ReadAllLines(config);
		for (int i = 0; i < lines->Length; i++)
		{
			auto data = lines[i]->Split('=');
			auto val = MarshalString(data[1]);
			stringstream ss(val);
			if (data[0]->ToLower() == "gamedatapath") {
				GameDataFolderPath = val;
			}
			if (data[0]->ToLower() == "averageframerate") {
				ss >> averageFrameRate;
			}
			if (data[0]->ToLower() == "texturesize") {
				ss >> textureSize;
			}
		}
		if (averageFrameRate <= 0) averageFrameRate = 0.1f;
	}
	else {
		System::IO::File::WriteAllText(config, "GameDataPath=\naverageFrameRate=0.1f\nTextureSize=1024");
	}
	if (argc == 1) {
		string versionText = "SpecialEdition (SE)";
		if (IsSpecialEdition == false) versionText = "Legendary Edition (LE)";
		MsgBox("Welecom to use Skyrim Gif " + versionText + "\n\nDrag drop some gif files on this app.\nPlease download \"texconv\" for convert dds. \n\n[Configuration]\nGameDataPath="
			+ GameDataFolderPath + "\nAverageframerate="
			+ to_string(averageFrameRate)
			+ "\nTextureSize=" + to_string(textureSize)
		);
		if (DoesMeshTemplateExists() == false) GenerateGifMesh(gcnew System::String(GifMeshTemplate().c_str()));
		return 0;
	}
	for (size_t i = 1; i < argc; i++)
	{
		if (System::IO::Path::GetExtension(gcnew System::String(argv[i]))->ToLower() == ".gif")
			GenerateGifAssets(argv[i], IsSpecialEdition, GameDataFolderPath, averageFrameRate, textureSize);
		else print("\nFailed to convert" + string(argv[i]));
	}

	print("\n\nConvert Gif Completed");

	return 0;
}