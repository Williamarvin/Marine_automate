/*****************************************************************/
/*    NAME: Tyler Paine, Mike Defilippo                          */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
/*    DATE: 25 APRIL 2024                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#include "BlueBoat.h"
#include "BlueBoat_Info.h"
#include "ColorParse.h"
#include "MBUtils.h"
#include <string>

using namespace std;

int main(int argc, char *argv[]) {
  string mission_file;
  string run_command = argv[0];

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
    else if (i == 2)
      run_command = argi;
  }

  if (mission_file == "")
    showHelpAndExit();

  cout << termColor("green");
  cout << "iBlueBoat launching as " << run_command << endl;
  cout << termColor() << endl;

  BlueBoat BlueBoat;

  BlueBoat.Run(run_command.c_str(), mission_file.c_str());

  return (0);
}
