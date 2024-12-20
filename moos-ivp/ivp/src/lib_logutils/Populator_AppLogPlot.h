/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Populator_AppLogPlot.h                               */
/*    DATE: Oct 16th, 2021                                       */
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

#ifndef POPULATOR_APP_LOG_PLOT_HEADER
#define POPULATOR_APP_LOG_PLOT_HEADER

#include "ALogEntry.h"
#include "AppLogPlot.h"
#include <string>
#include <vector>

class Populator_AppLogPlot {
public:
  Populator_AppLogPlot() {}
  ~Populator_AppLogPlot() {}

  bool populateFromEntries(const std::vector<ALogEntry> &);
  AppLogPlot getAppLogPlot() { return (m_alplot); }

protected:
  AppLogPlot m_alplot;
};
#endif
