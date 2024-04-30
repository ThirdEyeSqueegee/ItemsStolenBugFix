#include "Settings.h"

void Settings::LoadSettings() noexcept
{
    logger::info("Loading settings");

    CSimpleIniA ini;

    ini.SetUnicode();
    ini.LoadFile(R"(.\Data\SKSE\Plugins\ItemsStolenBugFix.ini)");

    debug_logging = ini.GetBoolValue("Log", "Debug");

    if (debug_logging) {
        spdlog::set_level(spdlog::level::debug);
        logger::debug("Debug logging enabled");
    }

    coins_flag = ini.GetBoolValue("General", "bEnableCoins");

    logger::info("Loaded settings");
    logger::info("\tbEnableCoins = {}", coins_flag);
}
