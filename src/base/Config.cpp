//
// Created by xhy on 2022/5/17.
//

#include "Config.h"

#include "BlockRotateHelper.h"
#include "CommandHelper.h"
#include "Shortcuts.h"
#include "TrapdoorMod.h"

namespace trapdoor {

    namespace {

        void setIntValue(int& key, int value, const std::string& name, int min, int max) {
            if (value >= min && value <= max) {
                key = value;
                trapdoor::logger().info("Set [{}] to {}", name, key);
            } else {
                trapdoor::logger().warn("Value of [{}] should within {} to {},set to default {}",
                                        name, min, max, key);
            }
        }

        void setBoolValue(bool& key, bool value, const std::string& name) {
            key = value;
            trapdoor::logger().info("Set [{}] to {}", name, key);
        }

    }  // namespace

    bool Configuration::init(const std::string& fileName) {
        if (!readConfigFile(fileName)) return false;
        if (!readCommandConfigs()) return false;
        if (!readBasicConfigs()) return false;
        if (!readShortcutConfigs()) return false;
        if (!readDefaultEnableFunctions()) return false;
        if (!readTweakConfigs()) return false;
        return true;
    }

    bool Configuration::readConfigFile(const std::string& path) {
        try {
            this->config.clear();
            std::ifstream i(path);
            i >> this->config;
            trapdoor::logger().info("Read getConfig file {} successfully", path);
            return true;
        } catch (std::exception&) {
            trapdoor::logger().error("Can't read configuration file: {}", path);
            return false;
        }
    }

    bool Configuration::readCommandConfigs() {
        auto cc = this->config["commands"];
        CommandConfig tempConfig;
        try {
            for (const auto& i : cc.items()) {
                const auto& value = i.value();
                tempConfig.enable = value["enable"].get<bool>();
                tempConfig.permissionLevel = value["permission-level"].get<int>();
                this->commandsConfigs.insert({i.key(), tempConfig});
            }
        } catch (const std::exception& e) {
            trapdoor::logger().error("error read commands: {}", e.what());
            return false;
        }
        return true;
    }
    bool Configuration::readBasicConfigs() {
        try {
            auto bc = this->config["basic-config"];
            auto pl = bc["particle-performance-level"].get<int>();
            auto pv = bc["particle-view-distance"].get<int>();
            auto hudFreq = bc["hud-refresh-freq"].get<int>();
            auto tdh = bc["tool-damage-threshold"].get<int>();
            auto keepSimPlayerInv = bc["keep-sim-player-inv"].get<bool>();
            auto severCrashToken = bc["server-crash-token"].get<std::string>();

            auto& cfg = this->basicConfig;
            setIntValue(cfg.particleLevel, pl, "particle performance level", 1, 3);
            setIntValue(cfg.particleViewDistance2D, pv * pv, "particle view distance", 0,
                        INT32_MAX);
            setIntValue(cfg.hudRefreshFreq, hudFreq, "hud refresh frequency", 1, 100000);
            setIntValue(cfg.toolDamageThreshold, tdh, "tool damage threshold", -100, 65536);
            setBoolValue(cfg.keepSimPlayerInv, keepSimPlayerInv, "keep sim player inv");
            this->basicConfig.serverCrashToken = severCrashToken;
        } catch (const std::exception& e) {
            trapdoor::logger().error("error read basic-config: {}", e.what());
            return false;
        }
        return true;
    }
    bool Configuration::readShortcutConfigs() {
        try {
            auto cc = this->config["shortcuts"];
            for (const auto& i : cc.items()) {
                Shortcut sh;
                const auto& value = i.value();
                auto type = value["type"].get<std::string>();
                auto actions = value["actions"];
                for (const auto& act : actions) {
                    sh.actions.push_back(act.get<std::string>());
                }
                if (sh.actions.empty()) {
                    trapdoor::logger().error("Shortcut {} has no action", i.key());
                    continue;
                }
                if (type == "use") {
                    sh.type = ShortcutType::USE;
                    sh.setItem(value["item"].get<std::string>());
                    sh.prevent = value["prevent"].get<bool>();
                    this->shortcuts.push_back(sh);
                    trapdoor::logger().debug("Shortcut: {}", sh.getDescription());
                } else if (type == "use-on") {
                    sh.type = ShortcutType::USE_ON;
                    sh.setItem(value["item"].get<std::string>());
                    sh.setBlock(value["block"].get<std::string>());
                    sh.prevent = value["prevent"].get<bool>();
                    this->shortcuts.push_back(sh);
                    trapdoor::logger().debug("Shortcut: {}", sh.getDescription());
                } else if (type == "destroy") {
                    sh.type = ShortcutType::DESTROY;
                    sh.setItem(value["item"].get<std::string>());
                    sh.setBlock(value["block"].get<std::string>());
                    sh.prevent = value["prevent"].get<bool>();
                    this->shortcuts.push_back(sh);
                    trapdoor::logger().debug("Shortcut: {}", sh.getDescription());
                }

                else if (type == "command") {
                    auto command = value["command"].get<std::string>();
                    registerShortcutCommand(command, actions);
                    continue;
                } else {
                    trapdoor::logger().error("unknown shortcut type: {}", type);
                }
            }
        } catch (const std::exception& e) {
            trapdoor::logger().error("error read shortcut getConfig: {}", e.what());
            return false;
        }
        return true;
    }

    CommandConfig Configuration::getCommandConfig(const std::string& command) {
        auto it = this->commandsConfigs.find(command);
        if (it == this->commandsConfigs.end()) {
            trapdoor::logger().warn("Can not find config info of [{}],it will not be registered",
                                    command);
            return {false, 2};
        }
        return it->second;
    }
    bool Configuration::readDefaultEnableFunctions() {
        try {
            auto def = this->config["default-enable-functions"];
            auto& mod = trapdoor::mod();
            auto hc = def["hopper-counter"].get<bool>();
            auto br = def["block-rotate"].get<bool>();
            auto hud = def["hud"].get<bool>();
            mod.getHopperChannelManager().setAble(hc);
            mod.getHUDHelper().setAble(hud);
            trapdoor::setBlockRotationAble(br);
            trapdoor::logger().info("Set [hopper counter] to {}", hc);
            trapdoor::logger().info("Set [HUD] to {}", hud);
            trapdoor::logger().info("Set [Block rotate] to {}", br);
        } catch (const std::exception& e) {
            trapdoor::logger().error("error read default functions config: {}", e.what());
            return false;
        }

        return true;
    }
    bool Configuration::readTweakConfigs() {
        try {
            auto det = this->config["default-enable-tweaks"];
            auto& mod = trapdoor::mod();
            auto forcePlace = det["force-place-level"].get<int>();
            auto forceOpenContainer = det["force-open-container"].get<bool>();
            auto dropperNoCost = det["dropper-no-cost"].get<bool>();
            auto autoSelectTool = det["auto-select-tool"].get<bool>();

            auto& cfg = this->tweakConfig;
            setBoolValue(cfg.autoSelectTool, autoSelectTool, "auto select tools");
            setBoolValue(cfg.dropperNoCost, dropperNoCost, "dropper no cost");
            setBoolValue(cfg.forceOpenContainer, forceOpenContainer, "force open container");
            setIntValue(cfg.forcePlaceLevel, forcePlace, "force place level", 0, 2);
        } catch (const std::exception& e) {
            trapdoor::logger().error("error read tweak config: {}", e.what());
            return false;
        }

        return true;
    }
    std::string Configuration::dumpConfigInfo()  // NOLINT
    {
        return "Developing!";
    }

}  // namespace trapdoor
