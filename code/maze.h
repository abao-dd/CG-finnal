#ifndef MAZE_H
#define MAZE_H
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include "shader.h"

class maze {
public:
    maze(int row=10, int cloumn=10, float maze_size=10, float maze_height=2.5,
        float maze_floor=0, float asp=0.9);
    float get_maze_size();
    float get_maze_floor();
    float get_maze_height();
    float get_cell_size();
    glm::vec3 get_entrancePos();
    glm::vec3 get_exitPos();
    void print_maze();
    bool mazepeng(glm::vec3 m1, glm::vec3 m2, glm::vec3 m3);
    void maze_render(Shader& simpleDepthShader, unsigned int boxVAO, unsigned int diffuseMapwhite, unsigned int diffuseMapblue);
    void mini_maze_render(Shader& minimazeShader, unsigned int lightCubeVAO);
private:
    float maze_size;
    float maze_height;
    float maze_floor;
    float asp;
    float cell_size;
    float path_size;
    float box_width;
    std::vector<std::vector<bool>> maze_map;
    std::vector<std::vector<bool>> minimap;
    std::vector<glm::vec3> cubeScales;
    std::vector<glm::vec3> cubePositions;
    glm::vec3 entrancePos;
    glm::vec3 exitPos;
    void make_maze();
    static bool linepeng(glm::vec3 a1, glm::vec3 a2, glm::vec3 b1, glm::vec3 b2);
    static bool rectanglepeng(glm::vec3 a1, glm::vec3 a2, glm::vec3 a3, glm::vec3 b1,
        glm::vec3 b2, glm::vec3 b3);
};


#endif
