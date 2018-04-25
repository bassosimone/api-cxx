// Part of Measurement Kit <https://measurement-kit.github.io/>.
// Measurement Kit is free software under the BSD license. See AUTHORS
// and LICENSE for more information on the copying conditions.
#ifndef MEASUREMENT_KIT_CXX_CXX_HPP
#define MEASUREMENT_KIT_CXX_CXX_HPP

#include <measurement_kit/ffi.h>

#include <stdint.h>

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#ifdef MK_CXX_TRACE_EVENTS
#include <iostream>
#endif

#include <measurement_kit/cxx/json.hpp>

namespace mk {
namespace cxx {

#define MK_CXX_LOG_QUIET 0
#define MK_CXX_LOG_ERR 1
#define MK_CXX_LOG_INFO 2
#define MK_CXX_LOG_DEBUG 3
#define MK_CXX_LOG_DEBUG2 4

class FailureMeasurementInfo {
 public:
  std::string failure;
};
using FailureMeasurementCb = std::function<void(FailureMeasurementInfo)>;

class FailureMeasurementSubmissionInfo {
 public:
  int64_t idx = -1;
  std::string json_str;
  std::string failure;
};
using FailureMeasurementSubmissionCb =
    std::function<void(FailureMeasurementSubmissionInfo)>;

class FailureStartupInfo {
 public:
  std::string failure;
};
using FailureStartupCb = std::function<void(FailureStartupInfo)>;

class LogEntryInfo {
 public:
  std::string log_level;
  std::string message;
};
using LogEntryCb = std::function<void(LogEntryInfo)>;

class MeasurementInfo {
 public:
  int64_t idx = -1;
  std::string json_str;
};
using MeasurementCb = std::function<void(MeasurementInfo)>;

class StatusEndInfo {
 public:
  double downloaded_kb = 0.0;
  double uploaded_kb = 0.0;
  std::string failure;
};
using StatusEndCb = std::function<void(StatusEndInfo)>;

class StatusGeoipLookupInfo {
 public:
  std::string probe_ip;
  std::string probe_asn;
  std::string probe_cc;
};
using StatusGeoipLookupCb = std::function<void(StatusGeoipLookupInfo)>;

class StatusProgressInfo {
 public:
  double percentage = 0.0;
  std::string message;
};
using StatusProgressCb = std::function<void(StatusProgressInfo)>;

using StatusQueuedCb = std::function<void()>;

class StatusMeasurementStartedInfo {
 public:
  int64_t idx = -1;
  std::string input;
};
using StatusMeasurementStartedCb =
    std::function<void(StatusMeasurementStartedInfo)>;

class StatusMeasurementUploadedInfo {
 public:
  int64_t idx = -1;
};
using StatusMeasurementUploadedCb =
    std::function<void(StatusMeasurementUploadedInfo)>;

class StatusMeasurementDoneInfo {
 public:
  int64_t idx = -1;
};
using StatusMeasurementDoneCb = std::function<void(StatusMeasurementDoneInfo)>;

class StatusReportCreatedInfo {
 public:
  std::string report_id;
};
using StatusReportCreatedCb = std::function<void(StatusReportCreatedInfo)>;

using StatusStartedCb = std::function<void()>;

class StatusUpdatePerformanceInfo {
 public:
  std::string direction;
  double elapsed = 0.0;
  int64_t num_streams = -1;
  double speed_kbps = 0.0;
};
using StatusUpdatePerformanceCb =
    std::function<void(StatusUpdatePerformanceInfo)>;

class StatusUpdateWebsitesInfo {
 public:
  std::string url;
  std::string status;
};
using StatusUpdateWebsitesCb = std::function<void(StatusUpdateWebsitesInfo)>;

using UnhandledEventCb = std::function<void(std::string)>;

class TaskDeleter {
 public:
  void operator()(mk_task_t *task) noexcept {
    mk_task_destroy(task);  // handles nullptr
  }
};

class EventDeleter {
 public:
  void operator()(mk_event_t *event) noexcept {
    mk_event_destroy(event);  // handles nullptr
  }
};

class Inputless {
 public:
  Inputless &set_output_filepath(const std::string &s) {
    settings_["output_filepath"] = s;
    return *this;
  }

  Inputless &set_log_filepath(const std::string &s) {
    settings_["log_filepath"] = s;
    return *this;
  }

