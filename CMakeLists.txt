cmake_minimum_required(VERSION 3.25)

project(SyncRT)


find_package(Geant4 REQUIRED ui_all vis_all)
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
	${PROJECT_SOURCE_DIR}/vis.mac
 DESTINATION ./)