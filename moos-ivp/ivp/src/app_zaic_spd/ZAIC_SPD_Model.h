/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ZAIC_SPD_Model.h                                     */
/*    DATE: May 9th, 2016                                        */
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

#ifndef ZAIC_SPD_MODEL_HEADER
#define ZAIC_SPD_MODEL_HEADER

#include "IvPDomain.h"
#include "ZAIC_Model.h"
#include "ZAIC_SPD.h"
#include <string>

class ZAIC_SPD_Model : public ZAIC_Model {
public:
  ZAIC_SPD_Model();
  ~ZAIC_SPD_Model(){};

public: // Virtual functions overloaded
  IvPFunction *getIvPFunction();
  void setDomain(unsigned int){};
  bool setParam(std::string, std::string);
  void moveX(double amt);
  void disableLowSpeed();
  void disableHighSpeed();
  void currMode(int);

public:
  void setDomain(std::string);
  int getCurrMode() const { return (m_curr_mode); }

  void print() const;

  // Getters
  double getMedVal();
  double getLowVal();
  double getHghVal();
  double getLowValUtil();
  double getHghValUtil();

  double getLowMinUtil();
  double getHighMinUtil();
  double getMaxUtil();

protected:
  ZAIC_SPD *m_zaic_spd;
  int m_curr_mode;

  unsigned int m_ipf_pieces;
};
#endif
