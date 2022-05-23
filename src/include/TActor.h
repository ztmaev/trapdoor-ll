#ifndef TRAPDOOR_TACTOR
#define TRAPDOOR_TACTOR
#include <MC/ActorUniqueID.hpp>
#include <cstdint>

#include "TBlockPos.h"

namespace tr {

    struct TActorUniqueIDHash {
        static uint64_t mceHaseAccLong(uint64_t a1, uint64_t a2) {
            return ((a1 >> 2) + (a1 << 6) + a2 + 2654435769u) ^ a1;
        }

        static uint64_t mceHsaLongLong(uint64_t a1, uint64_t a2) {
            auto x = mceHaseAccLong(0, a1);
            return mceHaseAccLong(x, a2);
        }

        size_t operator()(const ActorUniqueID &key) const {
            return mceHsaLongLong((key.id) >> 32, key.id);
        }
    };

    struct TDwellerData {
        uint64_t tick;
        TBlockPos pos;
    };

}  // namespace tr
#endif
