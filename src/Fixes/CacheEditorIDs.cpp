#include "Cache.h"
#include "Fixes.h"

//Cache skipped formEditorIDs
namespace Fixes::CacheFormEditorIDs
{
	struct SetFormEditorID
	{
		static bool thunk(RE::TESForm* a_this, const char* a_str)
		{
			if (!string::is_empty(a_str) && !a_this->IsDynamicForm()) {
				const auto& [map, lock] = RE::TESForm::GetAllFormsByEditorID();
				const RE::BSWriteLockGuard locker{ lock };
				if (map) {
					map->emplace(a_str, a_this);
				}
				Cache::EditorID::GetSingleton()->CacheEditorID(a_this, a_str);
			}
			return func(a_this, a_str);
		}
		static inline REL::Relocation<decltype(thunk)> func;

		static inline constexpr size_t idx{ 0x33 };
	};

	void Install()
	{
		stl::write_vfunc<RE::TESNPC, SetFormEditorID>();
		stl::write_vfunc<RE::Actor, SetFormEditorID>();
		logger::info("Installed editorID cache");
	}
}
