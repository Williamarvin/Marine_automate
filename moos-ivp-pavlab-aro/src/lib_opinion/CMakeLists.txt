#--------------------------------------------------------
# The CMakeLists.txt for:                  lib_opinion
# Author(s):                              Tyler Paine
#--------------------------------------------------------


SET(SRC
  Option.cpp
  OpinionRecord.cpp
  )

SET(HEADERS
  Option.h
  Option.h
)

# Build Library
ADD_LIBRARY(opinion ${SRC})
