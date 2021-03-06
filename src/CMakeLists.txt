set(qapt_SRCS
    backend.cpp
    cache.cpp
    package.cpp
    config.cpp
    history.cpp
    debfile.cpp
    dependencyinfo.cpp
    changelog.cpp
    transaction.cpp
    transactiondbus.cpp
    downloadprogress.cpp
    markingerrorinfo.cpp
    sourceentry.cpp
    sourceslist.cpp
    )

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src
    )

add_subdirectory(worker)

set(qapt_HDRS
    package.h
    debfile.h
    dependencyinfo.h
    changelog.h
    globals.h
    downloadprogress.h
    markingerrorinfo.h
    sourceentry.h
    )

qt4_add_dbus_interface(qapt_SRCS
                       worker/org.kubuntu.qaptworker2.xml
                       workerdbus)

add_library(qapt SHARED ${qapt_SRCS})
set_target_properties(qapt PROPERTIES
    VERSION ${qapt_lib_VERSION}
    SOVERSION ${qapt_lib_SOVERSION}
    )

target_link_libraries(qapt
    ${QT_QTDBUS_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    ${APTPKG_LIBRARIES}
    ${XAPIAN_LIBRARIES}
    )

install(TARGETS qapt
    LIBRARY DESTINATION lib${LIB_SUFFIX})

install(FILES
    backend.h
    changelog.h
    config.h
    debfile.h
    dependencyinfo.h
    history.h
    package.h
    globals.h
    transaction.h
    downloadprogress.h
    markingerrorinfo.h
    sourceentry.h
    sourceslist.h

    DESTINATION
    include/libqapt COMPONENT Devel)
