cmake_minimum_required(VERSION 2.8)	

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(simplearchiver)
			
set(SOURCE_EXE main.c)
		
set(SOURCE_LIB archiver.c)			

add_library(archiver STATIC ${SOURCE_LIB})	

add_executable(sarc ${SOURCE_EXE})	

target_link_libraries(sarc archiver)		
