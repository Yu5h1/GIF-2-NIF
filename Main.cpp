// Gif2Nif.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <sstream>
#include <NifFile.h>
#include <GifUtil.h>
#include <DebugUtil.h>
#include <PathUtil.h>
#include <NifVersionConverter.h>

using namespace std;

System::String^ ToString_clr(string txt) { return gcnew System::String(txt.c_str()); }

std::string GifUtil::GifConvertInfo::filterColor = "";
int GifUtil::GifConvertInfo::OutputTextureSize = 1024;
int GifUtil::GifConvertInfo::FrameSizeExtend = 0;
float GifUtil::GifConvertInfo::averageFrameRate = 0.1f;
bool GifUtil::GifConvertInfo::TextureSizeByDimension = false;

string appFolder = getAppFolder();
string TagOfSpecificShapeName = "";
string nameSet = "";
bool IsSpecialEdition = false;
string outputFolderPath = "";
float MaxMeshSize = 50;
bool FlipU = false;
bool OverwriteNameSet = false;
string texconvFormatArg = IsSpecialEdition ? "BC7_UNORM" : "BC1_UNORM";
string texconv = appFolder + "\\texconv.exe";
string SubFolder = "";

bool DoesTexconvExists = System::IO::File::Exists(ToString_clr(texconv));

string GifMeshTemplate() { return appFolder + "\\GifMeshTemplate.nif"; }

void GenerateGifMesh(System::String^ path) {
	System::IO::File::WriteAllBytes(path, System::Convert::FromBase64String("R2FtZWJyeW8gRmlsZSBGb3JtYXQsIFZlcnNpb24gMjAuMi4wLjcKBwACFAEMAAAADQAAAFMAAAABAClPcHRpbWl6ZWQgd2l0aCBTU0UgTklGIE9wdGltaXplciB2My4wLjcuAAEACgAKAAAAQlNGYWRlTm9kZQoAAABOaVRyaVNoYXBlDwAAAE5pQWxwaGFQcm9wZXJ0eQ4AAABOaVRyaVNoYXBlRGF0YRgAAABCU0xpZ2h0aW5nU2hhZGVyUHJvcGVydHknAAAAQlNMaWdodGluZ1NoYWRlclByb3BlcnR5RmxvYXRDb250cm9sbGVyEwAAAE5pRmxvYXRJbnRlcnBvbGF0b3ILAAAATmlGbG9hdERhdGESAAAAQlNTaGFkZXJUZXh0dXJlU2V0CAAAAEJTWEZsYWdzAAABAAIAAwAEAAUABgAHAAcACAAFAAYACQBYAAAAYQAAAA8AAAAcAQAAZAAAACIAAAAIAAAAMAAAADAAAAAoAAAAIgAAAAgAAAAIAAAAAwAAAAcAAAAHAAAAZ2lmTm9kZQcAAABnaWZNZXNoAwAAAEJTWAAAAAAAAAAAAQAAAAwAAAD/////AAAIAAAAAAAAAAAAAAAAAAAAgD8AAAAAAAAAAAAAAAAAAIA/AAAAAAAAAAAAAAAAAACAPwAAgD//////AQAAAAEAAAAAAAAAAQAAAAAAAAD/////DgAIAAAAAAAAAAAAAAAAAAAAgD8AAAAAAAAAAAAAAAAAAIA/AAAAAAAAAAAAAAAAAACAPwAAgD//////AwAAAP////8AAAAAAAAAAAAEAAAAAgAAAP////8AAAAA/////+wSgAAAAAAEAAAAAVq14sEAAAAAyGEiQlm14kEAAAAAymEiQgR340EAAAAAkAkAQQN348EAAAAAdhEAQQEQAAAAAAHQ3qQ0AACAP4hdjrOqFp40AACAP9SdvbPQ3qQ0AACAP4hdjrNMoas0AACAP4CJPrMh+Dq8jtSGs7z7f7+nMaO8T/uws//yf78h+Dq8jtSGs7z7f7/uJT67c406s7r/f7+8+3+/C0SlNCL4Ojz+8n+/YwCfNKcxozy8+3+/C0SlNCL4Ojy6/3+/zrKrNPAlPjsAgBE6RE5EP3q9wUHt7AJCAAAAgD4AAAAAAAAAAAAAAAAAAAAAAACAPgAAgD4AAIA+AAD/////AgAGAAAAAQAAAQACAAIAAwAAAAAAAgAAAP////8AAAAABQAAAAEDQILRgAAAAAAAAAAAAAAAAIA/AACAPwkAAAAAAIA/AACAPwAAgD8AAIA/AwAAAAAAgD8AAAAAAABIQgAAgD8AAIA/AACAPwAAAD+amZk+AAAAQAoAAAAIAAAAgD8AAAAAAAAAAI/C9T0EAAAABgAAABQAAAAAAAAACAAAAAUAAAAFAAAAAAAAAAAAAACPwvU9AACAPo/CdT4AAAA/7FG4PgAAQD+PwvU+AACAPwUAAAAFAAAAAAAAAAAAAACPwvU8AACAPo/CdT0AAAA/7FG4PQAAQD+PwvU9AACAPwkAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD/////CAAAAIA/AAAAAAAAAACPwvU+BAAAAAsAAAAWAAAA//9//wcAAAACAAAACwAAAAEAAAAAAAAA"));
}

