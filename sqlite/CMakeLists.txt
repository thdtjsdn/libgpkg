#
# Static and shared library version of sqlite3.
# libgpkg is not linked against this version of sqlite3, it is only used to:
# - provide the sqlite3 and sqlite3ext headers
# - provide a sqlite3 binary with a known configuration for the unit tests
#
add_library( sqlite_static STATIC sqlite3.c )
set_target_properties( sqlite_static PROPERTIES OUTPUT_NAME "sqlite3" )
set_target_properties( sqlite_static PROPERTIES LIBRARY_OUTPUT_DIRECTORY static )

add_library( sqlite_shared SHARED sqlite3.c )
set_target_properties( sqlite_shared PROPERTIES OUTPUT_NAME "sqlite3" )
set_target_properties( sqlite_shared PROPERTIES LIBRARY_OUTPUT_DIRECTORY shared )

add_definitions( -D_HAVE_SQLITE_CONFIG_H=1 )

check_include_file(stdint.h HAVE_STDINT_H)
check_include_file(inttype.h HAVE_INTTYPES_H)
check_include_file(utime.h HAVE_UTIME_H)

check_function_exists(gmtime_r HAVE_GMTIME_R)
check_function_exists(localtime_r HAVE_LOCALTIME_R)
check_function_exists(posix_fallocate HAVE_POSIX_FALLOCATE)
check_function_exists(usleep HAVE_USLEEP)
check_function_exists(fdatasync HAVE_FDATASYNC)
check_function_exists(fullfsync HAVE_FULLFSYNC)
check_function_exists(strerror_r HAVE_STRERROR_R)

set( SQLITE_ENABLE_RTREE 1 )

find_package( Threads )
if( CMAKE_USE_PTHREADS_INIT )
  set( SQLITE_THREADSAFE 2 )
  target_link_libraries( sqlite_static "${CMAKE_THREAD_LIBS_INIT}" )
  target_link_libraries( sqlite_shared "${CMAKE_THREAD_LIBS_INIT}" )
elseif( CMAKE_USE_WIN32_THREADS_INIT )
  set( SQLITE_THREADSAFE 2 )
else()
  set( SQLITE_THREADSAFE 1 )
endif()

if( UNIX )
  target_link_libraries( sqlite_static "${CMAKE_DL_LIBS}" )
  target_link_libraries( sqlite_shared "${CMAKE_DL_LIBS}" )
endif()

configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/config.h" )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

#
# Strip 'lib' prefix when compiling for windows
#
if ( WIN32 )
  set_target_properties( sqlite_static PROPERTIES PREFIX "" )
  set_target_properties( sqlite_shared PROPERTIES PREFIX "" )
endif()

if ( MSVC )
  target_compile_definitions( sqlite_shared PRIVATE "-DSQLITE_API=__declspec\(dllexport\)" )
endif()


