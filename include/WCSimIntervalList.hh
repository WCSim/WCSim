#ifndef WCSim_IntervalList
#define WCSim_IntervalList

#include <vector>

/////////////////////////////////////////////////////////////////
//
// An interval list is the union of zero or more non-overlapping
// closed intervals in the domain of real numbers.
//
// By definition, two intervals in the list cannot overlap and
// should be represented as one larger interval spanning the two
// original intervals.  Testing for containment in an interval
// list constructed this way can be done in O(log n) time.
//
/////////////////////////////////////////////////////////////////

class WCSimIntervalList {
public:
  
  // Create the empty interval
  WCSimIntervalList();
  
  // Take the union of the interval [begin, end] and
  // this interval list.  If this interval overlaps with
  // existing intervals, the list is recomputed to preserve
  // the interval condition
  // Requirement: begin < end
  void AddInterval(double begin, double end);
  
  // Take the union of the new interval list with this one
  void AddIntervalList(const WCSimIntervalList &intervalList);
  
  // Clear all intervals
  void Clear();
  
  // Test for containment of point within one of the intervals
  bool Contains(double point) const;
  
  // Return a list of start times for intervals in this list
  std::vector<double> GetBeginPoints() const;

  // Return a list of start times for intervals in this list
  std::vector<double> GetEndPoints() const;
  
private:
  std::vector<double> fPoints;
};

#endif
