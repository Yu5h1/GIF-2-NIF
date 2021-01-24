// Gif2Nif.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <sstream>
#include <NifFile.h>
#include <GifUtil.h>
#include <DebugUtil.h>
#include <PathUtil.h>
#include <StringUtil.h>

using namespace std;

System::String^ ToString_clr(string txt) { return gcnew System::String(txt.c_str()); }

bool IsSpecialEdition = false;
std::string GifUtil::GifConvertInfo::filterColor = "";
int GifUtil::GifConvertInfo::OutputTextureSize = 1024;
int GifUtil::GifConvertInfo::FrameSizeExtend = 0;
float GifUtil::GifConvertInfo::averageFrameRate = 0.1f;
bool GifUtil::GifConvertInfo::TextureSizeByDimension = false;

string appFolder = getAppFolder();
string appName = getAppName();
string appFileName = getAppFileName();
string appPath = getAppPath();

string TagOfSpecificShapeName = "gif";
string SizeMode = "";
string nameSet = "";
string outputFolderPath = "";
float MaxMeshSize = 50;
bool FlipU = false;
bool OverwriteNameSet = false;
string texconvFormatArg = IsSpecialEdition ? "BC7_UNORM" : "BC1_UNORM";
string texconv = appFolder + "\\texconv.exe";

bool DoesTexconvExists = System::IO::File::Exists(ToString_clr(texconv));

string DefaultGifMeshTemplatePath = appFolder + "\\GifMeshTemplate.nif";
string GifMeshTemplatePath = DefaultGifMeshTemplatePath;


void GenerateGifMesh(string path) {
	System::IO::File::WriteAllBytes(ToString_clr(path), System::Convert::FromBase64String("R2FtZWJyeW8gRmlsZSBGb3JtYXQsIFZlcnNpb24gMjAuMi4wLjcKBwACFAEMAAAADQAAAFMAAAABAClPcHRpbWl6ZWQgd2l0aCBTU0UgTklGIE9wdGltaXplciB2My4wLjcuAAEACgAKAAAAQlNGYWRlTm9kZQoAAABOaVRyaVNoYXBlDwAAAE5pQWxwaGFQcm9wZXJ0eQ4AAABOaVRyaVNoYXBlRGF0YRgAAABCU0xpZ2h0aW5nU2hhZGVyUHJvcGVydHknAAAAQlNMaWdodGluZ1NoYWRlclByb3BlcnR5RmxvYXRDb250cm9sbGVyEwAAAE5pRmxvYXRJbnRlcnBvbGF0b3ILAAAATmlGbG9hdERhdGESAAAAQlNTaGFkZXJUZXh0dXJlU2V0CAAAAEJTWEZsYWdzAAABAAIAAwAEAAUABgAHAAcACAAFAAYACQBYAAAAYQAAAA8AAAAcAQAAZAAAACIAAAAIAAAAMAAAADAAAAAoAAAAIgAAAAgAAAAIAAAAAwAAAAcAAAAHAAAAZ2lmTm9kZQcAAABnaWZNZXNoAwAAAEJTWAAAAAAAAAAAAQAAAAwAAAD/////AAAIAAAAAAAAAAAAAAAAAAAAgD8AAAAAAAAAAAAAAAAAAIA/AAAAAAAAAAAAAAAAAACAPwAAgD//////AQAAAAEAAAAAAAAAAQAAAAAAAAD/////DgAIAAAAAAAAAAAAAAAAAAAAgD8AAAAAAAAAAAAAAAAAAIA/AAAAAAAAAAAAAAAAAACAPwAAgD//////AwAAAP////8AAAAAAAAAAAAEAAAAAgAAAP////8AAAAA/////+wSgAAAAAAEAAAAAVq14sEAAAAAyGEiQlm14kEAAAAAymEiQgR340EAAAAAkAkAQQN348EAAAAAdhEAQQEQAAAAAAHQ3qQ0AACAP4hdjrOqFp40AACAP9SdvbPQ3qQ0AACAP4hdjrNMoas0AACAP4CJPrMh+Dq8jtSGs7z7f7+nMaO8T/uws//yf78h+Dq8jtSGs7z7f7/uJT67c406s7r/f7+8+3+/C0SlNCL4Ojz+8n+/YwCfNKcxozy8+3+/C0SlNCL4Ojy6/3+/zrKrNPAlPjsAgBE6RE5EP3q9wUHt7AJCAAAAgD4AAAAAAAAAAAAAAAAAAAAAAACAPgAAgD4AAIA+AAD/////AgAGAAAAAQAAAQACAAIAAwAAAAAAAgAAAP////8AAAAABQAAAAEDQILRgAAAAAAAAAAAAAAAAIA/AACAPwkAAAAAAIA/AACAPwAAgD8AAIA/AwAAAAAAgD8AAAAAAABIQgAAgD8AAIA/AACAPwAAAD+amZk+AAAAQAoAAAAIAAAAgD8AAAAAAAAAAI/C9T0EAAAABgAAABQAAAAAAAAACAAAAAUAAAAFAAAAAAAAAAAAAACPwvU9AACAPo/CdT4AAAA/7FG4PgAAQD+PwvU+AACAPwUAAAAFAAAAAAAAAAAAAACPwvU8AACAPo/CdT0AAAA/7FG4PQAAQD+PwvU9AACAPwkAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD/////CAAAAIA/AAAAAAAAAACPwvU+BAAAAAsAAAAWAAAA//9//wcAAAACAAAACwAAAAEAAAAAAAAA"));
}

