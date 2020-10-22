# CMake generated Testfile for 
# Source directory: /home/davidyi624/secure-aggregation/server
# Build directory: /home/davidyi624/secure-aggregation/server
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(HostTest "/home/davidyi624/secure-aggregation/server/tests/Test")
set_tests_properties(HostTest PROPERTIES  _BACKTRACE_TRIPLES "/home/davidyi624/secure-aggregation/server/CMakeLists.txt;24;add_test;/home/davidyi624/secure-aggregation/server/CMakeLists.txt;0;")
subdirs("enclave")
subdirs("host")
subdirs("tests")
