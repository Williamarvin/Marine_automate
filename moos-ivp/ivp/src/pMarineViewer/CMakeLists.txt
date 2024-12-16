cmake_minimum_required(VERSION 3.10)
project(pMarineViewer)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find CURL
find_package(CURL REQUIRED)

# Find OpenCV
find_package(OpenCV REQUIRED)  # Ensure OpenCV is found

# Find OpenGL
find_package(OpenGL REQUIRED)  # Add this line to find OpenGL

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pMarineViewer ${SRC})

TARGET_LINK_LIBRARIES(pMarineViewer         
  ${MOOS_LIBRARIES}
  ${MOOSGeodesy_LIBRARIES}
  ${SYSTEM_LIBS}
  genutil
  marineview
  contacts
  geometry
  ucommand
  realm
  apputil
  mbutil
  bhvutil
  fltk 
  fltk_gl 
  dl
  tiff 
  ${CURL_LIBRARIES} 
  ${OpenCV_LIBS}  # Link against OpenCV libraries
  ${OPENGL_LIBRARIES}  # Link against OpenGL libraries
)

include_directories(${CURL_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})  # Include directories for CURL and OpenCV

ADD_TEST(MyTest1 ${EXECUTABLE_OUTPUT_PATH}/pMarineViewer --version)