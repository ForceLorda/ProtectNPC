#include "Fixes.h"

void Fixes::PostLoad::Install()
{

	logger::info("{:*^30}", "FIXES"sv);

	CacheFormEditorIDs::Install();
	
}


