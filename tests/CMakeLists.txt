find_path(ATF_INCLUDE_DIR atf-c++.hpp)

include_directories("${ATF_INCLUDE_DIR}")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Kyuafile.in
  ${CMAKE_CURRENT_BINARY_DIR}/Kyuafile)

add_definitions(-DDATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

add_library(regexbench_lib STATIC ../src/match.cpp
  ../src/PcapSource.cpp ../src/Rule.cpp ../src/Session.cpp ../src/Engine.cpp)

add_executable(t_metadata t_metadata.cpp)
target_link_libraries(t_metadata ${ATF_LIB} ${PCAP_LIB} regexbench_lib ${Boost_LIBRARIES})

if(REMATCHCOMP_LIB AND REMATCHEXEC_LIB AND PCRE2_LIB)
  add_executable(t_session t_session.cpp)
  target_link_libraries(t_session engines ${ATF_LIB} ${PCAP_LIB} ${PCRE2_LIB}
    ${REMATCHCOMP_LIB} ${REMATCHEXEC_LIB} regexbench_lib ${Boost_LIBRARIES})
endif()

if(RE2_LIB)
  add_executable(t_re2 t_re2.cpp)
  target_link_libraries(t_re2 engines ${ATF_LIB} ${PCAP_LIB} ${RE2_LIB}
    regexbench_lib ${Boost_LIBRARIES})
endif()

add_executable(t_option t_option.cpp ${ENGINE_SRCS})
target_link_libraries(t_option
  engines ${ATF_LIB} ${PCAP_LIB} ${Boost_LIBRARIES} regexbench_lib)
if(HYPERSCAN_LIB)
  target_link_libraries(t_option ${HYPERSCAN_LIB})
endif()
if(PCRE2_LIB)
  target_link_libraries(t_option ${PCRE2_LIB})
endif()
if(RE2_LIB)
  target_link_libraries(t_option ${RE2_LIB})
endif()
if(REMATCHCOMP_LIB AND REMATCHEXEC_LIB)
  target_link_libraries(t_option ${REMATCHCOMP_LIB} ${REMATCHEXEC_LIB})
endif()
