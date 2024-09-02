
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

LIST(APPEND amkv_folder db table)

FOREACH (folder ${amkv_folder})
    FILE(GLOB srcs ${folder}/*.cc)
    LIST(FILTER srcs EXCLUDE REGEX ${folder}/.*test.*)
    LIST(APPEND amkv_src ${srcs})

    FILE(GLOB test_srcs ${folder}/*test.cc)
    LIST(APPEND amkv_test_src ${test_srcs})
ENDFOREACH ()

ADD_LIBRARY(libamkv OBJECT ${amkv_src})
TARGET_LINK_LIBRARIES(libamkv)

ADD_EXECUTABLE(amkv main.cc)
TARGET_LINK_LIBRARIES(amkv libamkv)

IF (AMKV_BUILD_TEST)
    ADD_DEFINITIONS(-DAMKV_BUILD_TEST)

    MESSAGE("Hello GTest")
ENDIF ()