# build unit test
add_executable(EventLoopTest EventLoopTest.cc)
target_link_libraries(EventLoopTest net::library base::library ${LIBEV_LIB})

add_executable(LogTest LogTest.cc)
target_link_libraries(LogTest base::library net::library)

add_executable(TcpServerTest TcpServerTest.cc)
target_link_libraries(TcpServerTest net::library base::library util::library ${LIBEV_LIB})

add_executable(TcpClientTest TcpClientTest.cc)
target_link_libraries(TcpClientTest net::library base::library util::library ${LIBEV_LIB})