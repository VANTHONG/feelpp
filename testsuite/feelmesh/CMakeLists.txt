set_directory_properties(PROPERTIES LABEL testmesh )
foreach(TEST entity mesh regiontree mesh_codim1 kdtree P1mesh meshmarker)

  feelpp_add_test( ${TEST} )

endforeach()

# add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS ${TESTS} )