void GenerateGifAssets(string outputFolder, std::string sourcePath, string outputName)
{
	bool useGameFolder = outputFolder != "";
	if (!DoesPathOccupied(sourcePath,true)) return;

	string gifName = outputName == "" ? GetFileNameWithoutExtension(sourcePath) : outputName;

	string MeshOutputFolder = appFolder;
	string TextureOutputFolder = appFolder;

	if (useGameFolder) {
		if (!DoesPathOccupied(outputFolder)) {
			MsgBox(outputFolder + "\n GameDataFolder does not exists.");
			return;
		}
		string MOF = outputFolder + "\\meshes";
		string TOF = outputFolder + "\\textures";
		if (DoesPathOccupied(MOF) == false) { System::IO::Directory::CreateDirectory(ToString_clr(MOF)); }
		if (DoesPathOccupied(TOF) == false) { System::IO::Directory::CreateDirectory(ToString_clr(TOF)); }
		MeshOutputFolder = MOF;
		TextureOutputFolder = TOF;
	}
	string nifoutput = MeshOutputFolder + "\\" + gifName + ".nif";
	auto outputPng = ToString_clr(TextureOutputFolder + "\\" + gifName + ".png");

	NifFile target;

	if (DoesPathOccupied(GifMeshTemplatePath)) {
		target.Load(GifMeshTemplatePath);
	}else {
		if (GifMeshTemplatePath == DefaultGifMeshTemplatePath) {
			GenerateGifMesh(nifoutput);
			target.Load(nifoutput);
		} else MsgBox(GifMeshTemplatePath+"\n is not exists ! ");
	}
	BSXFlags* bsxFlags = nullptr;
	auto extraDatas = target.GetRootNode()->GetExtraData();
	if (extraDatas.GetSize() > 0) {
		for (auto extradata : extraDatas)
		{
			if (target.GetHeader().GetBlockTypeStringById(extradata.GetIndex()) == "BSXFlags") {
				bsxFlags = target.GetHeader().GetBlock<BSXFlags>(extradata.GetIndex());
				break;
			}
		}
	}
	if (bsxFlags == nullptr) {
		bsxFlags = new BSXFlags();		
		target.AssignExtraData(target.GetRootNode(), bsxFlags);
	}
	bsxFlags->SetIntegerData(11);
	bsxFlags->SetName("BSX");

	float timeLength = 0;
	int gifWidth = 0, gifHeight = 0;

	if (SizeMode != "" && SizeMode.length() >= 3 && StringContain(SizeMode, ":")) {
		auto AspectRatioValues = StringSplit(SizeMode, ":");
		gifWidth = stoi(AspectRatioValues[0]);
		gifHeight = stoi(AspectRatioValues[1]);
	}
	int spriteDimension, resultTextureSize;

	vector<float> gifKeysTimes = GifUtil::ConvertToSpriteSheets(ToString_clr(sourcePath), 1,
		gifWidth, gifHeight, timeLength, outputPng, spriteDimension, resultTextureSize);

	string TextureSlotValue = "textures\\" + gifName ;

	if (spriteDimension < 2) return;
	if (System::IO::File::Exists(outputPng)) {
		TextureOutputFolder = MarshalString(System::IO::Path::GetDirectoryName(outputPng));
		string textureSizeString = to_string(resultTextureSize);
		if (DoesTexconvExists) {
			Process^ convertddsProcess = gcnew Process();
			convertddsProcess->StartInfo->FileName = ToString_clr(texconv);
			convertddsProcess->StartInfo->Arguments = " -o \"" + ToString_clr(
				TextureOutputFolder +
				+"\" -y -w " + textureSizeString + " -h " +
				textureSizeString + " -f "+ texconvFormatArg +" \"" + MarshalString(outputPng) + "\""
			);

			convertddsProcess->Start();
			convertddsProcess->WaitForExit();
			System::IO::File::Delete(outputPng);
			TextureSlotValue += ".dds";
		}
		else {
			TextureSlotValue += ".png";
		}
	} else return; 

	float diff = (gifWidth < gifHeight ? gifHeight : gifWidth) / (MaxMeshSize < 1 ? 20 : MaxMeshSize);

	float halfHeight = (gifHeight / diff)*0.5f;
	float halfWidth = (gifWidth / diff)*0.5f;

	float uvDimension = 1.0f / spriteDimension;
	for (auto &refShape : target.GetShapes()) {
		if (TagOfSpecificShapeName != "") {
			if (!ToString_clr(refShape->GetName())->ToLower()->Contains(ToString_clr(TagOfSpecificShapeName))) {
				continue;
			}
		}
		auto parent = target.GetParentNode(refShape);

		int shaderIndex = refShape->GetShaderPropertyRef();
		auto shader = target.GetHeader().GetBlock<BSShaderProperty>(shaderIndex);

		target.SetTextureSlot(shader, TextureSlotValue, 0);//Set texture

		auto geomData = target.GetHeader().GetBlock<NiGeometryData>(refShape->GetDataRef());

		if (parent->GetBlockName() == "NiBillboardNode") FlipU = !FlipU;

		// Deprecated because of glitch
		// shader->uvScale = Vector2(FlipU ? -uvDimension: uvDimension, uvDimension);

		if (geomData->vertices.size() == 4) {
			// A square plane
			float Ul = FlipU ? uvDimension : 0;
			float Ur = FlipU ? 0 : uvDimension;
			geomData->uvSets[0][0] = Vector2(Ur, 0);
			geomData->uvSets[0][1] = Vector2(Ul, 0);
			geomData->uvSets[0][2] = Vector2(Ul, uvDimension);
			geomData->uvSets[0][3] = Vector2(Ur, uvDimension);
			if (SizeMode == "" || SizeMode == "default") {
				geomData->vertices[0] = Vector3(-halfWidth, 0, halfHeight);
				geomData->vertices[1] = Vector3(halfWidth, 0, halfHeight);
				geomData->vertices[2] = Vector3(halfWidth, 0, -halfHeight);
				geomData->vertices[3] = Vector3(-halfWidth, 0, -halfHeight);
			}
		}else {
			for (int i = 0; i < geomData->uvSets[0].size(); i++)
			{
				auto curUVvert = geomData->uvSets[0][i];
				auto x = curUVvert.u*uvDimension;
				x = FlipU ? 1 - x : x;
				geomData->uvSets[0][i] = Vector2(x,curUVvert.v*uvDimension);
			}
		}

		gifKeysTimes.insert(gifKeysTimes.begin(), 0);

		int columnControllerIndex = shader->GetControllerRef();
		if (columnControllerIndex < 0) {
			auto columnController = new BSLightingShaderPropertyFloatController();			
			columnController->SetFlags(8);
			columnController->SetTypeOfControlledVariable(20);
			columnController->SetTargetRef(shaderIndex);
			auto columnsInterpolator = new NiFloatInterpolator();
			columnController->SetInterpolatorRef(target.GetHeader().AddBlock(columnsInterpolator));
			auto floatdata = new NiFloatData();
			floatdata->SetInterpolationType(KeyType::CONST_KEY);
			columnsInterpolator->SetDataRef(target.GetHeader().AddBlock(floatdata));

			columnControllerIndex = target.GetHeader().AddBlock(columnController);
			shader->SetControllerRef(columnControllerIndex);
		}
		auto columnsController = target.GetHeader().GetBlock<BSLightingShaderPropertyFloatController>(columnControllerIndex);
		//MsgBox(columnController);

		int rowControllerIndex = columnsController->GetNextControllerRef();
		if (rowControllerIndex < 0) {
			auto rowsController = new BSLightingShaderPropertyFloatController();
			rowsController->SetTypeOfControlledVariable(22);
			rowsController->SetFlags(8);
			rowsController->SetTargetRef(shaderIndex);
			auto rowsInterpolator = new NiFloatInterpolator();
			rowsController->SetInterpolatorRef(target.GetHeader().AddBlock(rowsInterpolator));
			auto floatdata = new NiFloatData();
			floatdata->SetInterpolationType(KeyType::CONST_KEY);
			rowsInterpolator->SetDataRef(target.GetHeader().AddBlock(floatdata));

			rowControllerIndex = target.GetHeader().AddBlock(rowsController);
			columnsController->SetNextControllerRef(rowControllerIndex);
		}
		auto rowsController = target.GetHeader().GetBlock<BSLightingShaderPropertyFloatController>(rowControllerIndex);
		//MsgBox(columnsController->typeOfControlledVariable.ToString());
		//MsgBox(rowsController->typeOfControlledVariable.ToString());
		//return;

		columnsController->SetStartTime(0);
		auto columnsInterpolator = target.GetHeader().GetBlock<NiFloatInterpolator>(columnsController->GetInterpolatorRef());
		auto columnsFloatData = target.GetHeader().GetBlock<NiFloatData>(columnsInterpolator->GetDataRef());
		columnsFloatData->ClearKeys();
		rowsController->SetStartTime(0);
		auto rowsInterpolator = target.GetHeader().GetBlock<NiFloatInterpolator>(rowsController->GetInterpolatorRef());
		auto rowsFloatData = target.GetHeader().GetBlock<NiFloatData>(rowsInterpolator->GetDataRef());
		rowsFloatData->ClearKeys();
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
					k.value = uvDimension * o;
					columnsFloatData->AddKey(k);
					if (o == 0) {
						auto rk = Key<float>();
						rk.value = uvDimension * i;
						rk.time = accumulator;
						rowsFloatData->AddKey(rk);
					}
				}
			}
		}		
		columnsController->SetStopTime(accumulator);
		rowsController->SetStopTime(accumulator);

	}
	target.PrettySortBlocks();
	target.Save(nifoutput);
}
System::String^ MakeLegalFilePath(System::String^ filename) {
	auto illeagalChar = (gcnew System::String("*:?\"<>|"))->ToCharArray();
	filename = filename->Replace("/", "\\");
	filename = filename->Replace("\\\\", "\\");
	auto sep = gcnew cli::array<System::Char>{ '\\' };
	auto separateParts = filename->Split(sep, System::StringSplitOptions::None);
	filename = separateParts[separateParts->Length - 1];

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
		SetValue("TexconvFormatArg", texconvFormatArg) +
		SetValue("TagOfSpecificShapeName", TagOfSpecificShapeName) +
		SetValue("SizeMode", SizeMode)
		
		//"FrameSizeExtend=0"
	));
}
void ShowSetting() {
	MsgBox("gamedatapath=" + outputFolderPath +
		"\naverageframerate=" + to_string(GifUtil::GifConvertInfo::averageFrameRate) +
		"\ntexturesize=" + to_string(GifUtil::GifConvertInfo::OutputTextureSize) +
		"\nmaxmeshsize=" + to_string(MaxMeshSize) +
		"\nfiltercolor=" + GifUtil::GifConvertInfo::filterColor +
		"\nnameset=" + nameSet +
		"\noverwritenameset=" + (OverwriteNameSet ? "true" : "false") +
		"\nflipu=" + (FlipU ? "true" : "false") +
		"\ntagofspecificshapename=" + TagOfSpecificShapeName +
		"\ntexconvformatarg=" + texconvFormatArg +
		"\nsizemode=" + SizeMode +
		"\nGifMeshTemplatePath="+GifMeshTemplatePath);
}
void LoadSetting(vector<string> lines) {
	for (int i = 0; i < lines.size(); i++)
	{		
		auto data = StringSplit(lines[i], "=");
		if (data.size() > 1) {			
			auto item = StringTolower(data[0]);
			auto stringVal = data[1];
			for (auto symbol : { "-",";" }) stringVal = TrimRight(stringVal, symbol, -1);
			auto val = StringReplace(StringReplace(stringVal, " ", ""), "\t", "");
			bool boolVal = val == "true" || val == "1";
			stringstream ss(val);
			if (item == "gamedatapath") outputFolderPath = stringVal;
			else if (item == "averageframerate") ss >> GifUtil::GifConvertInfo::averageFrameRate;
			else if (item == "texturesize") ss >> GifUtil::GifConvertInfo::OutputTextureSize;
			else if (item == "maxmeshsize") ss >> MaxMeshSize;
			else if (item == "filtercolor") GifUtil::GifConvertInfo::filterColor = val;
			else if (item == "nameset") nameSet = stringVal;
			else if (item == "overwritenameset") OverwriteNameSet = boolVal;
			else if (item == "flipu") FlipU = boolVal;
			else if (item == "tagofspecificshapename") TagOfSpecificShapeName = stringVal;
			else if (item == "texconvformatarg") texconvFormatArg = val;
			else if (item == "sizemode") { SizeMode = val; }
			else if (item == "gifmeshtemplatepath") {
				GifMeshTemplatePath = stringVal; 
				if (!StringContain(GifMeshTemplatePath,":\\")) 
					GifMeshTemplatePath = appFolder + "\\" + GifMeshTemplatePath;
				if (!StringContain(GifMeshTemplatePath, ".nif"))
					GifMeshTemplatePath += ".nif";
			}
		}
	}
	if (GifUtil::GifConvertInfo::averageFrameRate <= 0) GifUtil::GifConvertInfo::averageFrameRate = 0.1f;
}

