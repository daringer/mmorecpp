#include <iostream>
#include <string>
#include <algorithm>

#include <xunit.h>

using namespace std;
using namespace MM_NAMESPACE();

std::map<int, std::string> 
XUnit::scale2unit = std::map<int, std::string>
    {{SI_ATTO,  "a"}, {SI_PICO, "p"}, {SI_NANO, "n"}, {SI_MICRO, "u"},
     {SI_MILLI, "m"}, {SI_NONE, " "}, {SI_KILO, "k"}, {SI_MEGA, "M"},
     {SI_GIGA, "G"}, {SI_TERA, "T"}, {SI_PETA, "P"}, {SI_EXA, "E"},
     {SI_ZETA, "Z"}};

void XUnit::si_scale() {
  const double upper = 1000.0;
  const double lower = 1.0;
  const int e_add = 3;

  // catch zeros, corner-cases, life insurance follows (in-while)
  if (c_data == 0 || c_data == 0.0 || c_data == 0.0f)
    return;

  double last_c_data = c_data;
  if (c_data > upper) {
    while (c_data > upper) {
      c_data /= upper;
      exponent += e_add;
    }
  } else if (c_data < lower) {
    while (c_data < lower) {
      c_data *= upper;
      exponent -= e_add;
    }
  }
}

std::string XUnit::get(const bool& add_unit) const {
  if (force_scientific)
    return cache + unit_suffix;
  else
    return cache + ((add_unit) ? unit() : "");
}

std::string XUnit::unit() const {
  if(scale2unit.find(exponent) == scale2unit.end())
    return "?" + unit_suffix;
  return scale2unit.at(exponent) + unit_suffix;
}

std::string XUnit::lpad(const size_t& pad_width, const std::string& padchar) {
  std::string out = get();
  return ((out.length() >= pad_width) ? "" : 
        std::string(pad_width - out.length(), padchar.c_str()[0])) + out;
}

std::string XUnit::rpad(const size_t& pad_width, const std::string& padchar) {
  std::string out = get();
  return out + ((out.length() >= pad_width) ? "" : 
        std::string(pad_width - out.length(), padchar.c_str()[0]));
}

std::string XUnit::format() {
  ss.str("");
  ss.clear();
  
  // keep original format to reset later
  std::ios_base::fmtflags ff = ss.flags();

  if(force_scientific)
    ss.setf(ss.scientific);
  else
    ss.setf(ss.fixed);
  ss.precision(decimal_precision);
  ss << ((force_scientific) ? data : c_data);
  cache = ss.str();

  // set back to default format
  ss.setf(ff);
  return cache;
}

std::ostream& MM_NAMESPACE()::operator<<(std::ostream& os, \
    const MM_NAMESPACE()::XUnit& x) {
  return (os << x.get());
}


