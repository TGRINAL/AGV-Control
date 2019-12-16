#ifndef DYNPARAMTABLE_H
#define DYNPARAMTABLE_H

#include <QString>

#define SIZE_OF_ARRAY(ary) sizeof(ary) / sizeof(*ary)
#define DYNPARAM_NUM 7
//type, NodeName, paramName, min, max

/*const QString dynparamTable[DYNPARAM_NUM][5] = {
    "float", "move_base/local_costmap/inflation_layer", "inflation_radius", "0.0", "50.0",
    "float", "move_base/global_costmap/inflation_layer", "inflation_radius", "0.0" , "50.0",
    "float", "move_base/TrajectoryPlannerROS", "pdist_scale", "0.0", "5.0",
    "float", "move_base/TrajectoryPlannerROS", "gdist_scale", "0.0", "5.0",
    "float", "move_base/TrajectoryPlannerROS", "max_vel_x", "0.0", "20.0",
    "float", "move_base/TrajectoryPlannerROS", "max_vel_theta", "0.0", "20.0"
};*/

const QString dynparamTable[DYNPARAM_NUM][5] = {
    "float", "move_base/DWAPlannerROS", "max_vel_x", "0.0", "10.0",
    "float", "move_base/DWAPlannerROS", "max_rot_vel", "0.0" , "10.0",
    "float", "move_base/DWAPlannerROS", "path_distance_bias", "0.0", "100.0",
    "float", "move_base/DWAPlannerROS", "goal_distance_bias", "0.0", "100.0",
    "float", "move_base/DWAPlannerROS", "xy_goal_tolerance", "-1.0", "1.0",
    "float", "move_base/DWAPlannerROS", "yaw_goal_tolerance", "-1.0", "1.0",
    "float", "move_base/local_costmap/inflation_layer", "inflation_radius", "0.0", "50.0"
};

#endif // DYNPARAMTABLE_H
