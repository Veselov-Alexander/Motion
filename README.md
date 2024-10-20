<h3 align="center"><img src="https://github.com/user-attachments/assets/4214ea17-8d8d-4621-b7e7-c97317dbbe33" alt="logo" height="200px"></h3>

<div align="center">

</div>

<p align="center">Computational motion planning in planar space</p>
<h4 align="center">

  
  <a href="https://github.com/alexander-veselov/Motion/commits/master/">
    <img alt="GitHub last commit" src="https://img.shields.io/github/last-commit/alexander-veselov/Motion">
  </a>
</h4>

This application enables pathfinding in planar 2D space from point A to point B, taking into account obstacles and the robot's shape. It supports multiple pathfinding algorithms, allowing users to explore different approaches to solving the motion planning problem.

Main features:
 - Planar 2D pathfinding
 - Custom-shaped robot support
 - Dynamic obstacle handling
 - Multiple pathfinding algorithms for comparison
 - Area scanning in unknown environment
 - Visualization of intermediate steps for better understanding

Originally developed as part of a diploma project, this tool is ideal for studying or teaching motion planning and related algorithms.

![animation](https://github.com/user-attachments/assets/e4b1b54e-1fc0-4d72-beb5-878456bb2e31)


# Dependencies

- [Qt](https://www.qt.io/product/framework)
- [CGAL](https://www.cgal.org/)
- [Boost](https://www.boost.org/)

# Build
Project uses [vcpkg](https://github.com/microsoft/vcpkg) package manager and [CMake](https://cmake.org/).

Quick Visual Studio project generation steps:
```
cmake . -B build -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
```

# Functionality and algorithms

#### 1. Minkowski Sum for Configuration Space Calculation<br>
The application uses the Minkowski sum to compute the configuration space, determining the allowed positions where the robot can be placed. This method accounts for both the robot's shape and the obstacles in the environment, ensuring accurate pathfinding in complex scenarios.

#### 2. Pathfinding Algorithms<br>
The app supports multiple pathfinding algorithms, allowing users to explore different approaches to motion planning:

 - RRT (Rapidly-exploring Random Tree) for probabilistic path exploration.
 - Visibility Graph for constructing optimal paths through free space by connecting visible vertices.
 - Voronoi Map to plan paths while maximizing distance from obstacles.

#### 3. Graph-based Map Storage<br>
A graph structure is used to store preprocessed maps when utilizing the Visibility Graph or Voronoi Map algorithms, optimizing performance during repeated path calculations.

#### 4. Dijkstra's Algorithm for Pathfinding in Graphs
Dijkstra's algorithm is utilized to efficiently search for the shortest path in a graph, ensuring optimal routing.

#### 5. GUI Implementation with Qt<br>
The application features a user-friendly graphical interface built using the Qt framework, making it easy to interact with various tools and visualizations for motion planning.

#### 6. State Design Pattern for UI Flows<br>
The "state" design pattern is implemented to manage multiple UI workflows, such as:

 - Creating and placing objects
 - Reshaping the robot
 - Dragging the robot across the workspace

#### 7. Custom File Format for Scene Management<br>
The app introduces a file format for saving and loading scenes, enabling users to store and retrieve their work efficiently.

#### 8. Random Obstacle Map Generation<br>
The application can generate random obstacle maps, including labyrinth-like structures, allowing for extensive testing and experimentation with different environments.

#### 9. Experimental Algorithm for Unknown Environment Navigation<br>
An experimental algorithm is included for robot navigation in unknown environments, simulating real-world scenarios where obstacles are discovered during movement.

# Gallery
<p align="center">
    <img width="49%" src="https://github.com/user-attachments/assets/c54b2f56-0f78-4085-aa46-e14aca6f6963"/>
&nbsp;
    <img width="49%" src="https://github.com/user-attachments/assets/39a6a0c0-d567-42ab-9319-8ad7b419d522"/>
</p>

