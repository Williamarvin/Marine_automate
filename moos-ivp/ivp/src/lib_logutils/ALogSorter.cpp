/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ALogSorter.cpp                                       */
/*    DATE: June 22nd, 2013                                      */
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

#include "ALogSorter.h"
#include "LogUtils.h"
#include "MBUtils.h"
#include <iostream>

using namespace std;

//--------------------------------------------------------
// Procedure: addEntry()
//   Returns: true if sorting was required

bool ALogSorter::addEntry(const ALogEntry &entry, bool forced_order) {
  // Case 1: list is empty, just add the new entry
  if (m_entries.size() == 0) {
    m_entries.push_back(entry);
    return (false);
  }

  // Case 2: new entry is a line with no or bogus timestamp due to being
  // a continuation of a previous line as with DB_VARSUMMARY. Give it a
  // forced time stamp which is just the timestamp at the end of the list
  if (forced_order) {
    ALogEntry entry_copy = entry;
    double time_stamp_prev = m_entries.back().time();
    entry_copy.setTimeStamp(time_stamp_prev);
    m_entries.push_back(entry_copy);
    return (false);
  }

  // Case 3: new entry is in correct order, just add new entry
  double time_stamp_this = entry.time();
  double time_stamp_prev = m_entries.back().time();
  if (time_stamp_this >= time_stamp_prev) {
    m_entries.push_back(entry);
    return (false);
  }

  // Case 3: new entry is out of order!!
#if 1
  if (time_stamp_this < m_entries.front().time())
    m_sort_warnings++;

  m_entries.push_back(entry);
  m_entries.sort();
#endif

#if 0
  bool inserted = false;
  list<ALogEntry>::reverse_iterator p;
  for(p=m_entries.rbegin(); ((p!=m_entries.rend()) && !inserted); p++) {
    if(time_stamp_this <= p->time()) {
      p--;
      m_entries.insert(p.base(), entry);
      inserted = true;
    }
  }
  if(!inserted) {
    m_entries.push_front(entry);
    cout << "Sort warning!!!" << endl;
  }
#endif

  return (true);
}

//--------------------------------------------------------
// Procedure: popEntry()

ALogEntry ALogSorter::popEntry() {
  ALogEntry return_entry;
  if (m_entries.size() > 0) {
    return_entry = m_entries.front();
    m_entries.pop_front();
  }

  // If we're not checking for duplicates, we're done now
  if (!m_check_for_duplicates)
    return (return_entry);

  // Go through the remaining entries and continue to check for
  // duplicate. We assume the entries are all sorted by now, so we can
  // stop checking once we hit an entry with a different timestamp

  double return_entry_tstamp = return_entry.time();

  list<ALogEntry>::iterator p;
  for (p = m_entries.begin(); p != m_entries.end();) {
    ALogEntry entry = *p;
    if (entry.time() != return_entry_tstamp)
      break;

    if (entry == return_entry)
      p = m_entries.erase(p);
    else
      p++;
  }

  return (return_entry);
}
