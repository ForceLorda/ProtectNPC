#pragma once

class Settings
{
public:
	[[nodiscard]] static Settings* GetSingleton();
	void Setting();
	void Load();
	bool IsExclusionNPC(RE::FormID a_formID);
	bool IsProtectNPC(RE::FormID a_formID);
	bool enableLogs = false;

private:
	std::vector<std::string> npcExcludedList;
	std::vector<std::string> npcProtectList;
	std::vector<std::string> SplitString(const std::string& s, char delimiter);
	
	
};


