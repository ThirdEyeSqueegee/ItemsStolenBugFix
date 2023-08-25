#include "Hooks.h"

#include "Settings.h"

namespace Hooks {
    void Install() {
        stl::write_vfunc<RE::TESFlora, ActivateFlora>();
        logger::info("Installed TESFlora::Activate hook");

        stl::write_vfunc<RE::PlayerCharacter, AddObjectToContainer>();
        logger::info("Installed PlayerCharacter::AddObjectToContainer hook");

        if (Settings::coins_flag) {
            stl::write_vfunc<RE::PlayerCharacter, PickupObject>();
            logger::info("Installed PlayerCharacter::PickupObject hook");
        }
    }

    bool ActivateFlora::Thunk(RE::TESFlora* a_this, RE::TESObjectREFR* a_targetRef, RE::TESObjectREFR* a_activatorRef, std::uint8_t a_arg3,
                              RE::TESBoundObject* a_object, std::int32_t a_targetCount) {
        const auto result  = func(a_this, a_targetRef, a_activatorRef, a_arg3, a_object, a_targetCount);
        const auto name    = a_targetRef->GetName();
        const auto form_id = a_targetRef->GetFormID();

        if ("Coin Purse"sv.compare(name))
            return result;

        if (a_targetRef->IsCrimeToActivate()) {
            logger::debug("Player stealing {} (0x{:x})", name, form_id);
            IncrementStat();
        }

        return result;
    }

    void AddObjectToContainer::Thunk(RE::Actor* a_this, RE::TESBoundObject* a_object, RE::ExtraDataList* a_extraList, std::int32_t a_count,
                                     RE::TESObjectREFR* a_fromRefr) {
        if (a_object && a_fromRefr && a_extraList) {
            logger::debug("Object: {} (0x{:x})", a_object->GetName(), a_object->GetFormID());
            const auto form_type = a_fromRefr->GetBaseObject()->GetFormType();
            logger::debug("\tFrom: {} (0x{:x}) (type {})", a_fromRefr->GetName(), a_fromRefr->GetFormID(), form_type);
            if (const auto owner = a_extraList->GetOwner(); form_type == RE::FormType::Container) {
                const auto owner_name = owner->GetName();
                logger::debug("\tOwner: {} (0x{:x})", owner_name, owner->GetFormID());
                if (!owner->IsPlayerRef()) {
                    logger::debug("\tPlayer is not owner {}. Incrementing Items Stolen stat...", owner_name);
                    IncrementStat();
                }
            }
        }

        return func(a_this, a_object, a_extraList, a_count, a_fromRefr);
    }

    void PickupObject::Thunk(RE::PlayerCharacter* a_this, RE::TESObjectREFR* a_object, uint32_t a_count, bool a_arg3, bool a_playSound) {
        func(a_this, a_object, a_count, a_arg3, a_playSound);

        if (!a_object->IsGold())
            return;

        if (a_object->IsCrimeToActivate()) {
            logger::debug("Player stealing coin");
            IncrementStat();
        }
    }

    bool IncrementStat(const RE::BSFixedString* a_stat, std::int32_t a_value) {
        static REL::Relocation<decltype(&IncrementStat)> func{ RELOCATION_ID(16117, 16359) };
        return func(a_stat, a_value);
    }
}
