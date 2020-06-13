project(sound)

if(UNIX)

  set(BASS_ARCHIVE_URL "http://us.un4seen.com/files/bass24-linux.zip")
  set(BASS_ARCHIVE_HASH 75d38ecc27f195c033380d0f5621d42b1a6351ae)
  set(BASS_ROOT_PATH ${CMAKE_CURRENT_BINARY_DIR}/contrib/bass)
  set(BASS_LIB_PATH x64/libbass.so)

  set(BASS_LIB_FULLPATH ${BASS_ROOT_PATH}/${BASS_LIB_PATH})
endif(UNIX)


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
    IMPORTED_LOCATION ${BASS_LIB_FULLPATH}
)

add_library(sound STATIC
  include/sound.hpp
  src/sound.cpp
)

target_include_directories(sound
PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
PRIVATE
  ${BASS_ROOT_PATH}
)

target_link_libraries(sound PUBLIC
  bass
  common
  config
)
