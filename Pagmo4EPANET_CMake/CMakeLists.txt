# CMakeList.txt : CMake project for Pagmo4EPANET_CMake, include source and define
# project specific logic here.
#

# Look for an installation of pagmo in the system.
find_package(Pagmo REQUIRED)
#find_package(Pagmo REQUIRED GLOBAL)

add_subdirectory(src)

# Add source to this project's executable.
add_executable (p4e_cm "Pagmo4EPANET_CMake.cpp" "Pagmo4EPANET_CMake.h" )


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET p4e_cm PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
target_link_libraries(p4e_cm PUBLIC Pagmo::pagmo Stuff)
#target_link_libraries(p4e_cm  Stuff)

target_include_directories(p4e_cm PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/Pagmo4EPANET_CMake/include"
                          )
