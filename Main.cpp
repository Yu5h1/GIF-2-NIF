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

std::string GifUtil::GifConvertInfo::filterColor = "";
int GifUtil::GifConvertInfo::OutputTextureSize = 1024;
int GifUtil::GifConvertInfo::FrameSizeExtend = 0;
float GifUtil::GifConvertInfo::averageFrameRate = 0.1f;
bool GifUtil::GifConvertInfo::TextureSizeByDimension = false;

string nameSet = "";
bool IsSpecialEdition = true;
string outputFolderPath = "";
string texconvFormatArg = IsSpecialEdition ? "BC7_UNORM":"BC1_UNORM";
float MaxMeshSize = 50;
bool FlipU = false;
bool OverwriteNameSet = false;


string GifMeshTemplate() { return (getAppFolder() + "\\GifMeshTemplate.nif"); }

bool DoesMeshTemplateExists() {
	return System::IO::File::Exists(gcnew System::String(GifMeshTemplate().c_str()));
}

System::String^ ToString_clr(string txt) { return gcnew System::String(txt.c_str()); }


void GenerateGifAssets(string outputFolder, std::string sourceGif, string replaceName)
{
	bool useGameFolder = outputFolder != "";
	if (System::IO::File::Exists(gcnew System::String(sourceGif.c_str())) == false) {
		MsgBox(sourceGif + "\n does not exists.");
		return;
	}

	string appFolder = getAppFolder();
	string gifName = replaceName == "" ? GetFileNameWithoutExtension(sourceGif) : replaceName;

	System::String^ GameDataFolder = gcnew System::String(outputFolder.c_str());

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
	auto outputPng = ToString_clr(TextureOutputFolder + "\\" + gifName + ".png");

	NifFile target;

	if (DoesMeshTemplateExists()) {
		target.Load(MarshalString(MeshTemplatePath));
	}
	else {
		GenerateGifMesh(gcnew System::String(nifoutput.c_str()));
		target.Load(nifoutput);
	}

	//target.SetNodeName(0, gifName.c_str());

	float timeLength = 0;
	int gifWidth, gifHeight;

	int spriteDimension, resultTextureSize;

	vector<float> gifKeysTimes = GifUtil::ConvertToSpriteSheets(gcnew System::String(sourceGif.c_str()), 1,
		gifWidth, gifHeight, timeLength, outputPng, spriteDimension, resultTextureSize);

	if (spriteDimension < 2) return;
	if (System::IO::File::Exists(outputPng)) {
		auto texconv = gcnew System::String((appFolder + "\\texconv.exe").c_str());
		string textureSizeString = to_string(resultTextureSize);
		if (System::IO::File::Exists(texconv)) {
			Process^ convertddsProcess = gcnew Process();
			convertddsProcess->StartInfo->FileName = texconv;
			convertddsProcess->StartInfo->Arguments = " -o \"" + ToString_clr(
				TextureOutputFolder +
				+"\" -y -w " + textureSizeString + " -h " +
				textureSizeString + " -f "+ texconvFormatArg +" \"" + MarshalString(outputPng) + "\""
			);

			convertddsProcess->Start();
			convertddsProcess->WaitForExit();
			System::IO::File::Delete(outputPng);
		}
		else {
			MsgBox("texconv.exe\n does not exist.\n failed convert to DDS");
		}
	}

	float diff = (gifWidth < gifHeight ? gifHeight : gifWidth) / (MaxMeshSize < 1 ? 20 : MaxMeshSize);

	float halfHeight = (gifHeight / diff)*0.5f;
	float halfWidth = (gifWidth / diff)*0.5f;



	float uvInterval = 1.0f / spriteDimension;
	for (auto &refShape : target.GetShapes()) {
		
		auto parent = target.GetParentNode(refShape);

		NiShader* shader = target.GetShader(refShape);

		string gifdds = string(textureGifDir + "\\" + gifName + ".dds");
		target.SetTextureSlot(shader, gifdds, 0);//Set texture

		auto geomData = target.GetHeader().GetBlock<NiGeometryData>(refShape->GetDataRef());

		if (parent->GetBlockName() == "NiBillboardNode") FlipU = !FlipU;

		float Ul = FlipU ? uvInterval : 0;
		float Ur = FlipU ? 0 : uvInterval;

		geomData->uvSets[0][0] = Vector2(Ur, 0);
		geomData->uvSets[0][1] = Vector2(Ul, 0);
		geomData->uvSets[0][2] = Vector2(Ul, uvInterval);
		geomData->uvSets[0][3] = Vector2(Ur, uvInterval);

		geomData->vertices[0] = Vector3(-halfWidth, 0, halfHeight);
		geomData->vertices[1] = Vector3(halfWidth, 0, halfHeight);
		geomData->vertices[2] = Vector3(halfWidth, 0, -halfHeight);
		geomData->vertices[3] = Vector3(-halfWidth, 0, -halfHeight);

		gifKeysTimes.insert(gifKeysTimes.begin(), 0);

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
				if (index < gifKeysTimes.size()) {
					accumulator += gifKeysTimes[index];
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
	target.PrettySortBlocks();
	target.Save(nifoutput);

	if (IsSpecialEdition) {
		ConvertFileByVersion(nifoutput.c_str(), "SE");
	}
}
System::String^ GetlegalFileName(System::String^ filename) {
	auto illeagalChar = (gcnew System::String("\\/*:?\"<>|"))->ToCharArray();
	for (int i = 0; i < illeagalChar->Length; i++)
	{		
		filename = filename->Replace(illeagalChar[i].ToString(), "");
	}
	filename = filename->Replace("|", "");
	while (filename->StartsWith("\t") || filename->StartsWith(" "))
	{
		filename = filename->Remove(0, 1);
	}
	while (filename->EndsWith("\t") || filename->EndsWith(" "))
	{
		filename = filename->Remove(filename->ToCharArray()->Length-1, 1);
	}	
	return filename;
}

System::String^ SetValue(System::String^ itemName,System::String^ val , System::String^ comment = "") {
	if (comment != System::String::Empty) comment = "\t--" + comment;
	return itemName + "=" + val + comment + "\n";
}
System::String^ SetValue(System::String^ itemName,int val, System::String^ comment = "") { return SetValue(itemName, val.ToString(), comment); }
System::String^ SetValue(System::String^ itemName, float val, System::String^ comment = "") { return SetValue(itemName, val.ToString(), comment); }
System::String^ SetValue(System::String^ itemName, std::string val, System::String^ comment = "") { return SetValue(itemName, ToString_clr(val), comment); }

void CreateConfig(System::String^ path) {
	System::IO::File::WriteAllText(path, gcnew System::String(
		SetValue("GameDataPath", outputFolderPath) +
		SetValue("averageFrameRate", GifUtil::GifConvertInfo::averageFrameRate) +
		SetValue("TextureSize", GifUtil::GifConvertInfo::OutputTextureSize) +
		SetValue("maxMeshSize", MaxMeshSize) +
		SetValue("FilterColor", GifUtil::GifConvertInfo::filterColor, "black , pureblack") +
		SetValue("FlipU", FlipU) +
		SetValue("NameSet", nameSet) +
		SetValue("OverwriteNameSet", OverwriteNameSet) +
		SetValue("TexconvFormatArg", texconvFormatArg)
		//"FrameSizeExtend=0"
	));
}

int main(int argc, char* argv[], char* const envp[])
{
	auto appName = System::IO::Path::GetFileNameWithoutExtension(ToString_clr(getAppPath()));
	auto config = gcnew System::String(getAppFolder().c_str()) + "\\" + appName + ".config";
	if (System::IO::File::Exists(config)) {
		auto lines = System::IO::File::ReadAllLines(config);
		for (int i = 0; i < lines->Length; i++)
		{
			auto data = lines[i]->Split('=');
			if (data->Length > 1) {
				auto item = data[0]->ToLower();		
				auto stringVal = data[1]->Split(';','-')[0];
				auto NoSpaceValue = stringVal->Split(' ', '\t')[0];
				auto mStringVal = MarshalString(stringVal);

				auto val = MarshalString(NoSpaceValue);
				bool boolVal = val == "true" || val == "1";
				stringstream ss(val);
				if (item == "gamedatapath") outputFolderPath = mStringVal;
				if (item == "averageframerate") ss >> GifUtil::GifConvertInfo::averageFrameRate;
				if (item == "texturesize") ss >> GifUtil::GifConvertInfo::OutputTextureSize;
				if (item == "maxmeshsize") ss >> MaxMeshSize;
				if (item == "filtercolor") GifUtil::GifConvertInfo::filterColor = val;
				if (item == "nameset") nameSet = MarshalString(GetlegalFileName(stringVal));
				if (item == "overwritenameset") OverwriteNameSet = boolVal;
				if (item == "flipu") FlipU = boolVal;
				if (item == "texconvformatarg") texconvFormatArg = val;
				//if (item == "framesizeextend") ss >> GifUtil::GifConvertInfo::FrameSizeExtend;
			}
			
		}
		if (GifUtil::GifConvertInfo::averageFrameRate <= 0) GifUtil::GifConvertInfo::averageFrameRate = 0.1f;
	}else { CreateConfig(config); }

	if (argc == 1) {
		CreateConfig(config);

		MsgBox("Welecom to use Skyrim Gif " +
			string(IsSpecialEdition ? "SpecialEdition (SE)" : "Legendary Edition (LE)") +
			"\n\nUsage:\n" +
			"Drag drop your gif files over this app then drop.\n" +
			"Please download \"texconv\" for convert dds.\n\n"
		);
		if (DoesMeshTemplateExists() == false) GenerateGifMesh(gcnew System::String(GifMeshTemplate().c_str()));
		return 0;
	}
	int LastNum = 0;
	if (nameSet != "" && !OverwriteNameSet) {
		auto outputPath = System::IO::Directory::Exists(ToString_clr(outputFolderPath)) ?
			outputFolderPath : getAppFolder();
		LastNum = System::IO::Directory::GetFiles(ToString_clr(outputPath), ToString_clr(nameSet + "*.nif"))->Length;
	}
	for (size_t i = 1; i < argc; i++)
	{
		if (System::IO::Path::GetExtension(gcnew System::String(argv[i]))->ToLower() == ".gif") {
			string curReplaceName = "";
			if (nameSet != "") {
				int num = i - 1 + LastNum;
				curReplaceName = nameSet;
				if (num > 0) {
					curReplaceName += MarshalString(num.ToString("00"));
				}
			}
			GenerateGifAssets(outputFolderPath, argv[i], curReplaceName);
		}
		else print("\nFailed to convert" + string(argv[i]));
	}

	print("\n\nConvert Gif Completed");

	return 0;
}