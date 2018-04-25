// Part of Measurement Kit <https://measurement-kit.github.io/>.
// Measurement Kit is free software under the BSD license. See AUTHORS
// and LICENSE for more information on the copying conditions.

#include <measurement_kit/cxx/cxx.hpp>

#include <iostream>

int main() {
  mk::cxx::Ndt()
      .set_verbosity(MK_CXX_LOG_INFO)
      .add_annotation("scope", "example")
      .on_status_update_performance(
          [](mk::cxx::StatusUpdatePerformanceInfo info) {
            std::clog << info.direction << ": after " << info.elapsed
                      << " s speed " << info.speed_kbps << " kbit/s with "
                      << info.num_streams << " flows" << std::endl;
          })
      .run();
}
