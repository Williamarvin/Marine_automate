/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: SplitHandler.cpp                                     */
/*    DATE: February 2nd, 2015                                   */
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

#include "SplitHandler.h"
#include "ColorParse.h"
#include "LogUtils.h"
#include "MBUtils.h"
#include "TermUtils.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

//--------------------------------------------------------
// Constructor()

SplitHandler::SplitHandler(string alog_file) {
  // Init config parameters
  m_alog_file = alog_file;
  m_verbose = false;
  m_progress = false;
  m_max_cache = 125; // Default limit for concurrent fopen fileptrs

  // Init state variables
  m_alog_file_confirmed = false;
  m_split_dir_prior = false;
  m_max_cache_exceeded = false;

  m_vip_cache.insert("NAV_X");
  m_vip_cache.insert("NAV_Y");
  m_vip_cache.insert("NAV_SPEED");
  m_vip_cache.insert("NAV_HEADING");
  m_vip_cache.insert("NAV_HEADING_OVER_GROUND");
  m_vip_cache.insert("NAV_LAT");
  m_vip_cache.insert("NAV_LONG");
  m_vip_cache.insert("DESIRED_RUDDER");
  m_vip_cache.insert("DESIRED_THRUST");
  m_vip_cache.insert("BHV_IPF");
  m_vip_cache.insert("PID_REPORT");
  m_vip_cache.insert("APP_LOG");
  m_vip_cache.insert("NODE_REPORT");
}

//--------------------------------------------------------
// Procedure: handle()

bool SplitHandler::handle() {
  bool ok = true;

  ok = ok && handlePreCheckALogFile();

  ok = ok && handlePreCheckSplitDir();

  ok = ok && handleMakeSplitFiles();

  ok = ok && handleMakeSplitSummary();

  if (m_split_dir_prior)
    ok = true;

  return (ok);
}

//--------------------------------------------------------
// Procedure: setMaxFilePtrCache()

void SplitHandler::setMaxFilePtrCache(unsigned int val) {
  m_max_cache = val;

  if (m_max_cache < 10)
    m_max_cache = 10;

  if (m_max_cache > 2000)
    m_max_cache = 2000;
}

//--------------------------------------------------------
// Procedure: handlePreCheckALogFile()
//   Purpose: Make sure we have a proper, readable alog file
//      Note: This function is public to support cases where a user
//            wishes to validate the alog file first. Perhaps in a
//            case where several split handlers are being used.

bool SplitHandler::handlePreCheckALogFile() {
  // Part 1: Check if alog_file has already been cofirmed
  if (m_alog_file_confirmed)
    return (true);

  // Part 2: Check if the alogfile ends in .alog
  if (!strEnds(m_alog_file, ".alog")) {
    cout << "Input file must be an alog file, with .alog suffix." << endl;
    return (false);
  }

  // Part 3: Ensure that the alog filename contains no white space
  if (strContainsWhite(m_alog_file)) {
    cout << "Input file must be an alog file, with no white space." << endl;
    return (false);
  }

  // Part 4: Ensure that input alog file exists and can be opened for reading.
  FILE *file_in = fopen(m_alog_file.c_str(), "r");
  if (!file_in) {
    cout << "file " << m_alog_file << " cannnot find or open." << endl;
    return (false);
  }
  fclose(file_in);

  m_alog_file_confirmed = true;
  return (true);
}

//--------------------------------------------------------
// Procedure: handleMakeSplitFiles()

