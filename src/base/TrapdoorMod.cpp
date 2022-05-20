#include "TrapdoorMod.h"

#include "CommandHelper.h"
#include "LoggerAPI.h"

namespace tr {

    Logger &logger() {
        static Logger logger("Trapdoor");
        return logger;
    }

    TrapdoorMod &mod() {
        static TrapdoorMod mod;
        return mod;
    }

    void InitMod() {
        logger().consoleLevel = 8;
        setupCommands();
    }

    void setupCommands() {
        SetupTickCommand();
        SetupTestCommand();
        SetupVillageCommand();
        SetupProfCommand();
        SetupLogCommand();
    }

    void HeavyTick() {}

    void LightTick() {}

}  // namespace tr
