#include "csv_curve_publisher.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>

std::string formatTimeOfDay(int64_t timeNs) {
    const int64_t msOfDay = timeNs / 1'000'000;
    const int64_t h  = msOfDay / 3'600'000;
    const int64_t m  = (msOfDay / 60'000) % 60;
    const int64_t s  = (msOfDay / 1'000) % 60;
    const int64_t ms = msOfDay % 1'000;

    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(2) << h  << ':'
        << std::setw(2) << m  << ':'
        << std::setw(2) << s  << '.'
        << std::setw(3) << ms;
    return oss.str();
}

std::string todayUtcYmd() {
    const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm utcTm{};
    gmtime_r(&now, &utcTm);

    std::ostringstream oss;
    oss << std::put_time(&utcTm, "%Y-%m-%d");
    return oss.str();
}

CsvCurvePublisher::CsvCurvePublisher(const std::string& path, const std::string& dateYmd)
    : dateYmd_(dateYmd) {
    // Append if the file already has data (and a header); otherwise start
    // fresh with a header line matching bookSnapshot.q's expected columns.
    std::ifstream existing(path);
    const bool needsHeader = !existing.good() || existing.peek() == std::ifstream::traits_type::eof();
    existing.close();

    out_.open(path, std::ios::app);
    if (!out_.is_open()) {
        throw std::runtime_error("failed to open " + path + " for writing");
    }
    if (needsHeader) {
        out_ << "sym,date,time,rate\n";
    }
}

void CsvCurvePublisher::publish(const std::string& table, const NormalizedTick& tick) {
    if (table != "curve") {
        return;
    }
    out_ << tick.sym << ','
         << dateYmd_ << ','
         << formatTimeOfDay(tick.timeNs) << ','
         << tick.rate << '\n';
    out_.flush();
}
