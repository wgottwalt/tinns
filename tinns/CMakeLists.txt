CMAKE_MINIMUM_REQUIRED  (VERSION 3.0)
PROJECT                 (TinNS LANGUAGES CXX)

# needed packages
SET                     (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")
FIND_PACKAGE            (MYSQL REQUIRED)
FIND_PACKAGE            (SQLite3 REQUIRED)
FIND_PACKAGE            (PCRE REQUIRED)
FIND_PACKAGE            (ZLIB REQUIRED)
FIND_PACKAGE            (Lua REQUIRED)

# includes and paths/directories
SET                     (CMAKE_INCLUDE_CURRENT_DIR on)
INCLUDE_DIRECTORIES     (${PROJECT_SOURCE_DIR})
SET                     (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
SET                     (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# compiler settings
IF                      (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    MESSAGE             ("--> building on Linux")
    SET                 (CMAKE_CXX_FLAGS "-std=c++17 -pthread -fPIC -W -Wall -Wextra -Os -s")
    SET                 (CMAKE_C_FLAGS "-std=c18 -pthread -fPIC -W -Wall -Wextra -Os -s")
ELSE                    ()
    MESSAGE             (FATAL_ERROR "--> unsupported build host: ${CMAKE_SYSTEM_NAME}")
ENDIF                   ()
# compiler options
SET                     (USE_SQLITE3 false CACHE BOOL "Use SQLITE3 instead of MySQL")
SET                     (USE_MYSQL_INCLUDE_DIR true CACHE BOOL "MySQL includes in system path")
IF                      (USE_SQLITE3)
    ADD_DEFINITIONS     (-DSQLITE3)
ENDIF                   (USE_SQLITE3)
IF                      (USE_MYSQL_INCLUDE_DIR)
    ADD_DEFINITIONS     (-DMYSQL_INC_DIR)
ENDIF                   (USE_MYSQL_INCLUDE_DIR)

# sources
ADD_SUBDIRECTORY        (common)
ADD_SUBDIRECTORY        (gameserver)
ADD_SUBDIRECTORY        (infoserver)
ADD_SUBDIRECTORY        (patchserver)
