#pragma once

#include <fstream>
#include <string>

#include "tick_publisher.hpp"

// Appends normalized curve ticks to a CSV file in the exact schema
// bookSnapshot.q reads via ("SDTF"; enlist ",") 0: `:curves.csv -
// header "sym,date,time,rate", date as YYYY-MM-DD, time as HH:MM:SS.mmm.
// Only rows published to the "curve" table are written; a date must be
// supplied since NormalizedTick only carries a time-of-day.
class CsvCurvePublisher : public TickPublisher {
public:
    // dateYmd: the trade date to stamp every row with, e.g. "2026-08-11".
    CsvCurvePublisher(const std::string& path, const std::string& dateYmd);

    void publish(const std::string& table, const NormalizedTick& tick) override;

private:
    std::ofstream out_;
    std::string   dateYmd_;
};

// Formats ns-since-midnight as HH:MM:SS.mmm, matching q's time literal.
std::string formatTimeOfDay(int64_t timeNs);

// Returns today's UTC date as YYYY-MM-DD.
std::string todayUtcYmd();
