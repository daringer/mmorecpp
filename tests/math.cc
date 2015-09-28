#include <stdlib.h>
#include <unistd.h>

#include <iostream>

#include "tools.h"

using namespace TOOLS;
using namespace TOOLS::UNIT_TEST;
using namespace TOOLS::MATH;
using namespace std;


/* Someday, someone has to decide where they belong...
 * Boost keeps them pre-configured inside their code-base, mmmh */
typedef tStreamingKMeans<> StreamingKMeans;
typedef tVector3D<> Vector3D;
typedef tPose3D<> Pose3D; 
typedef tPoseMatrix<> PoseMatrix;


START_SUITE(MathToolsTestSuite) {
  REG_TEST(empty_container)
  REG_TEST(mean_simple)
  REG_TEST(mean_types)
  REG_TEST(median_simple)
  REG_TEST(variance_simple)
  REG_TEST(sigma_simple)
  REG_TEST(custom_val_type_class)
  REG_TEST(moving_ana_simple)
  REG_TEST(vector_dot)
  REG_TEST(vector_bootstrap)
  REG_TEST(vector_cross)
  REG_TEST(vector_scalar)
  REG_TEST(kmeans_bootstrap)
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
  CTest& operator+=(const CTest& x) {
    myval += x.myval;
    return *this;
  }
  CTest operator++() {
    myval += 1;
    return *this;
  }

  // for sorting in median!
  bool operator>(const CTest& x) { return (myval > x.myval); }
  bool operator<(const CTest& x) { return (myval < x.myval); }

  // equal checking
  bool operator==(const CTest& x) { return (myval == x.myval); }

  // primitive type representation
  operator float() { return myval; }
};

void setup() { precision = 0.00001f; }

void tear_down() {}

MAKE_TEST(empty_container) {
  std::vector<float> t0;
  CHECK_EXC(EmptyDataContainerError, mean(t0));
  CHECK_EXC(EmptyDataContainerError, median(t0));
  CHECK_EXC(EmptyDataContainerError, variance(t0));
  CHECK_EXC(EmptyDataContainerError, sigma(t0));
}

MAKE_TEST(mean_simple) {

  std::vector<float> t1{1.00f, 2.00f, 3.00f};
  float out = mean(t1);
  CHECK_APPROX(out, 2.00000f, precision);

  std::vector<float> t2{1.50f, 22.00f, 3432.00f};
  out = mean(t2);
  CHECK_APPROX(out, 1151.83333f, precision);

  std::vector<float> t3{1.50f, 22.00f, 3432.00f, 213.423f, 23423.3423f, 0.0f};
  out = mean(t3);
  CHECK_APPROX(out, 4515.37755f, precision);
}

MAKE_TEST(mean_types) {
  std::vector<int> t1{213, 312, 322};
  int out = mean(t1);
  CHECK_EQ(out, 282);
}

MAKE_TEST(median_simple) {
  std::vector<int> t1{123, 312, 1, 2, 3, 4, 5};
  int out = median(t1);
  CHECK_EQ(out, 4);
}

MAKE_TEST(variance_simple) {
  std::vector<float> t1{123, 312, 1, 2, 3, 4, 5};
  float out = variance(t1);
  CHECK_APPROX(out, 83599.428571428f, precision);
}

MAKE_TEST(sigma_simple) {
  std::vector<float> t1{123, 312, 1, 2, 3, 4, 5};
  float out = sigma(t1);
  CHECK_APPROX(out, 289.1356577308108f, precision);
}

MAKE_TEST(custom_val_type_class) {
  std::vector<CTest> t1{123.f, 312.f, 1.f, 2.f, 3.f, 4.f, 5.f};
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
  std::vector<float> t1{123.f, 312.f, 1.f, 2.f, 3.f, 4.f, 5.f};
  MovingAverage<float> foo1(3);
  MovingMedian<float> foo2(3);
  ExponentialMovingAverage<float> foo3;

  CHECK_EXC(AnalysisWindowIllegalError, foo2.set_window_size(0));
  CHECK_EXC(AnalysisWindowIllegalError, foo1.set_window_size(-5));
  CHECK_EXC(NoWindowSizeApplicableError, foo3.set_window_size(-5));

  CHECK_EXC(NoValueToInterpolateError, foo1.get_next_interpolated());
  CHECK_EXC(NoValueToInterpolateError, foo2.get_next_interpolated());
  CHECK_EXC(NoValueToInterpolateError, foo3.get_next_interpolated());

  std::vector<float> res1 {123.0f, 217.5f, 145.33333333f, 105.0f,
                             2.0f,   3.0f,   4.0f};
  std::vector<float> res2 {123.0f, 312.0f, 123.0f, 2.0f, 2.0f, 3.0f, 4.0f};

  for (uint i=0; i < t1.size(); ++i) {
    CHECK_APPROX(foo1.get_next(t1.at(i)), res1.at(i), precision);
    CHECK_APPROX(foo2.get_next(t1.at(i)), res2.at(i), precision);
  }
}