bool SplitHandler::handleMakeSplitFiles() {
  FILE *file_in = fopen(m_alog_file.c_str(), "r");
  if (!file_in) {
    cout << "Unable to open [" << m_alog_file << "] exiting." << endl;
    return (false);
  }

  char carriage_return = 13;
  unsigned int lines_read = 0;

  bool done = false;
  while (!done) {
    string line_raw = getNextRawLine(file_in);

    if (m_progress) {
      lines_read++;
      if ((lines_read % 5000) == 0) {
        cout << "  Lines Read: " << uintToCommaString(lines_read);
        cout << carriage_return << flush;
      }
    }

    // cout << "line: [" << line_raw << "]" << endl;
    //  Check if the line has the timestamp
    if ((m_logstart.length() == 0) && strContains(line_raw, "LOGSTART")) {
      line_raw = findReplace(line_raw, "LOGSTART", "X");
      biteStringX(line_raw, 'X');
      m_logstart = line_raw;
      continue;
    }
    // Check if the line is a comment
    if ((line_raw.length() > 0) && (line_raw.at(0) == '%'))
      continue;
    // Check for end of file
    if (line_raw == "eof")
      break;

    // Otherwise handle a normal line
    string varname = getVarName(line_raw);

    // Replace slashes in variable names - filesystems get confused
    varname = findReplace(varname, "/", "_");

    // Reject any line that doesn't begin with a number
    string one_char = line_raw.substr(0, 1);
    if (!isNumber(one_char) || (varname == "DB_VARSUMMARY"))
      continue;

    string tstamp = getTimeStamp(line_raw);
    if (m_time_min == "")
      m_time_min = tstamp;
    m_time_max = tstamp;

    if ((varname == "VIEW_POINT") || (varname == "VIEW_POLYGON") ||
        (varname == "VIEW_SEGLIST") || (varname == "VIEW_CIRCLE") ||
        (varname == "GRID_INIT") || (varname == "VIEW_MARKER") ||
        (varname == "GRID_DELTA") || (varname == "VIEW_SEGLR") ||
        (varname == "VIEW_ARROW") || (varname == "VIEW_RANGE_PULSE") ||
        (varname == "VIEW_COMMS_PULSE"))
      varname = "VISUALS";

    // A measure implemented here to accommodate older alogfile formats where
    // the behavior name in BHV_IPF is foobar1234 where 1234 is the helm
    // iteration. In Release 15.4 , the format it foobar^1234. If the separator
    // is not found, we look to see if the behavior name ends with the current
    // helm iteration, and remove it. Luckily the current helm iteration is
    // apparently reliably logged before the BHV_IPF. So we note it here to
    // apply this measure. Bit of a hack, but should be not needed once the
    // newer format (with the '^' separator) is more widely adopted.
    if (varname == "IVPHELM_ITER") {
      string sval = getDataEntry(line_raw);
      string iter = biteString(sval, '.');
      m_curr_helm_iter = iter;
    }

    // Handle BHV_IPF: Break out into sep files for each behavior
    // P,waypt_return^445,1,1,H,16,445:waypt_return,2,35,1,100,D,
    if (varname == "BHV_IPF") {
      string sval = getDataEntry(line_raw);
      biteString(sval, ',');
      string bhv_name = biteString(sval, ',');
      if (strContains(bhv_name, '^'))
        bhv_name = biteString(bhv_name, '^');
      else
        bhv_name = findReplace(bhv_name, m_curr_helm_iter, "");
      varname = "BHV_IPF_" + bhv_name;
      m_bhv_names.insert(bhv_name);
    }

    // Handle APP_LOG: Break out into sep files for each MOOSApp
    if (varname == "APP_LOG") {
      string src = getSourceName(line_raw);
      varname = "APP_LOG_" + src;
      m_applogging_app_names.insert(src);
    }

    // Part 1: Determine the vehicle name if not already known
    // Typically the MOOSDB automatically names itself MOOSDB_COMMUNITY,
    // For example, MOOSDB_alpha. DB_TIME is published by the MOOSDB.
    if ((m_vname.length() == 0) && (varname == "DB_TIME")) {
      string var_src = getSourceName(line_raw);
      biteString(var_src, '_');
      m_vname = var_src;
    }

    if ((m_vtype.length() == 0) && ((varname == "NODE_REPORT_LOCAL") ||
                                    (varname == "NODE_REPORT_LOCAL_FIRST"))) {

      string sval = tolower(getDataEntry(line_raw));
      string vtype = tokStringParse(sval, "type", ',', '=');
      string vcolor = tokStringParse(sval, "color", ',', '=');
      string vlength = tokStringParse(sval, "length", ',', '=');
      if (vtype != "")
        m_vtype = vtype;
      if (vcolor != "")
        m_vcolor = vcolor;
      if (vlength != "")
        m_vlength = vlength;
    }

    // Part 2: Check if the file ptr for this variable already exists.
    // If not, create a new file pointer and add it to the map.
    bool cached_file_ptr = false;

    FILE *file_ptr = 0;
    if (m_file_ptr.count(varname) == 1) {
      file_ptr = m_file_ptr[varname];
      cached_file_ptr = true;
    } else {
      string new_file = m_basedir + "/" + varname + ".klog";
      errno = 0;
      FILE *new_ptr = fopen(new_file.c_str(), "a");
      if (new_ptr) {
        // 125 seems to be a safe bet for all OS types for allowing
        // simultaneously open file pointers. Everything after 125
        // will be slower.
        bool vip = false;
        if (m_vip_cache.count(varname) != 0)
          vip = true;

        if (vip || (m_file_ptr.size() <= m_max_cache)) {
          m_file_ptr[varname] = new_ptr;
          cached_file_ptr = true;
          if (m_verbose) {
            if (vip)
              cout << "VIP " << flush;
            cout << "Caching: " << varname;
            cout << " (" << m_file_ptr.size() << ")" << m_max_cache << endl;
          }
        } else
          m_max_cache_exceeded = true;

        file_ptr = new_ptr;
      } else {
        cout << "Unable to open new file for VarName: [[" << varname << "]]";
        cout << endl;
        cout << " full filename: [[" << new_file << "]]" << endl;
        cout << "Error: " << errno << endl;
        break;
      }
    }

    // Part 3: Update the type information
    if (m_var_type[varname] != "string") {
      string vardata = getDataEntry(line_raw);
      if (!isNumber(vardata))
        m_var_type[varname] = "string";
      else
        m_var_type[varname] = "double";
    }

    // Part 4: Update the source information
    string varsrc = getSourceNameNoAux(line_raw);
    m_var_srcs[varname].insert(varsrc);

    // Part 5: Write the line to the appropriate file
    fprintf(file_ptr, "%s\n", line_raw.c_str());
    if (!cached_file_ptr)
      fclose(file_ptr);
  }
  if (m_progress) {
    cout << termColor("blue");
    cout << "  Lines Read: " << uintToCommaString(lines_read) << endl;
    cout << termColor();
  }

  if (m_verbose)
    cout << "Done writing to klog files. Total files: " << m_file_ptr.size()
         << endl;

  // Close all the file pointers before finishing
  map<string, FILE *>::iterator p;
  for (p = m_file_ptr.begin(); p != m_file_ptr.end(); p++) {
    FILE *ptr = p->second;
    fclose(ptr);
  }

  if (m_max_cache_exceeded) {
    cout << "WARNING: Maximum concurrent fopen fileptr cache exceeded." << endl;
    cout << "This is not an error, but the alog file pre-splitting    " << endl;
    cout << "phase will be slower in these cases.                     " << endl;
    cout << "Total unique varnames: " << m_var_type.size() << endl;
  }

  if (file_in)
    fclose(file_in);

  return (true);
}

