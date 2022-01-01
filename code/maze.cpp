#include "maze.h"

maze::maze(int row, int cloumn, float maze_siz, float maze_heigh,
    float maze_floo, float as) :maze_size(maze_siz), maze_height(maze_heigh), maze_floor(maze_floo), asp(as) {
    srand(time(0));
    std::vector<std::vector<std::vector<int>>> maze;
    maze.resize(row);
    for (int i = 0; i < row; i++) {
        maze[i].resize(cloumn);
        for (int j = 0; j < cloumn; j++) {
            maze[i][j].resize(5, 0);
        }
    }
    std::vector<std::vector<int>> history;
    history.push_back({ 0, 0 });
    while (history.size() > 0) {
        std::vector<int> tem = history[rand() % history.size()];
        int r = tem[0], c = tem[1];
        maze[r][c][4] = 1;
        history.erase(remove(history.begin(), history.end(), tem), history.end());
        std::vector<int> check;
        if (c > 0) {
            if (maze[r][c - 1][4] == 1) {
                check.push_back(0);  // l
            }
            else if (maze[r][c - 1][4] == 0) {
                history.push_back({ r, c - 1 });
                maze[r][c - 1][4] = 2;
            }
        }
        if (r > 0) {
            if (maze[r - 1][c][4] == 1) {
                check.push_back(1);  // u
            }
            else if (maze[r - 1][c][4] == 0) {
                history.push_back({ r - 1, c });
                maze[r - 1][c][4] = 2;
            }
        }
        if (c < cloumn - 1) {
            if (maze[r][c + 1][4] == 1) {
                check.push_back(2);  // r
            }
            else if (maze[r][c + 1][4] == 0) {
                history.push_back({ r, c + 1 });
                maze[r][c + 1][4] = 2;
            }
        }
        if (r < row - 1) {
            if (maze[r + 1][c][4] == 1) {
                check.push_back(3);  // d
            }
            else if (maze[r + 1][c][4] == 0) {
                history.push_back({ r + 1, c });
                maze[r + 1][c][4] = 2;
            }
        }
        if (check.size() > 0) {
            int direction = check[rand() % check.size()];
            maze[r][c][direction] = 1;
            if (direction == 0) {
                maze[r][c - 1][2] = 1;
            }
            else if (direction == 1) {
                maze[r - 1][c][3] = 1;
            }
            else if (direction == 2) {
                maze[r][c + 1][0] = 1;
            }
            else {
                maze[r + 1][c][1] = 1;
            }
        }
    }
    maze[0][0][0] = 1;
    maze[row - 1][cloumn - 1][2] = 1;
    maze_map.resize(maze.size() * 3);
    for (int i = 0; i < maze.size() * 3; i++) {
        maze_map[i].resize(maze[0].size() * 3, false);
    }
    for (int i = 0; i < maze.size(); i++) {
        for (int j = 0; j < maze[0].size(); j++) {
            maze_map[i * 3 + 1][j * 3 + 1] = true;
            if (maze[i][j][0] == 1) {
                maze_map[i * 3 + 1][j * 3] = true;
            }
            if (maze[i][j][1] == 1) {
                maze_map[i * 3][j * 3 + 1] = true;
            }
            if (maze[i][j][2] == 1) {
                maze_map[i * 3 + 1][j * 3 + 2] = true;
            }
            if (maze[i][j][3] == 1) {
                maze_map[i * 3 + 2][j * 3 + 1] = true;
            }
        }
    }
    make_maze();
    //minimaze
    minimap.resize(maze_map.size());
    for (int i = 0; i < maze_map.size(); i++) {
        minimap[i].resize(maze_map[0].size(), false);
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            minimap[i][j] = true;
        }
    }
}

void maze::print_maze() {
    for (int i = 0; i < maze_map.size(); i++) {
        for (int j = 0; j < maze_map[0].size(); j++) {
            if (maze_map[i][j]) {
                std::cout << ".";
            }
            else {
                std::cout << "#";
            }
        }
        std::cout << "\n";
    }
}

