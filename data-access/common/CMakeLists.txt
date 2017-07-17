
set(COMMON_SRC thread_pool.cc util.cc rocksdb.cc redis.cc consistent_hash.cc)

include_directories(${COMMON_DIR})
include_directories(${THIRDPARTY_INCLUDE_DIR})
link_directories(${THIRDPARTY_LIB_DIR})
link_directories(${COMMON_DIR})

add_library(common ${COMMON_SRC})
target_link_libraries(common pthread)
target_link_libraries(common rt)
target_link_libraries(common z)
target_link_libraries(common rocksdb)
target_link_libraries(common hiredis)