int main(int argc, char* argv[], char* const envp[])
{	
	auto configPath = appFolder + "\\" + appName + ".config";

	if (DoesPathOccupied(configPath)) {
		LoadSetting(ReadAllLines(configPath));
	}else { CreateConfig(ToString_clr(configPath)); }

	if (!DoesTexconvExists) {
		MsgBox("texconv.exe\n does not exists.\n image will convert to PNG");
	}

	//ShowSetting();
	//return 0;
	vector<string> gifImgs;
	if (argc == 1) {
		CreateConfig(ToString_clr(configPath));
		MsgBox("Welecom to use Skyrim Gif " +
			string(IsSpecialEdition ? "SpecialEdition (SE)" : "Legendary Edition (LE)") +
			"\n\nUsage:\n" +
			"Drag drop your gif files over this app then drop.\n" +
			"Please download \"texconv\" for convert dds.\n\n"
		);
		if (!DoesPathOccupied(DefaultGifMeshTemplatePath)) GenerateGifMesh(DefaultGifMeshTemplatePath);
		return 0;
	} else
	{
		vector<string> overwriteSettingList;
		for (int i = 1; i < argc; i++) {
			if (StringContain(argv[i], ".gif")) gifImgs.push_back(argv[i]);
			else if (StringContain(argv[i],"="))
				overwriteSettingList.push_back(argv[i]);
		}
		LoadSetting(overwriteSettingList);
	}

	int LastNum = 1;	
	if (nameSet != "" && !OverwriteNameSet) {
		auto outputFolder = DoesPathOccupied(outputFolderPath) ? outputFolderPath+"\\meshes" : appFolder;
		if (DoesPathOccupied(outputFolder)) {
			int existsAssetsCount = System::IO::Directory::GetFiles(ToString_clr(outputFolder), ToString_clr(nameSet + "*.nif"))->Length;
			if (existsAssetsCount > 0) LastNum = existsAssetsCount;
		}
	}
	for (int i = 0; i < gifImgs.size(); i++)
	{
		string outputName = "";
		if (nameSet != "") {
			int num = i + LastNum;
			outputName = nameSet + MarshalString(num.ToString("00"));
		}		
		GenerateGifAssets(outputFolderPath, gifImgs[i], outputName);
	}
	print("\n\nConvert Gif Completed");
	return 0;
}