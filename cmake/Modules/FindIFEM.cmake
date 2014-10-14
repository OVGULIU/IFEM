# IFEM uses C++ and Fortran
ENABLE_LANGUAGE(CXX)
ENABLE_LANGUAGE(Fortran)

# Custom profiles

IF(NOT IFEM_BUILD_TYPE)
  SET(IFEM_BUILD_TYPE ${CMAKE_BUILD_TYPE})
ENDIF(NOT IFEM_BUILD_TYPE)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release)
ENDIF(NOT CMAKE_BUILD_TYPE)

IF(${CMAKE_BUILD_TYPE} MATCHES "Nopt")
  SET(CMAKE_BUILD_TYPE Debug)
ELSEIF(${CMAKE_BUILD_TYPE} MATCHES "Debug-MPI")
  SET(CMAKE_BUILD_TYPE Debug)
ELSEIF(${CMAKE_BUILD_TYPE} MATCHES "Nomp")
  SET(CMAKE_BUILD_TYPE Release)
ELSEIF(${CMAKE_BUILD_TYPE} MATCHES "Release-MPI")
  SET(CMAKE_BUILD_TYPE Release)
ENDIF(${CMAKE_BUILD_TYPE} MATCHES "Nopt")

IF(NOT IFEM_FORCE_SYSTEM_LIB AND NOT IFEM_AS_SUBMODULE)
  FIND_PATH(IFEM_PATH
    NAMES ${IFEM_BUILD_TYPE}/IFEM.h
    PATHS ${PROJECT_SOURCE_DIR}/..
          ${PROJECT_SOURCE_DIR}/../..
          ${PROJECT_SOURCE_DIR}/../../.. NO_DEFAULT_PATHS)
    SET(IFEM_H_PATH ${IFEM_PATH}/${IFEM_BUILD_TYPE})
  IF(NOT IFEM_PATH)
  FIND_PATH(IFEM_PATH
    NAMES IFEM.h
    PATHS ${PROJECT_SOURCE_DIR}/..
          ${PROJECT_SOURCE_DIR}/../..
          ${PROJECT_SOURCE_DIR}/../../.. NO_DEFAULT_PATHS)
    SET(IFEM_H_PATH ${IFEM_PATH})
  ENDIF(NOT IFEM_PATH)
ENDIF(NOT IFEM_FORCE_SYSTEM_LIB AND NOT IFEM_AS_SUBMODULE)

