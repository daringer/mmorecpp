#pragma once 

#include <unistd.h>
#include <math.h>

#include <iostream>
#include <string>
#include <map>
#include <sstream>

#include "xstring.h"

namespace TOOLS {

typedef enum {
  SI_ATTO = -18,
  SI_FEMTO = -15,
  SI_PICO = -12,
  SI_NANO = -9,
  SI_MICRO = -6,
  SI_MILLI = -3,
  SI_NONE = 0,
  SI_KILO = 3,
  SI_MEGA = 6,
  SI_GIGA = 9,
  SI_TERA = 12,
  SI_PETA = 15,
  SI_EXA = 18,
  SI_ZETA = 21
} SI_SCALING_BASE10;

typedef enum {
/*  SI_ATTO = -60,
  SI_FEMTO = -50,
  SI_PICO = -40,
  SI_NANO = -30,
  SI_MICRO = -20,
  SI_MILLI = -10,*/
  SI2_NONE = 0,
  SI2_KILO = 10,
  SI2_MEGA = 20,
  SI2_GIGA = 30,
  SI2_TERA = 40,
  SI2_PETA = 50,
  SI2_EXA = 60,
  SI2_ZETA = 70
} SI_SCALING_BASE2;

class XUnit {
 public:
  const double data;
  double c_data;

  size_t decimal_precision;
  size_t base;
  int exponent;
  int e_step;
  bool force_scientific;
  std::string unit_suffix;

  static std::map<int, std::string> scale2unit;

  template <typename T>
  XUnit(const T& d, const std::string& unitname = "", const size_t& decimals = 2,
        const bool& e_notation = false, const int& has_exp = 0)
      : data(static_cast<double>(d)), c_data(static_cast<double>(d)),
        decimal_precision(decimals), base(10), exponent(has_exp), e_step(3),
        force_scientific(e_notation), unit_suffix(unitname), cache(""), ss("") {

    si_scale();
    format();
  }

  std::string format();
  std::string get(const bool& add_unit=true) const;
  std::string unit() const;

  std::string rpad(const size_t& pad_width, const std::string& padchar=" ");
  std::string lpad(const size_t& pad_width, const std::string& padchar=" ");

  private:
    std::string cache;
    std::stringstream ss;
    void si_scale();
};

std::ostream& operator<<(std::ostream& os, const XUnit& x);

}



