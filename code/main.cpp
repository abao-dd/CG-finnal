#include "render.h"

//ostream& operator<<(ostream& o, glm::vec3 v3)
//{
//    o << "(" << v3.x << ", " << v3.y << ", " << v3.z << ")";
//    return o;
//}

void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


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

    // ������ɫ������
    // maze
    Shader boxShader("shaderfile/material_vs.txt", "shaderfile/material_fs.txt");
    //minimaze
    //Shader minimazeShader("shaderfile/minimaze_vs.txt", "shaderfile/minimaze_fs.txt");
    //skybox
    Shader skyboxShader("shaderfile/skybox_vs.txt", "shaderfile/skybox_fs.txt");
    //light
    Shader lightCubeShader("shaderfile/light_vs.txt", "shaderfile/light_fs.txt");

    //��Ӱ��ͼ�������ͼ
    Shader simpleDepthShader("shaderfile/shadowmap_vs.txt", "shaderfile/shadowmap_fs.txt");
    //Shader debugDepthQuad("shaderfile/debug_quad_vs.txt", "shaderfile/debug_quad_fs.txt");
    // ģ��
    Shader modelShader("shaderfile/model_loading_vs.txt", "shaderfile/model_loading_fs.txt");
    // ����
    Shader textShader("shaderfile/freetype_vs.txt", "shaderfile/freetype_fs.txt");

    // ����ģ��
    // ����ģ��
    Model islandModel("model/82-island/Files/fbx file/island.fbx");
    // ����ģ��
    myModel = Model("model/Ayaka model/Ayaka model.pmx");

    // �Թ�����
    cell_size = m.get_cell_size();
    entrancePos = m.get_entrancePos();
    exitPos = m.get_exitPos();

    // ��պ���ͼ
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

    //�������
    unsigned int diffuseMapwhite = loadTexture("textures/63.jpg");
    unsigned int diffuseMapblue = loadTexture("textures/147fq.jpg");
    unsigned int diffusemount = loadTexture("textures/61.jpg");

    //��Ȼ���
    depth_buffer();
    
    // ������������
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

        //��ʾ֡��
        show_maze_fps();

        // ������̺����������
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ����״̬������ͬ״̬��Ⱦ��ͬ����
        // ----------
        // ����ʼĿ¼��״̬
        if (gameState == STARTMENU) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            textShader.use();
            // "�Թ�̽��"
            sentences[0].Draw(textShader);
            //"��ʼ��Ϸ"
            sentences[1].Draw(textShader);
            // "��Ϸ˵��"
            sentences[2].Draw(textShader);
            // "�鿴����"
            sentences[3].Draw(textShader);
        }

        // ����Ϸ˵����״̬
        if (gameState == INTRODUCTION) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            // "�ӳ����۸��ӵ��Թ���"
            sentences[5].Draw(textShader);
            RenderText(textShader, "awsd", 0.24 * SCR_WIDTH, 0.58 * SCR_HEIGHT, 1.0f * SCR_WIDTH / 800, glm::vec3(1.0, 1.0, 1.0));
            // "���������ƶ�"
            sentences[6].Draw(textShader);
            // "����ƶ������ӽ�"
            sentences[7].Draw(textShader);
            // "�����ֽ�������"
            sentences[8].Draw(textShader);
            // "����"
            sentences[4].Draw(textShader);
        }

        // ���鿴����״̬
        if (gameState == MESHVIEWING) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            
            // ��Ⱦ����
            modelShader.use();
            myModel.SetModelTransformation(glm::vec3(0.0f, -myModel.getOriginalCenter().y * 0.2, 0.0f), 0.0f, 0.2f);
            render_character(modelShader, myModel, cameraMesh);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            // "����"
            sentences[4].Draw(textShader);
        }

        // ����ʼ��Ϸ��״̬
        if (gameState == GAMING) {
            if (firstGaming) {
                mciSendString("play mp3 from 0 repeat", NULL, 0, NULL);
                // �ս�����Ϸģʽʱ�����ú������������ĳ�ʼλ��
                modelShader.use();
                MODELSCALE = cell_size / 40;
                myModel.SetModelTransformation(entrancePos, glm::radians(90.0f));

                cameraGaming = Camera(myModel.getChangedCenter() + glm::vec3(-cell_size / 2, 0.0f, 0.0f),
                    myModel.getChangedCenter(), cell_size / 2);

                firstGaming = false;
            }
            //��׽���������
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            // 1. render depth of scene to texture (from light's perspective)
            // --------------------------------------------------------------
            glm::mat4 lightProjection, lightView;
            glm::mat4 lightSpaceMatrix;
            float near_plane = 1.0f, far_plane = 7.5f;
            lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
            //�޸���ͼλ�ÿ��Կ�����Ӱ
            lightView = glm::lookAt(glm::vec3(-2.0f, 5.0f, -3.0f), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
            lightSpaceMatrix = lightProjection * lightView;
            // render scene from light's point of view
            simpleDepthShader.use();
            simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

            //����
            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);

            render_shadowsense(simpleDepthShader, diffuseMapwhite, diffuseMapblue, cameraGaming);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            // reset viewport
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //�ڶ�����Ⱦ
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //��Ⱦ�ƹ�
            lightCubeShader.use();
            render_light(lightCubeShader, currentFrame, cameraGaming);

            // bind diffuse map,������
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, diffuseMapwhite);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            //��Ⱦ�Թ�
            boxShader.use();
            render_sense(boxShader, lightSpaceMatrix, diffuseMapwhite, diffuseMapblue, currentFrame, cameraGaming);

            // ��Ⱦ��
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, diffusemount);
            modelShader.use();
            render_island(modelShader, islandModel, cameraGaming);

            // ��Ⱦ����
            modelShader.use();
            render_character(modelShader, myModel, cameraGaming);

            // ��պ�
            skyboxShader.use();
            render_skybox(skyboxShader, cubemapTexture, cameraGaming);
        }

        // ����Ϸ������״̬
        if (gameState == ENDING) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

            //��պ�
            skyboxShader.use();
            render_skybox(skyboxShader, cubemapTexture, cameraGaming);

            // "��ϲ���ӳ����Թ�"
            sentences[9].Draw(textShader);
            // "����"
            sentences[4].Draw(textShader);
        }


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    deletevao();
    glfwTerminate();
    return 0;
}

