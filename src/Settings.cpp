#include "Settings.h"

void Settings::LoadSettings()
{
    logger::info("Loading settings");

    CSimpleIniA ini;

    ini.SetUnicode();
    ini.LoadFile(R"(.\Data\SKSE\Plugins\ItemsStolenBugFix.ini)");

    debug_logging = ini.GetBoolValue("Log", "Debug");

    if (debug_logging)
    {
        spdlog::get("Global")->set_level(spdlog::level::level_enum::debug);
        logger::debug("Debug logging enabled");
    }

    coins_flag = ini.GetBoolValue("General", "bEnableCoins");

    logger::info("Loaded settings");
    logger::info("\tbEnableCoins = {}", coins_flag);
}
