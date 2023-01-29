#include "Cache.h"
#include "Settings.h"
namespace ProtectNPC
{
		
		static auto dataHandler = RE::TESDataHandler::GetSingleton();

		class TESObjectLoadedEventHandler : public RE::BSTEventSink<RE::TESObjectLoadedEvent>
		{
		public:
			using EventResult = RE::BSEventNotifyControl;

			static TESObjectLoadedEventHandler* GetSingleton()
			{
				static TESObjectLoadedEventHandler singleton;
				return std::addressof(singleton);
			}

			virtual EventResult ProcessEvent(const RE::TESObjectLoadedEvent* a_event, RE::BSTEventSource<RE::TESObjectLoadedEvent>*) override
			{
				if (!a_event) {
					return RE::BSEventNotifyControl::kContinue;
				}

				auto actor = RE::TESForm::LookupByID<RE::Actor>(a_event->formID);

				if (!actor || actor->IsPlayerRef() || actor->IsPlayerTeammate()) {
					return RE::BSEventNotifyControl::kContinue;
				}

				auto npc = actor->GetActorBase();
				std::string editorid = Cache::EditorID::GetSingleton()->GetEditorID(npc->formID);
				auto name = npc->GetName();

				if (!npc) {
					return RE::BSEventNotifyControl::kContinue;
				}

				auto unique = npc->IsUnique();

				if (!unique) {
					//非独特但是要保护的NPC
					if (Settings::GetSingleton()->IsProtectNPC(npc->formID)) {
						npc->actorData.actorBaseFlags.set(RE::ACTOR_BASE_DATA::Flag::kProtected);
						actor->boolFlags.set(RE::Actor::BOOL_FLAGS::kProtected);
						if (Settings::GetSingleton()->enableLogs) {
							logger::info("非独特但是要保护的NPC信息：{},{}", editorid, name);
						}
						return RE::BSEventNotifyControl::kContinue;
					} else {
						return RE::BSEventNotifyControl::kContinue;
					}
					
				}

				bool protectedNPC = npc->IsProtected();

				if (protectedNPC) {
					return RE::BSEventNotifyControl::kContinue;
				}

				bool isExclusionNPC = Settings::GetSingleton()->IsExclusionNPC(npc->formID);
				//排除的独特NPC
				if (isExclusionNPC) {
					if (Settings::GetSingleton()->enableLogs) {
						logger::info("排除的独特NPC信息：{},{}", editorid, name);
					}
					
					return RE::BSEventNotifyControl::kContinue;
				}

				//保护独特的NPC
				npc->actorData.actorBaseFlags.set(RE::ACTOR_BASE_DATA::Flag::kProtected);
				actor->boolFlags.set(RE::Actor::BOOL_FLAGS::kProtected);
				if (Settings::GetSingleton()->enableLogs) {
					logger::info("保护NPC信息：{},{}", editorid, name);
				}
				return RE::BSEventNotifyControl::kContinue;
			}

		protected:
			TESObjectLoadedEventHandler() = default;
			TESObjectLoadedEventHandler(const TESObjectLoadedEventHandler&) = delete;
			TESObjectLoadedEventHandler(TESObjectLoadedEventHandler&&) = delete;
			virtual ~TESObjectLoadedEventHandler() = default;

			TESObjectLoadedEventHandler& operator=(const TESObjectLoadedEventHandler&) = delete;
			TESObjectLoadedEventHandler& operator=(TESObjectLoadedEventHandler&&) = delete;
		};

}

