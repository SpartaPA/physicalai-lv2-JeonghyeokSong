cmake_minimum_required(VERSION 3.20)
project(vision)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(OpenCV REQUIRED)

# target_include_directories(vision_node PUBLIC ${OpenCV_INCLUDE_DIRS})

add_executable(vision_node src/vision_node.cpp)

target_link_libraries(
  vision_node
  PUBLIC
  rclcpp::rclcpp
  cv_bridge::cv_bridge
  ${OpenCV_LIBS}
)

install(TARGETS
  vision_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
