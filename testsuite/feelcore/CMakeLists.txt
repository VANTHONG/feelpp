set_directory_properties(PROPERTIES LABEL testcore )
IF ( TBB_FOUND )
  SET( OTESTS tbb )
ENDIF()
foreach(TEST traits singleton factory debug context simget ${OTESTS} env)

  feelpp_add_test( ${TEST} )

endforeach()


add_executable(test_boostmpi test_boostmpi.cpp)
target_link_libraries(test_boostmpi feelpp ${FEELPP_LIBRARIES} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} )
add_dependencies(testsuite test_boostmpi)
add_test(NAME test_boostmpi COMMAND mpirun -np 2 test_boostmpi --log_level=message  )

feelpp_add_test(gatherscatter_feelpp SRCS test_gatherscatter.cpp DEFS FEELPP_BUG=1)
feelpp_add_test(gatherscatter )

feelpp_add_application(mpi SRCS test_mpi.cpp)
feelpp_add_test( fusion )
