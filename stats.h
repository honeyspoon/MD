#ifndef STATS_H
#define STATS_H

#include "outch.h"

namespace stats
{
    // AoS instead of SoA for cache
    // and array instead of hash
    extern int system_events[MAX_STREAMS];
    extern int accepted[MAX_STREAMS];
    extern int replaced[MAX_STREAMS];
    extern int executed[MAX_STREAMS];
    extern int executed_shares[MAX_STREAMS];
    extern int cancelled[MAX_STREAMS];

    void aggregate_stats();
}

#endif // STATS_H