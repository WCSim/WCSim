void calcPhotoCoverage(double WCPMTPercentCoverage,
		       double WCPMTPercentCoverage2,
		       double WCIDDiameter = 64.8, // m
		       double WCPMTRadius = 0.254, // m
		       double mPMT_vessel_radius = 0.254) // m
{
  cout << "WCPMTPercentCoverage:  " << WCPMTPercentCoverage << endl
       << "WCPMTPercentCoverage2: " << WCPMTPercentCoverage2 << endl;

  cout << "WCIDDiameter:       " << WCIDDiameter << endl
       << "WCPMTRadius:        " << WCPMTRadius << endl
       << "mPMT_vessel_radius: " << mPMT_vessel_radius << endl;

  int WCPMTperCellHorizontal = std::lround((WCPMTPercentCoverage+WCPMTPercentCoverage2) / WCPMTPercentCoverage2);
  int WCBarrelNumPMTHorizontal = std::lround(WCIDDiameter * sqrt(TMath::Pi() * (WCPMTPercentCoverage+WCPMTPercentCoverage2)) /
					     (10.*TMath::Max(WCPMTRadius,mPMT_vessel_radius)));
  double WCBarrelNumPMTHorizontal_db = WCIDDiameter * sqrt(TMath::Pi() * (WCPMTPercentCoverage+WCPMTPercentCoverage2)) /
    (10.*TMath::Max(WCPMTRadius,mPMT_vessel_radius));

  cout << "WCPMTperCellHorizontal:      " << WCPMTperCellHorizontal << endl
       << "WCBarrelNumPMTHorizontal:    " << WCBarrelNumPMTHorizontal << endl
       << "WCBarrelNumPMTHorizontal_db: " << WCBarrelNumPMTHorizontal_db << endl;

  int WCBarrelRingNPhi = WCBarrelNumPMTHorizontal/WCPMTperCellHorizontal;
  int remainder = WCBarrelNumPMTHorizontal % WCPMTperCellHorizontal;
  cout << "WCBarrelRingNPhi:    " << WCBarrelRingNPhi << endl
       << "remainder:           " << remainder << endl;

  cout << std::setprecision(15) << endl;
  
  //option 1: scale down
  double scale1 = TMath::Power((WCBarrelNumPMTHorizontal - remainder) / WCBarrelNumPMTHorizontal_db, 2);
  cout << "Option 1: scale down by " << scale1
       << " = (" << WCBarrelNumPMTHorizontal - remainder
       << " / " << WCBarrelNumPMTHorizontal_db << ")**2" << endl
       << endl
       << "/WCSim/SetPMTPercentCoverage  " << WCPMTPercentCoverage  * scale1 << endl
       << "/WCSim/SetPMTPercentCoverage2 " << WCPMTPercentCoverage2 * scale1 << endl
       << endl;

  //option 2: scale up
  double scale2 = TMath::Power((WCBarrelNumPMTHorizontal - remainder + WCPMTperCellHorizontal) / WCBarrelNumPMTHorizontal_db, 2);
  cout << "Option 2: scale up by " << scale2
       << " = (" << WCBarrelNumPMTHorizontal - remainder + WCPMTperCellHorizontal
       << " / " << WCBarrelNumPMTHorizontal_db << ")**2" << endl
       << endl
       << "/WCSim/SetPMTPercentCoverage  " << WCPMTPercentCoverage  * scale2 << endl
       << "/WCSim/SetPMTPercentCoverage2 " << WCPMTPercentCoverage2 * scale2 << endl
       << endl;  
}
