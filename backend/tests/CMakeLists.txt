find_package(GTest REQUIRED)
include(GoogleTest)

# ═════════════════════════════════════════════════════════════════════════════
# TEST CONFIGURATION
# ═════════════════════════════════════════════════════════════════════════════

set(TEST_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../interfaces
    ${CMAKE_CURRENT_SOURCE_DIR}/../dto
    ${CMAKE_CURRENT_SOURCE_DIR}/../models
    ${CMAKE_CURRENT_SOURCE_DIR}/../exceptions
)

# Helper function to add a test with standard configuration
function(sv_add_test TARGET_NAME SOURCE_FILE)
    add_executable(${TARGET_NAME} ${SOURCE_FILE})
    
    # Include directories
    target_include_directories(${TARGET_NAME}
        PRIVATE ${TEST_INCLUDE_DIRS}
    )
    
    # Link libraries
    target_link_libraries(${TARGET_NAME}
        PRIVATE
            Threads::Threads
            Drogon::Drogon
            opencv_core
            opencv_imgproc
            opencv_dnn
            jsoncpp
            onnxruntime
            GTest::gtest_main
            GTest::gmock
    )
    
    # Compiler flags (from parent function)
    if(MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE
            /W4 /WX /permissive- /std:c++17
        )
    else()
        target_compile_options(${TARGET_NAME} PRIVATE
            -Wall -Wextra -Wpedantic
        )
    endif()
    
    # Sanitizers for tests
    if(SV_ENABLE_SANITIZERS AND NOT MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE
            -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer
        )
        target_link_options(${TARGET_NAME} PRIVATE
            -fsanitize=address -fsanitize=undefined
        )
    endif()
    
    # Register test with CTest
    gtest_discover_tests(${TARGET_NAME}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        PROPERTIES TIMEOUT 300
    )
endfunction()

# ═════════════════════════════════════════════════════════════════════════════
# TEST TARGETS
# ═════════════════════════════════════════════════════════════════════════════

sv_add_test(sv_detection_service_test   DetectionServiceTest.cpp)
sv_add_test(sv_onnx_detector_test       OnnxDetectorTest.cpp)
sv_add_test(sv_preprocessor_test        PreprocessorTest.cpp)

