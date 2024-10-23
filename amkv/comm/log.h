#pragma once

#include <sys/stat.h>

#include <iomanip>
#include <iostream>

#include "fmt/compile.h"
#include "fmt/core.h"
#include "glog/export.h"
#include "glog/logging.h"

namespace amkv::comm {

#define FMT_FORMAT(format_str, ...) fmt::format(FMT_COMPILE(format_str), ##__VA_ARGS__)

#define LOG_FORMAT(_fmt_, args...)                                                                   \
  ({                                                                                                 \
    std::string prefix = FMT_FORMAT("{}:{}:{}", strrchr(__FILE__, '/') + 1, __LINE__, __FUNCTION__); \
    std::string text = FMT_FORMAT(_fmt_, ##args);                                                    \
    std::string log = FMT_FORMAT("{} {}", prefix, text);                                             \
    log;                                                                                             \
  });

#define TRACE(_fmt_, args...)                           \
  do {                                                  \
    LOG(INFO) << "TRACE " << LOG_FORMAT(_fmt_, ##args); \
  } while (0);

#define DEBUG(_fmt_, args...)                            \
  do {                                                   \
    DLOG(INFO) << "DEBUG " << LOG_FORMAT(_fmt_, ##args); \
  } while (0);

#define INFO(_fmt_, args...)                           \
  do {                                                 \
    LOG(INFO) << "INFO " << LOG_FORMAT(_fmt_, ##args); \
  } while (0);

#define WARN(_fmt_, args...)                              \
  do {                                                    \
    LOG(WARNING) << "WARN " << LOG_FORMAT(_fmt_, ##args); \
  } while (0);

#define ERROR(_fmt_, args...)                            \
  do {                                                   \
    LOG(ERROR) << "ERROR " << LOG_FORMAT(_fmt_, ##args); \
  } while (0);

#define FATAL(_fmt_, args...)                            \
  do {                                                   \
    LOG(FATAL) << "FATAL " << LOG_FORMAT(_fmt_, ##args); \
  } while (0);

inline void CustomPrefix(std::ostream& s, const google::LogMessageInfo& l, void*) {
  s << std::setw(4) << 1900 + l.time.year();
  s << '-';
  s << std::setw(2) << 1 + l.time.month();
  s << '-';
  s << std::setw(2) << l.time.day();
  s << ' ';
  s << std::setw(2) << l.time.hour() << ':';
  s << std::setw(2) << l.time.min() << ':';
  s << std::setw(2) << l.time.sec() << ",";
  s << std::setw(6) << l.time.usec();
  s << ' ';
  s << l.thread_id;
  // s << "[" << l.filename << ':' << l.line_number << "]";
};

inline int init_log(char* bin_name) {
  google::InitGoogleLogging(bin_name, &CustomPrefix);
  FLAGS_stop_logging_if_full_disk = true;
  FLAGS_logbufsecs = 0;
  FLAGS_logtostderr = false;
  FLAGS_alsologtostderr = true;
  FLAGS_log_dir = "log";
  FLAGS_timestamp_in_logfile_name = false;

  if ((access(FLAGS_log_dir.c_str(), 0)) == -1) {
    mkdir(FLAGS_log_dir.c_str(), S_IRWXU);
  }

  google::SetLogFilenameExtension("log");
  google::SetLogDestination(google::GLOG_ERROR, "log/task_error.");

  return 0;
}
}  // namespace amdb