void maze::make_maze() {
    cell_size = maze_size / (maze_map.size() / 3);
    path_size = cell_size * asp;
    box_width = cell_size * (1 - asp) / 2;
    entrancePos = glm::vec3(-maze_size / 2 + cell_size / 2, maze_floor, -maze_size / 2 + cell_size / 2);
    exitPos = glm::vec3(maze_size / 2 - cell_size / 2, maze_floor, maze_size / 2 - cell_size / 2);
    cubeScales.push_back(glm::vec3(box_width, maze_height, box_width));
    cubeScales.push_back(glm::vec3(path_size, maze_height, box_width));
    cubeScales.push_back(glm::vec3(box_width, maze_height, path_size));
    cubeScales.push_back(glm::vec3(maze_size, 0.1f, maze_size));
    float cell_b_x, cell_b_z;
    for (int i = 0; i < maze_map.size(); i++) {
        for (int j = 0; j < maze_map[0].size(); j++) {
            cell_b_x = -maze_size / 2 + j / 3 * cell_size;
            cell_b_z = -maze_size / 2 + i / 3 * cell_size;
            if (!maze_map[i][j]) {
                if (i % 3 == 0) {
                    if (j % 3 == 0) {
                        cubePositions.push_back(glm::vec3(cell_b_x, maze_floor, cell_b_z));
                    }
                    else if (j % 3 == 1) {
                        cubePositions.push_back(
                            glm::vec3(cell_b_x + box_width, maze_floor, cell_b_z));
                    }
                    else {
                        cubePositions.push_back(glm::vec3(cell_b_x + box_width + path_size,
                            maze_floor, cell_b_z));
                    }
                }
                else if (i % 3 == 1) {
                    if (j % 3 == 0) {
                        cubePositions.push_back(
                            glm::vec3(cell_b_x, maze_floor, cell_b_z + box_width));
                    }
                    else if (j % 3 == 2) {
                        cubePositions.push_back(glm::vec3(cell_b_x + box_width + path_size,
                            maze_floor,
                            cell_b_z + box_width));
                    }
                }
                else {
                    if (j % 3 == 0) {
                        cubePositions.push_back(glm::vec3(
                            cell_b_x, maze_floor, cell_b_z + box_width + path_size));
                    }
                    else if (j % 3 == 1) {
                        cubePositions.push_back(
                            glm::vec3(cell_b_x + box_width, maze_floor,
                                cell_b_z + box_width + path_size));
                    }
                    else {
                        cubePositions.push_back(
                            glm::vec3(cell_b_x + box_width + path_size, maze_floor,
                                cell_b_z + box_width + path_size));
                    }
                }
            }
        }
    }
    // floor
    cubePositions.push_back(
        glm::vec3(-maze_size / 2, maze_floor - 0.1f, -maze_size / 2));
}

bool maze::mazepeng(glm::vec3 m1, glm::vec3 m2, glm::vec3 m3) {
    int column = (m2.x + maze_size / 2) / cell_size;
    int row = (m2.z + maze_size / 2) / cell_size;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (row + i < 0 || column + j < 0 || row + i >= maze_map.size() / 3 || column + j >= maze_map[0].size() / 3) {
                continue;
            }
            float cell_b_x, cell_b_z;
            cell_b_x = -maze_size / 2 + (column + j) * cell_size;
            cell_b_z = -maze_size / 2 + (row + i) * cell_size;
            int col = (column + j) * 3;
            int ro = (row + i) * 3;
            if (!maze_map[ro][col] && rectanglepeng(m1, m2, m3, glm::vec3(cell_b_x, 0.0f, cell_b_z + box_width),
                glm::vec3(cell_b_x + box_width, 0.0f, cell_b_z + box_width),
                glm::vec3(cell_b_x + box_width, 0.0f, cell_b_z))) {
                return true;
            }
            if (!maze_map[ro][col + 2] && rectanglepeng(m1, m2, m3, glm::vec3(cell_b_x + box_width + path_size, 0.0f, cell_b_z + box_width),
                glm::vec3(cell_b_x + cell_size, 0.0f, cell_b_z + box_width),
                glm::vec3(cell_b_x + cell_size, 0.0f, cell_b_z))) {
                return true;
            }
            if (!maze_map[ro + 2][col] && rectanglepeng(m1, m2, m3, glm::vec3(cell_b_x, 0.0f, cell_b_z + cell_size),
                glm::vec3(cell_b_x + box_width, 0.0f, cell_b_z + cell_size),
                glm::vec3(cell_b_x + box_width, 0.0f, cell_b_z + box_width + path_size))) {
                return true;
            }
            if (!maze_map[ro + 2][col + 2] && rectanglepeng(m1, m2, m3, glm::vec3(cell_b_x + box_width + path_size, 0.0f, cell_b_z + cell_size),
                glm::vec3(cell_b_x + cell_size, 0.0f, cell_b_z + cell_size),
                glm::vec3(cell_b_x + cell_size, 0.0f, cell_b_z + box_width + path_size))) {
                return true;
            }
            //
            if (!maze_map[ro][col + 1] && rectanglepeng(m1, m2, m3, glm::vec3(cell_b_x + box_width, 0.0f, cell_b_z + box_width),
                glm::vec3(cell_b_x + box_width + path_size, 0.0f, cell_b_z + box_width),
                glm::vec3(cell_b_x + box_width + path_size, 0.0f, cell_b_z))) {
                return true;
            }
            if (!maze_map[ro + 1][col] && rectanglepeng(m1, m2, m3, glm::vec3(cell_b_x, 0.0f, cell_b_z + box_width + path_size),
                glm::vec3(cell_b_x + box_width, 0.0f, cell_b_z + box_width + path_size),
                glm::vec3(cell_b_x + box_width, 0.0f, cell_b_z + box_width))) {
                return true;
            }
            if (!maze_map[ro + 1][col + 2] && rectanglepeng(m1, m2, m3, glm::vec3(cell_b_x + box_width + path_size, 0.0f, cell_b_z + box_width + path_size),
                glm::vec3(cell_b_x + cell_size, 0.0f, cell_b_z + box_width + path_size),
                glm::vec3(cell_b_x + cell_size, 0.0f, cell_b_z + box_width))) {
                return true;
            }
            if (!maze_map[ro + 2][col + 1] && rectanglepeng(m1, m2, m3, glm::vec3(cell_b_x + box_width, 0.0f, cell_b_z + cell_size),
                glm::vec3(cell_b_x + box_width + path_size, 0.0f, cell_b_z + cell_size),
                glm::vec3(cell_b_x + box_width + path_size, 0.0f, cell_b_z + box_width + path_size))) {
                return true;
            }
        }
    }
    return false;
}

