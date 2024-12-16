/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: July 26th 2020                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include "MBUtils.h"
#include "SurveyPlan_GUI.h"
#include <FL/Fl.H>
#include <iostream>

using namespace std;

void showHelpAndExit();

//--------------------------------------------------------
// Procedure: idleProc()

void idleProc(void *) {
  Fl::flush();
  millipause(10);
}

//--------------------------------------------------------
// Procedure: main()

int main(int argc, char *argv[]) {
  Fl::add_idle(idleProc);

  SurveyPlan_GUI gui(1000, 800, "MIT Survey Plan Viewer");

  gui.addConfigParam("--poly=pts={-60,-100:-10,-200:160,-110:110,-10}");

  for (int i = 1; i < argc; i++) {
    string argi = argv[i];

    bool handled = true;
    if ((argi == "-h") || (argi == "--help"))
      showHelpAndExit();

    else if ((argi == "-v") || (argi == "--verbose"))
      gui.addConfigParam(argi);
    else if (strEnds(argi, ".tif"))
      gui.addConfigParam(argi);

    else if (strBegins(argi, "--lane_angle="))
      gui.addConfigParam(argi);
    else if (strBegins(argi, "--lane_width="))
      gui.addConfigParam(argi);

    if (!handled) {
      cout << "survey_plan: Bad Arg: " << argi << endl;
      exit(1);
    }
  }

  gui.m_wt_viewer->handleConfigParams();
  gui.m_wt_viewer->handlePostConfigParams();

  gui.updateXY();

  return Fl::run();
}

//--------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit() {
  cout << "Usage:                                              " << endl;
  cout << "  survey_plan  [OPTIONS] image.tif                  " << endl;
  cout << "                                                    " << endl;
  cout << "Synopsis:                                           " << endl;
  cout << "  The survey_plan utility renders a polygon search  " << endl;
  cout << "  region and generates and renders a search plan to " << endl;
  cout << "  to cover that region, at a configurable angle.    " << endl;
  cout << "                                                    " << endl;
  cout << "Options:                                            " << endl;
  cout << "  -h,--help      Displays this help message         " << endl;
  cout << "                                                    " << endl;
  cout << "  --lane_angle=<angle>                              " << endl;
  cout << "  --lane_width=<width>                              " << endl;
  cout << "                                                    " << endl;
  cout << "Examples:                                           " << endl;
  cout << "  survey_plan                                       " << endl;
  exit(0);
}