  Inputless &set_verbosity(uint64_t v) {
    v = (v < MK_CXX_LOG_QUIET) ? MK_CXX_LOG_QUIET : v;
    v = (v > MK_CXX_LOG_DEBUG2) ? MK_CXX_LOG_DEBUG2 : v;
    verbosity_ = v;
    return *this;
  }

  Inputless &decrease_verbosity() {
    verbosity_ -= (verbosity_ > MK_CXX_LOG_QUIET) ? 1 : 0;
    return *this;
  }

  Inputless &increase_verbosity() {
    verbosity_ += (verbosity_ < MK_CXX_LOG_DEBUG2) ? 1 : 0;
    return *this;
  }

  // TODO(bassosimone): add methods for setting commonly used options.

  Inputless &set_option(const std::string &k, const std::string &v) {
    settings_["options"][k] = v;
    return *this;
  }

  Inputless &set_option(const std::string &k, int64_t v) {
    settings_["options"][k] = v;
    return *this;
  }

  Inputless &set_option(const std::string &k, double v) {
    settings_["options"][k] = v;
    return *this;
  }

  Inputless &add_annotation(const std::string &k, const std::string &v) {
    settings_["annotations"][k] = v;
    return *this;
  }

  Inputless &add_annotation(const std::string &k, int64_t v) {
    settings_["annotations"][k] = v;
    return *this;
  }

  Inputless &add_annotation(const std::string &k, double v) {
    settings_["annotations"][k] = v;
    return *this;
  }

  Inputless &on_failure_measurement(FailureMeasurementCb cb) {
    failure_measurement_cbs_.push_back(std::move(cb));
    return *this;
  }

  Inputless &on_failure_measurement_submission(
      FailureMeasurementSubmissionCb cb) {
    failure_measurement_submission_cbs_.push_back(std::move(cb));
    return *this;
  }

  Inputless &on_failure_startup(FailureStartupCb cb) {
    failure_startup_cbs_.push_back(std::move(cb));
    return *this;
  }

  Inputless &on_log(LogEntryCb cb) {
    log_cbs_.push_back(std::move(cb));
    return *this;
  }

  Inputless &on_measurement(MeasurementCb cb) {
    measurement_cbs_.push_back(std::move(cb));
    return *this;
  }

  Inputless &on_status_end(StatusEndCb cb) {
    status_end_cbs_.push_back(std::move(cb));
    return *this;
  }

  Inputless &on_status_geoip_lookup(StatusGeoipLookupCb cb) {
    status_geoip_lookup_cbs_.push_back(std::move(cb));
    return *this;
  }

  Inputless &on_status_progress(StatusProgressCb cb) {
    status_progress_cbs_.push_back(std::move(cb));
    return *this;
  }

  Inputless &on_status_queued(StatusQueuedCb cb) {
    status_queued_cbs_.push_back(std::move(cb));
    return *this;
  }

  Inputless &on_status_measurement_started(StatusMeasurementStartedCb cb) {
    status_measurement_started_cbs_.push_back(std::move(cb));
    return *this;
  }

  Inputless &on_status_measurement_uploaded(
      StatusMeasurementUploadedCb cb) {
    status_measurement_uploaded_cbs_.push_back(std::move(cb));
    return *this;
  }

  Inputless &on_status_measurement_done(StatusMeasurementDoneCb cb) {
    status_measurement_done_cbs_.push_back(std::move(cb));
    return *this;
  }

  Inputless &on_status_report_created(StatusReportCreatedCb cb) {
    status_report_created_cbs_.push_back(std::move(cb));
    return *this;
  }

  Inputless &on_status_started(StatusStartedCb cb) {
    status_started_cbs_.push_back(std::move(cb));
    return *this;
  }

  Inputless &on_status_update_performance(StatusUpdatePerformanceCb cb) {
    status_update_performance_cbs_.push_back(std::move(cb));
    return *this;
  }

  Inputless &on_status_update_websites(StatusUpdateWebsitesCb cb) {
    status_update_websites_cbs_.push_back(std::move(cb));
    return *this;
  }

  Inputless &on_unhandled_event(UnhandledEventCb cb) {
    unhandled_event_cbs_.push_back(std::move(cb));
    return *this;
  }

