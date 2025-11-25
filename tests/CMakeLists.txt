# Find GTest
find_package(GTest REQUIRED)

# Enable testing
enable_testing()

# Manually list test files (CMake will detect changes)
set(TEST_SOURCES
    test_treeNode.cpp
    test_commands.cpp
    test_treeNodePool.cpp
    test_treeStructure.cpp
    test_AVLTree.cpp
    test_findingSmallest.cpp
    test_countSmallerThan.cpp
    # Add new tests here manually
)

foreach(test_source ${TEST_SOURCES})
    get_filename_component(test_name ${test_source} NAME_WE)
    
    message(STATUS "Adding test: ${test_name}")
    
    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name} 
        GTest::gtest 
        GTest::gtest_main 
        SearchingTreeLib
    )
    add_test(NAME ${test_name} COMMAND ${test_name})
    set_tests_properties(${test_name} PROPERTIES
        LABELS "UnitTests"
        TIMEOUT 30
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endforeach()
