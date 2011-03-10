#include "cxxtest/TestSuite.h"
#include "WCSimIntervalList.hh"

class TestWCSimIntervalList : public CxxTest::TestSuite
{
public:
  void FillInterval1(WCSimIntervalList &list) {
    list.AddInterval(0.0, 1.0);
    list.AddInterval(2.0, 3.5);
    list.AddInterval(4.0, 8.0);
  }
  
  void FillInterval2(WCSimIntervalList &list) {
    list.AddInterval(-0.5, 1.5);
    list.AddInterval(1.75, 1.95);
    list.AddInterval(5.0, 6.0);
  }
  
  void TestEmpty() {
    WCSimIntervalList l;
    std::vector<double> emptyVector(0);
    
    TS_ASSERT(!l.Contains(0.0));
    TS_ASSERT(!l.Contains(-10.0));
    TS_ASSERT(!l.Contains(10.0));
    
    // Empty list
    TS_ASSERT_EQUALS(l.GetBeginPoints(), emptyVector);
    TS_ASSERT_EQUALS(l.GetEndPoints(), emptyVector);
  }
  
  void TestAddIntervalSimple() {
    WCSimIntervalList l;
    
    TS_ASSERT(!l.Contains(5.0));
    l.AddInterval(0.0, 10.0);
    
    // The interval is closed
    TS_ASSERT(l.Contains(5.0));
    TS_ASSERT(l.Contains(0.0));
    TS_ASSERT(l.Contains(10.0));
    TS_ASSERT(!l.Contains(-1.0));
    TS_ASSERT(!l.Contains(10.1));
  }
  
  void TestAddIntervalOverlap() {
    WCSimIntervalList l;

    TS_ASSERT(!l.Contains(5.0));
    TS_ASSERT(!l.Contains(15.0));

    l.AddInterval(0.0, 10.0);
    TS_ASSERT(l.Contains(5.0));
    TS_ASSERT(!l.Contains(15.0));
    
    l.AddInterval(5.0, 15.0);
    TS_ASSERT(l.Contains(5.0));
    TS_ASSERT(l.Contains(15.0));
  }
  
  void TestAddIntervalMultiple() {
    WCSimIntervalList l1;
    FillInterval1(l1);
    
    TS_ASSERT(l1.Contains(0.0));
    TS_ASSERT(!l1.Contains(1.5));
    TS_ASSERT(l1.Contains(3.0));
    TS_ASSERT(l1.Contains(4.0));
    TS_ASSERT(!l1.Contains(10.0));
  }
  
  void TestAddIntervalList() {
    WCSimIntervalList l1, l2;
    FillInterval1(l1);
    FillInterval2(l2);
    
    l1.AddIntervalList(l2);

    TS_ASSERT(l1.Contains(-0.3));
    TS_ASSERT(l1.Contains(0.0));
    TS_ASSERT(l1.Contains(1.5));
    TS_ASSERT(!l1.Contains(1.99));
    TS_ASSERT(l1.Contains(3.0));
    TS_ASSERT(l1.Contains(4.0));
    TS_ASSERT(l1.Contains(5.5));
    TS_ASSERT(!l1.Contains(10.0));
  }
  
  void TestClear() {
    WCSimIntervalList l1;
    
    FillInterval1(l1);
    
    l1.Clear();
    
    TS_ASSERT(!l1.Contains(0.0));
    TS_ASSERT(!l1.Contains(1.5));
    TS_ASSERT(!l1.Contains(3.0));
    TS_ASSERT(!l1.Contains(4.0));
    TS_ASSERT(!l1.Contains(10.0));
  }
  
  void TestGetBeginPoints() {
    WCSimIntervalList l1;
    std::vector<double> trueBeginPoints(3);
    trueBeginPoints[0] = 0.0; 
    trueBeginPoints[1] = 2.0;
    trueBeginPoints[2] = 4.0;
    
    FillInterval1(l1);
    TS_ASSERT_EQUALS(l1.GetBeginPoints(), trueBeginPoints);
  }
  
  void TestGetEndPoints() {
    WCSimIntervalList l1;
    std::vector<double> trueEndPoints(3);
    trueEndPoints[0] = 1.0; 
    trueEndPoints[1] = 3.5;
    trueEndPoints[2] = 8.0;
    
    FillInterval1(l1);
    TS_ASSERT_EQUALS(l1.GetEndPoints(), trueEndPoints);
  }
};
