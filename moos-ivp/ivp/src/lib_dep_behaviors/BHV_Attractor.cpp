/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_Attractor.cpp                                    */
/*    DATE: June 2008                                            */
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

#include "BHV_Attractor.h"
#include "AOF_AttractorCPA.h"
#include "AngleUtils.h"
#include "BuildUtils.h"
#include "MBUtils.h"
#include "OF_Coupler.h"
#include "OF_Reflector.h"
#include "ZAIC_PEAK.h"
#include <cmath>
#include <cstdlib>
#include <iostream>

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

BHV_Attractor::BHV_Attractor(IvPDomain gdomain) : IvPContactBehavior(gdomain) {
  this->setParam("descriptor", "(d)bhv_cutrange");
  this->setParam("build_info", "uniform_box =discrete@course:2,speed:3");
  this->setParam("build_info", "uniform_grid=discrete@course:8,speed:6");

  // These parameters really should be set in the behavior file, but are
  // left here for now to smoothen the transition (Aug 10, 2008, mikerb)
  // this->setParam("runflag",  "ATTRACTOR_ACTIVE=1");
  // this->setParam("idleflag", "ATTRACTOR_ACTIVE=0");

  // this->setParam("activeflag",   "ATTRACTED=1");
  // this->setParam("inactiveflag", "ATTRACTED=0");

  m_domain = subDomain(m_domain, "course,speed");

  // If equal: 100% priority above, 0% priority below
  // If not equal: Linear scale between 0-100 in between

  m_min_priority_range = 0;
  m_max_priority_range = 0;

  m_min_util_cpa_dist = 100;
  m_max_util_cpa_dist = 0;

  m_extrapolate = true;
  m_patience = 0;
  m_giveup_range = 0; // meters - zero means never give up
  m_time_on_leg = 15; // seconds

  strength = 1.0;

  m_decay_start = 5;
  m_decay_end = 10;

  m_extrapolator.setDecay(m_decay_start, m_decay_end);

  addInfoVars("NAV_X, NAV_Y, NAV_SPEED, NAV_HEADING");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_Attractor::setParam(string g_param, string g_val) {
  if (IvPContactBehavior::setParam(g_param, g_val))
    return (true);

  if (g_param == "dist_priority_interval") {
    g_val = stripBlankEnds(g_val);
    vector<string> svector = parseString(g_val, ',');
    if (svector.size() != 2)
      return (false);
    if (!isNumber(svector[0]) || !isNumber(svector[1]))
      return (false);

    double dval1 = atof(svector[0].c_str());
    double dval2 = atof(svector[1].c_str());
    if ((dval1 < 0) || (dval2 < 0) || (dval1 > dval2))
      return (false);
    m_min_priority_range = dval1;
    m_max_priority_range = dval2;
    return (true);
  } else if (g_param == "cpa_utility_interval") {
    g_val = stripBlankEnds(g_val);
    vector<string> svector = parseString(g_val, ',');
    if (svector.size() != 2)
      return (false);
    if (!isNumber(svector[0]) || !isNumber(svector[1]))
      return (false);

    double dval1 = atof(svector[0].c_str());
    double dval2 = atof(svector[1].c_str());
    if ((dval1 < 0) || (dval2 < 0) || (dval1 >= dval2))
      return (false);
    m_max_util_cpa_dist = dval1;
    m_min_util_cpa_dist = dval2;
    return (true);
  } else if (g_param == "giveup_range") {
    double dval = atof(g_val.c_str());
    if ((dval < 0) || (!isNumber(g_val)))
      return (false);
    m_giveup_range = dval;
    return (true);
  }

  else if (g_param == "patience") {
    double dval = atof(g_val.c_str());
    if ((dval < 0) || (dval > 100) || (!isNumber(g_val)))
      return (false);
    m_patience = dval;
    return (true);
  }

  else if (g_param == "strength") {
    double dval = atof(g_val.c_str());
    if ((dval < 0) || (dval > 100) || (!isNumber(g_val)))
      return (false);
    strength = dval;
    return (true);
  }

  return (false);
}

//-----------------------------------------------------------
// Procedure: onSetParamComplete

void BHV_Attractor::onSetParamComplete() {
  m_trail_point.set_label(m_us_name + "_attractor");
  m_trail_point.set_active("false");
  string bhv_tag = tolower(getDescriptor());
}

//-----------------------------------------------------------
// Procedure: onIdleState

void BHV_Attractor::onIdleState() {
  // Below commented out now since we can accomlish this now with
  // runflags, idleflags, activeflags, inactiveflags

  // postMessage("ATTRACTOR_ACTIVE", 0);
  // postMessage("ATTRACTED", 0);
}

//-----------------------------------------------------------
// Procedure: onRunToIdleState

void BHV_Attractor::onRunToIdleState() { postErasableTrailPoint(); }

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_Attractor::onRunState() {
  // Below handled now with active/inactive flags - mikerb, Aug10,08
  // postMessage("ATTRACTOR_ACTIVE", 1);

  if (m_contact == "") {
    postEMessage("contact ID not set.");
    return (0);
  }

  // Set m_osx, m_osy, m_osh, m_osv, m_cnx, m_cny, m_cnh, m_cnv
  if (!updatePlatformInfo())
    return (0);

  double curr_time = getBufferCurrTime();
  double cnutc = m_cnos.cnutc();
  postIntMessage("ATTRACTOR_CONTACT_TIME", cnutc);
  postIntMessage("ATTRACTOR_DELTA_TIME", curr_time - cnutc);

  // Calculate the relevance first. If zero-relevance, we won't
  // bother to create the objective function.
  double relevance = getRelevance(m_osx, m_osy, m_cnx, m_cny);

  if (relevance <= 0) {
    // Below handled now with active/inactive flags - mikerb, Aug10,08
    // postMessage("ATTRACTED", 0);
    return (0);
  }

  // Below handled now with active/inactive flags - mikerb, Aug10,08
  // postMessage("ATTRACTED", 1);

  m_trail_point.set_vertex(m_cnx, m_cny);
  postViewableTrailPoint();

  double dist = hypot((m_osx - m_cnx), (m_osy - m_cny));

  IvPFunction *ipf = 0;

  // first make sure we point in the right direction

  double angle = headingToRadians(m_osh);
  double vx = cos(angle);
  double vy = sin(angle);

  double cos_ang = vx * (m_cnx - m_osx) + vy * (m_cny - m_osy);
  double current_relevance = relevance;

  if (cos_ang < 0) {
    double hdg_ang = relAng(m_osx, m_osy, m_cnx, m_cny);

    ZAIC_PEAK hdg_zaic(m_domain, "course");
    hdg_zaic.setSummit(hdg_ang);
    hdg_zaic.setValueWrap(true);
    hdg_zaic.setPeakWidth(120);
    hdg_zaic.setBaseWidth(60);
    hdg_zaic.setSummitDelta(10.0);
    hdg_zaic.setMinMaxUtil(0, 100);
    IvPFunction *hdg_ipf = hdg_zaic.extractIvPFunction();

    ZAIC_PEAK spd_zaic(m_domain, "speed");

    spd_zaic.setSummit(m_cnv);
    spd_zaic.setPeakWidth(0.1);
    spd_zaic.setBaseWidth(2.0);
    spd_zaic.setSummitDelta(50.0);

    IvPFunction *spd_ipf = spd_zaic.extractIvPFunction();

    OF_Coupler coupler;
    ipf = coupler.couple(hdg_ipf, spd_ipf);

    current_relevance = 2 * relevance;
  } else if (dist > m_min_priority_range) {
    AOF_AttractorCPA aof(m_domain);
    aof.setParam("cnlat", m_cny);
    aof.setParam("cnlon", m_cnx);
    aof.setParam("cncrs", m_cnh);
    aof.setParam("cnspd", m_cnv);
    aof.setParam("oslat", m_osy);
    aof.setParam("oslon", m_osx);
    aof.setParam("tol", m_time_on_leg);
    aof.setParam("patience", m_patience);
    aof.setParam("min_util_cpa_dist", m_min_util_cpa_dist);
    aof.setParam("max_util_cpa_dist", m_max_util_cpa_dist);
    bool ok1 = aof.initialize();

    if (!ok1) {
      postWMessage("Error in initializing AOF_AttractorCPA.");
      return (0);
    }

    OF_Reflector reflector(&aof);
    reflector.create(m_build_info);
    if (!reflector.stateOK())
      postWMessage(reflector.getWarnings());
    else
      ipf = reflector.extractIvPFunction();
  } else {
    ZAIC_PEAK hdg_zaic(m_domain, "course");
    hdg_zaic.setSummit(m_cnh);
    hdg_zaic.setValueWrap(true);
    hdg_zaic.setPeakWidth(120);
    hdg_zaic.setBaseWidth(60);
    hdg_zaic.setSummitDelta(10.0);
    hdg_zaic.setMinMaxUtil(0, 100);
    IvPFunction *hdg_ipf = hdg_zaic.extractIvPFunction();

    ZAIC_PEAK spd_zaic(m_domain, "speed");

    spd_zaic.setSummit(m_cnv);
    spd_zaic.setPeakWidth(0.1);
    spd_zaic.setBaseWidth(2.0);
    spd_zaic.setSummitDelta(50.0);

    IvPFunction *spd_ipf = spd_zaic.extractIvPFunction();

    OF_Coupler coupler;
    ipf = coupler.couple(hdg_ipf, spd_ipf);
  }

  // Check for properly created IvPFunction before operating on it.
  if (ipf) {
    ipf->getPDMap()->normalize(0.0, 100.0);
    ipf->setPWT(current_relevance * m_priority_wt);
  }

  return (ipf);
}

//-----------------------------------------------------------
// Procedure: getRelevance

double BHV_Attractor::getRelevance(double osX, double osY, double cnX,
                                   double cnY) {
  double range_max = m_max_priority_range;
  double range_min = m_min_priority_range;

  // Should be caught when setting the parameters, but check again
  if ((range_max < 0) || (range_min < 0) || (range_min > range_max)) {
    postWMessage("Priority Range Error");
    return (0);
  }

  double dist = hypot((osX - cnX), (osY - cnY));
  postIntMessage("ATTRACTOR_RANGE_TO_CONTACT", dist);

  if ((m_giveup_range > 0) && (dist > m_giveup_range))
    return (0);

  double total_range = range_max - range_min;
  // double exp_wgt= 1.0-exp(-10*(dist-range_min)/range_max);
  // double exp_wgt = 1;
  double pct = 1;
  // if total_range==0 one of the two cases will result in a return

  if (dist >= range_max)
    pct = 1 + strength;
  else if (dist < range_min)
    pct = dist / range_min;
  else if (total_range > 0)
    pct = (1 + strength * (dist - range_min) / total_range);
  else
    pct = 1;

  // Changed to be ATTACTOR_RELEVANCE instead of TRAIL_RELEVANCE and
  // to post as integer between 0-100, not 0-1.0 to reduce postings.
  // mikerb Aug 09, 2008
  postIntMessage("ATTRACTOR_RELEVANCE", pct * 100);
  return (pct);
}

//-----------------------------------------------------------
// Procedure: postViewableTrailPoint

void BHV_Attractor::postViewableTrailPoint() {
  m_trail_point.set_active(true);
  string spec = m_trail_point.get_spec();
  postMessage("VIEW_POINT", spec);
}

//-----------------------------------------------------------
// Procedure: postErasableTrailPoint

void BHV_Attractor::postErasableTrailPoint() {
  m_trail_point.set_active(false);
  string spec = m_trail_point.get_spec();
  postMessage("VIEW_POINT", spec);
}
