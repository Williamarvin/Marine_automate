/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
/*    DATE: Jun 26th 2008                                        */
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
#include "PXR_Info.h"
#include "PXR_MOOSApp.h"
#include "ReleaseInfo.h"

using namespace std;

int main(int argc, char *argv[]) {
  string mission_file;
  string run_command = argv[0];
  string incoming_var;
  string outgoing_var;

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
    else if (strBegins(argi, "--in="))
      incoming_var = argi.substr(5);
    else if (strBegins(argi, "--out="))
      outgoing_var = argi.substr(6);
    else if (i == 2)
      run_command = argi;
  }

  if (mission_file == "")
    showHelpAndExit();

  cout << termColor("green");
  cout << "pXRelay launching as " << run_command << endl;
  cout << termColor() << endl;

  PXR_MOOSApp relayer;
  if (incoming_var != "")
    relayer.setIncomingVar(incoming_var);
  if (outgoing_var != "")
    relayer.setOutgoingVar(outgoing_var);

  relayer.Run(run_command.c_str(), mission_file.c_str(), argc, argv);

  return (0);
}
