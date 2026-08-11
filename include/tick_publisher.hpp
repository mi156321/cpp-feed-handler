#pragma once

#include <memory>
#include <string>

#include "normalized_tick.hpp"

// Publishing sink for normalized ticks. Implementations decide where the
// tick actually goes - a live tickerplant over kdb+ IPC in production, or
// stdout for local dev/testing when the KX headers aren't available.
class TickPublisher {
public:
    virtual ~TickPublisher() = default;
    virtual void publish(const std::string& table, const NormalizedTick& tick) = 0;
};

// Prints each tick to stdout. Always available; used by default when
// WITH_KDB is off, and useful in tests regardless.
class ConsolePublisher : public TickPublisher {
public:
    void publish(const std::string& table, const NormalizedTick& tick) override;
};

#ifdef WITH_KDB
// Publishes into a kdb+ tickerplant via the official k.h C API, the same
// conceptual handshake as q's hopen/(neg h)(...) async send.
class KdbTickerplantPublisher : public TickPublisher {
public:
    KdbTickerplantPublisher(const std::string& host, int port, const std::string& usernamePassword);
    ~KdbTickerplantPublisher() override;

    KdbTickerplantPublisher(const KdbTickerplantPublisher&) = delete;
    KdbTickerplantPublisher& operator=(const KdbTickerplantPublisher&) = delete;

    void publish(const std::string& table, const NormalizedTick& tick) override;

private:
    int handle_;
};
#endif

// Factory: builds a KdbTickerplantPublisher when WITH_KDB is enabled and
// connection details are supplied, otherwise falls back to ConsolePublisher.
std::unique_ptr<TickPublisher> makeDefaultPublisher(const std::string& host, int port, const std::string& usernamePassword);