bool maze::linepeng(glm::vec3 a1, glm::vec3 a2, glm::vec3 b1, glm::vec3 b2) {
    glm::vec3 b1a1 = a1 - b1, b1b2 = b2 - b1, b1a2 = a2 - b1, a1b1 = b1 - a1, a1a2 = a2 - a1, a1b2 = b2 - a1;
    if ((b1a1.z * b1b2.x - b1a1.x * b1b2.z) * (b1a2.z * b1b2.x - b1a2.x * b1b2.z) >= 0) {
        return false;
    }
    if ((a1b1.z * a1a2.x - a1b1.x * a1a2.z) * (a1b2.z * a1a2.x - a1b2.x * a1a2.z) >= 0) {
        return false;
    }
    return true;
}
bool maze::rectanglepeng(glm::vec3 a1, glm::vec3 a2, glm::vec3 a3, glm::vec3 b1,
    glm::vec3 b2, glm::vec3 b3) {
    // a2,b2是直角
    glm::vec3 a4 = a1 + a3 - a2;
    glm::vec3 b4 = b1 + b3 - b2;
    if (linepeng(a1, a2, b1, b2)) {
        return true;
    }
    if (linepeng(a1, a2, b2, b3)) {
        return true;
    }
    if (linepeng(a1, a2, b3, b4)) {
        return true;
    }
    if (linepeng(a1, a2, b4, b1)) {
        return true;
    }
    if (linepeng(a2, a3, b1, b2)) {
        return true;
    }
    if (linepeng(a2, a3, b2, b3)) {
        return true;
    }
    if (linepeng(a2, a3, b3, b4)) {
        return true;
    }
    if (linepeng(a2, a3, b4, b1)) {
        return true;
    }
    if (linepeng(a3, a4, b1, b2)) {
        return true;
    }
    if (linepeng(a3, a4, b2, b3)) {
        return true;
    }
    if (linepeng(a3, a4, b3, b4)) {
        return true;
    }
    if (linepeng(a3, a4, b4, b1)) {
        return true;
    }
    if (linepeng(a4, a1, b1, b2)) {
        return true;
    }
    if (linepeng(a4, a1, b2, b3)) {
        return true;
    }
    if (linepeng(a4, a1, b3, b4)) {
        return true;
    }
    if (linepeng(a4, a1, b4, b1)) {
        return true;
    }
    return false;
}

float maze::get_maze_size() {
    return maze_size;
}