MAKE_TEST(vector_dot) {
  Vector3D v1(32.5f, 13.4f, 45.5f);
  Vector3D v2(55.3f, 4.99f, -12.552f);
  Vector3D::value_type x = v1.dot(v2);
  CHECK_APPROX(x, 1293.0f, 1.e-2);
}

MAKE_TEST(vector_bootstrap) {
  tVector3D<float> v1(1.f, 2.f, 4.f);
  tVector3D<float> v11(v1);
  CHECK_EQ(v1, v11);

  tVector3D<int> v2(1, 2, 4);
  tVector3D<int> v22(v2);
  CHECK_EQ(v2, v22);
  
  tVector3D<double> v3(1.0, 2.0, 4.0);
  tVector3D<double> v33(v3);
  CHECK_EQ(v3, v33);

  tVector3D<short> v4(1, 2, 4);
  tVector3D<short> v44(v4);
  CHECK_EQ(v4, v44);
}

MAKE_TEST(vector_cross) {
  Vector3D v1(1.5f, 3.5f, 6.6f);
  Vector3D v2(16.5f, 66.5f, 16.6f);
  Vector3D v3 = v1.cross(v2);
  CHECK_APPROX(v3.x, -380.8f, 1.e-2);
  CHECK_APPROX(v3.y,   84.0f, 1.e-2);
  CHECK_APPROX(v3.z,   42.0f, 1.e-2);
}

MAKE_TEST(vector_scalar) {
  Vector3D v1(1.3f, 48.3f, 9.0f);
  Vector3D::value_type sc = 10.0f;
  Vector3D v2 = v1 * sc;
  CHECK_APPROX(v2.x, 13.0f, 1.e-2);
  CHECK_APPROX(v2.y, 483.0f, 1.e-2);
  CHECK_APPROX(v2.z, 90.0f, 1.e-2);

  sc = 5.f;
  Vector3D v3 = v2 + sc;
  Vector3D v4 = v3 - sc;

  sc = 10.0f;
  Vector3D v5 = v4 / sc;
  CHECK_APPROX(v5.x, v1.x, 1.e-2);
  CHECK_APPROX(v5.y, v1.y, 1.e-2);
  CHECK_APPROX(v5.z, v1.z, 1.e-2);
}

MAKE_TEST(matrix_vector_mult) {
  PoseMatrix m;
  m.set_row(0,  32.5f,   23.3f, 12.4f,  2.4f);
  m.set_row(1, -13.4f, -221.4f, 23.4f,  5.5f);
  m.set_row(2,  45.5f,   54.3f, 22.4f, -4.5f);

  Vector3D v(1.5f, 5.5f, 52.5f);

  Vector3D trans_p = m.pos_trans(v);
  Vector3D trans_d = m.dir_trans(v);

  CHECK_APPROX(trans_p.x, 2363.8f, 1.e-2);
  CHECK_APPROX(trans_p.y, 1668.0f, 1.e-2);
  CHECK_APPROX(trans_p.z, 1323.3f, 1.e-2);

  CHECK_APPROX(trans_d.x, 2363.8f, 1.e-2);
  CHECK_APPROX(trans_d.y, 1668.0f, 1.e-2);
  CHECK_APPROX(trans_d.z, 1323.3f, 1.e-2);
}

MAKE_TEST(kmeans_bootstrap) {
  /*--test_data/kmeans_test_data_1.txt: 
   * - python-sklearn -> [ 0.175, 0.637, 1.300 ]
   *     max_no_imp=10, n_init=3, max_iter=100, reassign_ratio=0.01, batch=100
   * - StreamingKMeans -> [0.12664 0.0667672 0.03125 ] 
   *     no_convergence_chk, no init search, max_iter=100, no dist caching,
   * -----> still some stuff to be done!
   *  
   *--test_data/kmeans_test_data_2.txt:  
   * ooops, using data will will be in the one cluster the first 1/3, then
   * inside the second 1/3 and inside the third at the end, rough test data.
   * shuffling it once, already lead to (same conditions as above):
   * - python-sklearb ->  [ 0.1646345, 1.33934378, 0.60907686 ]
   * - StreamingKMeans -> [ 0.154886,  1.27896,    0.554659 ]
   *
   * ====> YES, more than just acceptable for a from scratch implementation
   *
   *
   */
  StreamingKMeans km(3);
  km.min_batch_size = 10;
  km.always_fill_batch = true;
  km.num_iters = 10;

  FS::Path fn("test_data/kmeans_test_data_2.txt");
  CHECK(fn.exists());

  XString s(TOOLS::IO::readfile(fn.path));
  tStringList sl = s.split();

  vector<float> raw_data;
  for(auto&& x : sl)
    raw_data.push_back(static_cast<float>(real(x)));

  km.add_data(raw_data);

  for(auto&& x : km.get_centroids())
      cout << x << endl;
}

END_SUITE()
