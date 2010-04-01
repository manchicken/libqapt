include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

set(qapt_worker_SRCS
    main.cpp
    worker.cpp
)

set(qapt_worker_HDRS
    worker.h
)

qt4_add_dbus_adaptor(qapt_worker_SRCS
    org.kubuntu.qaptworker.xml
    worker.h
    QAptWorker
)

automoc4_add_executable(qaptworker
    ${qapt_worker_SRCS}
    ${qapt_worker_adaptor_SRCS}
)

# see our worker is pretty small :D
target_link_libraries(qaptworker
    ${QT_QTCORE_LIBRARY}
    polkit-qt-core-1
    qapt
)

install(TARGETS qaptworker DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

dbus_add_activation_system_service(org.kubuntu.qaptworker.service.in)

install(FILES org.kubuntu.qaptworker.conf DESTINATION /etc/dbus-1/system.d)
install(FILES org.kubuntu.qaptworker.policy DESTINATION ${CMAKE_INSTALL_PREFIX}/share/polkit-1/actions)