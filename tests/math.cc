#include <stdlib.h>
#include <unistd.h>

#include <iostream>

#include "tools.h"

#include "../src/math/basic.h"
#include "../src/math/moving_ana.h"

using namespace TOOLS;
using namespace TOOLS::UNIT_TEST;
using namespace TOOLS::MATH;
using namespace std;

START_SUITE(MathToolsTestSuite) {
  REG_TEST(empty_container)
  REG_TEST(mean_simple)
  REG_TEST(mean_types)
  REG_TEST(median_simple)
  REG_TEST(variance_simple)
  REG_TEST(sigma_simple)
  REG_TEST(custom_val_type_class)
  REG_TEST(moving_ana_simple)
}

float precision;

/** example of the usage of a user-defined numeric-type which does only need 
 *  the defined interface members in order to be used by the math functions */
class CTest {
  public:
   // internal storage
   float myval;

   // copy ctor
   CTest(const CTest& obj) : myval(obj.myval) {}
   // ctor from primitive
   CTest(const float& x) : myval(x) {}
   
   // always needed elementary math ops
   CTest operator/(const CTest& x) { return CTest(myval / x.myval); }
   CTest operator+(const CTest& x) { return CTest(myval + x.myval); }
   CTest operator-(const CTest& x) { return CTest(myval - x.myval); }
   CTest operator*(const CTest& x) { return CTest(myval * x.myval); } 
   
   // for mean/variance/sigma
   CTest& operator+=(const CTest& x) { myval += x.myval; return *this; }
   CTest operator++() { myval += 1; return *this; }
   
   // for sorting in median!
   bool operator>(const CTest& x) { return (myval > x.myval); }
   bool operator<(const CTest& x) { return (myval < x.myval); }
   
   // equal checking
   bool operator==(const CTest& x) { return (myval == x.myval); }
   
   // primitive type representation
   operator float() { return myval; }
};


void setup() {
  precision = 0.001f;
}

void tear_down() {}

MAKE_TEST(empty_container) {
  std::vector<float> t0;
  CHECK_EXC(EmptyDataContainerError, mean(t0));
  CHECK_EXC(EmptyDataContainerError, median(t0));
  CHECK_EXC(EmptyDataContainerError, variance(t0));
  CHECK_EXC(EmptyDataContainerError, sigma(t0));
}

MAKE_TEST(mean_simple) {

  std::vector<float> t1 {1.00f, 2.00f, 3.00f};
  float out = mean(t1);
  CHECK_APPROX(out, 2.00000f, precision);
  
  std::vector<float> t2 {1.50f, 22.00f, 3432.00f};
  out = mean(t2);
  CHECK_APPROX(out, 1151.83333f, precision);

  std::vector<float> t3 {1.50f, 22.00f, 3432.00f, 213.423f, 23423.3423f, 0.0f};
  out = mean(t3);
  CHECK_APPROX(out, 4515.37755f, precision);
}

MAKE_TEST(mean_types) {
  std::vector<int> t1 {213, 312, 322};
  int out = mean(t1);
  CHECK_EQ(out, 282);
}

MAKE_TEST(median_simple) {
  std::vector<int> t1 {123, 312, 1, 2, 3, 4, 5};
  int out = median(t1);
  CHECK_EQ(out, 4);
}

MAKE_TEST(variance_simple) {
  std::vector<float> t1 {123, 312, 1, 2, 3, 4, 5};
  float out = variance(t1);
  CHECK_APPROX(out, 83599.428571428f, precision);
}

MAKE_TEST(sigma_simple) {
  std::vector<float> t1 {123, 312, 1, 2, 3, 4, 5};
  float out = sigma(t1);
  CHECK_APPROX(out, 289.1356577308108f, precision);
}

MAKE_TEST(custom_val_type_class) {
  std::vector<CTest> t1 {123.f, 312.f, 1.f, 2.f, 3.f, 4.f, 5.f};
  CTest out = mean(t1);
  CHECK_APPROX(out.myval, 64.28571428571429f, precision);
  out = median(t1);
  CHECK_APPROX(out.myval, 4.000f, precision);
  out = variance(t1);
  CHECK_APPROX(out.myval, 83599.42857142858f, precision);
  out = sigma(t1);
  CHECK_APPROX(out.myval, 289.1356577308108f, precision);
}

MAKE_TEST(moving_ana_simple) {
  std::vector<float> t1 {123.f, 312.f, 1.f, 2.f, 3.f, 4.f, 5.f};
  MovingAverage<float> foo1(3);
  MovingMedian<float> foo2(3);

  CHECK_EXC(AnalysisWindowIllegalError, foo2.set_window_size(0));
  CHECK_EXC(AnalysisWindowIllegalError, foo1.set_window_size(-5));

  CHECK_EXC(NoValueToInterpolateError, foo1.get_next_interpolated());
  CHECK_EXC(NoValueToInterpolateError, foo2.get_next_interpolated());

  std::vector<float> res1 {123.0f, 217.5f, 145.33333333f, \
                          105.0f, 2.0f, 3.0f, 4.0f};
  std::vector<float> res2 {123.0f, 312.0f, 123.0f,        \
                          2.0f, 2.0f, 3.0f, 4.0f};

  for (uint i=0; i<t1.size(); ++i) {
    CHECK_APPROX(foo1.get_next(t1.at(i)), res1.at(i), precision);
    CHECK_APPROX(foo2.get_next(t1.at(i)), res2.at(i), precision);
  }
}



END_SUITE()
