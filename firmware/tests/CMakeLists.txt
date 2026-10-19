cmake_minimum_required(VERSION 3.16)

project(stackchan_firmware_tests LANGUAGES CXX)

enable_testing()

add_executable(motion_math_test
    motion_math_test.cpp
    ../main/stackchan/motion/motion_math.cpp
)

target_include_directories(motion_math_test PRIVATE
    ../main
)

target_compile_features(motion_math_test PRIVATE cxx_std_17)

add_test(NAME motion_math_test COMMAND $<TARGET_FILE:motion_math_test>)

add_executable(category_test navigation/category_test.cpp)
add_executable(raster_test navigation/raster_test.cpp)
add_executable(catalog_bounded_test navigation/catalog_bounded_test.cpp
    ../main/apps/app_launcher/view/effect_catalog/effect_catalog.cpp)
add_executable(runtime_stop_test navigation/runtime_stop_test.cpp)
foreach(target category_test raster_test catalog_bounded_test runtime_stop_test)
    target_include_directories(${target} PRIVATE ../main)
    target_compile_features(${target} PRIVATE cxx_std_17)
    add_test(NAME ${target} COMMAND $<TARGET_FILE:${target}>)
endforeach()
find_package(Python3 COMPONENTS Interpreter REQUIRED)
add_test(NAME screensaver_consumers_test COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/navigation/screensaver_consumers_test.py)
add_test(NAME launcher_navigation_test COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/navigation/launcher_test.py)
add_test(NAME generated_teardown_test COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/navigation/teardown_test.py)
set_tests_properties(generated_teardown_test PROPERTIES SKIP_RETURN_CODE 77)
add_test(NAME protocol_lifetime_test COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/navigation/protocol_lifetime_test.py)
set_tests_properties(protocol_lifetime_test PROPERTIES SKIP_RETURN_CODE 77)
add_test(NAME ble_stop_test COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/navigation/ble_stop_test.py)
add_executable(station_handoff_test navigation/station_handoff_test.cpp)
target_include_directories(station_handoff_test PRIVATE ../main)
target_compile_features(station_handoff_test PRIVATE cxx_std_17)
add_test(NAME station_handoff_test COMMAND station_handoff_test)
add_test(NAME station_sdk_test COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/navigation/station_sdk_test.py)
add_test(NAME phone_station_test COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/navigation/phone_station_test.py)
add_test(NAME station_event_test COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/navigation/station_event_test.py)

add_executable(screensaver_management_test navigation/screensaver_management_test.cpp)
target_include_directories(screensaver_management_test PRIVATE ../main)
target_compile_features(screensaver_management_test PRIVATE cxx_std_17)
add_test(NAME screensaver_management_test COMMAND screensaver_management_test)
add_test(NAME screensaver_management_ui_test COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/navigation/screensaver_management_ui_test.py)

add_test(NAME nested_usb_close_test COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/navigation/nested_usb_close_test.py)

add_test(NAME usb_actor_close_test COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/navigation/usb_actor_close_test.py)

add_test(NAME companion_entry_test COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/navigation/companion_entry_test.py)

add_executable(moire_removal_test navigation/moire_removal_test.cpp
    ../main/apps/app_launcher/view/effect_catalog/effect_catalog.cpp)
target_include_directories(moire_removal_test PRIVATE ../main)
target_compile_features(moire_removal_test PRIVATE cxx_std_17)
add_test(NAME moire_removal_test COMMAND moire_removal_test)
