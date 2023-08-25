#pragma once

namespace Hooks {
    void Install();

    class ActivateFlora : public Singleton<ActivateFlora> {
    public:
        static bool Thunk(RE::TESFlora* a_this, RE::TESObjectREFR* a_targetRef, RE::TESObjectREFR* a_activatorRef, std::uint8_t a_arg3,
                          RE::TESBoundObject* a_object, std::int32_t a_targetCount);

        inline static REL::Relocation<decltype(&Thunk)> func;

        static constexpr std::size_t idx{ 55 }; // 0x37
    };

    class AddObjectToContainer : public Singleton<AddObjectToContainer> {
    public:
        static void Thunk(RE::Actor* a_this, RE::TESBoundObject* a_object, RE::ExtraDataList* a_extraList, std::int32_t a_count,
                          RE::TESObjectREFR* a_fromRefr);

        inline static REL::Relocation<decltype(&Thunk)> func;

        static constexpr std::size_t idx{ 90 }; // 0x5a
    };

    class PickupObject : public Singleton<PickupObject> {
    public:
        static void Thunk(RE::PlayerCharacter* a_this, RE::TESObjectREFR* a_ref, uint32_t a_count, bool a_arg3, bool a_playSound);

        inline static REL::Relocation<decltype(&Thunk)> func;

        inline static std::size_t idx{ 204 }; // 0xcc
    };

    inline static const RE::BSFixedString items_stolen{ "Items Stolen" };

    static bool IncrementStat(const RE::BSFixedString* a_stat = &items_stolen, std::int32_t a_value = 1);
}
