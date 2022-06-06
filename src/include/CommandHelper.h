//
// Created by xhy on 2022/5/17.
//

#ifndef TRAPDOOR_COMMANDHELPER_H
#define TRAPDOOR_COMMANDHELPER_H

//clang-format off

#include <MC/Player.hpp>
#include <MC/Vec3.hpp>
#include <string>
#include <tuple>
#include <utility>

#include "Global.h"

//clang-format on

class CommandOutput;
namespace tr {
    struct ActionResult {
        std::string msg;
        bool success;

        ActionResult(std::string m, bool su);

        void SendTo(CommandOutput &output) const;
    };

    Vec3 getLookAtPos(Player *player);

    ActionResult ErrorMsg(const std::string &msg);

    void setup_tickCommand(int);
    void setup_profCommand(int);

    void setup_villageCommand(int);
    void setup_logCommand(int);
    void setup_testCommand(int);
    void setup_spawnCommand(int);
    void setup_dataCommand(int);
    void setup_hsaCommand(int);
    void setup_trapdoorCommand(int);
    void setup_counterCommand(int);
    void setup_funcCommand(int);
    void setup_playerCommand(int);
    void setup_hudCommand(int);

}  // namespace tr

#endif  // TRAPDOOR_COMMANDHELPER_H
