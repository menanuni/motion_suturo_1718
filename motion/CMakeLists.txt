cmake_minimum_required(VERSION 2.8.3)
project(motion)

find_package(catkin REQUIRED COMPONENTS
        roscpp
        std_msgs
        geometry_msgs
        message_generation
        actionlib_msgs
        actionlib
        moveit_ros_planning_interface
        tf
)
find_package(Boost REQUIRED COMPONENTS 
        system
)

#add_action_files(
#        DIRECTORY action
#        FILES MovingCommand.action
#)

#generate_messages(
#        DEPENDENCIES std_msgs geometry_msgs actionlib_msgs
#)

catkin_package(
        CATKIN_DEPENDS message_runtime std_msgs geometry_msgs tf
)


include_directories(
        ${catkin_INCLUDE_DIRS}
)

#create executables
add_executable(main src/main.cpp)
target_link_libraries(main ${catkin_LIBRARIES})