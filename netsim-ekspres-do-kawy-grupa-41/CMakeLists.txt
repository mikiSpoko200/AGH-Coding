cmake_minimum_required(VERSION 3.13)
project(net_simulation)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -Werror -Wpedantic -pedantic-errors -Werror=switch)

include_directories(include)

set(SOURCE_FILES
        src/package.cpp
        src/storage_types.cpp
        src/nodes.cpp
        src/helpers.cpp
        src/factory.cpp
        src/reports.cpp
        src/simulation.cpp
        )

set(rak src/factory.cpp)

add_executable(test ${rak} main.cpp)

add_executable(main__debug ${SOURCE_FILES} main.cpp)

target_compile_definitions(main__debug PUBLIC EXERCISE_ID=EXERCISE_ID_FACTORY)

add_executable(${PROJECT_NAME}__debug ${SOURCE_FILES} main.cpp)

set(SOURCE_FILES_TESTS_package
        test/test_package.cpp
        )

set(SOURCE_FILES_TESTS_storage_types
        test/test_storage_types.cpp
        )

set(SOURCE_FILES_TESTS_nodes
        test/test_nodes.cpp
        )

set(SOURCE_FILES_TESTS_factory
        test/test_Factory.cpp
        )

set(SOURCE_FILES_TESTS_factoryIO
        test/test_factory_io.cpp
        )

set(SOURCE_FILES_TESTS_reports
        test/test_reports.cpp
        )

set(SOURCE_FILES_TESTS_simulation
        test/test_simulate.cpp
        )

# Trzeba dodawać nazwy konfiguracji: test_<nazwa> zgodne z definicjami powyżej
list(APPEND name_list package nodes storage_types factory factoryIO reports simulation)

foreach(name IN LISTS name_list)

    set(TEST_FILES SOURCE_FILES_TESTS_${name})
    add_executable(${PROJECT_NAME}__test_${name} ${SOURCE_FILES} ${${TEST_FILES}} test/main_gtest.cpp)

    target_compile_definitions(${PROJECT_NAME}__test_${name} PUBLIC EXERCISE_ID=EXERCISE_ID_FACTORY)

    target_include_directories(${PROJECT_NAME}__test_${name} PUBLIC
            googletest-master/googlemock/include
            googletest-master/googletest/include
            mocks
            )

    target_link_libraries(${PROJECT_NAME}__test_${name} gmock)

endforeach()

add_subdirectory(googletest-master)
