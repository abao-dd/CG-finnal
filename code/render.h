#ifndef RENDER_H
#define RENDER_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"shader.h"
#include"camera.h"
#include"model.h"
#include"maze.h"
#include"text.h"

#include<iostream>
#include<iomanip>

// window settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// fps
float m_fps_time_recorder = 0.0;
int m_fps_counter = 0;
float fps;

//maze
int row = 10;
int column = 10;
float maze_size = 10.0;
float maze_height = 2.5f;
float maze_floor = 0.5f;
float maze_asp = 0.9;
maze m = maze(row, column, maze_size, maze_height, maze_floor, maze_asp);
float cell_size;
glm::vec3 entrancePos;  // 迷宫入口坐标
glm::vec3 exitPos;  // 迷宫出口坐标

// camera
Camera camera1;
Camera camera2(glm::vec3(0.0f, 10.0f, 0.0f));
//FPCamera camera3;
Camera camera3(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f), 3.0f);
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
float xoffset;
float yoffset;
bool firstMouse = true;
bool MouseLeftButtonPress = false;

Model myModel;
float modelYaw = 90.0f;

// 游戏状态
enum GameState {
    STARTMENU,
    INTRODUCTION,
    MESHVIEWING,
    GAMING,
    ENDING
};

// 游戏初始状态渲染"开始目录"界面
GameState gameState = STARTMENU;

bool firstGaming = true;

//light
glm::vec3 lightPos;

//阴影实现相关变量
unsigned int planeVAO = 0, planeVBO = 0;
unsigned int skyboxVAO = 0, skyboxVBO = 0;
unsigned int VBO = 0, boxVAO = 0;

void render_plane() {
    if (planeVBO == 0) {
        float planeVertices[] = {
            // positions            // normals         // texcoords
             25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
            -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
            -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

             25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
            -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
             25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
        };
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &planeVBO);
        glBindVertexArray(planeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindVertexArray(0);
    }

}

void render_skybox(const Shader& skyboxShader, unsigned int& cubemapTexture, Camera& camera) {
    if (skyboxVAO == 0) {
        float skyboxVertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
        };
        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    // draw skybox as last
    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
    skyboxShader.setMat4("view", view);
    skyboxShader.setMat4("projection", projection);
    // skybox cube
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default
}

void render_box() {
    if (boxVAO == 0) {
        //pos,col,uv,nor
        float vertices[] = {
             0.0f,0.0f,0.0f,  0.0f,0.0f,  0.0f,0.0f,-1.0f,
             1.0f,1.0f,0.0f,  1.0f,1.0f,  0.0f,0.0f,-1.0f,
             1.0f,0.0f,0.0f,  1.0f,0.0f,  0.0f,0.0f,-1.0f,
             1.0f,1.0f,0.0f,  1.0f,1.0f,  0.0f,0.0f,-1.0f,
             0.0f,0.0f,0.0f,  0.0f,0.0f,  0.0f,0.0f,-1.0f,
             0.0f,1.0f,0.0f,  0.0f,1.0f,  0.0f,0.0f,-1.0f,

             0.0f,0.0f,1.0f,  0.0f,0.0f,  0.0f,0.0f,1.0f,
             1.0f,0.0f,1.0f,  1.0f,0.0f,  0.0f,0.0f,1.0f,
             1.0f,1.0f,1.0f,  1.0f,1.0f,  0.0f,0.0f,1.0f,
             1.0f,1.0f,1.0f,  1.0f,1.0f,  0.0f,0.0f,1.0f,
             0.0f,1.0f,1.0f,  0.0f,1.0f,  0.0f,0.0f,1.0f,
             0.0f,0.0f,1.0f,  0.0f,0.0f,  0.0f,0.0f,1.0f,

             0.0f,1.0f,1.0f,  1.0f,0.0f,  -1.0f,0.0f,0.0f,
             0.0f,1.0f,0.0f,  1.0f,1.0f,  -1.0f,0.0f,0.0f,
             0.0f,0.0f,0.0f,  0.0f,1.0f,  -1.0f,0.0f,0.0f,
             0.0f,0.0f,0.0f,  0.0f,1.0f,  -1.0f,0.0f,0.0f,
             0.0f,0.0f,1.0f,  0.0f,0.0f,  -1.0f,0.0f,0.0f,
             0.0f,1.0f,1.0f,  1.0f,0.0f,  -1.0f,0.0f,0.0f,

             1.0f,1.0f,1.0f,  1.0f,0.0f,  1.0f,0.0f,0.0f,
             1.0f,0.0f,0.0f,  0.0f,1.0f,  1.0f,0.0f,0.0f,
             1.0f,1.0f,0.0f,  1.0f,1.0f,  1.0f,0.0f,0.0f,
             1.0f,0.0f,0.0f,  0.0f,1.0f,  1.0f,0.0f,0.0f,
             1.0f,1.0f,1.0f,  1.0f,0.0f,  1.0f,0.0f,0.0f,
             1.0f,0.0f,1.0f,  0.0f,0.0f,  1.0f,0.0f,0.0f,

             0.0f,0.0f,0.0f,  0.0f,1.0f,  0.0f,-1.0f,0.0f,
             1.0f,0.0f,0.0f,  1.0f,1.0f,  0.0f,-1.0f,0.0f,
             1.0f,0.0f,1.0f,  1.0f,0.0f,  0.0f,-1.0f,0.0f,
             1.0f,0.0f,1.0f,  1.0f,0.0f,  0.0f,-1.0f,0.0f,
             0.0f,0.0f,1.0f,  0.0f,0.0f,  0.0f,-1.0f,0.0f,
             0.0f,0.0f,0.0f,  0.0f,1.0f,  0.0f,-1.0f,0.0f,

             0.0f,1.0f,0.0f,  0.0f,1.0f,  0.0f,1.0f,0.0f,
             1.0f,1.0f,1.0f,  1.0f,0.0f,  0.0f,1.0f,0.0f,
             1.0f,1.0f,0.0f,  1.0f,1.0f,  0.0f,1.0f,0.0f,
             1.0f,1.0f,1.0f,  1.0f,0.0f,  0.0f,1.0f,0.0f,
             0.0f,1.0f,0.0f,  0.0f,1.0f,  0.0f,1.0f,0.0f,
             0.0f,1.0f,1.0f,  0.0f,0.0f,  0.0f,1.0f,0.0f
        };
        glGenVertexArrays(1, &boxVAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(boxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }
    glBindVertexArray(boxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void render_shadowsense(Shader& shader, unsigned int& diffuseMapwhite, unsigned int& diffuseMapblue, Camera& camera) {
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    //人物
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-2 * maze_size + cell_size / 2, 0.0f, -maze_size / 2 + cell_size / 2));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.14f, 0.14f, 0.14f));
    shader.setMat4("model", model);

    //岛屿
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(maze_size / 2 + cell_size / 2, 0.0f, -maze_size / 2 + cell_size / 2));
    model = glm::scale(model, glm::vec3(cell_size / 40, cell_size / 40, cell_size / 40));
    shader.setMat4("model", model);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMapwhite);
    // bind diffuse map,绑定纹理 .new
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMapwhite);

    m.maze_render(shader, boxVAO, diffuseMapwhite, diffuseMapblue);
}


