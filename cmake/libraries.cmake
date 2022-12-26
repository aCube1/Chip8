include_guard()

set(SDL2_VERSION 2.24.2)
set(LIBS_DIR ${PROJECT_SOURCE_DIR}/libs)

function(link_default_libraries target)
  add_definitions(-DLOG_USE_COLOR=1)
  target_sources(
    ${target}
    PRIVATE
      ${LIBS_DIR}/log/log.c
  )
  target_include_directories(
    ${target}
    PRIVATE
      ${LIBS_DIR}/log
  )

  add_subdirectory(${LIBS_DIR}/cargs)

  find_package(SDL2 ${SDL2_VERSION} REQUIRED)

  target_link_libraries(${target}
    SDL2::SDL2
    cargs
    m
  )
endfunction()
