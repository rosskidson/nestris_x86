#pragma once

namespace nestris_x86 {

class Das {
 public:
  constexpr static int NTSC_FULL_CHARGE = 16;
  constexpr static int NTSC_MIN_CHARGE = 10;
  constexpr static int PAL_FULL_CHARGE = 12;
  constexpr static int PAL_MIN_CHARGE = 8;

  Das(const int das_full_charge, const int das_min_charge)
      : das_full_charge_{das_full_charge}, das_min_charge_{das_min_charge} {}

  inline void fullyChargeDas(int& das_counter) const { das_counter = das_full_charge_; }
  inline void softResetDas(int& das_counter) const { das_counter = das_min_charge_; }
  inline void hardResetDas(int& das_counter) const { das_counter = 0; }
  inline bool dasFullyCharged(const int das_counter) const {
    return das_counter >= das_full_charge_;
  }
  inline bool dasSoftlyCharged(const int das_counter) const {
    return das_counter >= das_min_charge_;
  }

  inline int getFullDasChargeCount() const { return das_full_charge_; }
  inline int getMinDasChargeCount() const { return das_min_charge_; }

 private:
  int das_full_charge_;
  int das_min_charge_;
};
}  // namespace nestris_x86