bool DoesMeshTemplateExists() {
	return System::IO::File::Exists(gcnew System::String(GifMeshTemplate().c_str()));
}



void GenerateGifAssets(string outputFolder, std::string sourceGif, string replaceName)
{
	bool useGameFolder = outputFolder != "";
	if (System::IO::File::Exists(gcnew System::String(sourceGif.c_str())) == false) {
		MsgBox(sourceGif + "\n does not exists.");
		return;
	}

	string gifName = replaceName == "" ? GetFileNameWithoutExtension(sourceGif) : replaceName;

	System::String^ GameDataFolder = gcnew System::String(outputFolder.c_str());

	string MeshOutputFolder = appFolder;
	string TextureOutputFolder = appFolder;

	string textureGifDir = "Textures" + SubFolder;

	if (useGameFolder) {
		if (System::IO::Directory::Exists(GameDataFolder) == false) {
			MsgBox(GameDataFolder + "\n GameDataFolder does not exists.");
			return;
		}
		System::String^ MOF = GameDataFolder + "\\meshes"+ ToString_clr(SubFolder);
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
		GenerateGifMesh(ToString_clr(nifoutput.c_str()));
		target.Load(nifoutput);
	}

	bool hasBsxflags = false;
	auto extraDatas = target.GetRootNode()->GetExtraData();
	if (extraDatas.GetSize() > 0) {
		for (auto extradata : extraDatas)
		{
			if (target.GetHeader().GetBlockTypeStringById(extradata.GetIndex()) == "BSXFlags") {
				hasBsxflags = true;
				break;
			}
		}
	}
	if (!hasBsxflags) {
		auto bsxFlags = new BSXFlags();
		bsxFlags->SetIntegerData(11);
		bsxFlags->SetName("BSX");
		target.AssignExtraData(target.GetRootNode(), bsxFlags);
	}

	float timeLength = 0;
	int gifWidth, gifHeight;

	int spriteDimension, resultTextureSize;

	vector<float> gifKeysTimes = GifUtil::ConvertToSpriteSheets(gcnew System::String(sourceGif.c_str()), 1,
		gifWidth, gifHeight, timeLength, outputPng, spriteDimension, resultTextureSize);

	string TextureSlotValue = textureGifDir + "\\" + gifName ;

	if (spriteDimension < 2) return;
	if (System::IO::File::Exists(outputPng)) {
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
			geomData->vertices[0] = Vector3(-halfWidth, 0, halfHeight);
			geomData->vertices[1] = Vector3(halfWidth, 0, halfHeight);
			geomData->vertices[2] = Vector3(halfWidth, 0, -halfHeight);
			geomData->vertices[3] = Vector3(-halfWidth, 0, -halfHeight);
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
			columnController->typeOfControlledVariable = 20;
			columnController->SetTargetRef(shaderIndex);
			auto columnsInterpolator = new NiFloatInterpolator();
			columnController->SetInterpolatorRef(target.GetHeader().AddBlock(columnsInterpolator));
			auto floatdata = new NiFloatData();
			floatdata->data.interpolation = KeyType::CONST_KEY;
			columnsInterpolator->SetDataRef(target.GetHeader().AddBlock(floatdata));

			columnControllerIndex = target.GetHeader().AddBlock(columnController);
			shader->SetControllerRef(columnControllerIndex);
		}
		auto columnsController = target.GetHeader().GetBlock<BSLightingShaderPropertyFloatController>(columnControllerIndex);
		//MsgBox(columnController);
		

		int rowControllerIndex = columnsController->GetNextControllerRef();
		if (rowControllerIndex < 0) {
			auto rowsController = new BSLightingShaderPropertyFloatController();
			rowsController->typeOfControlledVariable = 22;
			rowsController->SetFlags(8);
			rowsController->SetTargetRef(shaderIndex);
			auto rowsInterpolator = new NiFloatInterpolator();
			rowsController->SetInterpolatorRef(target.GetHeader().AddBlock(rowsInterpolator));
			auto floatdata = new NiFloatData();
			floatdata->data.interpolation = KeyType::CONST_KEY;
			rowsInterpolator->SetDataRef(target.GetHeader().AddBlock(floatdata));

			rowControllerIndex = target.GetHeader().AddBlock(rowsController);
			columnsController->SetNextControllerRef(rowControllerIndex);
		}
		auto rowsController = target.GetHeader().GetBlock<BSLightingShaderPropertyFloatController>(rowControllerIndex);
		//MsgBox(columnsController->typeOfControlledVariable.ToString());
		//MsgBox(rowsController->typeOfControlledVariable.ToString());
		//return;

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
					k.value = uvDimension * o;
					columnsFloatData->data.AddKey(k);
					if (o == 0) {
						auto rk = Key<float>();
						rk.value = uvDimension * i;
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
	auto illeagalChar = (gcnew System::String("*:?\"<>|"))->ToCharArray();
	filename = filename->Replace("/", "\\");
	filename = filename->Replace("\\\\", "\\");
	auto sep = gcnew cli::array<System::Char>{ '\\' };
	auto separateParts = filename->Split(sep, System::StringSplitOptions::None);

	if (separateParts->Length > 1) {
		SubFolder = "";
		for (int i = 0; i < separateParts->Length-1; i++)
		{
			SubFolder += "\\"+MarshalString(separateParts[i]);
		}
	}
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
	auto nameprefix = ToString_clr(SubFolder == "" ? "" : SubFolder + "\\") ;
	if (nameprefix != "") {
		while (nameprefix->StartsWith("\\"))
		{
			nameprefix = nameprefix->Remove(0, 1);
		}
	}
	System::IO::File::WriteAllText(path, gcnew System::String(
		SetValue("GameDataPath", outputFolderPath) +
		SetValue("averageFrameRate", GifUtil::GifConvertInfo::averageFrameRate) +
		SetValue("TextureSize", GifUtil::GifConvertInfo::OutputTextureSize) +
		SetValue("maxMeshSize", MaxMeshSize) +
		SetValue("FilterColor", GifUtil::GifConvertInfo::filterColor, "black , pureblack") +
		SetValue("FlipU", FlipU) +
		SetValue("NameSet", MarshalString(nameprefix)+nameSet) +
		SetValue("OverwriteNameSet", OverwriteNameSet) +
		SetValue("TexconvFormatArg", texconvFormatArg) +
		SetValue("TagOfSpecificShapeName", TagOfSpecificShapeName)
		
		//"FrameSizeExtend=0"
	));
}

int main(int argc, char* argv[], char* const envp[])
{	
	auto appName = System::IO::Path::GetFileNameWithoutExtension(ToString_clr(getAppPath()));
	auto config = gcnew System::String(appFolder.c_str()) + "\\" + appName + ".config";	

	if (!DoesTexconvExists) {
		MsgBox("texconv.exe\n does not exists.\n image will convert to PNG");
	}

	if (System::IO::File::Exists(config)) {
		auto lines = System::IO::File::ReadAllLines(config);
		for (int i = 0; i < lines->Length; i++)
		{
			auto data = lines[i]->Split('=');
			if (data->Length > 1) {
				auto item = data[0]->ToLower();		
				auto stringVal = data[1]->Split(';','-')[0];
				auto MarshalTextVal = MarshalString(stringVal);


				auto NoSpaceValue = stringVal->Split(' ', '\t')[0];
				

				auto val = MarshalString(NoSpaceValue);
				bool boolVal = val == "true" || val == "1";
				stringstream ss(val);
				if (item == "gamedatapath") outputFolderPath = MarshalTextVal;
				if (item == "averageframerate") ss >> GifUtil::GifConvertInfo::averageFrameRate;
				if (item == "texturesize") ss >> GifUtil::GifConvertInfo::OutputTextureSize;
				if (item == "maxmeshsize") ss >> MaxMeshSize;
				if (item == "filtercolor") GifUtil::GifConvertInfo::filterColor = val;
				if (item == "nameset") nameSet = MarshalString(GetlegalFileName(stringVal));
				if (item == "overwritenameset") OverwriteNameSet = boolVal;
				if (item == "flipu") FlipU = boolVal;
				if (item == "tagofspecificshapename") TagOfSpecificShapeName = MarshalTextVal;
				if (item == "texconvformatarg") texconvFormatArg = val;
				//if (item == "framesizeextend") ss >> GifUtil::GifConvertInfo::FrameSizeExtend;
			}
			
		}
		if (GifUtil::GifConvertInfo::averageFrameRate <= 0) GifUtil::GifConvertInfo::averageFrameRate = 0.1f;
	}else { CreateConfig(config); }

	//return 0;

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
			outputFolderPath : appFolder;
		LastNum = System::IO::Directory::GetFiles(ToString_clr(outputPath), ToString_clr(nameSet + "*.nif"))->Length;
	}
	for (size_t i = 1; i < argc; i++)
	{
		auto curFileType = System::IO::Path::GetExtension(ToString_clr(argv[i]))->ToLower();
		if (curFileType == ".gif") {
			string curReplaceName = "";
			if (nameSet != "") {
				int num = i - 1 + LastNum;
				curReplaceName = nameSet;
				if (num > 0) {
					curReplaceName += MarshalString(num.ToString("00"));
				}
			}
			GenerateGifAssets(outputFolderPath, argv[i], curReplaceName);
		}else print("\nFailed to convert" + string(argv[i]));
	}

	print("\n\nConvert Gif Completed");

	return 0;
}