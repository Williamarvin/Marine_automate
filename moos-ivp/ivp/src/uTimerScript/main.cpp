/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
/*    DATE: May 21, 2009                                         */
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

#include "ColorParse.h"
#include "MBUtils.h"
#include "OpenURL.h"
#include "TS_Info.h"
#include "TS_MOOSApp.h"

using namespace std;

int main(int argc, char *argv[]) {
  string mission_file;
  string run_command = argv[0];
  bool verbose_setting = true;
  bool shuffle_setting = true;

  for (int i = 1; i < argc; i++) {
    string argi = argv[i];
    if ((argi == "-v") || (argi == "--version") || (argi == "-version"))
      showReleaseInfoAndExit();
    else if ((argi == "-e") || (argi == "--example") || (argi == "-example"))
      showExampleConfigAndExit();
    else if ((argi == "-h") || (argi == "--help") || (argi == "-help"))
      showHelpAndExit();
    else if ((argi == "-i") || (argi == "--interface"))
      showInterfaceAndExit();
    else if (strEnds(argi, ".moos") || strEnds(argi, ".moos++"))
      mission_file = argv[i];
    else if (strBegins(argi, "--alias="))
      run_command = argi.substr(8);
    else if (strBegins(argi, "--verbose=")) {
      string val = tolower(argi.substr(10));
      setBooleanOnString(verbose_setting, val);
    } else if (strBegins(argi, "--shuffle=")) {
      string val = tolower(argi.substr(10));
      setBooleanOnString(shuffle_setting, val);
    } else if ((argi == "-w") || (argi == "--web") || (argi == "-web"))
      openURLX("https://oceanai.mit.edu/ivpman/apps/uTimerScript");
    else if (i == 2)
      run_command = argi;
  }

  if (mission_file == "")
    showHelpAndExit();

  cout << termColor("green");
  cout << "uTimerScript launching as " << run_command << endl;
  cout << termColor() << endl;

  TS_MOOSApp timer_script;
  timer_script.setVerbose(verbose_setting);
  timer_script.setShuffle(shuffle_setting);
  timer_script.Run(run_command.c_str(), mission_file.c_str(), argc, argv);

  return (0);
}
