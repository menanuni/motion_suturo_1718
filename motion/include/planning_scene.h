#ifndef SUTURO_MOTION_MAIN_PLANNING_SCENE_H
#define SUTURO_MOTION_MAIN_PLANNING_SCENE_H

#include <ros/ros.h>
#include <map>
#include <geometry_msgs/PoseStamped.h>
#include <geometric_shapes/shapes.h>
#include <geometric_shapes/mesh_operations.h>
#include <geometric_shapes/shape_operations.h>
#include <knowledge_msgs/GetFixedKitchenObjects.h>
#include <knowledge_msgs/PerceivedObjectBoundingBox.h>

#include <moveit_msgs/CollisionObject.h>
#include <moveit/move_group_interface/move_group.h>
#include <moveit/robot_state/conversions.h>
#include <moveit/collision_detection/collision_matrix.h>
#include <moveit_msgs/CollisionObject.h>
#include <moveit_msgs/PlanningScene.h>
#include <moveit_msgs/AllowedCollisionMatrix.h>
#include <moveit_msgs/AllowedCollisionEntry.h>
#include <moveit_msgs/GetPlanningScene.h>
#include <moveit/planning_scene_monitor/planning_scene_monitor.h>
#include <moveit_msgs/DisplayTrajectory.h>
#include <moveit_msgs/CollisionObject.h>


#include "point_transformer.h"

/**
 * Class to control the planningscene.
 * Currently used to add the kitchen to the moveit planningscene.
 */

using namespace std;

class PlanningSceneController {
private:
    ros::NodeHandle node_handle;
    PointTransformer transformer;
    ros::Publisher planningScenePublisher;
    ros::ServiceClient getPlanningSceneClient;
    ros::Publisher attachObjectPublisher;
    ros::Publisher collisionObjectPublisher;
    ros::WallDuration sleep_t;

    boost::shared_ptr<tf::TransformListener> tf;
    planning_scene_monitor::PlanningSceneMonitorPtr planningSceneMonitor;


    const string meshPathPrefix = "package://knowledge_common/meshes/";

    /**
     * Returns a Mesh object for the mesh-file at the given path.
     * @param meshPath the path to the mesh-file.
     */
    shape_msgs::Mesh getMeshFromResource(const string meshPath);

    /**
     * Checks whether the object is in the collision world of the scene used by movegroup.
     * @param objectName the object to check for.
     */
    bool isInCollisionWorld(const string objectName);

    /**
     * Checks whether the object is attached/detached from the robot in the scene used by movegroup.
     * @param objectName the object to check for.
     * @param link the link the object should be attached to.
     */
    bool isAttached(const string objectName, const string link);

public:

    /**
     * Constructor.
     * @param nh Node Handle.
     */
    PlanningSceneController(const ros::NodeHandle &nh);

    /**
     * Adds the Objects from the knowledge response to the moveit planning scene.
     *
     * @param res The knowledge response containing information about the objects in the iai kitchen.
     * @return true/false whether the objects could be added or not.
     */
    bool
    addKitchenCollisionObjects(knowledge_msgs::GetFixedKitchenObjects::Response &res);

    /**
     * Callback function.
     * Adds a mesh-collider into the planningscene for a newly perceived object.
     *
     * @param newPerceivedObject The data for the object to be added to the planningscene.
     * @return true/false whether the object could be added or not.
     */
    bool
    addPerceivedObjectToEnvironment(const knowledge_msgs::PerceivedObjectBoundingBox::ConstPtr newPerceivedObject);

    /**
     * Adds the object with the given name to the environment of the plannningscene.
     *
     * @param objectName The name of the object to be added to the planningscene.
     * @param meshPath the path of the mesh for the object.
     * @param pose the pose of the mesh to spawn in the planning scene.
     * @return true/false whether the object could be added or not.
     */
    bool
    addObjectToEnvironment(const string objectName, const string meshPath, const
    geometry_msgs::PoseStamped pose);

    /**
     * Removes the object with the given name from the environment of the plannningscene.
     *
     * @param objectName The name of the object to be removed from the planningscene.
     * @return true/false whether the object could be removed or not.
     */
    bool
    removeObjectFromEnvironment(const string objectName);

    /**
     * Attach object to robot after grasping it.
     *
     * @param objectName the name of the object to attach to the robot.
     * @param link the link the object shall be attached to.
     * @return true/false whether the object was successfully attached.
     */
    bool
    attachObject(const string objectName, const string link);

    /**
     * Attach object to robot after a human placed the object in the robots gripper.
     *
     * @param objectName the name of the object to attach to the robots gripper.
     * @param pose the pose of the object placed by a human.
     * @param gripperLinks the names of the gripper components. Used for allowing the collision of the object with this components.
     * @return true/false whether the object was successfully attached.
     */
    bool
    attachObjectFromHuman(const string objectName, const geometry_msgs::PoseStamped pose, vector<string> gripperLinks);

    /*
     * Removes the object, which currently is attached, from the robot and the planningscene.
     *
     * @param objectName the name of the object to detach and remove
     * @return if the operation was successful or not.
     */
    bool removeAttachedObject(const string objectName);

    /**
     * Detach object from robot after releasing it.
     *
     * @param objectName the name of the object to detach from the robot.
     * @param link the link the object shall be detached from.
     * @return true/false whether the object was successfully detached.
     */
    bool
    detachObject(const string objectName, const string link);

    /**
     * Add the object to collision matrix. The permission to collide with this object
     * is given in the second parameter.
     * If the object is not already part of the collision matrix, it get's added.
     *
     * @param objectName the name of the object to add to planning scenes collision matrix.
     * @param allowed states out whether the collision shall be allowed or not.
     */
    void addObjectToCollisionMatrix(const string objectName, bool allowed);

    /**
     * Allows collision for the object 'object' with the objects in 'objectNames'. Collision with all other objects
     * is set to not allowed.
     *
     * @param object the object to allow collision for.
     * @param objectNames the objects to allow collision with.
     * @return whether the operation was successful or not.
     */
    bool allowCollisionForSetOfObjects(const string object, const vector<string> objectNames);

    /**
     * Checks for collision for the actual state of the planning scene and the robot and returns the result.
     *
     * @return the result of the collision checking.
     */
    collision_detection::CollisionResult checkForCollision();

    /**
     * Calculates the shortest distance of the robot state to a collision with objects in the planning scene and
     * returns it.
     *
     * @param robotState the state to calculate the distance to a collision for.
     * @return the distance to the next collision.
     */
    double distanceToCollision(robot_state::RobotState &robotInitialState,
                               const moveit_msgs::RobotState &solutionState);
};


#endif //SUTURO_MOTION_MAIN_PLANNING_SCENE_H
