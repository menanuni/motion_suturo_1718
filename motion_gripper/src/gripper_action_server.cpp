#include <ros/ros.h>
#include <motion_msgs/GripperAction.h>
#include <actionlib/server/simple_action_server.h>
#include <Gripper.h>

const std::string right_gripper_controller_name = "r_gripper_controller/gripper_action";
const std::string left_gripper_controller_name = "l_gripper_controller/gripper_action";

class GripperActionServer {
private:
    ros::NodeHandle node_handle;
    actionlib::SimpleActionServer<motion_msgs::GripperAction> action_server;
    Gripper left_gripper;
    Gripper right_gripper;
    motion_msgs::GripperResult result;

    boost::optional<Gripper &> determineGripper(int gripperNo) {
        if (gripperNo == motion_msgs::GripperGoal::LEFT) {
            return left_gripper;
        }
        if (gripperNo == motion_msgs::GripperGoal::RIGHT) {
            return right_gripper;
        }
        return boost::none;
    }

public:
    GripperActionServer(const ros::NodeHandle &nh) :
            node_handle(nh),
            left_gripper(left_gripper_controller_name),
            right_gripper(right_gripper_controller_name),
            action_server(node_handle, "gripper", boost::bind(&GripperActionServer::executeCommand, this, _1), false) {
        action_server.start();
    };

    void executeCommand(const motion_msgs::GripperGoalConstPtr &goal) {
        boost::optional<Gripper &> gripper = determineGripper(goal->gripper);
        if (gripper.is_initialized()) {
            actionlib::SimpleClientGoalState resultState = gripper.get().moveGripper(goal->position, goal->force);
            if (resultState == actionlib::SimpleClientGoalState::SUCCEEDED) {
                ROS_INFO("OPENED/CLOSED Gripper to Goal!");
                result.successful = true;
                action_server.setSucceeded(result);
            } else {
                ROS_INFO("Something went wrong!");
                ROS_INFO("resultState: %s", resultState.toString().c_str());
                result.successful = false;
                action_server.setAborted(result, "SOMETHING WENT WRONG");
            }
        } else {
            ROS_ERROR("UNKNOWN GRIPPER");
            result.successful = false;
            action_server.setAborted(result, "UNKNOWN GRIPPER");
        }
    };
};

int main(int argc, char **argv) {
    ros::init(argc, argv, "simple_gripper");
    ros::NodeHandle node_handle;

    GripperActionServer gripperActionServer(node_handle);

    ros::spin();

    return 0;
}