  void run() {
    switch (verbosity_) {
      case MK_CXX_LOG_ERR:
        settings_["log_level"] = "ERR";
        break;
      case MK_CXX_LOG_INFO:
        settings_["log_level"] = "INFO";
        break;
      case MK_CXX_LOG_DEBUG:
        settings_["log_level"] = "DEBUG";
        break;
      case MK_CXX_LOG_DEBUG2:
        settings_["log_level"] = "DEBUG2";
        break;
      default:
        // NOTHING
        break;
    }
    if (!!taskup_) {
      throw std::runtime_error("cannot run more than one test");
    }
    taskup_.reset(mk_task_start(settings_.dump().c_str()));
    while (!mk_task_is_done(taskup_.get())) {
      nlohmann::json ev;
      {
        std::unique_ptr<mk_event_t, EventDeleter> eventup;
        eventup.reset(mk_task_wait_for_next_event(taskup_.get()));
        if (!eventup) {
          throw std::runtime_error("got nullptr next event");
        }
        const char *s = mk_event_serialize(eventup.get());
        assert(s != nullptr);
#ifdef MK_CXX_TRACE_EVENTS
        std::clog << "mk::cxx: Got event: " << s << std::endl;
#endif
        ev = nlohmann::json::parse(s);
      }
      std::string key = ev.at("key");
      if (key == "failure.measurement") {
        FailureMeasurementInfo info;
        info.failure = ev.at("value").at("failure");
        for (auto &cb : failure_measurement_cbs_) {
          cb(info);
        }
      } else if (key == "failure.measurement_submission") {
        FailureMeasurementSubmissionInfo info;
        info.idx = ev.at("value").at("idx");
        info.json_str = ev.at("value").at("json_str");
        info.failure = ev.at("value").at("failure");
        for (auto &cb : failure_measurement_submission_cbs_) {
          cb(info);
        }
      } else if (key == "failure.startup") {
        FailureStartupInfo info;
        info.failure = ev.at("value").at("failure");
        for (auto &cb : failure_startup_cbs_) {
          cb(info);
        }
      } else if (key == "log") {
        LogEntryInfo info;
        info.log_level = ev.at("value").at("log_level");
        info.message = ev.at("value").at("message");
        for (auto &cb : log_cbs_) {
          cb(info);
        }
      } else if (key == "measurement") {
        MeasurementInfo info;
        info.idx = ev.at("value").at("idx");
        info.json_str = ev.at("value").at("json_str");
        for (auto &cb : measurement_cbs_) {
          cb(info);
        }
      } else if (key == "status.end") {
        StatusEndInfo info;
        info.downloaded_kb = ev.at("value").at("downloaded_kb");
        info.uploaded_kb = ev.at("value").at("uploaded_kb");
        info.failure = ev.at("value").at("failure");
        for (auto &cb : status_end_cbs_) {
          cb(info);
        }
      } else if (key == "status.geoip_lookup") {
        StatusGeoipLookupInfo info;
        info.probe_ip = ev.at("value").at("probe_ip");
        info.probe_asn = ev.at("value").at("probe_asn");
        info.probe_cc = ev.at("value").at("probe_cc");
        for (auto &cb : status_geoip_lookup_cbs_) {
          cb(info);
        }
      } else if (key == "status.progress") {
        StatusProgressInfo info;
        info.percentage = ev.at("value").at("percentage");
        info.message = ev.at("value").at("message");
        for (auto &cb : status_progress_cbs_) {
          cb(info);
        }
      } else if (key == "status.queued") {
        for (auto &cb : status_queued_cbs_) {
          cb();
        }
      } else if (key == "status.measurement_started") {
        StatusMeasurementStartedInfo info;
        info.idx = ev.at("value").at("idx");
        info.input = ev.at("value").at("input");
        for (auto &cb : status_measurement_started_cbs_) {
          cb(info);
        }
      } else if (key == "status.measurement_uploaded") {
        StatusMeasurementUploadedInfo info;
        info.idx = ev.at("value").at("idx");
        for (auto &cb : status_measurement_uploaded_cbs_) {
          cb(info);
        }
      } else if (key == "status.measurement_done") {
        StatusMeasurementDoneInfo info;
        info.idx = ev.at("value").at("idx");
        for (auto &cb : status_measurement_done_cbs_) {
          cb(info);
        }
      } else if (key == "status.report_created") {
        StatusReportCreatedInfo info;
        info.report_id = ev.at("value").at("report_id");
        for (auto &cb : status_report_created_cbs_) {
          cb(info);
        }
      } else if (key == "status.started") {
        for (auto &cb : status_started_cbs_) {
          cb();
        }
      } else if (key == "status.update.performance") {
        StatusUpdatePerformanceInfo info;
        info.direction = ev.at("value").at("direction");
        info.elapsed = ev.at("value").at("elapsed");
        info.num_streams = ev.at("value").at("num_streams");
        info.speed_kbps = ev.at("value").at("speed_kbps");
        for (auto &cb : status_update_performance_cbs_) {
          cb(info);
        }
      } else if (key == "status.update.websites") {
        StatusUpdateWebsitesInfo info;
        info.url = ev.at("value").at("url");
        info.status = ev.at("value").at("status");
        for (auto &cb : status_update_websites_cbs_) {
          cb(info);
        }
      } else if (key == "task_terminated") {
        // NOTHING on purpose
      } else {
        auto s = ev.dump();
        for (auto &cb : unhandled_event_cbs_) {
          cb(s);
        }
      }
    }
  }

