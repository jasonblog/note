include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third_party/catch)

add_executable(simple_factory_test simple_factory/client_test.cc)
add_executable(strategy_test strategy/client_test.cc)
add_executable(decorator_test decorator/client_test.cc)
add_executable(proxy_test proxy/client_test.cc)
add_executable(builder_test builder/client_test.cc)
add_executable(observer_test observer/client_test.cc)
add_executable(abstract_factory_test abstract_factory/client_test.cc)
add_executable(state_test state/client_test.cc)
add_executable(memento_test memento/client_test.cc)

add_executable(singleton_test
  singleton/client_test.cc
  ${CMAKE_SOURCE_DIR}/src/singleton/singleton.cc
)

add_executable(command_test command/client_test.cc)
add_executable(responsibility_test chain_of_responsibility/client_test.cc)