// processInput()����������̺�������¼�����������״̬������Ӧ����Ӧ
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    // esc���˳�����ȫ״̬ͨ��
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // ����ʼĿ¼��״̬������������¼����������ͬ�����ֵ���״̬���ı仯��
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
                    cameraMesh.MouseSensitivity = 0.2f;
                }
                sentences[1].process_release();
                sentences[2].process_release();
                sentences[3].process_release();
            }
            MouseLeftButtonPress = false;
        }
    }
    
    // ����Ϸ˵����״̬������������¼�����������������ء����Իص���ʼ���档
    else if (gameState == INTRODUCTION) {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            if (MouseLeftButtonPress == false) {
                glm::dvec2 mousePos = glm::dvec2(0.0, 0.0);
                glfwGetCursorPos(window, &mousePos.x, &mousePos.y);
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

    // ���鿴����״̬��������̵���¼�����������������ء����Իص���ʼ���档
    else if (gameState == MESHVIEWING) {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            if (MouseLeftButtonPress == false) {
                glm::dvec2 mousePos = glm::dvec2(0.0, 0.0);
                glfwGetCursorPos(window, &mousePos.x, &mousePos.y);
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

    // ����Ϸ�С�״̬��������̵���¼�������awsd���������ƶ�
    else if (gameState == GAMING) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            myModel.ProcessKeyboard(FORWARD, deltaTime, cameraGaming, m);
            cameraGaming.ProcessKeyboard(myModel.getChangedCenter());
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            myModel.ProcessKeyboard(BACKWARD, deltaTime, cameraGaming, m);
            cameraGaming.ProcessKeyboard(myModel.getChangedCenter());
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            myModel.ProcessKeyboard(RIGHT, deltaTime, cameraGaming, m);
            cameraGaming.ProcessKeyboard(myModel.getChangedCenter());
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            myModel.ProcessKeyboard(LEFT, deltaTime, cameraGaming, m);
            cameraGaming.ProcessKeyboard(myModel.getChangedCenter());
        }

         //�ж��ߵ�����
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

    // ����Ϸ������״̬������������¼�����������������ء����Իص���ʼ����
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

// ����ƶ����Զ����øú������������λ�ú�ƫ��
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

    // ���鿴����״̬
    if (gameState == MESHVIEWING) {
        // ��갴�²��϶������ԴӲ�ͬ�ӽǲ鿴����
        if (MouseLeftButtonPress) {
            cameraMesh.ProcessMouseMovement(xoffset, yoffset, true);
        }
    }

    // ����Ϸ�С�״̬
    if (gameState == GAMING) {
        cameraGaming.ProcessMouseMovement(xoffset, yoffset);
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

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    cameraGaming.ProcessMouseScroll(yoffset);
    cameraMesh.ProcessMouseScroll(yoffset);
}