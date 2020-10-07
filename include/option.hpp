#pragma once

#include <string>
#include <vector>

namespace tetris_clone {

class OptionInterface {
 public:
  OptionInterface(const std::string& display_name) : display_name_{display_name} {}
  virtual ~OptionInterface() {}
  virtual bool nextOptionAvailable() const = 0;
  virtual bool prevOptionAvailable() const = 0;
  virtual void selectNextOption() = 0;
  virtual void selectPrevOption() = 0;
  virtual std::string getSelectedOptionText() const = 0;
  std::string getDisplayName() { return display_name_; }

 private:
  std::string display_name_;
};

class BoolOption : public OptionInterface {
 public:
  BoolOption(const std::string& display_name, const bool default_value)
      : OptionInterface(display_name), option_value_(default_value) {}
  bool nextOptionAvailable() const override { return not option_value_; }
  bool prevOptionAvailable() const override { return option_value_; }
  void selectNextOption() override { option_value_ = true; }
  void selectPrevOption() override { option_value_ = false; }
  std::string getSelectedOptionText() const override { return option_value_ ? "ON" : "OFF"; }
  bool getSelectedOption() const { return option_value_; }

 private:
  bool option_value_;
};
inline bool getBoolOption(const OptionInterface& option) {
    return dynamic_cast<const BoolOption&>(option).getSelectedOption();
}

class StringOption : public OptionInterface {
 public:
  StringOption(const std::string& display_name, const std::vector<std::string>& options,
               const int default_idx = 0)
      : OptionInterface(display_name), options_{options}, selected_option_idx_(default_idx) {}
  bool nextOptionAvailable() const override { return selected_option_idx_ < options_.size() - 1; }
  bool prevOptionAvailable() const override { return selected_option_idx_ > 0; }
  void selectNextOption() override {
    if (nextOptionAvailable()) {
      ++selected_option_idx_;
    }
  }
  void selectPrevOption() override {
    if (prevOptionAvailable()) {
      --selected_option_idx_;
    }
  }
  std::string getSelectedOptionText() const override { return options_.at(selected_option_idx_); }
  std::string getSelectedOption() const { return getSelectedOptionText(); };

 private:
  int selected_option_idx_;
  std::vector<std::string> options_;
};

class IntOption : public OptionInterface {
 public:
  IntOption(const std::string& display_name, const int min_val, const int max_val,
            const int default_val)
      : OptionInterface(display_name),
        min_value_(min_val),
        max_value_(max_val),
        value_(default_val) {}

  bool nextOptionAvailable() const override { return value_ < max_value_; }
  bool prevOptionAvailable() const override { return value_ > min_value_; }
  void selectNextOption() override {
    if (nextOptionAvailable()) {
      ++value_;
    }
  }
  void selectPrevOption() override {
    if (prevOptionAvailable()) {
      --value_;
    }
  }
  std::string getSelectedOptionText() const override { return std::to_string(value_); }
  int getSelectedOption() const { return value_; }

 private:
  int min_value_;
  int max_value_;
  int value_;
};

inline int getIntOption(const OptionInterface& option) {
    return dynamic_cast<const IntOption&>(option).getSelectedOption();
}
}  // namespace tetris_clone
