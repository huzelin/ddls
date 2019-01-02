add_executable(bench_zoo bench_zoo.cc)
target_link_libraries(bench_zoo ddls)

add_executable(bench_feeder bench_feeder.cc)
target_link_libraries(bench_feeder ddls)
