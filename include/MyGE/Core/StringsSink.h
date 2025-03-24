#include <spdlog/sinks/base_sink.h>

#include <deque>
#include <mutex>
#include <string>

namespace My::MyGE {
class StringsSink : public spdlog::sinks::base_sink<std::mutex> {
 public:
  // not thread safe
  const std::deque<std::string>& GetLogs() const noexcept { return logs; }

  // thread safe
  std::vector<std::string> CopyLogs() const noexcept;

  // thread safe
  void Clear();

 protected:
  virtual void sink_it_(const spdlog::details::log_msg& msg) override;
  virtual void flush_() override {}

 private:
  std::deque<std::string> logs;
};
}  // namespace My::MyGE
