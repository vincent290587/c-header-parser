add_executable(unit_tests test.cpp )
target_link_libraries (unit_tests Sqr)
add_test(test1 unit_tests)

#add_executable(unit_tests2 test.cpp )
#target_link_libraries (unit_tests2 Sqr)
#add_test(test2 unit_tests2)