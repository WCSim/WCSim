#ifndef WCSimRootTools_h
#define WCSimRootTools_h 1

#include <vector>
#include <iostream>

using std::vector;

/*
template <typename T> bool ComparisonPassed(const T val1, const T val2, 
		      const char * callerclass, const char * callerfunc, const char * tag);
template <typename T> bool ComparisonPassedVec(const vector<T> & val1, const vector<T> & val2,
					       const char * callerclass, const char * callerfunc, const char * tag);
*/

template <typename T>
bool ComparisonPassed(T val1, T val2, const char * callerclass, const char * callerfunc, const char * tag){
  {
	if(val1 - val2) {
	  std::cerr << "INT" << callerclass << "::" << callerfunc << " " << tag << " not equal: " << val1 << ", " << val2 << " diff " << val1 - val2 << std::endl;
	  return false;
	}
	else {
#ifdef VERBOSE_COMPARISON
	  cout << callerclass << "::" << callerfunc << " " << tag << " equal: " << val1 << ", " << val2 << endl;
#endif
	  return true;
	}
  }
}
bool ComparisonPassedVec(const vector<int> & val1, const vector<int> & val2,
			 const char * callerclass, const char * callerfunc, const char * tag);
bool ComparisonPassedVec(const vector<long> & val1, const vector<long> & val2,
			 const char * callerclass, const char * callerfunc, const char * tag);
bool ComparisonPassedVec(const vector<float> & val1, const vector<float> & val2,
			 const char * callerclass, const char * callerfunc, const char * tag);
bool ComparisonPassedVec(const vector<double> & val1, const vector<double> & val2,
			 const char * callerclass, const char * callerfunc, const char * tag);

#endif //WCSimRootTools_h
