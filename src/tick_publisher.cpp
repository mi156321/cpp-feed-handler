#include "tick_publisher.hpp"

#include <iostream>
#include <stdexcept>

void ConsolePublisher::publish(const std::string& table, const NormalizedTick& tick) {
    std::cout << "upd " << table
              << " sym=" << tick.sym
              << " time=" << tick.timeNs << "ns"
              << " rate=" << tick.rate
              << " source=" << tick.source
              << std::endl;
}

#ifdef WITH_KDB
extern "C" {
#include "k.h"
}

KdbTickerplantPublisher::KdbTickerplantPublisher(const std::string& host, int port, const std::string& usernamePassword)
    : handle_(khpu(host.c_str(), port, usernamePassword.c_str())) {
    if (handle_ <= 0) {
        throw std::runtime_error("failed to connect to tickerplant at " + host + ":" + std::to_string(port));
    }
}

KdbTickerplantPublisher::~KdbTickerplantPublisher() {
    if (handle_ > 0) {
        kclose(handle_);
    }
}

void KdbTickerplantPublisher::publish(const std::string& table, const NormalizedTick& tick) {
    // Row order matches the `curve` schema: sym, time, rate, source.
    K row = knk(4,
        ks(const_cast<S>(tick.sym.c_str())),
        ktj(-KN, tick.timeNs),
        kf(tick.rate),
        ks(const_cast<S>(tick.source.c_str()))
    );

    // Async send, mirroring q's (neg h)(`upd; table; enlist row).
    K result = k(-handle_, "upd", ks(const_cast<S>(table.c_str())), knk(1, row), (K)0);
    if (result) {
        r0(result);
    }
}
#endif

std::unique_ptr<TickPublisher> makeDefaultPublisher(const std::string& host, int port, const std::string& usernamePassword) {
#ifdef WITH_KDB
    return std::make_unique<KdbTickerplantPublisher>(host, port, usernamePassword);
#else
    (void)host;
    (void)port;
    (void)usernamePassword;
    return std::make_unique<ConsolePublisher>();
#endif
}
