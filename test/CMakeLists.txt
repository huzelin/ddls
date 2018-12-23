add_subdirectory(hpps)

add_custom_target(utest_data ALL DEPENDS)
add_custom_command(TARGET utest_data
	PRE_BUILD
	COMMAND
	cp -r ${CMAKE_CURRENT_SOURCE_DIR}/hpps/utest_data ${PROJECT_BINARY_DIR}/test/hpps/
	COMMENT "Copy utest_data")
