#pragma once

namespace tetris_clone {
class Das {
 public:
  Das(const int das_full_charge, const int das_min_charge)
      : das_full_charge_{das_full_charge}, das_min_charge_{das_min_charge} {}

  inline void fullyChargeDas(int& das_counter) const { das_counter = das_full_charge_; }
  inline void softResetDas(int& das_counter) const { das_counter = das_min_charge_; }
  inline void hardResetDas(int& das_counter) const { das_counter = 0; }
  inline bool dasFullyCharged(int& das_counter) const { return das_counter >= das_full_charge_; }

  inline int getFullDasChargeCount() const {return das_full_charge_;}
  inline int getMinDasChargeCount() const {return das_min_charge_;}

 private:
  int das_full_charge_;
  int das_min_charge_;
};
}  // namespace tetris_clone
