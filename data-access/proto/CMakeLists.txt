
set(PROTO_SRC minos.pb.cc rpc_client.cc transmit.pb.cc manage.pb.cc)
include_directories(${THIRDPARTY_INCLUDE_DIR})
include_directories(${PROTO_DIR})
link_directories(${THIRDPARTY_LIB_DIR})

add_library(minos_client ${PROTO_SRC})
target_link_libraries(minos_client pthread)
target_link_libraries(minos_client rt)
target_link_libraries(minos_client sofa-pbrpc)
target_link_libraries(minos_client glog)
target_link_libraries(minos_client protobuf)
target_link_libraries(minos_client snappy)
