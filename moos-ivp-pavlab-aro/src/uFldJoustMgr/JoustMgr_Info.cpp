/****************************************************************/
/*   NAME: Mike Benjamin                                        */
/*   ORGN: MIT Cambridge MA                                     */
/*   FILE: JoustMgr_Info.cpp                                    */
/*   DATE: Mar 2nd 2024                                         */
/****************************************************************/

#include "JoustMgr_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"
#include <cstdlib>
#include <iostream>

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis()

void showSynopsis() {
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The uFldJoustMgr app, the joust manager, is designed to run on");
  blk("  the shoreside during Jousting style missions, typically for   ");
  blk("  testing collision avoidance behaviors. The joust manager will ");
  blk("  ingest a configuration file, usually generared by pickjoust.  ");
  blk("  The joust file contains legruns overlayed on a defined circle.");
  blk("  Legruns are then conveyed to each newly detected vehicle via  ");
  blk("  incoming node reports. In this way, the mission folder on each");
  blk("  deployed vehicle does not get its assignment until run time.  ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit() {
  blu("=============================================================== ");
  blu("Usage: uFldJoustMgr file.moos [OPTIONS]                         ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias", "=<ProcessName>                                      ");
  blk("      Launch uFldJoustMgr with the given process name           ");
  blk("      rather than uFldJoustMgr.                                 ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of uFldJoustMgr.              ");
  blk("                                                                ");
  blk("Note: If argv[2] does not otherwise match a known option,       ");
  blk("      then it will be interpreted as a run alias. This is       ");
  blk("      to support pAntler launching conventions.                 ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showExampleConfigAndExit()

void showExampleConfigAndExit() {
  blu("=============================================================== ");
  blu("uFldJoustMgr Example MOOS Configuration                         ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uFldJoustMgr                                    ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  joust_file = joust.tx                                         ");
  blk("  updates    = LR_UPDATE                                        ");
  blk("}                                                               ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showInterfaceAndExit()

void showInterfaceAndExit() {
  blk("                                                                ");
  blu("=============================================================== ");
  blu("uFldJoustMgr INTERFACE                                          ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  NODE_REPORT = NAME=alpha,TYPE=UUV,TIME=1252348077.59,         ");
  blk("                COLOR=red,X=51.71,Y=-35.50, LAT=43.824981,      ");
  blk("                LON=-70.329755,SPD=2.0,HDG=118.8, YAW=118.8,    ");
  blk("                DEPTH=4.6,LENGTH=3.8, MODE=MODE@ACTIVE:LOITERING");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  LR_UPDATE_ABE = vx1=x=84.4,y=-110.3#vx2=x=15.5,y=-69.4        ");
  blk("                                                                ");
  blk(" Note: The variable LR_UPDATE_ABE is derived partly from the    ");
  blk("       \"updates\" config parameter and partly because it is    ");
  blk("       being communicated to vehicle abe.                       ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit()

void showReleaseInfoAndExit() {
  showReleaseInfo("uFldJoustMgr", "none");
  exit(0);
}
