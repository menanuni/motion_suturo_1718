#include <Gripper.h>

Gripper::Gripper(const std::string actionName) {
    gripper_client_ = new GripperClient(actionName, true);
    ROS_INFO("Connecting to Gripper Client - %s", actionName.c_str());
    while (!gripper_client_->waitForServer(ros::Duration(5.0))) {
        ROS_INFO("Waiting for the %s action server to come up", actionName.c_str());
    }
}

Gripper::~Gripper() {
    delete gripper_client_;
}

actionlib::SimpleClientGoalState Gripper::moveGripper(float position, float effort) {
    pr2_controllers_msgs::Pr2GripperCommandGoal goal = generateGripperGoal(position, effort);
    ROS_INFO("Sending gripper goal");
    gripper_client_->sendGoalAndWait(goal, ros::Duration(5.0));
    //gripper_client_->waitForResult(ros::Duration(30.0));
    ROS_INFO("reached goal: %d", gripper_client_->getResult().get()->reached_goal);
    ROS_INFO("stalled: %d", gripper_client_->getResult().get()->stalled);
    return gripper_client_->getState();
}

void Gripper::open() {
    pr2_controllers_msgs::Pr2GripperCommandGoal open = generateGripperGoal(0.07, -1);
    ROS_INFO("Sending open goal");
    gripper_client_->sendGoal(open);
    gripper_client_->waitForResult();
    if (gripper_client_->getState() == actionlib::SimpleClientGoalState::SUCCEEDED) {
        ROS_INFO("The gripper opened!");
    } else {
        ROS_INFO("The gripper failed to open.");
        //ROS_INFO(gripper_client_->getState().toString().c_str());
        if (gripper_client_->isServerConnected()) {
            ROS_INFO("Client is connected");
        } else {
            ROS_INFO("Client NOT connected");
        }
    }
}

void Gripper::close() {
    pr2_controllers_msgs::Pr2GripperCommandGoal squeeze = generateGripperGoal(0.01, -1);
    ROS_INFO("Sending squeeze goal");
    gripper_client_->sendGoal(squeeze);
    gripper_client_->waitForResult(ros::Duration(5.0));
    ROS_INFO("Waited 5 seconds for goal");
    if (gripper_client_->getState() == actionlib::SimpleClientGoalState::SUCCEEDED) {
        ROS_INFO("The gripper closed!");
    } else {
        ROS_INFO("The gripper failed to close.");
        //ROS_INFO(gripper_client_->getState().toString().c_str());
        if (gripper_client_->isServerConnected()) {
            ROS_INFO("Client is connected");
        } else {
            ROS_INFO("Client NOT connected");
        }
    }
}
