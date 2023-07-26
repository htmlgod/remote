include(InstallRequiredSystemLibraries)
set(
    CPACK_PACKAGE_NAME ${PROJECT_NAME}
)
set(
    CPACK_PACKAGE_DESCRIPTION_SUMMARY "RemoteDesktop Server Application"
)
set(CPACK_PACKAGE_VENDOR "CNIIAG")

set(CPACK_VERBATIM_VARIABLES YES)

set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
SET(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_SOURCE_DIR}/_packages")
set(CPACK_STRIP_FILES YES)

set(
    CPACK_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
)

set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local/${PROJECT_NAME}")

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

set(CPACK_PACKAGE_CONTACT "cniiag@cniiag.ru")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "YOUR NAME <${CPACK_PACKAGE_CONTACT}>")

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)#ONE_PER_GROUP)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS YES)

include(CPack)