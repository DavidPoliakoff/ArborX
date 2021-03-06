
set(ARBORX_DEVICE_TYPES)
if(Kokkos_ENABLE_SERIAL)
  list(APPEND ARBORX_DEVICE_TYPES Kokkos::Serial::device_type)
endif()
if(Kokkos_ENABLE_OPENMP)
  list(APPEND ARBORX_DEVICE_TYPES Kokkos::OpenMP::device_type)
endif()
if(Kokkos_ENABLE_CUDA)
  list(APPEND ARBORX_DEVICE_TYPES Kokkos::CudaSpace::device_type)
endif()
if(Kokkos_ENABLE_PTHREAD)
  list(APPEND ARBORX_DEVICE_TYPES Kokkos::Threads::device_type)
endif()
if(Kokkos_ENABLE_HIP)
  list(APPEND ARBORX_DEVICE_TYPES Kokkos::Experimental::HIPSpace::device_type)
endif()
if(Kokkos_ENABLE_OPENMPTARGET)
  list(APPEND ARBORX_DEVICE_TYPES Kokkos::Experimental::OpenMPTarget::device_type)
endif()
if(Kokkos_ENABLE_SYCL)
  list(APPEND ARBORX_DEVICE_TYPES Kokkos::Experimental::SYCL::device_type)
endif()

string(REPLACE ";" "," ARBORX_DEVICE_TYPES "${ARBORX_DEVICE_TYPES}")

if(NOT ARBORX_DEVICE_TYPES)
  message(SEND_ERROR "Kokkos_DEVICES must include at least one of 'SERIAL', 'OPENMP', 'CUDA', 'HIP', 'OPENMPTARGET', 'SYCL' or 'PTHREAD'!")
endif()

configure_file(ArborX_EnableDeviceTypes.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/ArborX_EnableDeviceTypes.hpp @ONLY)

find_package(Boost 1.67.0 REQUIRED COMPONENTS unit_test_framework)
# CMake Boost version check is tricky due to multiple changes to the way Boost
# package stores version information
if(NOT DEFINED Boost_VERSION_MINOR)
  set(Boost_VERSION_MINOR ${Boost_MINOR_VERSION})
endif()
if(Kokkos_ENABLE_CUDA AND Boost_VERSION_MINOR GREATER_EQUAL 69)
  message(FATAL_ERROR "Tests in CUDA-enabled ArborX are only compatible with Boost version 1.67 or 1.68")
endif()

# Compile only, nothing to run
add_executable(ArborX_AccessTraits.exe tstAccessTraits.cpp)
target_link_libraries(ArborX_AccessTraits.exe PRIVATE ArborX)

add_executable(ArborX_Callbacks.exe tstCallbacks.cpp)
target_link_libraries(ArborX_Callbacks.exe PRIVATE ArborX)

add_executable(ArborX_DetailsConcepts.exe tstDetailsConcepts.cpp)
target_link_libraries(ArborX_DetailsConcepts.exe PRIVATE ArborX)

