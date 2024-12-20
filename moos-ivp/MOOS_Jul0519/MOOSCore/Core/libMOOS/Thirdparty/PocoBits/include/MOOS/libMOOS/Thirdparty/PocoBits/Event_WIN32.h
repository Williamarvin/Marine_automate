
//
// Event_WIN32.h
//
// $Id: //poco/svn/Foundation/include/Poco/Event_WIN32.h#2 $
//
// Library: Foundation
// Package: Threading
// Module:  Event
//
// Definition of the EventImpl class for WIN32.
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

#ifndef MOOS_POCO_Foundation_Event_WIN32_INCLUDED
#define MOOS_POCO_Foundation_Event_WIN32_INCLUDED

#include "MOOS/libMOOS/Thirdparty/PocoBits/Exception.h"
#include "MOOS/libMOOS/Thirdparty/PocoBits/Foundation.h"
#include "MOOS/libMOOS/Thirdparty/PocoBits/UnWindows.h"

namespace MOOS {
namespace Poco {

class MOOS_POCO_Foundation_API EventImpl {
protected:
  EventImpl(bool autoReset = false);
  ~EventImpl();
  void setImpl();
  void waitImpl();
  bool waitImpl(long milliseconds);
  void resetImpl();

private:
  HANDLE _event;
};

//
// inlines
//
inline void EventImpl::setImpl() {
  if (!SetEvent(_event)) {
    throw SystemException("cannot signal event");
  }
}

inline void EventImpl::resetImpl() {
  if (!ResetEvent(_event)) {
    throw SystemException("cannot reset event");
  }
}

} // namespace Poco
} // namespace MOOS

#endif // MOOS_POCO_Foundation_Event_WIN32_INCLUDED
