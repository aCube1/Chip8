include_guard()

set(SDL2_VERSION 2.24.2)
set(VENDOR_DIR ${PROJECT_SOURCE_DIR}/libs)

function(link_default_libraries target)
  find_package(SDL2 ${SDL2_VERSION} REQUIRED)

  target_link_libraries(${target}
    SDL2::SDL2
    m
  )

  add_definitions(-DLOG_USE_COLOR=1)
  target_sources(
    ${target}
    PRIVATE
      ${VENDOR_DIR}/log/log.c
  )
  target_include_directories(
    ${target}
    PRIVATE
      ${VENDOR_DIR}/log
  )
endfunction()
