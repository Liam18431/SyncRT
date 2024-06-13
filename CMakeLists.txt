cmake_minimum_required(VERSION 3.20)

project(SyncRT)


option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
	find_package(Geant4 REQUIRED ui_all vis_all)
else()
	find_package(Geant4 REQUIRED)
endif()

include(${Geant4_USE_FILE})

file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)

add_executable(${PROJECT_NAME} SyncRT.cc ${sources} ${headers})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} ${Geant4_LIBRARIES})


install(TARGETS ${PROJECT_NAME} DESTINATION ./)

install(FILES 
	${PROJECT_SOURCE_DIR}/plot_dose_cube.py
	${PROJECT_SOURCE_DIR}/spectrum.dat
 DESTINATION ./)