float maze::get_maze_floor() {
    return maze_floor;
}

float maze::get_maze_height() {
    return maze_height;
}

float maze::get_cell_size() {
    return cell_size;
}

glm::vec3 maze::get_entrancePos() {
    return entrancePos;
}
glm::vec3 maze::get_exitPos() {
    return exitPos;
}

void maze::maze_render(Shader& simpleDepthShader, unsigned int boxVAO, unsigned int diffuseMapwhite, unsigned int diffuseMapblue) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMapwhite);
    int num_cube = 0;
    glm::mat4 model;
    for (unsigned int i = 0; i < maze_map.size(); i++)
    {
        for (unsigned int j = 0; j < maze_map[0].size(); j++) {
            if (!maze_map[i][j]) {
                model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
                model = glm::translate(model, cubePositions[num_cube]);
                num_cube++;
                if (i % 3 != 1) {
                    if (j % 3 == 1) {
                        model = glm::scale(model, cubeScales[1]);
                    }
                    else {
                        model = glm::scale(model, cubeScales[0]);
                    }
                }
                else if (i % 3 == 1) {
                    model = glm::scale(model, cubeScales[2]);
                }
                simpleDepthShader.setMat4("model", model);
                glBindVertexArray(boxVAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMapblue);
    model = glm::mat4(1.0f);
    model = glm::translate(model, cubePositions[num_cube]);
    num_cube++;
    model = glm::scale(model, cubeScales[3]);
    simpleDepthShader.setMat4("model", model);
    glBindVertexArray(boxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void maze::mini_maze_render(Shader& minimazeShader, unsigned int lightCubeVAO) {
    glBindVertexArray(lightCubeVAO);
    float bex, bez;
    glm::mat4 model;
    for (unsigned int i = 0; i < minimap.size(); i++)
    {
        for (unsigned int j = 0; j < minimap[0].size(); j++) {
            model = glm::mat4(1.0f); 
            bex = -maze_size / 2 + cell_size * (j / 3);
            bez = -maze_size / 2 + cell_size * (i / 3);
            if (i % 3 == 0) {
                if (j % 3 == 0) {
                    model = glm::translate(model, glm::vec3(bex, maze_floor, bez));
                    model = glm::scale(model, glm::vec3(box_width, box_width, box_width));
                }
                else if (j % 3 == 1) {
                    model = glm::translate(model, glm::vec3(bex + box_width, maze_floor, bez));
                    model = glm::scale(model, glm::vec3(path_size, box_width, box_width));
                }
                else {
                    model = glm::translate(model, glm::vec3(bex + box_width + path_size, maze_floor, bez));
                    model = glm::scale(model, glm::vec3(box_width, box_width, box_width));
                }
            }
            else if (i % 3 == 1) {
                if (j % 3 == 0) {
                    model = glm::translate(model, glm::vec3(bex, maze_floor, bez + box_width));
                    model = glm::scale(model, glm::vec3(box_width, box_width, path_size));
                }
                else if (j % 3 == 1) {
                    model = glm::translate(model, glm::vec3(bex + box_width, maze_floor, bez + box_width));
                    model = glm::scale(model, glm::vec3(path_size, box_width, path_size));
                }
                else {
                    model = glm::translate(model, glm::vec3(bex + box_width + path_size, maze_floor, bez + box_width));
                    model = glm::scale(model, glm::vec3(box_width, box_width, path_size));
                }
            }
            else {
                if (j % 3 == 0) {
                    model = glm::translate(model, glm::vec3(bex, maze_floor, bez + box_width + path_size));
                    model = glm::scale(model, glm::vec3(box_width, box_width, box_width));
                }
                else if (j % 3 == 1) {
                    model = glm::translate(model, glm::vec3(bex + box_width, maze_floor, bez + box_width + path_size));
                    model = glm::scale(model, glm::vec3(path_size, box_width, box_width));
                }
                else {
                    model = glm::translate(model, glm::vec3(bex + box_width + path_size, maze_floor, bez + box_width + path_size));
                    model = glm::scale(model, glm::vec3(box_width, box_width, box_width));
                }
            }
            if (!minimap[i][j]) {
                minimazeShader.setInt("white", 0);
                minimazeShader.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            else if (maze_map[i][j]) {
                minimazeShader.setInt("white", 2);
                minimazeShader.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            else {
                minimazeShader.setInt("white", 1);
                minimazeShader.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
    }
}