#include "Settings.h"
#include <Cache.h>

Settings* Settings::GetSingleton()
{
	static Settings singleton;
	return std::addressof(singleton);
}

void Settings::Setting()
{
		constexpr auto path = L"Data/SKSE/Plugins/ProtectNPC.ini";

		CSimpleIniA ini;
		ini.SetUnicode();

		ini.LoadFile(path);
		std::string enableLogsSettings = ini.GetValue("Settings", "enableLogs");
		if (strcmp(enableLogsSettings.c_str(), "true") == 0){  //比较两个字符串是否相等。
			enableLogs = true;
		} else {
			enableLogs = false;
		}
		logger::info("enableLogs:{}", enableLogs);
}

void Settings::Load()
{

	std::vector<std::string> files;
	auto constexpr folder = R"(Data\SKSE\Plugins\ProtectNPC)";
	for (const auto& entry : std::filesystem::directory_iterator(folder)) {
		if (entry.exists() && !entry.path().empty() && entry.path().extension() == ".ini"sv) {
			auto path = entry.path().string();
			files.push_back(path);
		}
	}

	for (auto& path : files) {
		CSimpleIniA ini;
		ini.SetUnicode();
		ini.SetMultiKey();
		auto iniFIleName = path.c_str();
		ini.LoadFile(iniFIleName);
		//排除独特的NPC
		std::string ExcludedList = ini.GetValue("Settings", "ExcludedList", "");
		if (ExcludedList.size()>0) {
			auto npcExclusionVector = SplitString(ExcludedList, ',');
			for (size_t i = 0; i < npcExclusionVector.size(); i++) {
				auto npcExclusion = npcExclusionVector[i];
				string::trim(npcExclusion);
				npcExcludedList.emplace_back(npcExclusion);
			}
		}
		//要保护的非独特NPC
		std::string ProtectList = ini.GetValue("Settings", "ProtectList", "");
		if (ProtectList.size() > 0) {
			auto ProtectVector = SplitString(ProtectList, ',');
			for (size_t i = 0; i < ProtectVector.size(); i++) {
				auto npcProtect = ProtectVector[i];
				string::trim(npcProtect);
				npcProtectList.emplace_back(npcProtect);
			}
		}
	}
	//打印排除NPC
	std::stringstream ssExcluded;
	for (size_t i = 0; i < npcExcludedList.size(); i++) {
		auto npcExclusion = npcExcludedList[i];
		if (i != 0) {
			ssExcluded << ", ";
		}
		ssExcluded << npcExclusion;
	}
	auto printExcludedContent = ssExcluded.str();
	logger::info("ExcludedList:{}", printExcludedContent);

	//打印保护NPC
	std::stringstream ssProtect;
	for (size_t i = 0; i < npcProtectList.size(); i++) {
		auto npcProtect = npcProtectList[i];
		if (i != 0) {
			ssProtect << ", ";
		}
		ssProtect << npcProtect;
	}
	auto printProtectContent = ssProtect.str();
	logger::info("ProtectList:{}", printProtectContent);
	
}
bool Settings::IsExclusionNPC(RE::FormID a_formID)
{
	bool isExclusionNPC = false;
	std::string editorid = Cache::EditorID::GetSingleton()->GetEditorID(a_formID);
	std::vector<std::string> npcExclusions = Settings::GetSingleton()->npcExcludedList;
	if (std::find(npcExclusions.begin(), npcExclusions.end(), editorid) != npcExclusions.end()) {
		isExclusionNPC = true;
	}
	return isExclusionNPC;
}

bool Settings::IsProtectNPC(RE::FormID a_formID)
{
	bool IsProtectNPC = false;
	std::string editorid = Cache::EditorID::GetSingleton()->GetEditorID(a_formID);
	std::vector<std::string> npcProtects = Settings::GetSingleton()->npcProtectList;
	if (std::find(npcProtects.begin(), npcProtects.end(), editorid) != npcProtects.end()) {
		IsProtectNPC = true;
	}
	return IsProtectNPC;
}

std::vector<std::string> Settings::SplitString(const std::string& s, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}

