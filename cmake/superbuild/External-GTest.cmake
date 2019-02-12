message( "External project - GTest" )
set (GTest_VERSION 1.8.0)

# Build Static Libs for GTest as that is recommended
ExternalProject_Add(GTest
    DEPENDS Glog
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-${GTest_VERSION}
    CMAKE_GENERATOR ${CMAKE_GENERATOR}
    UPDATE_COMMAND ""
    CMAKE_ARGS
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
        -DBUILD_SHARED_LIBS:BOOL=OFF
        -DBUILD_GTEST:BOOL=ON
        -DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}
        -DBUILD_GMOCK:BOOL=ON
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
)


set(GTEST_ROOT "${CMAKE_INSTALL_PREFIX}" CACHE PATH "GTest Root Dir")
