#--------------------------------------------------------
# The CMakeLists.txt for:                 app_survey_plan
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS wsock32)
else (${WIN32})
  SET(SYSTEM_LIBS m)
endif (${WIN32})

# Set GL Libraries
if (${APPLE})
  SET(GL_LIBS fltk_gl)
else (${APPLE})
  SET(GL_LIBS fltk_gl GL)
endif (${APPLE})

SET(SRC 
  SurveyPlan_Viewer.cpp
  SurveyPlan_GUI.cpp
  main.cpp
  )

ADD_EXECUTABLE(survey_plan ${SRC})

TARGET_LINK_LIBRARIES(survey_plan
  marineview
  MOOSGeodesy
  contacts
  geometry
  survey
  mbutil
  fltk 
  ${GL_LIBS}
  dl
  tiff
  ${SYSTEM_LIBS}
)
  
# http://developer.apple.com/qa/qa2007/qa1567.html
IF (${APPLE})
  SET_TARGET_PROPERTIES(survey_plan PROPERTIES	  
    LINK_FLAGS "-framework OpenGL -framework AGL -framework Cocoa"
    )
ENDIF (${APPLE})

