#include "Hooks.h"

#include "Settings.h"

namespace Hooks {
    void Install() {
        if (REL::Module::IsVR())
            PickupObject::idx = 206; // 0xce

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
        const auto result{ func(a_this, a_targetRef, a_activatorRef, a_arg3, a_object, a_targetCount) };

        if (!a_targetRef)
            return result;

        const auto name{ a_targetRef->GetName() };
        const auto form_id{ a_targetRef->GetFormID() };

        if ("Coin Purse"sv.compare(name))
            return result;

        if (a_targetRef->IsCrimeToActivate()) {
            logger::debug("Player stealing {} (0x{:x}). Incrementing Items Stolen...", name, form_id);
            IncrementStat();
        }

        return result;
    }

    void AddObjectToContainer::Thunk(RE::Actor* a_this, RE::TESBoundObject* a_object, RE::ExtraDataList* a_extraList, std::int32_t a_count,
                                     RE::TESObjectREFR* a_fromRefr) {
        func(a_this, a_object, a_extraList, a_count, a_fromRefr);

        if (!(a_fromRefr && a_extraList && a_object))
            return;

        if (const auto ui{ RE::UI::GetSingleton() }) {
            if (ui->IsMenuOpen(RE::BarterMenu::MENU_NAME))
                return;
        }

        if (const auto origin_base_obj{ a_fromRefr->GetBaseObject() }) {
            const auto origin_name{ a_fromRefr->GetName() };
            const auto origin_form_id{ a_fromRefr->GetFormID() };
            const auto origin_form_type{ origin_base_obj->GetFormType() };
            const auto obj_name{ a_object->GetName() };
            const auto obj_form_id{ a_object->GetFormID() };
            logger::debug("Taking object {} (0x{:x}) from {} (0x{:x}) ({})", obj_name, obj_form_id, origin_name, origin_form_id, origin_form_type);
            if (origin_form_type == RE::FormType::Container) {
                if (const auto player{ RE::PlayerCharacter::GetSingleton() }) {
                    if (const auto container_owner{ a_fromRefr->GetOwner() }) {
                        if (container_owner->IsPlayerRef()) {
                            logger::debug("Container {} belongs to player", origin_name);
                            return;
                        }
                    }
                    if (const auto owner{ a_extraList->GetOwner() }) {
                        const auto owner_name{ owner->GetName() };
                        logger::debug("\tOwner: {} (0x{:x})", owner_name, owner->GetFormID());
                        if (const auto owner_faction{ owner->As<RE::TESFaction>() }) {
                            if (player->IsInFaction(owner_faction)) {
                                logger::debug("Player belongs to owner faction {}", owner_faction->GetName());
                                return;
                            }
                            logger::debug("\tPlayer is not owner of {} (0x{:x}). Incrementing Items Stolen...", obj_name, obj_form_id);
                            IncrementStat();
                            return;
                        }
                        if (!owner->IsPlayerRef()) {
                            logger::debug("\tPlayer is not owner of {} (0x{:x}). Incrementing Items Stolen...", obj_name, obj_form_id);
                            IncrementStat();
                        }
                    }
                }
            }
        }
    }

    void PickupObject::Thunk(RE::PlayerCharacter* a_this, RE::TESObjectREFR* a_ref, uint32_t a_count, bool a_arg3, bool a_playSound) {
        func(a_this, a_ref, a_count, a_arg3, a_playSound);

        if (!a_ref)
            return;

        if (const auto base_obj{ a_ref->GetBaseObject() }) {
            if (!base_obj->IsGold())
                return;
        }

        if (a_ref->IsCrimeToActivate()) {
            logger::debug("Player stealing coin. Incrementing Items Stolen...");
            IncrementStat();
        }
    }

    bool IncrementStat(const RE::BSFixedString* a_stat, std::int32_t a_value) {
        static REL::Relocation<decltype(&IncrementStat)> func{ RELOCATION_ID(16117, 16359) };
        return func(a_stat, a_value);
    }
}
