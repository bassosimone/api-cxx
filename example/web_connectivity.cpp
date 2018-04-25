// Part of Measurement Kit <https://measurement-kit.github.io/>.
// Measurement Kit is free software under the BSD license. See AUTHORS
// and LICENSE for more information on the copying conditions.

#include <measurement_kit/cxx/cxx.hpp>

#include <iostream>

int main() {
  mk::cxx::WebConnectivity()
    .add_input("https://www.google.com/")
    .add_input("https://www.x.org/")
    .set_verbosity(MK_CXX_LOG_INFO)
    .add_annotation("scope", "example")
    .on_status_update_websites([](mk::cxx::StatusUpdateWebsitesInfo info) {
      std::clog << info.url << ": " << info.status << std::endl;
    })
    .run();
}
