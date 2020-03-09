# CMake generated Testfile for 
# Source directory: /home/g/cadmium_concurrent_experiments/devstone
# Build directory: /home/g/cadmium_concurrent_experiments/devstone
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(cadmium_dynamic_LI_test "cadmium_dynamic_LI_test")
set_tests_properties(cadmium_dynamic_LI_test PROPERTIES  _BACKTRACE_TRIPLES "/home/g/cadmium_concurrent_experiments/devstone/CMakeLists.txt;116;add_test;/home/g/cadmium_concurrent_experiments/devstone/CMakeLists.txt;0;")
add_test(Cadmium_generator_LI_3x3 "test/check_generated_LI_against_ref.sh")
set_tests_properties(Cadmium_generator_LI_3x3 PROPERTIES  WORKING_DIRECTORY "/home/g/cadmium_concurrent_experiments/devstone" _BACKTRACE_TRIPLES "/home/g/cadmium_concurrent_experiments/devstone/CMakeLists.txt;121;add_test;/home/g/cadmium_concurrent_experiments/devstone/CMakeLists.txt;0;")
