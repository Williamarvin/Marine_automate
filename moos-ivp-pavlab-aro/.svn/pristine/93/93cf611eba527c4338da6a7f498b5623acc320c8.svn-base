#--------------------------------------------------------
# The CMakeLists.txt for:                    uFldJoustMgr
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  JoustMgr.cpp
  JoustMgr_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(uFldJoustMgr ${SRC})

TARGET_LINK_LIBRARIES(uFldJoustMgr
   ${MOOS_LIBRARIES}
   apputil
   contacts
   geometry
   mbutil
   m
   pthread)

