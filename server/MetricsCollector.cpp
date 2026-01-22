#include "MetricsCollector.h"
#include <algorithm>

uint64_t MetricsCollector::now_ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();
}

void MetricsCollector::record_request(size_t bytes_sent) {
    const uint64_t now = now_ms();
    const size_t idx = now % WINDOW_MS;

    // Update ring buffer slot
    requests_per_ms_[idx].fetch_add(1, std::memory_order_relaxed);
    bytes_per_ms_[idx].fetch_add(bytes_sent, std::memory_order_relaxed);
    last_update_ms_[idx].store(now, std::memory_order_relaxed);

    // Update global totals
    total_requests_.fetch_add(1, std::memory_order_relaxed);
    total_bytes_.fetch_add(bytes_sent, std::memory_order_relaxed);
}

MetricsCollector::Metrics MetricsCollector::get_metrics() const {
    const uint64_t now = now_ms();
    uint64_t window_requests = 0;
    uint64_t window_bytes = 0;

    for (size_t i = 0; i < WINDOW_MS; ++i) {
        const uint64_t last_update = last_update_ms_[i].load(std::memory_order_relaxed);
        // Consider slot valid if updated in the last WINDOW_MS milliseconds
        if (now - last_update < WINDOW_MS) {
            window_requests += requests_per_ms_[i].load(std::memory_order_relaxed);
            window_bytes += bytes_per_ms_[i].load(std::memory_order_relaxed);
        }
    }

    const double rps = static_cast<double>(window_requests);
    const double mbps = static_cast<double>(window_bytes) / (1024.0 * 1024.0);

    return Metrics{
        .rps = rps,
        .mbps = mbps,
        .total_requests = total_requests_.load(std::memory_order_relaxed),
        .total_bytes = total_bytes_.load(std::memory_order_relaxed)
    };
} 