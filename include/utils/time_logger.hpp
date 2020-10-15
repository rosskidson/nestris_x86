#pragma once

#include <chrono>
#include <map>
#include <string>

#include "logging.hpp"

namespace time_logger{
using Clock = std::chrono::high_resolution_clock;
using Duration_ns = std::chrono::duration<int, std::nano>;

class TimeLogger {
 public:
  TimeLogger() : last_checkpoint_{Clock::now()} {}

  void logEvent(const std::string& name) {
    const auto now = Clock::now();
    const auto now_us =
        std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
    const auto last_checkpoint_us =
        std::chrono::duration_cast<std::chrono::microseconds>(last_checkpoint_.time_since_epoch());
    logged_events_[name] = (now_us.count() - last_checkpoint_us.count());
    last_checkpoint_ = now;
  }

  void printLoggedEvents() const {
    for (const auto& [name, time] : logged_events_) {
      LOG_INFO("    " << name << ": " << time);
    }
  }

 private:
  std::map<std::string, int> logged_events_;
  std::chrono::time_point<std::chrono::high_resolution_clock> last_checkpoint_;
};

}
