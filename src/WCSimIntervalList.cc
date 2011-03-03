#include "WCSimIntervalList.hh"
#include <algorithm>

// Implementation notes: The interval list is represented as a sorted
// array of unique values, where values with even index are beginings of intervals
// and values with odd index are ends of intervals.
//
// Invariants: fPoints.size() % 2 == 0, fPoints always in sorted order

WCSimIntervalList::WCSimIntervalList()
  : fPoints(0)
{
  // default to empty interval
}

void WCSimIntervalList::AddInterval(double begin, double end)
{
  std::vector<double> newPoints;
  
  // Copy before this interval
  unsigned index = 0;
  while (index < fPoints.size() && fPoints[index] <= begin) {
    newPoints.push_back(fPoints[index]);
    index++;
  }
  
  if (index % 2 == 0)
    newPoints.push_back(begin);
  
  // Discard points until end of this interval
  while (index < fPoints.size() && fPoints[index] <= end)
    index++;
    
  if (index % 2 == 0)
    newPoints.push_back(end);
    
  // Add remaining points
  while(index < fPoints.size()) {
    newPoints.push_back(fPoints[index]);
    index++;
  }
  
  fPoints = newPoints;
}

// Take the union of the new interval list with this one
void WCSimIntervalList::AddIntervalList(const WCSimIntervalList &intervalList)
{
  for (unsigned i=0; i < intervalList.fPoints.size(); i += 2)
    AddInterval(intervalList.fPoints[i], intervalList.fPoints[i+1]);
}

void WCSimIntervalList::Clear()
{
  fPoints.clear();
}

// Test for containment of point within one of the intervals
bool WCSimIntervalList::Contains(double point) const
{
  if (fPoints.size() == 0)
    return false; // No intervals
    
  std::vector<double>::const_iterator location;
  location = std::lower_bound(fPoints.begin(), fPoints.end(), point);

  if (location == fPoints.end())
    return false; // Off end of array, outside last interval
  else {
    int index = location - fPoints.begin();
    
    if (index % 2 == 0)
      return fPoints[index] == point;
    else if (index % 2 == 1)
      return true;
  }
  
  return false; // Never get here
}

// Return a list of start times for intervals in this list
std::vector<double> WCSimIntervalList::GetBeginPoints() const
{
  std::vector<double> beginPoints(fPoints.size()/2);
  
  for (unsigned i=0; i < fPoints.size(); i += 2)
    beginPoints[i/2] = fPoints[i];
    
  return beginPoints;
}

// Return a list of start times for intervals in this list
std::vector<double> WCSimIntervalList::GetEndPoints() const
{
  std::vector<double> endPoints(fPoints.size()/2);
  
  for (unsigned i=1; i < fPoints.size(); i += 2)
    endPoints[i/2] = fPoints[i];
    
  return endPoints;
}
