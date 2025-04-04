/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
/*    DATE: Dec 22nd, 2013                                       */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include "IterHandler.h"
#include "MBUtils.h"
#include "OpenURL.h"
#include "ReleaseInfo.h"
#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[]) {
  // Look for a request for version information
  if (scanArgs(argc, argv, "-v", "--version", "-version")) {
    showReleaseInfo("alogiter", "gpl");
    return (0);
  }
  if (scanArgs(argc, argv, "-w", "-web", "--web"))
    openURLX("https://oceanai.mit.edu/ivpman/apps/alogiter");

  // Look for a request for usage information
  if (scanArgs(argc, argv, "-h", "--help", "-help")) {
    cout << "Usage: " << endl;
    cout << "  alogiter in.alog [OPTIONS]                               "
         << endl;
    cout << "                                                           "
         << endl;
    cout << "Synopsis:                                                  "
         << endl;
    cout << "  Analyze the ITER_GAP and ITER_LEN information provided   "
         << endl;
    cout << "  by all applications recorded in the given alog file.     "
         << endl;
    cout << "                                                           "
         << endl;
    cout << "Standard Arguments:                                        "
         << endl;
    cout << "  file.alog - The input logfile.                           "
         << endl;
    cout << "                                                           "
         << endl;
    cout << "Options:                                                   "
         << endl;
    cout << "  -h,--help     Displays this help message                 "
         << endl;
    cout << "  -v,--version  Displays the current release version       "
         << endl;
    cout << "                                                           "
         << endl;
    cout << "  --web,-w   Open browser to:                              "
         << endl;
    cout << "             https://oceanai.mit.edu/ivpman/apps/alogiter  "
         << endl;
    cout << "                                                           "
         << endl;
    cout << "Further Notes:                                             "
         << endl;
    cout << "  See also: alogscan, alogrm, alogclip, alogview, aloggrep "
         << endl;
    cout << endl;
    return (0);
  }

  vector<string> keys;
  string alogfile_in;

  for (int i = 1; i < argc; i++) {
    string sarg = argv[i];
    if (strContains(sarg, ".alog"))
      alogfile_in = sarg;
  }

  if (alogfile_in == "") {
    cout << "No alog file given - exiting" << endl;
    exit(0);
  }

  cout << "Processing on file : " << alogfile_in << endl;

  IterHandler handler;
  bool handled = handler.handle(alogfile_in);

  if (handled)
    handler.printReport();
  else {
    cout << "Unhandled. Exiting now." << endl;
    return (1);
  }

  return (0);
}