IF(IFEM_PATH OR IFEM_AS_SUBMODULE)
  IF(IFEM_AS_SUBMODULE)
    # Build wants IFEM as a submodule
    MESSAGE(STATUS "Building IFEM as a submodule")
    IF(CMAKE_CROSSCOMPILING)
      set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE)
    endif()
    FIND_PATH(IFEM_PATH NAMES src/IFEM.h.in
                        PATHS ${PROJECT_SOURCE_DIR}/..
                              ${PROJECT_SOURCE_DIR}/../..
                              ${PROJECT_SOURCE_DIR}/../../.. NO_DEFAULT_PATHS)
    IF(NOT IFEM_PATH)
      MESSAGE(FATAL_ERROR "IFEM cannot be located, and we want it as a submodule")
    ENDIF(NOT IFEM_PATH)
    IF(CMAKE_CROSSCOMPILING)
      set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    endif()
    ADD_SUBDIRECTORY(${IFEM_PATH} IFEM.dir)
    INCLUDE(${CMAKE_BINARY_DIR}/IFEM.dir/IFEMFlags.cmake)
    SET(IFEM_LIBRARIES IFEM)
    IF(IFEM_USE_GETBEAM)
      set(IFEM_LIBRARIES ${IFEM_LIBRARIES} ${CMAKE_CURRENT_BINARY_DIR}/IFEM.dir/lib/libGETBeam.a)
    ENDIF(IFEM_USE_GETBEAM)
  ELSE(IFEM_AS_SUBMODULE)
    # Build is in-tree
    MESSAGE(STATUS "Using in-tree IFEM")
    SET(IFEM_INTREE_BUILD ON)
    SET(IFEM_INCLUDES ${IFEM_H_PATH}
                      ${IFEM_PATH}/src/ASM
                      ${IFEM_PATH}/src/Eig
                      ${IFEM_PATH}/src/LinAlg
                      ${IFEM_PATH}/src/SIM
                      ${IFEM_PATH}/src/Utility)

    FIND_LIBRARY(IFEM_LIBRARIES
      NAMES IFEM
      PATHS ${IFEM_PATH}/${IFEM_BUILD_TYPE}/lib
            ${IFEM_PATH}/lib
      NO_DEFAULT_PATH)
    IF(NOT IFEM_LIBRARIES)
      MESSAGE(WARNING "Could not find the in-tree IFEM library, "
        "we assume it will be built into a build-type dir")
    ENDIF(NOT IFEM_LIBRARIES)
    IF (NOT BUILD_SHARED_LIBS)
      FIND_PACKAGE(IFEMDeps REQUIRED)
      SET(IFEM_LIBRARIES ${IFEM_LIBRARIES} ${IFEM_DEPLIBS})
    ENDIF(NOT BUILD_SHARED_LIBS)
    IF(NOT IFEM_USE_SYSTEM_TINYXML)
      SET(IFEM_INCLUDES ${IFEM_INCLUDES}
                        ${IFEM_PATH}/3rdparty/tinyxml)
    ENDIF(NOT IFEM_USE_SYSTEM_TINYXML)
    IF(IFEM_USE_GETBEAM)
      FIND_LIBRARY(GETB_LIBRARIES
        NAMES GETBeam
        PATHS ${IFEM_PATH}/${IFEM_BUILD_TYPE}/lib
              ${IFEM_PATH}/lib
        NO_DEFAULT_PATH)
      IF(GETB_LIBRARIES)
        SET(IFEM_LIBRARIES ${IFEM_LIBRARIES} ${GETB_LIBRARIES})
      ELSE(GETB_LIBRARIES)
        SET(IFEM_USE_GETBEAM OFF)
      ENDIF(GETB_LIBRARIES)
    ENDIF(IFEM_USE_GETBEAM)
  ENDIF(IFEM_AS_SUBMODULE)
  add_custom_target(check-commits
                    COMMAND ${CMAKE_COMMAND}
                            -DPROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}
                            -DCMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}
                            -P ${IFEM_PATH}/Apps/Common/cmake/CheckCommits.cmake)
ELSE(IFEM_PATH OR IFEM_AS_SUBMODULE)
  # Build wants system IFEM
  IF(NOT DEFINED FORCE_SYSTEM_IFEM OR NOT "${FORCE_SYSTEM_IFEM}")
    MESSAGE(STATUS "No in-tree libIFEM found, looking for system library")
  ENDIF(NOT DEFINED FORCE_SYSTEM_IFEM OR NOT "${FORCE_SYSTEM_IFEM}")

  FIND_PATH(IFEM_INCLUDES
    NAMES SIMbase.h
    PATHS $ENV{HOME}/include
    PATH_SUFFIXES IFEM)

  FIND_LIBRARY(IFEM_LIBRARIES
    NAMES IFEM
    PATHS $ENV{HOME}/lib)

  # system lib always uses system tixml
  FIND_PACKAGE(TinyXML REQUIRED)
  # The damn DSO's on ubuntu need us to explicitly link these
  FIND_PACKAGE(CBLAS REQUIRED)
  FIND_PACKAGE(LAPACK REQUIRED)
  FIND_PACKAGE(GoTools REQUIRED)
  # Need these as they determine ABI (c++-11 vs c++-03)
  SET(IFEM_CXX_FLAGS ${IFEM_CXX_FLAGS} ${GoTools_CXX_FLAGS})
  SET(IFEM_DEPLIBS ${TINYXML_LIBRARIES}
                   ${CBLAS_LIBRARIES}
                   ${LAPACK_LIBRARIES}
                   ${GoTools_LIBRARIES})
  SET(IFEM_H_PATH ${IFEM_INCLUDES})
