import sys
if sys.prefix == '/usr':
    sys.real_prefix = sys.prefix
    sys.prefix = sys.exec_prefix = '/home/yzh/桌面/project-ros/ros-study/chapt2/ws_ros/install/pkg02_helloworld_py'