add_executable(ArborX_DetailsUtils.exe tstDetailsUtils.cpp utf_main.cpp)
# TODO link Boost::dynamic_linking interface target to enable dynamic linking
# (adds BOOST_ALL_DYN_LINK)
target_link_libraries(ArborX_DetailsUtils.exe PRIVATE ArborX Boost::unit_test_framework)
target_compile_definitions(ArborX_DetailsUtils.exe PRIVATE BOOST_TEST_DYN_LINK)
target_include_directories(ArborX_DetailsUtils.exe PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
add_test(NAME ArborX_DetailsUtils_Test COMMAND ./ArborX_DetailsUtils.exe)

add_executable(ArborX_DetailsAlgorithms.exe tstDetailsAlgorithms.cpp)
target_link_libraries(ArborX_DetailsAlgorithms.exe PRIVATE ArborX Boost::unit_test_framework)
target_compile_definitions(ArborX_DetailsAlgorithms.exe PRIVATE BOOST_TEST_MAIN BOOST_TEST_DYN_LINK)
target_include_directories(ArborX_DetailsAlgorithms.exe PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
add_test(NAME ArborX_DetailsAlgorithms_Test COMMAND ./ArborX_DetailsAlgorithms.exe)

set(ARBORX_TEST_QUERY_TREE_SOURCES)
foreach(_test Degenerate)
  set(_suffix "_InstantiateAll")
  file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/tstQueryTree${_test}${_suffix}.cpp"
    "#include <ArborX_LinearBVH.hpp>\n"
    "template <class MemorySpace> using ArborX__BoundingVolumeHierarchy = ArborX::BVH<MemorySpace>;\n"
    "#define ARBORX_TEST_TREE_TYPES Tuple<ArborX__BoundingVolumeHierarchy>\n"
    "#define ARBORX_TEST_DEVICE_TYPES std::tuple<${ARBORX_DEVICE_TYPES}>\n"
    "#include <tstQueryTree${_test}.cpp>\n"
  )
  list(APPEND ARBORX_TEST_QUERY_TREE_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/tstQueryTree${_test}${_suffix}.cpp")
endforeach()
add_executable(ArborX_LinearBVH.exe
  ${ARBORX_TEST_QUERY_TREE_SOURCES}
  tstQueryTreeComparisonWithBoost.cpp
  tstQueryTreeManufacturedSolution.cpp
  tstQueryTreeCallbacks.cpp
  tstQueryTreeTraversalPolicy.cpp
  tstKokkosToolsAnnotations.cpp
  utf_main.cpp)
target_link_libraries(ArborX_LinearBVH.exe PRIVATE ArborX Boost::unit_test_framework)
target_compile_definitions(ArborX_LinearBVH.exe PRIVATE BOOST_TEST_DYN_LINK)
target_include_directories(ArborX_LinearBVH.exe PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME ArborX_LinearBVH_Test COMMAND ./ArborX_LinearBVH.exe)

add_executable(ArborX_DetailsTreeConstruction.exe tstDetailsTreeConstruction.cpp utf_main.cpp)
target_link_libraries(ArborX_DetailsTreeConstruction.exe PRIVATE ArborX Boost::unit_test_framework)
target_compile_definitions(ArborX_DetailsTreeConstruction.exe PRIVATE BOOST_TEST_DYN_LINK)
target_include_directories(ArborX_DetailsTreeConstruction.exe PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
add_test(NAME ArborX_DetailsTreeConstruction_Test COMMAND ./ArborX_DetailsTreeConstruction.exe)

add_executable(ArborX_DetailsContainers.exe tstSequenceContainers.cpp tstContainerAdaptors.cpp tstHeapOperations.cpp)
target_link_libraries(ArborX_DetailsContainers.exe PRIVATE ArborX Boost::unit_test_framework)
target_compile_definitions(ArborX_DetailsContainers.exe PRIVATE BOOST_TEST_DYN_LINK)
add_test(NAME ArborX_DetailsContainers_Test COMMAND ./ArborX_DetailsContainers.exe)

add_executable(ArborX_DetailsTreeTraversal.exe tstDetailsTreeTraversal.cpp utf_main.cpp)
target_link_libraries(ArborX_DetailsTreeTraversal.exe PRIVATE ArborX Boost::unit_test_framework)
target_compile_definitions(ArborX_DetailsTreeTraversal.exe PRIVATE BOOST_TEST_DYN_LINK)
add_test(NAME ArborX_DetailsTreeTraversal_Test COMMAND ./ArborX_DetailsTreeTraversal.exe)

add_executable(ArborX_DetailsBatchedQueries.exe tstDetailsBatchedQueries.cpp utf_main.cpp)
target_link_libraries(ArborX_DetailsBatchedQueries.exe PRIVATE ArborX Boost::unit_test_framework)
target_compile_definitions(ArborX_DetailsBatchedQueries.exe PRIVATE BOOST_TEST_DYN_LINK)
target_include_directories(ArborX_DetailsBatchedQueries.exe PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
add_test(NAME ArborX_DetailsBatchedQueries_Test COMMAND ./ArborX_DetailsBatchedQueries.exe)

add_executable(ArborX_DetailsCrsGraphWrapperImpl.exe tstDetailsCrsGraphWrapperImpl.cpp utf_main.cpp)
target_link_libraries(ArborX_DetailsCrsGraphWrapperImpl.exe PRIVATE ArborX Boost::unit_test_framework)
target_compile_definitions(ArborX_DetailsCrsGraphWrapperImpl.exe PRIVATE BOOST_TEST_DYN_LINK)
target_include_directories(ArborX_DetailsCrsGraphWrapperImpl.exe PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
add_test(NAME ArborX_DetailsCrsGraphWrapperImpl_Test COMMAND ./ArborX_DetailsCrsGraphWrapperImpl.exe)

if(ARBORX_ENABLE_MPI)
  add_executable(ArborX_DistributedTree.exe tstDistributedTree.cpp tstKokkosToolsDistributedAnnotations.cpp utf_main.cpp)
  target_link_libraries(ArborX_DistributedTree.exe PRIVATE ArborX Boost::unit_test_framework)
  target_compile_definitions(ArborX_DistributedTree.exe PRIVATE BOOST_TEST_DYN_LINK ARBORX_MPI_UNIT_TEST)
  target_include_directories(ArborX_DistributedTree.exe PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
  add_test(NAME ArborX_DistributedTree_Test COMMAND ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS} ${MPIEXEC_PREFLAGS} ./ArborX_DistributedTree.exe ${MPIEXEC_POSTFLAGS})

  add_executable(ArborX_DetailsDistributedTreeImpl.exe tstDetailsDistributedTreeImpl.cpp utf_main.cpp)
  target_link_libraries(ArborX_DetailsDistributedTreeImpl.exe PRIVATE ArborX Boost::unit_test_framework)
  target_compile_definitions(ArborX_DetailsDistributedTreeImpl.exe PRIVATE BOOST_TEST_DYN_LINK ARBORX_MPI_UNIT_TEST)
  target_include_directories(ArborX_DetailsDistributedTreeImpl.exe PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
  add_test(NAME ArborX_DetailsDistributedTreeImpl_Test COMMAND ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS} ${MPIEXEC_PREFLAGS} ./ArborX_DetailsDistributedTreeImpl.exe ${MPIEXEC_POSTFLAGS})
endif()

add_executable(ArborX_BoostAdapters.exe tstBoostGeometryAdapters.cpp tstBoostRangeAdapters.cpp utf_main.cpp)
target_link_libraries(ArborX_BoostAdapters.exe PRIVATE ArborX Boost::unit_test_framework)
target_compile_definitions(ArborX_BoostAdapters.exe PRIVATE BOOST_TEST_DYN_LINK)
add_test(NAME ArborX_BoostAdapters_Test COMMAND ./ArborX_BoostAdapters.exe)

add_executable(ArborX_DetailsRay.exe tstDetailsRay.cpp utf_main.cpp)
target_link_libraries(ArborX_DetailsRay.exe PRIVATE ArborX Boost::unit_test_framework)
target_compile_definitions(ArborX_DetailsRay.exe PRIVATE BOOST_TEST_DYN_LINK)
add_test(NAME ArborX_DetailsRay_Test COMMAND ./ArborX_DetailsRay.exe)

add_subdirectory(headers_self_contained)