ENDIF(IFEM_PATH OR IFEM_AS_SUBMODULE)

IF(NOT IFEM_AS_SUBMODULE)
  INCLUDE(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(IFEM DEFAULT_MSG
                                    IFEM_LIBRARIES)

  # Export version information
  IF(IFEM_INCLUDES)
    EXECUTE_PROCESS(COMMAND cat "${IFEM_H_PATH}/IFEM.h" OUTPUT_VARIABLE IFEM_HEADER)
    STRING(REGEX MATCH "IFEM_VERSION_MAJOR ([0-9]+)" IFEM_VERSION_MAJOR ${IFEM_HEADER})
    STRING(REGEX REPLACE "IFEM_VERSION_MAJOR ([0-9]+)" "\\1" IFEM_VERSION_MAJOR "${IFEM_VERSION_MAJOR}")
    STRING(REGEX MATCH "IFEM_VERSION_MINOR ([0-9]+)" IFEM_VERSION_MINOR ${IFEM_HEADER})
    STRING(REGEX REPLACE "IFEM_VERSION_MINOR ([0-9]+)" "\\1" IFEM_VERSION_MINOR "${IFEM_VERSION_MINOR}")
    STRING(REGEX MATCH "IFEM_VERSION_PATCH ([0-9]+)" IFEM_VERSION_PATCH ${IFEM_HEADER})
    STRING(REGEX REPLACE "IFEM_VERSION_PATCH ([0-9]+)" "\\1" IFEM_VERSION_PATCH "${IFEM_VERSION_PATCH}")
    SET(IFEM_VERSION "${IFEM_VERSION_MAJOR}.${IFEM_VERSION_MINOR}.${IFEM_VERSION_PATCH}")
    SET(IFEM_ABI_VERSION ${IFEM_VERSION_MAJOR}.${IFEM_VERSION_MINOR})
  ENDIF(IFEM_INCLUDES)

  SET(IFEM_LIBRARIES ${IFEM_LIBRARIES}
                     ${IFEM_DEPLIBS})
  SET(IFEM_INCLUDES ${IFEM_INCLUDES} ${IFEM_DEPINCLUDES})
ENDIF(NOT IFEM_AS_SUBMODULE)

IF(IFEM_TEST_MEMCHECK)
  include(CTest)
ENDIF()

# Needed as we have templates using these flags
ENABLE_LANGUAGE(CXX)
IF(CMAKE_CXX_COMPILER_ID MATCHES Intel)
  SET(IFEM_CXX_FLAGS "${IFEM_CXX_FLAGS} -DUSE_MKL -mkl=sequential")
  SET(IFEM_BUILD_CXX_FLAGS "${IFEM_BUILD_CXX_FLAGS} -DUSE_MKL -mkl=sequential")
ELSE(CMAKE_CXX_COMPILER_ID MATCHES Intel)
  SET(IFEM_CXX_FLAGS "${IFEM_CXX_FLAGS} -DUSE_CBLAS")
  SET(IFEM_BUILD_CXX_FLAGS "${IFEM_BUILD_CXX_FLAGS} -DUSE_CBLAS")
ENDIF(CMAKE_CXX_COMPILER_ID MATCHES Intel)

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DINDEX_CHECK=2")
IF(VERBOSE_DEBUG GREATER 0)
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DINT_DEBUG=${VERBOSE_DEBUG}")
ENDIF(VERBOSE_DEBUG GREATER 0)
SET(IFEM_CXX_FLAGS "${IFEM_CXX_FLAGS} -DReal=double")

SET(IFEM_CONFIGURED 1)
if(NOT IFEM_TESTING_INCLUDED)
  include(IFEMTesting)
endif()
