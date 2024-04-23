#include <iostream>

#include "log.h"

#include "stats.h"

namespace stats
{
    int system_events[MAX_STREAMS] = {0};
    int accepted[MAX_STREAMS] = {0};
    int replaced[MAX_STREAMS] = {0};
    int executed[MAX_STREAMS] = {0};
    int executed_shares[MAX_STREAMS] = {0};
    int cancelled[MAX_STREAMS] = {0};

    // this struct does not have to be performant since it's used once at the end
    struct stats_t
    {
        std::string name;
        int system_events = 0;
        int accepted = 0;
        int replaced = 0;
        int executed = 0;
        int executed_shares = 0;
        int cancelled = 0;
    };

    void print_stats(const stats_t &stats)
    {
        print("");
        print(stats.name);
        print(" ", "Accepted", stats.accepted);
        print(" ", "System Event", stats.system_events);
        print(" ", "Replaced", stats.replaced);
        print(" ", "Cancelled", stats.cancelled);
        print(" ", "Executed", stats.executed, "messages | shares", stats.executed_shares);
    }

    // not sure what the alternative to keeping them global is
    void aggregate_stats()
    {
        stats_t stats[MAX_STREAMS] = {};
        for (int i = 0; i < MAX_STREAMS; i++)
        {
            stats[i] = {
                "Stream " + std::to_string(i),
                system_events[i],
                accepted[i],
                replaced[i],
                executed[i],
                executed_shares[i],
                cancelled[i]};
        }

        // aggregate total stats
        stats_t total_stats = {"Total"};
        for (int i = 0; i < MAX_STREAMS; i++)
        {
            total_stats.system_events += system_events[i];
            total_stats.accepted += accepted[i];
            total_stats.replaced += replaced[i];
            total_stats.executed += executed[i];
            total_stats.executed_shares += executed_shares[i];
            total_stats.cancelled += cancelled[i];
        }

        // output
        for (int i = 0; i < MAX_STREAMS; i++)
        {
            if (stats[i].accepted)
                print_stats(stats[i]);
        }
        print_stats(total_stats);
    }
}