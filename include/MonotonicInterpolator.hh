#ifndef SPLINE_H
#define SPLINE_H

#include <vector>


class MonotonicInterpolator{
public:
  // 1D constructor
  MonotonicInterpolator(const std::vector<double>& x_axis, const std::vector<double>& y_axis);

  // 2D constructor
  MonotonicInterpolator(const std::vector<double>& x_axis, const std::vector<double>& y_axis, const std::vector<std::vector<double>>& z_matrix);

  MonotonicInterpolator(const std::vector<double>& x_axis, std::vector<double>& slopes);
  
  MonotonicInterpolator(const std::vector<double>& x_axis, const std::vector<double>& y_axis, const std::vector<std::vector<double>>& z_grid, std::vector<std::vector<double>>& slopes);
 
  // Evaluate 1D/2D interpolation
  double Interpolate1D(double xval) const;
  double Interpolate2D(double xval, double yval) const;
  std::vector<std::vector<double>> GetSlopes2D() const {return m2d_;};
  std::vector<double> GetSlopes1D() const {return m1d_;};
  double Evaluate1D(double xval);
  double Evaluate2D(double xval, double yval);
  
private:
  std::vector<double> x_;
  std::vector<double> y_;                    
  std::vector<double> m1d_;
  std::vector<std::vector<double>> z_;
  std::vector<std::vector<double>> m2d_;
  double z_ev;
  
  int FindInterval(const std::vector<double>& x, double xval) const;
  std::vector<double> ComputeDeltas(const std::vector<double>& x, const std::vector<double>& y);
  std::vector<double> ComputeMonotonicSlopes(const std::vector<double>& x, const std::vector<double>& y);
  double PCHIPInterpolate(double xval, const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& m) const;
  std::vector<std::vector<double>> PreComputeSlopes2D();

};
#endif // SPLINE_H
