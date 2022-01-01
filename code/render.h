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

#include <Mmsystem.h>
#include <mciapi.h>
#pragma comment(lib, "Winmm.lib")

// window settings
GLFWwindow* window=NULL;
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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
//FPCamera camera3;
Camera camera3(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f), 3.0f);

//mouse
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
float xoffset;
float yoffset;
bool firstMouse = true;
bool MouseLeftButtonPress = false;

//模型
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

//顶点
unsigned int planeVAO = 0, planeVBO = 0;
unsigned int skyboxVAO = 0, skyboxVBO = 0;
unsigned int VBO = 0, boxVAO = 0;

// 加载文字字形纹理
std::string fontPathChi = "C:\\Windows\\Fonts\\simsun.ttc";   // 中文字体
std::string fontPathEng = "C:\\Windows\\Fonts\\arial.ttf";      // 英文字母字体
glm::uvec2 pixelSize = glm::uvec2(48, 48);                      // 字体宽高

//阴影实现相关变量
const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
unsigned int depthMapFBO; unsigned int depthMap;

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
}

//加载人物模型
void render_character(Shader& modelShader, Model& cmodel, Camera& camera)
{
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    modelShader.setMat4("projection", projection);

    glm::mat4 cModelView = camera.GetViewMatrix();
    modelShader.setMat4("view", cModelView);

    modelShader.setMat4("model", myModel.ModelMat);

    myModel.Draw(modelShader);
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

// 加载字形
void loadGlyph(Shader& textShader, std::string fontPathChinese, std::string fontPathEnglish, glm::uvec2 pixelSize)
{
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    textShader.use();
    glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // 传入参加获取字形纹理
    getUnicodeGlyph(Characters, fontPathChinese, pixelSize);
    getAsciiGlyph(Characters, fontPathEnglish, pixelSize);

    // Configure textVAO/textVBO for texture quads
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 设置需要的句子
    sentences.push_back(Sentence({ 0, 1, 2, 3 }, glm::vec2(SCR_WIDTH / 4, SCR_HEIGHT * 0.8), glm::vec2(0.0f, 0.0f),
        SCR_WIDTH * 2.0f / 800, glm::vec3(1.0f, 1.0f, 1.0f)));    // "迷宫探险"
    sentences.push_back(Sentence({ 4, 5, 6, 7 }, glm::vec2(SCR_WIDTH * 3 / 8, SCR_HEIGHT * 0.6), glm::vec2(0.0f, 0.0f),
        SCR_WIDTH * 1.0f / 800, glm::vec3(1.0f, 1.0f, 1.0f)));    // "开始游戏"
    sentences.push_back(Sentence({ 8, 9, 10, 11 }, glm::vec2(SCR_WIDTH * 3 / 8, SCR_HEIGHT * 0.4), glm::vec2(0.0f, 0.0f),
        SCR_WIDTH * 1.0f / 800, glm::vec3(1.0f, 1.0f, 1.0f)));    // "游戏说明"
    sentences.push_back(Sentence({ 12, 13, 14, 15 }, glm::vec2(SCR_WIDTH * 3 / 8, SCR_HEIGHT * 0.2), glm::vec2(0.0f, 0.0f),
        SCR_WIDTH * 1.0f / 800, glm::vec3(1.0f, 1.0f, 1.0f)));    // "查看网格"
    sentences.push_back(Sentence({ 16, 17 }, glm::vec2(0.85 * SCR_WIDTH, 0.07 * SCR_HEIGHT), glm::vec2(0.0f, 0.0f),
        SCR_WIDTH * 0.7f / 800, glm::vec3(1.0f, 1.0f, 1.0f)));    // "返回"
    sentences.push_back(Sentence({ 18, 19, 20, 21, 22, 23, 24, 25, 26, 27 }, glm::vec2(0.1 * SCR_WIDTH, 0.8 * SCR_HEIGHT), glm::vec2(0.0f, 0.0f),
        SCR_WIDTH * 1.35f / 800, glm::vec3(1.0f, 1.0f, 1.0f)));    // "逃出错综复杂的迷宫吧"
    sentences.push_back(Sentence({ 28, 29, 30, 31, 32, 33 }, glm::vec2(0.4125 * SCR_WIDTH, 0.58 * SCR_HEIGHT), glm::vec2(0.0f, 0.0f),
        SCR_WIDTH * 1.0f / 800, glm::vec3(1.0f, 1.0f, 1.0f)));    // "控制人物移动"
    sentences.push_back(Sentence({ 34, 35, 36, 37, 38, 39, 40, 41 }, glm::vec2(0.25 * SCR_WIDTH, 0.41 * SCR_HEIGHT), glm::vec2(0.0f, 0.0f),
        SCR_WIDTH * 1.0f / 800, glm::vec3(1.0f, 1.0f, 1.0f)));    // "鼠标移动调整视角"
    sentences.push_back(Sentence({ 42, 43, 44, 45, 46, 47, 48, 49 }, glm::vec2(0.25 * SCR_WIDTH, 0.25 * SCR_HEIGHT), glm::vec2(0.0f, 0.0f),
        SCR_WIDTH * 1.0f / 800, glm::vec3(1.0f, 1.0f, 1.0f)));    // "鼠标滚轮进行缩放"
    sentences.push_back(Sentence({ 50, 51, 52, 53, 54, 55, 56, 57 }, glm::vec2(SCR_WIDTH * 0.02f, SCR_HEIGHT * 0.7), glm::vec2(0.0f, 0.0f),
        SCR_WIDTH * 2.0f / 800, glm::vec3(1.0f, 1.0f, 1.0f)));    // "恭喜你逃出了迷宫"

}

//显示帧数
void show_maze_fps() {
    ++m_fps_counter;
    m_fps_time_recorder += deltaTime;
    if (m_fps_counter >= 100) {
        fps = static_cast<int>(m_fps_counter / m_fps_time_recorder);
        m_fps_counter = 0;
        m_fps_time_recorder = 0.0f;
        std::stringstream ss;
        ss << "maze FPS:" << std::setiosflags(std::ios::left) << std::setw(3) << fps;
        glfwSetWindowTitle(window, ss.str().c_str());
    }
}

void depth_buffer() {
    // configure depth map FBO,深度映射保存
    // -----------------------
    glGenFramebuffers(1, &depthMapFBO);  //为渲染的深度贴图创建一个帧缓冲对象
     //create depth texture
    glGenTextures(1, &depthMap);  //创建一个2D纹理，提供给帧缓冲的深度缓冲使用
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // 把生成的深度纹理作为帧缓冲的深度缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
#endif

