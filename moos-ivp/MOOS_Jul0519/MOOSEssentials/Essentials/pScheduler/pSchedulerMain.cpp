///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite
//
//   A suit of Applications and Libraries for Mobile Robotics Research
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and
//   Oxford University.
//
//   This software was written by Paul Newman at MIT 2001-2002 and Oxford
//   University 2003-2005. email: pnewman@robots.ox.ac.uk.
//
//   This file is part of a  MOOS Core Component.
//
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the GNU General Public License as
//   published by the Free Software Foundation; either version 2 of the
//   License, or (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
//   General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
//   02111-1307, USA.
//
//////////////////////////    END_GPL    //////////////////////////////////
///////////////////////////////////////////////////////////////////
//                                                               //
//          This file is part of MOOS Suite                      //
//            Copyright Paul Newman, September 2000                //
//                                                               //
///////////////////////////////////////////////////////////////////

#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOS/Utils/CommandLineParser.h"

#include "MOOSScheduler.h"

int main(int argc, char *argv[]) {

  // here we do some command line parsing...
  MOOS::CommandLineParser P(argc, argv);
  // mission file could be first free parameter
  std::string mission_file = P.GetFreeParameter(0, "Mission.moos");
  // mission name can be the  second free parameter
  std::string app_name = P.GetFreeParameter(1, "pScheduler");

  // GO!
  CMOOSScheduler TheScheduler;
  TheScheduler.Run(app_name, mission_file, argc, argv);

  return 0;
}
