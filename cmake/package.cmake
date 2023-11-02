set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "v4l2 streaming over an http server")
set(CPACK_PACKAGE_VENDOR "Transcelestial")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_CONTACT "dev@transcelestial.com")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
message("system processor ${CMAKE_SYSTEM_PROCESSOR}")
if (CMAKE_SYSTEM_PROCESSOR STREQUAL armv7hf)
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE armhf)
else()
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
endif()
set(CPACK_STRIP_FILES YES)
# disabled because we cross compile
#set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS YES)

set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_SOURCE_DIR}/cmake/deb/postinst;${CMAKE_SOURCE_DIR}/cmake/deb/postrm;${CMAKE_SOURCE_DIR}/cmake/deb/prerm")

install(FILES ${CMAKE_SOURCE_DIR}/cmake/deb/v4l2-stream.service DESTINATION lib/systemd/system)

install(FILES ${CMAKE_SOURCE_DIR}/conf/rpi-pipeline.json DESTINATION share/${PROJECT_NAME}/conf)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/web DESTINATION share/${PROJECT_NAME})

include(CPack)