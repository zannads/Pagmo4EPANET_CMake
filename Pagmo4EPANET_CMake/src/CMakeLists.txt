IF(WIN32)
	add_library(epanet SHARED IMPORTED )
	set_property(TARGET epanet PROPERTY
	            IMPORTED_LOCATION "${EPANET_ROOT}/build/bin/Release/epanet2.dll")
	set_property(TARGET epanet PROPERTY
		        IMPORTED_IMPLIB "${EPANET_ROOT}/build/lib/Release/epanet2.lib")
ENDIF(WIN32)

IF(APPLE)
	add_library(epanet SHARED IMPORTED)
	set_target_properties(epanet PROPERTIES
						 IMPORTED_LOCATION "${EPANET_ROOT}/build/lib/libepanet2.dylib" )
ENDIF(APPLE)

add_library(Stuff "logger.cpp" "NET1.cpp")

target_link_libraries(Stuff PUBLIC Pagmo::pagmo epanet)

target_include_directories(Stuff PUBLIC 
							Pagmo::pagmo
							"${PROJECT_BINARY_DIR}"
							"${PROJECT_SOURCE_DIR}"
							"${CMAKE_CURRENT_SOURCE_DIR}"
							"${PROJECT_SOURCE_DIR}/Pagmo4EPANET_CMake/include"
							"${EPANET_ROOT}/include"
)
