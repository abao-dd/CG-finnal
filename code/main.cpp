#include "render.h"

//ostream& operator<<(ostream& o, glm::vec3 v3)
//{
//    o << "(" << v3.x << ", " << v3.y << ", " << v3.z << ")";
//    return o;
//}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

int main()
{
    mciSendString("open \"music\\Hello.mp3\" type mpegvideo alias mp3", NULL, 0, NULL);
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);

    // 创建着色器程序
    // maze
    Shader boxShader("shaderfile/material_vs.txt", "shaderfile/material_fs.txt");
    //minimaze
    //Shader minimazeShader("shaderfile/minimaze_vs.txt", "shaderfile/minimaze_fs.txt");
    //skybox
    Shader skyboxShader("shaderfile/skybox_vs.txt", "shaderfile/skybox_fs.txt");
    //light
    Shader lightCubeShader("shaderfile/light_vs.txt", "shaderfile/light_fs.txt");

    //阴影贴图，深度贴图
    Shader simpleDepthShader("shaderfile/shadowmap_vs.txt", "shaderfile/shadowmap_fs.txt");
    Shader debugDepthQuad("shaderfile/debug_quad_vs.txt", "shaderfile/debug_quad_fs.txt");
    // 模型
    Shader modelShader("shaderfile/model_loading_vs.txt", "shaderfile/model_loading_fs.txt");
    // 文字
    Shader textShader("shaderfile/freetype_vs.txt", "shaderfile/freetype_fs.txt");

    // 加载模型
    // 岛屿模型
    Model islandModel("model/82-island/Files/fbx file/island.fbx");
    // 人物模型
    myModel = Model("model/Ayaka model/Ayaka model.pmx");

    // 迷宫参数
    cell_size = m.get_cell_size();
    entrancePos = m.get_entrancePos();
    exitPos = m.get_exitPos();

    // 天空盒贴图
    vector<std::string> faces
    {
        "textures/skybox/right.jpg",
        "textures/skybox/left.jpg",
        "textures/skybox/top.jpg",
        "textures/skybox/bottom.jpg",
        "textures/skybox/front.jpg",
        "textures/skybox/back.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    //添加纹理
    unsigned int diffuseMapwhite = loadTexture("textures/63.jpg");
    unsigned int diffuseMapblue = loadTexture("textures/147fq.jpg");
    unsigned int diffusemount = loadTexture("textures/61.jpg");

    //深度缓冲
    depth_buffer();
    
    // 加载文字字形纹理
    loadGlyph(textShader, fontPathChi, fontPathEng, pixelSize);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //显示帧数
        show_maze_fps();

        // input 
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 状态机处理，不同状态渲染不同界面
        // ----------
        // “开始目录”状态
        if (gameState == STARTMENU) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            textShader.use();
            // "迷宫探险"
            sentences[0].Draw(textShader);
            //"开始游戏"
            sentences[1].Draw(textShader);
            // "游戏说明"
            sentences[2].Draw(textShader);
            // "查看网格"
            sentences[3].Draw(textShader);
        }

        // “游戏说明”状态
        if (gameState == INTRODUCTION) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            // "逃出错综复杂的迷宫吧"
            sentences[5].Draw(textShader);
            RenderText(textShader, "awsd", 0.225 * SCR_WIDTH, 0.58 * SCR_HEIGHT, 1.25f, glm::vec3(1.0, 1.0, 1.0));
            // "控制人物移动"
            sentences[6].Draw(textShader);
            // "鼠标移动调整视角"
            sentences[7].Draw(textShader);
            // "鼠标滚轮进行缩放"
            sentences[8].Draw(textShader);
            // "返回"
            sentences[4].Draw(textShader);
        }

        // “查看网格”状态
        if (gameState == MESHVIEWING) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            
            // 渲染人物
            modelShader.use();
            myModel.SetModelTransformation(glm::vec3(0.0f, -myModel.getOriginalCenter().y * 0.2, 0.0f), 0.0f, 0.2f);
            render_character(modelShader, myModel, camera3);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            // "返回"
            sentences[4].Draw(textShader);
        }

        // “开始游戏”状态
        if (gameState == GAMING) {
            if (firstGaming) {
                mciSendString("play mp3 from 0 repeat", NULL, 0, NULL);
                // 刚进入游戏模式时，设置好人物和摄像机的初始位置
                modelShader.use();
                MODELSCALE = cell_size / 40;
                myModel.SetModelTransformation(entrancePos, glm::radians(90.0f));

                camera1 = Camera(myModel.getChangedCenter() + glm::vec3(-cell_size / 2, 0.0f, 0.0f),
                    myModel.getChangedCenter(), cell_size / 2);

                firstGaming = false;
            }
            //捕捉鼠标
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            // 1. render depth of scene to texture (from light's perspective)
            // --------------------------------------------------------------
            glm::mat4 lightProjection, lightView;
            glm::mat4 lightSpaceMatrix;
            float near_plane = 1.0f, far_plane = 7.5f;
            lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
            //修改视图位置可以看到阴影
            lightView = glm::lookAt(glm::vec3(-2.0f, 5.0f, -3.0f), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
            lightSpaceMatrix = lightProjection * lightView;
            // render scene from light's point of view
            simpleDepthShader.use();
            simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

            //首先
            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);

            render_shadowsense(simpleDepthShader, diffuseMapwhite, diffuseMapblue, camera1);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            // reset viewport
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //第二次渲染
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //渲染灯光
            lightCubeShader.use();
            render_light(lightCubeShader, currentFrame, camera1);

            // bind diffuse map,绑定纹理
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, diffuseMapwhite);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            //渲染迷宫
            boxShader.use();
            render_sense(boxShader, lightSpaceMatrix, diffuseMapwhite, diffuseMapblue, currentFrame, camera1);

            // 渲染岛
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, diffusemount);
            modelShader.use();
            render_island(modelShader, islandModel, camera1);

            // 渲染人物
            modelShader.use();
            render_character(modelShader, myModel, camera1);

            // 天空盒
            skyboxShader.use();
            render_skybox(skyboxShader, cubemapTexture, camera1);
        }

        // “游戏结束”状态
        if (gameState == ENDING) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

            //天空盒
            skyboxShader.use();
            render_skybox(skyboxShader, cubemapTexture, camera1);

            // "恭喜你逃出了迷宫"
            sentences[9].Draw(textShader);
            // "返回"
            sentences[4].Draw(textShader);
        }


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    deletevao();
    glfwTerminate();
    return 0;
}

