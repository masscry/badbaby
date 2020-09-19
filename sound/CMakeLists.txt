project(sound)

if(WIN32)
  set(BASS_ARCHIVE_URL "http://us.un4seen.com/files/bass24.zip")
  set(BASS_ARCHIVE_HASH e46b1718fb62bbdc8add935c52fc6e48d43b6d99)
  set(BASS_ROOT_PATH ${CMAKE_CURRENT_BINARY_DIR}/contrib/bass)
  set(BASS_DLL_PATH x64/mp3-free/bass.dll)
  set(BASS_LIB_PATH c/x64/bass.lib)
  set(BASS_INCLUDE_PATH c)
endif(WIN32)

if(UNIX)
  set(BASS_ARCHIVE_URL "http://us.un4seen.com/files/bass24-linux.zip")
  set(BASS_ARCHIVE_HASH 75d38ecc27f195c033380d0f5621d42b1a6351ae)
  set(BASS_ROOT_PATH ${CMAKE_CURRENT_BINARY_DIR}/contrib/bass)
  set(BASS_DLL_PATH x64/mp3-free/libbass.so)
  set(BASS_LIB_PATH ${BASS_DLL_PATH})
  set(BASS_INCLUDE_PATH .)
endif(UNIX)

set(BASS_LIB_FULLPATH ${BASS_ROOT_PATH}/${BASS_LIB_PATH})
set(BASS_DLL_FULLPATH ${BASS_ROOT_PATH}/${BASS_DLL_PATH})

if (NOT EXISTS ${BASS_LIB_FULLPATH})
  file(DOWNLOAD ${BASS_ARCHIVE_URL} ${CMAKE_CURRENT_BINARY_DIR}/bass24.zip
    TIMEOUT 60
    EXPECTED_HASH SHA1=${BASS_ARCHIVE_HASH}
  )

  execute_process(
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BASS_ROOT_PATH}
  )

  execute_process(
    COMMAND ${CMAKE_COMMAND} -E tar -xf ${CMAKE_CURRENT_BINARY_DIR}/bass24.zip
    WORKING_DIRECTORY ${BASS_ROOT_PATH}
  )
endif()

add_library(bass SHARED IMPORTED)
set_target_properties(bass
  PROPERTIES
    IMPORTED_IMPLIB ${BASS_LIB_FULLPATH}
)

set_target_properties(bass
  PROPERTIES
    IMPORTED_LOCATION ${BASS_DLL_FULLPATH}
)

set_target_properties(bass
  PROPERTIES
    IMPORTED_NO_SONAME TRUE
)

add_library(sound STATIC
  include/sound.hpp
  src/sound.cpp
)

if (WIN32)
  add_custom_command(
    TARGET sound POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:bass> ${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>
  )
endif(WIN32)

if(UNIX)
  add_custom_command(
    TARGET sound POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:bass> ${CMAKE_BINARY_DIR}/lib
  )
endif(UNIX)

target_include_directories(sound
PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
PRIVATE
  ${BASS_ROOT_PATH}/${BASS_INCLUDE_PATH}
)

target_link_libraries(sound PUBLIC
  bass
  common
  config
)

if (WIN32)
  install(FILES ${BASS_DLL_FULLPATH}
    DESTINATION bin
    CONFIGURATIONS Release
  )
endif(WIN32)

if (UNIX)
  install(FILES ${BASS_DLL_FULLPATH}
    DESTINATION lib
  )
endif(UNIX)