//--------------------------------------------------------
// Procedure: handleMakeSplitSummary()

bool SplitHandler::handleMakeSplitSummary() {
  string summary_file = m_basedir + "/summary.klog";

  FILE *f = fopen(summary_file.c_str(), "a");
  if (!f)
    return (false);

  string total_vars = uintToString(m_var_type.size());
  fprintf(f, "total_vars=%s\n", total_vars.c_str());

  fprintf(f, "logstart=%s\n", m_logstart.c_str());
  fprintf(f, "logtmin=%s\n", m_time_min.c_str());
  fprintf(f, "logtmax=%s\n", m_time_max.c_str());
  fprintf(f, "vname=%s\n", m_vname.c_str());
  if (m_vtype != "")
    fprintf(f, "vtype=%s\n", m_vtype.c_str());
  if (m_vcolor != "")
    fprintf(f, "vcolor=%s\n", m_vcolor.c_str());
  if (m_vlength != "")
    fprintf(f, "vlength=%s\n", m_vlength.c_str());

  if (m_bhv_names.size() != 0) {
    string bhvs = stringSetToString(m_bhv_names);
    fprintf(f, "bhvs=%s\n", bhvs.c_str());
  }

  if (m_applogging_app_names.size() != 0) {
    string apps = stringSetToString(m_applogging_app_names);
    fprintf(f, "applogging_apps=%s\n", apps.c_str());
  }

  map<string, string>::iterator p;
  for (p = m_var_type.begin(); p != m_var_type.end(); p++) {
    string varname = p->first;
    string vartype = p->second;

    set<string> srcs = m_var_srcs[varname];
    string str_srcs;
    set<string>::iterator p;
    for (p = srcs.begin(); p != srcs.end(); p++) {
      if (str_srcs != "")
        str_srcs += ":";
      str_srcs += *p;
    }

    fprintf(f, "var=%s, type=%s", varname.c_str(), vartype.c_str());
    fprintf(f, ", srcs=%s\n", str_srcs.c_str());
  }

  fclose(f);
  return (true);
}

//--------------------------------------------------------
// Procedure: handlePreCheckSplitDir
//   Purpose: (1) Make sure we don't already have the split directory
//            (2) Create and verify the split directory

bool SplitHandler::handlePreCheckSplitDir() {
  // Part 1: Make sure we have proper .alog file
  if (!m_alog_file_confirmed) {
    cout << "SplitHandler SetUp failed. Unconfirmed alog_file input." << endl;
    return (false);
  }

  // Part 2: Auto-build the base directory name if not provided explicitly.
  string basedir = m_given_dir;
  if (basedir == "") {
    basedir = m_alog_file;
    rbiteString(basedir, '.');
    basedir += "_alvtmp";
  }

  // Ensure that the base directory doesn't already exist.
  FILE *tmp1 = fopen(basedir.c_str(), "r");
  if (tmp1) {
    fclose(tmp1);
    string bit_basedir = rbiteString(basedir, '/');
    cout << "    Dir [" << bit_basedir << "] confirmed." << endl;
    m_split_dir_prior = true;
    return (false);
  }

  // Part 3: Create and Verify the split directory.
  // Make the base directory
  string cmd = "mkdir " + basedir;
  int result = system(cmd.c_str());
  if (result != 0)
    cout << "Possible err in SplitHandler syscmd mkdir" << endl;

  // Ensure that the base directory has indeed been created.
  FILE *tmp2 = fopen(basedir.c_str(), "r");
  if (!tmp2) {
    cout << "Cache dur [" << basedir << "] could not be created." << endl;
    return (false);
  }
  fclose(tmp2);

  m_basedir = basedir;
  return (true);
}
