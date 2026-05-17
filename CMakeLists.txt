cmake_minimum_required(VERSION 3.20)
project(engine_prototype LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
enable_testing()

add_executable(engine_prototype
    src/main.cpp src/core/Engine.cpp src/core/GameLoop.cpp src/core/EngineConfigLoader.cpp
    src/core/WorldClock.cpp src/core/SubsystemPipeline.cpp src/core/EntityManager.cpp
    src/core/TransformStore.cpp src/core/VelocityStore.cpp src/core/GravityStore.cpp src/core/ColliderStore.cpp src/core/World.cpp
    src/subsystems/Renderer.cpp src/subsystems/Physics.cpp src/subsystems/WorldSim.cpp)
target_include_directories(engine_prototype PRIVATE src)

add_executable(world_clock_tests tests/WorldClockTests.cpp src/core/WorldClock.cpp)
target_include_directories(world_clock_tests PRIVATE src)
add_test(NAME world_clock_tests COMMAND world_clock_tests)

add_executable(engine_config_loader_tests tests/EngineConfigLoaderTests.cpp src/core/EngineConfigLoader.cpp)
target_include_directories(engine_config_loader_tests PRIVATE src)
add_test(NAME engine_config_loader_tests COMMAND engine_config_loader_tests)

add_executable(subsystem_pipeline_tests tests/SubsystemPipelineTests.cpp src/core/SubsystemPipeline.cpp)
target_include_directories(subsystem_pipeline_tests PRIVATE src)
add_test(NAME subsystem_pipeline_tests COMMAND subsystem_pipeline_tests)

add_executable(world_tests tests/WorldTests.cpp src/core/EntityManager.cpp src/core/TransformStore.cpp src/core/VelocityStore.cpp src/core/GravityStore.cpp src/core/ColliderStore.cpp src/core/World.cpp)
target_include_directories(world_tests PRIVATE src)
add_test(NAME world_tests COMMAND world_tests)

option(ENABLE_PLAYTEST_SDL "Build SDL playtest executable" ON)
if(ENABLE_PLAYTEST_SDL)
  add_executable(engine_playtest
      src/playtest/main_playtest.cpp
      src/playtest/PlaytestApp.cpp
      src/playtest/PlaytestSimulation.cpp
      src/core/WorldClock.cpp)
  target_include_directories(engine_playtest PRIVATE src)

  set(SDL_LINKED OFF)
  find_package(SDL3 QUIET CONFIG)
  if(SDL3_FOUND AND TARGET SDL3::SDL3)
    target_link_libraries(engine_playtest PRIVATE SDL3::SDL3)
    target_compile_definitions(engine_playtest PRIVATE ENGINE_USE_SDL3=1)
    set(SDL_LINKED ON)
  endif()

  if(NOT SDL_LINKED)
    find_package(SDL2 QUIET CONFIG)
    if(NOT SDL2_FOUND)
      find_package(SDL2 QUIET)
    endif()

    if(SDL2_FOUND)
      if(TARGET SDL2::SDL2)
        target_link_libraries(engine_playtest PRIVATE SDL2::SDL2)
      endif()
      if(TARGET SDL2::SDL2main)
        target_link_libraries(engine_playtest PRIVATE SDL2::SDL2main)
      endif()
      set(SDL_LINKED ON)
    endif()
  endif()

  if(WIN32)
    target_compile_definitions(engine_playtest PRIVATE NOMINMAX)
  endif()
endif()

add_executable(playtest_simulation_tests
    tests/PlaytestSimulationTests.cpp
    src/playtest/PlaytestSimulation.cpp
    src/core/WorldClock.cpp)
target_include_directories(playtest_simulation_tests PRIVATE src)
add_test(NAME playtest_simulation_tests COMMAND playtest_simulation_tests)