void render_light(const Shader& lightCubeShader, float currentFrame, Camera& camera) {
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    lightPos = glm::vec3(-maze_size / 2, maze_floor + maze_height + cell_size, -maze_size / 2);
    //光源
    lightCubeShader.setMat4("projection", projection);
    lightCubeShader.setMat4("view", view);
    model = glm::mat4(1.0f);
    model = glm::rotate(model, currentFrame * 0.1f, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(cell_size, cell_size, cell_size));
    lightCubeShader.setMat4("model", model);
    render_box();
}


void render_sense(Shader& boxShader, glm::mat4& lightSpaceMatrix, unsigned int diffuseMapwhite, unsigned int diffuseMapblue, float currentFrame, Camera& camera) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, currentFrame * 0.1f, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(cell_size, cell_size, cell_size));
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);


    //迷宫着色器，材质和基础数值 
    boxShader.setInt("diffuseTexture", 0);
    boxShader.setInt("shadowMap", 1);
    boxShader.setMat4("projection", projection);
    boxShader.setMat4("view", view);
    boxShader.setVec3("viewPos", camera.Position);
    boxShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    boxShader.setInt("material.diffuse", 0);
    boxShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
    boxShader.setFloat("material.shininess", 32.0f);

    //平行光源
    boxShader.setVec3("dirLight.direction", -18.0f, 20.0f, -30.0f);
    //boxShader.setVec3("dirLight.direction", -12.0f, 15.0f, -20.0f);
    boxShader.setVec3("dirLight.ambient", 0.3f, 0.3f, 0.3f);
    boxShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    boxShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

    //点光源
    boxShader.setVec3("pointLights[0].position", glm::vec3(model * glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)));


    boxShader.setVec3("pointLights[0].ambient", 0.5f, 0.5f, 0.5f);
    boxShader.setVec3("pointLights[0].diffuse", 1.0f, 1.0f, 1.0f);
    boxShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    boxShader.setFloat("pointLights[0].constant", 1.0f);
    boxShader.setFloat("pointLights[0].linear", 0.09);
    boxShader.setFloat("pointLights[0].quadratic", 0.032);

    // 聚光灯
    boxShader.setVec3("spotLight.position", camera.Position);
    boxShader.setVec3("spotLight.direction", camera.Front);
    boxShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    boxShader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
    boxShader.setVec3("spotLight.specular", 0.0f, 0.0f, 0.0f);
    boxShader.setFloat("spotLight.constant", 1.0f);
    boxShader.setFloat("spotLight.linear", 0.09);
    boxShader.setFloat("spotLight.quadratic", 0.032);
    boxShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    boxShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

    m.maze_render(boxShader, boxVAO, diffuseMapwhite, diffuseMapblue);
}

void render_island(Shader& modelShader, Model& mmodel, Camera& camera) {
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    modelShader.setMat4("projection", projection);
    modelShader.setMat4("view", view);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-2 * maze_size + cell_size / 2, 0.0f, -maze_size / 2 + cell_size / 2));
    //model = glm::scale(model, glm::vec3(cell_size / 40, cell_size / 40, cell_size / 40));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.14f, 0.14f, 0.14f));
    modelShader.setMat4("model", model);
    mmodel.Draw(modelShader);

    ////人物模型
    //    //modelShader.use();
    //modelShader.setMat4("projection", projection);
    //modelShader.setMat4("view", view);
    //model = glm::mat4(1.0f);
    //model = glm::translate(model, glm::vec3(-maze_size / 2 + cell_size / 2, 0.5f, -maze_size / 2 + cell_size / 2));
    //model = glm::scale(model, glm::vec3(cell_size / 20, cell_size / 20, cell_size / 20));
    //modelShader.setMat4("model", model);
    //pmodel.Draw(modelShader);

}
void deletevao() {
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteVertexArrays(1, &boxVAO);
    glDeleteVertexArrays(1, &planeVAO);

    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteBuffers(1, &planeVBO);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

#endif