// processInput()函数处理键盘和鼠标点击事件，根据所处状态做出相应的响应
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    // esc键退出程序，全状态通用
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // “开始目录”状态，处理鼠标点击事件。鼠标点击不同的文字导致状态机的变化。
    if (gameState == STARTMENU) {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            if (MouseLeftButtonPress == false) {
                glm::dvec2 mousePos = glm::dvec2(0.0, 0.0);
                glfwGetCursorPos(window, &mousePos.x, &mousePos.y);
                if (sentences[1].judgeMouseButton(mousePos.x, SCR_HEIGHT - mousePos.y)) {
                    sentences[1].process_press();
                }
                if (sentences[2].judgeMouseButton(mousePos.x, SCR_HEIGHT - mousePos.y)) {
                    sentences[2].process_press();
                }
                if (sentences[3].judgeMouseButton(mousePos.x, SCR_HEIGHT - mousePos.y)) {
                    sentences[3].process_press();
                }
            }
            MouseLeftButtonPress = true;
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
            if (MouseLeftButtonPress == true) {
                glm::dvec2 mousePos = glm::dvec2(0.0, 0.0);
                glfwGetCursorPos(window, &mousePos.x, &mousePos.y);
                if (sentences[1].judgeMouseButton(mousePos.x, SCR_HEIGHT - mousePos.y) && sentences[1].textPressed) {
                    gameState = GAMING;
                    firstGaming = true;
                    firstMouse = true;
                    m = maze(row, column, maze_size, maze_height, maze_floor, maze_asp);
                }
                if (sentences[2].judgeMouseButton(mousePos.x, SCR_HEIGHT - mousePos.y) && sentences[2].textPressed) {
                    gameState = INTRODUCTION;
                }
                if (sentences[3].judgeMouseButton(mousePos.x, SCR_HEIGHT - mousePos.y) && sentences[3].textPressed) {
                    gameState = MESHVIEWING;
                }
                sentences[1].process_release();
                sentences[2].process_release();
                sentences[3].process_release();
                
            }
            MouseLeftButtonPress = false;
        }
    }
    
    // “游戏说明”状态，处理鼠标点击事件。鼠标点击“返回”可以回到开始界面。
    else if (gameState == INTRODUCTION) {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            if (MouseLeftButtonPress == false) {
                glm::dvec2 mousePos = glm::dvec2(0.0, 0.0);
                glfwGetCursorPos(window, &mousePos.x, &mousePos.y);
                /*std::cout << mousePos.x << " " << mousePos.y << std::endl;*/
                if (sentences[4].judgeMouseButton(mousePos.x, SCR_HEIGHT - mousePos.y)) {
                    sentences[4].process_press();
                }
                /*std::cout << "press" << std::endl;*/
                
            }
            MouseLeftButtonPress = true;
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
            if (MouseLeftButtonPress == true) {
                glm::dvec2 mousePos = glm::dvec2(0.0, 0.0);
                glfwGetCursorPos(window, &mousePos.x, &mousePos.y);
                if (sentences[4].judgeMouseButton(mousePos.x, SCR_HEIGHT - mousePos.y) && sentences[4].textPressed) {
                    gameState = STARTMENU;
                }
                sentences[4].process_release();
                /*std::cout << "release" << std::endl;*/
            }
            MouseLeftButtonPress = false;
        }
    }

    // “查看网格”状态，处理键盘和鼠标点击事件。。。。。。。。。。待修改
    else if (gameState == MESHVIEWING) {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            glm::dvec2 mousePos = glm::dvec2(0.0, 0.0);
            glfwGetCursorPos(window, &mousePos.x, &mousePos.y);

            lastX = mousePos.x;
            lastY = mousePos.y;
            xoffset = 0;
            yoffset = 0;

            if (MouseLeftButtonPress == false) {
                if (sentences[4].judgeMouseButton(mousePos.x, SCR_HEIGHT - mousePos.y)) {
                    sentences[4].process_press();
                }
            }

            MouseLeftButtonPress = true;
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
            if (MouseLeftButtonPress == true) {
                glm::dvec2 mousePos = glm::dvec2(0.0, 0.0);
                glfwGetCursorPos(window, &mousePos.x, &mousePos.y);

                if (sentences[4].judgeMouseButton(mousePos.x, SCR_HEIGHT - mousePos.y) && sentences[4].textPressed) {
                    gameState = STARTMENU;
                }
                sentences[4].process_release();
            }

            MouseLeftButtonPress = false;
        }

    }

    // 
    else if (gameState == GAMING) {
        float modelSpeed = 0.5f * deltaTime;
        float modelRotSpeed = 0.7f * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            glm::vec3 modelFront = camera1.Front;
            float originYaw = myModel.Yaw;
            glm::vec3 originPos = myModel.Position;

            if (modelFront.x >= 0) {
                //modelYaw = originYaw + modelRotSpeed * (acos(modelFront.z) - originYaw);
                modelYaw = acos(modelFront.z);
            }
            else {
                //modelYaw = originYaw + modelRotSpeed * (-acos(modelFront.z) - originYaw);
                modelYaw = -acos(modelFront.z);
            }


            myModel.SetModelTransformation(myModel.Position + modelSpeed * modelFront, modelYaw);

            if (m.mazepeng(myModel.getChangedBoxPoints()[2], myModel.getChangedBoxPoints()[1], myModel.getChangedBoxPoints()[0])) {
                myModel.Position -= modelSpeed * modelFront;
                myModel.SetModelTransformation(myModel.Position, originYaw);
            }

            camera1.ProcessKeyboard(myModel.getChangedCenter());
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            glm::vec3 modelFront = -camera1.Front;
            float originYaw = myModel.Yaw;
            glm::vec3 originPos = myModel.Position;

            if (modelFront.x >= 0) {
                //modelYaw = originYaw + modelRotSpeed * (acos(modelFront.z) - originYaw);
                modelYaw = acos(modelFront.z);
            }
            else {
                //modelYaw = originYaw + modelRotSpeed * (-acos(modelFront.z) - originYaw);
                modelYaw = -acos(modelFront.z);
            }

            myModel.SetModelTransformation(myModel.Position + modelSpeed * modelFront, modelYaw);

            if (m.mazepeng(myModel.getChangedBoxPoints()[2], myModel.getChangedBoxPoints()[1], myModel.getChangedBoxPoints()[0])) {
                myModel.Position -= modelSpeed * modelFront;
                myModel.SetModelTransformation(myModel.Position, originYaw);
            }

            camera1.ProcessKeyboard(myModel.getChangedCenter());
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            glm::vec3 modelFront = camera1.Right;
            float originYaw = myModel.Yaw;
            glm::vec3 originPos = myModel.Position;

            if (glm::dot(myModel.Front, camera1.Front) >= 0) {
                modelYaw = myModel.Yaw - modelRotSpeed * acos(0);
            }
            else {
                modelYaw = myModel.Yaw + modelRotSpeed * acos(0);
            }

            myModel.SetModelTransformation(myModel.Position + modelSpeed * modelFront, modelYaw);

            if (m.mazepeng(myModel.getChangedBoxPoints()[2], myModel.getChangedBoxPoints()[1], myModel.getChangedBoxPoints()[0])) {
                myModel.Position -= modelSpeed * modelFront;
                myModel.SetModelTransformation(myModel.Position, originYaw);
            }

            camera1.ProcessKeyboard(myModel.getChangedCenter());
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            glm::vec3 modelFront = -camera1.Right;
            float originYaw = myModel.Yaw;
            glm::vec3 originPos = myModel.Position;

            if (glm::dot(myModel.Front, camera1.Front) >= 0) {
                modelYaw = myModel.Yaw + modelRotSpeed * acos(0);
            }
            else {
                modelYaw = myModel.Yaw - modelRotSpeed * acos(0);
            }

            myModel.SetModelTransformation(myModel.Position + modelSpeed * modelFront, modelYaw);

            if (m.mazepeng(myModel.getChangedBoxPoints()[2], myModel.getChangedBoxPoints()[1], myModel.getChangedBoxPoints()[0])) {
                myModel.Position -= modelSpeed * modelFront;
                myModel.SetModelTransformation(myModel.Position, originYaw);
            }

            camera1.ProcessKeyboard(myModel.getChangedCenter());
        }

         //判断走到出口
        if (glm::distance(myModel.Position, exitPos) <= cell_size / 2) {
            gameState = ENDING;
        }
        //if (glm::distance(myModel.Position, entrancePos + glm::vec3(0.0f, 0.0f, cell_size)) <= cell_size / 2) {
        //    gameState = ENDING;
        //}
        //if (glm::distance(myModel.Position, entrancePos + glm::vec3(cell_size, 0.0f, 0.0f)) <= cell_size / 2) {
        //    gameState = ENDING;
        //}
    }

    else if (gameState == ENDING) {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            glm::dvec2 mousePos = glm::dvec2(0.0, 0.0);
            glfwGetCursorPos(window, &mousePos.x, &mousePos.y);

            if (MouseLeftButtonPress == false) {
                if (sentences[4].judgeMouseButton(mousePos.x, SCR_HEIGHT - mousePos.y)) {
                    sentences[4].process_press();
                }
            }

            MouseLeftButtonPress = true;
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
            if (MouseLeftButtonPress == true) {
                glm::dvec2 mousePos = glm::dvec2(0.0, 0.0);
                glfwGetCursorPos(window, &mousePos.x, &mousePos.y);

                if (sentences[4].judgeMouseButton(mousePos.x, SCR_HEIGHT - mousePos.y) && sentences[4].textPressed) {
                    gameState = STARTMENU;
                    mciSendString("stop mp3", NULL, 0, NULL);
                }
                sentences[4].process_release();
            }

            MouseLeftButtonPress = false;
        }
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// 鼠标移动会自动调用该函数，计算鼠标位置和偏移。。。。。。。。。待修改
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    xoffset = xpos - lastX;
    yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    if (gameState == MESHVIEWING) {
        if (MouseLeftButtonPress) {
            glm::mat4 cameraRotMat(1.0f);
            //cameraRotMat = glm::rotate(cameraRotMat, -xoffset * 0.001f, glm::vec3(0, 1, 0));
            //cameraRotMat = glm::rotate(cameraRotMat, yoffset * 0.001f, glm::vec3(1, 0, 0));
            if (std::abs(xoffset) > std::abs(yoffset)) {
                cameraRotMat = glm::rotate(glm::mat4(1.0f), -xoffset * 0.001f, glm::vec3(0, 1, 0));
                camera3.Position = glm::vec3(cameraRotMat * glm::vec4(camera3.Position, 1.0f));
            }
            else {
                //glm::vec3 rotAxie= camera3.Right;
                //if(rotAxie != glm::vec3(0.0f)) 
                cameraRotMat = glm::rotate(glm::mat4(1.0f), yoffset * 0.001f, camera3.Right);
                glm::vec3 originPos = camera3.Position;
                camera3.Position = glm::vec3(cameraRotMat * glm::vec4(camera3.Position, 1.0f));
                if (camera3.Position.x * originPos.x < 0 && camera3.Position.z * originPos.z < 0) {
                    //if (originPos.y > 0) {
                        //camera3.Position = glm::vec3(0.0f, 2.99, 0.0f);
                    //}
                    //else {
                    //    camera3.Position = glm::vec3(0.0f, -2.9, 0.0f);
                    //}
                }
            }
            camera3.Front = glm::normalize(glm::vec3(0.0f) - camera3.Position);
            camera3.Right = glm::normalize(glm::cross(camera3.Front, camera3.WorldUp));
            camera3.Up = glm::normalize(glm::cross(camera3.Right, camera3.Front));
        }
    }

    if (gameState == GAMING) {
        camera1.ProcessMouseMovement(xoffset, yoffset);
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera1.ProcessMouseScroll(yoffset);
    camera3.ProcessMouseScroll(yoffset);
}