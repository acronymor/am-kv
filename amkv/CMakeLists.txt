
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

LIST(APPEND amkv_folder comm util lsm db)

FOREACH (folder ${amkv_folder})
    FILE(GLOB srcs ${folder}/*.cc)
    LIST(FILTER srcs EXCLUDE REGEX ${folder}/.*test.*)
    LIST(APPEND amkv_src ${srcs})

    FILE(GLOB test_srcs ${folder}/*test.cc)
    LIST(APPEND amkv_test_src ${test_srcs})
ENDFOREACH ()

ADD_LIBRARY(libamkv OBJECT ${amkv_src})
TARGET_LINK_LIBRARIES(libamkv glog::glog fmt)

ADD_EXECUTABLE(amkv main.cc)
TARGET_LINK_LIBRARIES(amkv libamkv)

IF (AMKV_BUILD_TEST)
    ADD_DEFINITIONS(-DAMKV_BUILD_TEST)
    LIST(APPEND test_libraries gtest gtest_main)

    FOREACH (test_item ${amkv_test_src})
        GET_FILENAME_COMPONENT(test_target_name ${test_item} NAME_WE)
        ADD_EXECUTABLE(${test_target_name} ${test_item})
        TARGET_LINK_LIBRARIES(${test_target_name} libamkv ${test_libraries})
        ADD_TEST(NAME ${test_target_name} COMMAND ${test_target_name})
    ENDFOREACH ()
ENDIF ()