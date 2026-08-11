#include "source_normalizer.hpp"

#include <sstream>
#include <string>
#include <unordered_map>

namespace {

// ExchangeA symbols arrive bare (e.g. "SWAP1") and already match the
// downstream sym convention, so no rewriting is needed here - but the
// lookup stays localized to this translation unit in case that changes.
std::string canonicalSymbol(const std::string& rawSymbol) {
    return rawSymbol;
}

} // namespace

NormalizedTick normalizeSourceA(const RawTickSourceA& raw) {
    NormalizedTick tick;
    tick.sym    = canonicalSymbol(raw.symbol);
    // epoch ms -> ns since midnight (UTC), matching q's `timespan` semantics.
    tick.timeNs = (raw.epochMs % 86'400'000LL) * 1'000'000LL;
    // ExchangeA quotes rates in bps; the shared schema uses decimal rate.
    tick.rate   = raw.rateBps / 10'000.0;
    tick.source = "exchangeA";
    return tick;
}

NormalizedTick normalizeSourceB(const RawTickSourceB& raw) {
    NormalizedTick tick;
    tick.sym    = canonicalSymbol(raw.tag55Symbol);
    tick.timeNs = raw.tag273TimeNs;
    tick.rate   = raw.tag270Rate;
    tick.source = "exchangeB";
    return tick;
}

RawTickSourceB parseSourceBLine(const std::string& line) {
    std::unordered_map<std::string, std::string> fields;
    std::stringstream ss(line);
    std::string field;
    while (std::getline(ss, field, '|')) {
        const auto eq = field.find('=');
        if (eq == std::string::npos) {
            throw std::invalid_argument("malformed field in source B line: " + field);
        }
        fields[field.substr(0, eq)] = field.substr(eq + 1);
    }

    const auto require = [&](const char* tag) -> const std::string& {
        auto it = fields.find(tag);
        if (it == fields.end()) {
            throw std::invalid_argument(std::string("missing tag ") + tag + " in source B line: " + line);
        }
        return it->second;
    };

    RawTickSourceB raw;
    raw.tag55Symbol   = require("55");
    raw.tag273TimeNs  = std::stoll(require("273"));
    raw.tag270Rate    = std::stod(require("270"));
    return raw;
}
