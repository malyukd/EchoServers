#pragma once
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>

class MetricsCollector {
public:
    static constexpr size_t WINDOW_MS = 1000; // 1 second in milliseconds

    void record_request(size_t bytes_sent);

    struct Metrics {
        double rps;               // requests per second (over last 1s)
        double mbps;              // megabytes per second (over last 1s)
        uint64_t total_requests;  // total since start
        uint64_t total_bytes;     // total bytes sent since start
    };

    Metrics get_metrics() const;

private:
    // Ring buffers for last 1000 ms
    std::array<std::atomic<uint64_t>, WINDOW_MS> requests_per_ms_{};
    std::array<std::atomic<uint64_t>, WINDOW_MS> bytes_per_ms_{};
    std::array<std::atomic<uint64_t>, WINDOW_MS> last_update_ms_{};

    std::atomic<uint64_t> total_requests_{0};
    std::atomic<uint64_t> total_bytes_{0};

    static uint64_t now_ms();
};