##### Chapt1

MakeList文件-》转化为makefile然后makefile调用make指令就可以生成exe可执行文件

从CmakeList-》makefile只需要cmake指令



echo ROS_VERSION

加上`$`符号 echo $ROS_VERSION 这个符号会自动寻找到ROS_VERSION这个`环境变量`

```
echo $ROS_DISTRO #发行版本
printenv #打印环境变量
```

![image-20251214220519591](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20251214220519591.png)

- turtlesim----可执行包的名字
- turtlesim_node----可执行文件的名字
- run ----执行指令



```
做一件事情，
1.通过环境变量找小海龟可执行文件的名字
2.使用export修改AMENT_PRIFIX_PATH修改可执行文件的路径
```

ctrl+Alt+T开启新的终端

![image-20251214221806750](C:\Users\13859\AppData\Roaming\Typora\typora-user-images\image-20251214221806750.png)

ls -a列出所有文件