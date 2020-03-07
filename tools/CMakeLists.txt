CMAKE_MINIMUM_REQUIRED  (VERSION 3.0)
PROJECT                 (TinNS_Tools LANGUAGES CXX)

# needed packages
FIND_PACKAGE            (ZLIB REQUIRED)

# includes and paths/directories
SET                     (CMAKE_INCLUDE_CURRENT_DIR on)
INCLUDE_DIRECTORIES     (${PROJECT_SOURCE_DIR})
SET                     (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# compiler settings
IF                      (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    MESSAGE             (">>>> building on Linux")
    SET                 (CMAKE_CXX_FLAGS "-std=c++17 -pthread -fPIC -W -Wall -Wextra -Os -s")
    SET                 (CMAKE_C_FLAGS "-std=c18 -pthread -fPIC -W -Wall -Wextra -Os -s")
ELSE                    ()
    MESSAGE             (FATAL_ERROR ">>>> unsupported build host: ${CMAKE_SYSTEM_NAME}")
ENDIF                   ()

# executables
ADD_EXECUTABLE          (cleandepfile deptool/cleandepfile.cxx)

ADD_EXECUTABLE          (pak_decompress pak_decompress/pak_decompress.cxx)
TARGET_LINK_LIBRARIES   (pak_decompress ${ZLIB_LIBRARY})

ADD_EXECUTABLE          (vfs_viewer vfs_viewer/vfs_viewer.cxx)
