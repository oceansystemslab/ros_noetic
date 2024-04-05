# ROS Noetic on Ubuntu 22.04
Instructions for compiling standard ROS Noetic (ros-noetic-desktop) on Ubuntu 22.04 and possible compilation issues. Instructions adapted from [here](http://wiki.ros.org/noetic/Installation/Source).

## Instructions (easy mode)
```
sudo apt update
sudo apt upgrade
sudo apt install python3-pip
```
Then install rosdep which is not available in standard repos but available in python3 pip packages and other required packages
```
sudo pip3 install -U rosdep rosinstall_generator vcstool
sudo pip3 install -U catkin-pkg-modules==66.0.0
sudo pip3 install -U rospkg-modules==66.0.0
```
Then run
```
sudo rosdep init
rosdep update
```
Also install hddtemp which is dependency for 'diagnostic_common_diagnostics' package.
```
sudo add-apt-repository ppa:malcscott/ppa
sudo apt update 
sudo apt install hddtemp
```
Clone this repo in your 'home' folder
```
git clone https://github.com/oceansystemslab/ros_noetic.git
cd src
```
Now resolve rest of the dependencies for packages
```
rosdep install --from-paths ./src --ignore-packages-from-source --rosdistro noetic -y
```
Now compile the packages
```
./src/catkin/bin/catkin_make_isolated --install -DCMAKE_BUILD_TYPE=Release
```

Once compiled verify the installation
```
source ~/ros_noetic/install_isolated/setup.bash
roscore
```

## Instructions (recommended)
Make sure you have python3 pip install 
```
sudo apt update
sudo apt upgrade
sudo apt install python3-pip
```

Then install rosdep which is not available in standard repos but available in python3 pip packages
```
sudo pip3 install -U rosdep rosinstall_generator vcstool
```

Then run
```
sudo rosdep init
rosdep update
```

Also install hddtemp which is dependency for 'diagnostic_common_diagnostics' package.
```
sudo add-apt-repository ppa:malcscott/ppa
sudo apt update 
sudo apt install hddtemp
```

Now run following commands
```
mkdir ~/ros_catkin_ws
cd ~/ros_catkin_ws

rosinstall_generator desktop --rosdistro noetic --deps --tar > noetic-desktop.rosinstall
mkdir ./src
vcs import --input noetic-desktop.rosinstall ./src
```

Now resolve the dependencies for packages
```
rosdep install --from-paths ./src --ignore-packages-from-source --rosdistro noetic -y
```

At this step the dependency resolver might complain about missing packages like 'hddtemp'. Go to the package that complains in 'src' and comment out the required package. Install these two as well and comment these in 'package.xml' for the ros package that throws the dependency error. But make sure that these are installed.
```
sudo pip3 install -U catkin-pkg-modules==66.0.0
sudo pip3 install -U rospkg-modules==66.0.0
```

- If there a complain about 'rosdep-modules' just comment those and it doesn't affect the installation and running in anyway.

Now go to src and remove 'rosconsole' package as it relies on newer 'log4cxx' package. And clone one that is patched for it. 
```
cd src
rm -rf rosconsole

git clone https://github.com/dreuter/rosconsole.git
git checkout log4cxx-0.12
cd ..
```

Now we can compile the packages
```
./src/catkin/bin/catkin_make_isolated --install -DCMAKE_BUILD_TYPE=Release
```
- The compiler will throw an error for some packages that use 'rosconsole' complaining about things like 'shared_mutex'. This is because it is not compatible with older C++ standards. 
- For these packages, go to their 'CMakelists.txt' and comment out code that sets C++ standard and set to C++17 like this
```
set(CMAKE_CXX_STANDARD 17) # or
add_compile_options(-std=c++17) # or
set_directory_properties(PROPERTIES COMPILE_OPTIONS "-std=c++17") 
``` 

Once compiled verify the installation
```
source ~/ros_catkin_ws/install_isolated/setup.bash
roscore
```