 protected:
  nlohmann::json settings_;

 private:
  uint64_t verbosity_{};
  std::vector<FailureMeasurementCb> failure_measurement_cbs_;
  std::vector<FailureMeasurementSubmissionCb>
      failure_measurement_submission_cbs_;
  std::vector<FailureStartupCb> failure_startup_cbs_;
  std::vector<LogEntryCb> log_cbs_;
  std::vector<MeasurementCb> measurement_cbs_;
  std::vector<StatusEndCb> status_end_cbs_;
  std::vector<StatusGeoipLookupCb> status_geoip_lookup_cbs_;
  std::vector<StatusProgressCb> status_progress_cbs_;
  std::vector<StatusQueuedCb> status_queued_cbs_;
  std::vector<StatusMeasurementStartedCb> status_measurement_started_cbs_;
  std::vector<StatusMeasurementUploadedCb> status_measurement_uploaded_cbs_;
  std::vector<StatusMeasurementDoneCb> status_measurement_done_cbs_;
  std::vector<StatusReportCreatedCb> status_report_created_cbs_;
  std::vector<StatusStartedCb> status_started_cbs_;
  std::vector<StatusUpdatePerformanceCb> status_update_performance_cbs_;
  std::vector<StatusUpdateWebsitesCb> status_update_websites_cbs_;
  std::vector<UnhandledEventCb> unhandled_event_cbs_;
  std::unique_ptr<mk_task_t, TaskDeleter> taskup_;
};

class WithInput : public Inputless {
 public:
  WithInput &add_input(std::string s) {
    settings_["inputs"].push_back(std::move(s));
    return *this;
  }

  WithInput &add_input_filepath(std::string s) {
    settings_["input_filepaths"].push_back(std::move(s));
    return *this;
  }
};

class Dash : public Inputless {
 public:
  Dash() : Inputless() { settings_["name"] = "Dash"; }
};

class CaptivePortal : public Inputless {
 public:
  CaptivePortal() : Inputless() { settings_["name"] = "CaptivePortal"; }
};

class DnsInjection : public WithInput {
 public:
  DnsInjection() : WithInput() { settings_["name"] = "DnsInjection"; }
};

class FacebookMessanger : public Inputless {
 public:
  FacebookMessanger() : Inputless() {
    settings_["name"] = "FacebookMessanger";
  }
};

class HttpHeaderFieldManipulation : public Inputless {
 public:
  HttpHeaderFieldManipulation() : Inputless() {
    settings_["name"] = "HttpHeaderFieldManipulation";
  }
};

class HttpInvalidRequestLine : public Inputless {
 public:
  HttpInvalidRequestLine() : Inputless() {
    settings_["name"] = "HttpInvalidRequestLine";
  }
};

class MeekFrontedRequests : public WithInput {
 public:
  MeekFrontedRequests() : WithInput() {
    settings_["name"] = "MeekFrontedRequests";
  }
};

class MultiNdt : public Inputless {
 public:
  MultiNdt() : Inputless() { settings_["name"] = "MultiNdt"; }
};

class Ndt : public Inputless {
 public:
  Ndt() : Inputless() { settings_["name"] = "Ndt"; }
};

class TcpConnect : public WithInput {
 public:
  TcpConnect() : WithInput() { settings_["name"] = "TcpConnect"; }
};

class Telegram : public Inputless {
 public:
  Telegram() : Inputless() { settings_["name"] = "Telegram"; }
};

class WebConnectivity : public WithInput {
 public:
  WebConnectivity() : WithInput() {
    settings_["name"] = "WebConnectivity";
  }
};

class Whatsapp : public Inputless {
 public:
  Whatsapp() : Inputless() { settings_["name"] = "Whatsapp"; }
};

}  // namespace cxx
}  // namespace mk
#endif
