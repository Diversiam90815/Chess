function(AddCoverage target)
  if (MSVC)  # skip on MSVC/Windows
    message(STATUS "Coverage disabled on MSVC for target ${target}")
    return()
  endif()

  find_program(LCOV_PATH lcov)
  find_program(GENHTML_PATH genhtml)
  if (NOT LCOV_PATH OR NOT GENHTML_PATH)
    message(STATUS "lcov/genhtml not found; skipping coverage for ${target}")
    return()
  endif()

  add_custom_target(coverage-${target}
    COMMAND ${LCOV_PATH} -d . --zerocounters
    COMMAND $<TARGET_FILE:${target}>
    COMMAND ${LCOV_PATH} -d . --capture -o coverage.info
    COMMAND ${LCOV_PATH} -r coverage.info '/usr/include/*' -o filtered.info
    COMMAND ${GENHTML_PATH} -o coverage-${target} filtered.info --legend
    COMMAND ${CMAKE_COMMAND} -E rm -f coverage.info filtered.info
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
endfunction()

function(CleanCoverage target)
  add_custom_command(TARGET ${target} PRE_BUILD COMMAND
                     find ${CMAKE_BINARY_DIR} -type f
                     -name '*.gcda' -exec cmake -E rm {} +)
endfunction()

function(InstrumentForCoverage target)
  if (CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_options(${target} 
                           PRIVATE --coverage -fno-inline)
    target_link_options(${target} PUBLIC --coverage)
  endif()
endfunction()
