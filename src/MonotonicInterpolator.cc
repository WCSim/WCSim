 #include "MonotonicInterpolator.hh"
#include <algorithm>
//Monotonic Spline code using Fritsch–Carlson method

//Constructor for a 1D monotonic spline 
MonotonicInterpolator::MonotonicInterpolator(const std::vector<double>& x, const std::vector<double>& y){
  m1d_ = ComputeMonotonicSlopes(x, y);
}

//Constructor for a 2D monotonic spline
MonotonicInterpolator::MonotonicInterpolator(const std::vector<double>& x, const std::vector<double>& y_axis,
                                             const std::vector<std::vector<double>>& z_matrix) : x_(x), y_(y_axis), z_(z_matrix){
  m2d_ = PreComputeSlopes2D();
}

//Constructor for a 1D monotonic spline with predetermined slopes
MonotonicInterpolator::MonotonicInterpolator(const std::vector<double>& x_axis,std::vector<double>& slopes)
  : x_(x_axis), m1d_(slopes)
{}

//Constructor for a 2D monotonic spline with predetermined slopes
MonotonicInterpolator::MonotonicInterpolator(const std::vector<double>& x_axis, const std::vector<double>& y_axis, const std::vector<std::vector<double>>& z_grid,std::vector<std::vector<double>>& slopes)
  : x_(x_axis), y_(y_axis), z_(z_grid), m2d_(slopes)
{}

//Evaluate z value for the 2D monotonic spline at a given x and y point
double MonotonicInterpolator::Evaluate2D(double x, double y){
  std::vector<double> z_interp_y;
  for (size_t j = 0; j < y_.size(); ++j) {
    const auto& z_row = z_[j];
    const auto& slopes_ = m2d_[j];
    double zx = PCHIPInterpolate(x, x_, z_row, slopes_);
    z_interp_y.push_back(zx);
  }
  auto slopes_y = ComputeMonotonicSlopes(y_, z_interp_y);
  double result = PCHIPInterpolate(y, y_, z_interp_y, slopes_y);

  return std::max(0.0, result);

}

//Evaluate y value for the 1D monotonic spline at a given x point
double MonotonicInterpolator::Evaluate1D(double x) {
  auto slopes = ComputeMonotonicSlopes(x_, y_);
  double result = PCHIPInterpolate(x, x_, y_, slopes);

  return std::max(0.0, result);  // if clamping negative results is desired
}

//Identify the interval that the given value x lies in
int MonotonicInterpolator::FindInterval(const std::vector<double>& vals, double x) const{
  auto it = std::upper_bound(vals.begin(), vals.end(), x);
  int idx = std::max(0, int(it - vals.begin()) - 1);
  if (idx >= int(vals.size()) - 1) idx = vals.size() - 2;
  
  return idx;
}

// Compute gradients between points on the spline (delta)
std::vector<double> MonotonicInterpolator::ComputeDeltas(const std::vector<double>& x, const std::vector<double>& y) {
  std::vector<double> deltas;
  for (size_t i = 0; i + 1 < x.size(); ++i)
    deltas.push_back((y[i+1] - y[i]) / (x[i+1] - x[i]));
  
  return deltas;
}

// Compute monotonic slopes (m)
std::vector<double> MonotonicInterpolator::ComputeMonotonicSlopes(const std::vector<double>& x, const std::vector<double>& y) {
  size_t n = x.size();
  std::vector<double> ma(n);
  std::vector<double> d = ComputeDeltas(x, y);
  
  ma[0] = d[0];
  ma[n-1] = d[n-2];
  
  for (size_t i = 1; i < n - 1; ++i) {
    if (d[i-1] * d[i] <= 0) {
      ma[i] = 0;
    } else {
      double w1 = 2 * (x[i+1] - x[i]);
      double w2 = 2 * (x[i] - x[i-1]);
      ma[i] = (w1 + w2) / ((w1 / d[i-1]) + (w2 / d[i]));
    }
  }

    return ma;
}

// Monotonic 1D interpolation at x in [x0, x1]
double MonotonicInterpolator::PCHIPInterpolate(double x, const std::vector<double>& xs, const std::vector<double>& ys, const std::vector<double>& mas) const{
  int i = FindInterval(xs, x);
  double h = xs[i+1] - xs[i];
  double t = (x - xs[i]) / h;
  
  double t2 = t * t;
  double t3 = t2 * t;
  
  double h00 = 2*t3 - 3*t2 + 1;
  double h10 = t3 - 2*t2 + t;
  double h01 = -2*t3 + 3*t2;
  double h11 = t3 - t2;

  return h00 * ys[i] + h10 * h * mas[i] + h01 * ys[i+1] + h11 * h * mas[i+1];
}

double MonotonicInterpolator::Interpolate1D(double xval) const {
  return PCHIPInterpolate(xval, x_, y_, m1d_);
}

//Return slopes from spline calculation
std::vector<std::vector<double>> MonotonicInterpolator::PreComputeSlopes2D() {
  std::vector<std::vector<double>> slopes;
  for (const auto& row : z_)
    slopes.push_back(ComputeMonotonicSlopes(x_, row));
  
  
  return slopes;
}

//Interpolation applied across both x and y axes
double MonotonicInterpolator::Interpolate2D(double xval, double yval) const {

  int j = FindInterval(y_, yval);
  double f0 = PCHIPInterpolate(xval, x_, z_[j], m2d_[j]);
  double f1 = PCHIPInterpolate(xval, x_, z_[j + 1], m2d_[j + 1]);
  double t = (yval - y_[j]) / (y_[j + 1] - y_[j]);
  
  return (1 - t) * f0 + t * f1;
}
