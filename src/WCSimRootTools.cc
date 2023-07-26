#include "WCSimRootTools.hh"

#include "TMath.h"
#include "TString.h"
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;
using std::vector;


#ifndef VERBOSE_COMPARISON
//#define VERBOSE_COMPARISON
#endif
const double kASmallNum = 1E-6;

/*
template <typename T> bool ComparisonPassed(const T val1, const T val2, const char * callerclass, const char * callerfunc, const char * tag)
{
  if(TMath::Abs(val1 - val2) > kASmallNum) {
    cerr << callerclass << "::" << callerfunc << " " << tag << " not equal: " << val1 << ", " << val2 << " diff " << val1 - val2 << endl;
    return false;
  }
  else {
#ifdef VERBOSE_COMPARISON
    cout << callerclass << "::" << callerfunc << " " << tag << " equal: " << val1 << ", " << val2 << endl;
#endif
    return true;
  }
}

template <typename T> bool ComparisonPassedVec(const vector<T> & val1, const vector<T> & val2, const char * callerclass, const char * callerfunc, const char * tag)
{
  bool failed = false;
  if(val1.size() != val2.size()) {
    cerr << callerclass << "::" << callerfunc << " " << tag << " have unequal sizes: " << val1.size() << ", " << val2.size() << endl;
    failed = true;
  }
  const int n = TMath::Min(val1.size(), val2.size());
  for(int i = 0; i < n; i++) {
    failed = (!ComparisonPassed(val1[i], val2[i], callerclass, callerfunc, TString::Format("%s[%d]", tag, i)));
  }
  return !failed;
}
*/

bool ComparisonPassedVec(const vector<int> & val1, const vector<int> & val2, const char * callerclass, const char * callerfunc, const char * tag)
{
  bool failed = false;
  if(val1.size() != val2.size()) {
    cerr << callerclass << "::" << callerfunc << " " << tag << " have unequal sizes: " << val1.size() << ", " << val2.size() << endl;
    failed = true;
  }
  const int n = TMath::Min(val1.size(), val2.size());
  for(int i = 0; i < n; i++) {
    failed = (!ComparisonPassed(val1[i], val2[i], callerclass, callerfunc, TString::Format("%s[%d]", tag, i)));
  }
  return !failed;
}

bool ComparisonPassedVec(const vector<long> & val1, const vector<long> & val2, const char * callerclass, const char * callerfunc, const char * tag)
{
  bool failed = false;
  if(val1.size() != val2.size()) {
    cerr << callerclass << "::" << callerfunc << " " << tag << " have unequal sizes: " << val1.size() << ", " << val2.size() << endl;
    failed = true;
  }
  const int n = TMath::Min(val1.size(), val2.size());
  for(int i = 0; i < n; i++) {
    failed = (!ComparisonPassed(val1[i], val2[i], callerclass, callerfunc, TString::Format("%s[%d]", tag, i)));
  }
  return !failed;
}

bool ComparisonPassedVec(const vector<float> & val1, const vector<float> & val2, const char * callerclass, const char * callerfunc, const char * tag)
{
  bool failed = false;
  if(val1.size() != val2.size()) {
    cerr << callerclass << "::" << callerfunc << " " << tag << " have unequal sizes: " << val1.size() << ", " << val2.size() << endl;
    failed = true;
  }
  const int n = TMath::Min(val1.size(), val2.size());
  for(int i = 0; i < n; i++) {
    failed = (!ComparisonPassed(val1[i], val2[i], callerclass, callerfunc, TString::Format("%s[%d]", tag, i)));
  }
  return !failed;
}

bool ComparisonPassedVec(const vector<double> & val1, const vector<double> & val2, const char * callerclass, const char * callerfunc, const char * tag)
{
  bool failed = false;
  if(val1.size() != val2.size()) {
    cerr << callerclass << "::" << callerfunc << " " << tag << " have unequal sizes: " << val1.size() << ", " << val2.size() << endl;
    failed = true;
  }
  const int n = TMath::Min(val1.size(), val2.size());
  for(int i = 0; i < n; i++) {
    failed = (!ComparisonPassed(val1[i], val2[i], callerclass, callerfunc, TString::Format("%s[%d]", tag, i)));
  }
  return !failed;
}
