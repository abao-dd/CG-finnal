#ifndef TEXT_H
#define TEXT_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"shader.h"
#include"camera.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include<iostream>
#include<vector>
#include <map>
#include <string>

extern std::vector<int> unicode;

struct Character {
    unsigned int TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Horizontal offset to advance to next glyph
};

extern std::map<int, Character> Characters;

// (中文)句子类
class Sentence {
public:
    std::vector<int> unicodeIndex;  // 句子里每个字在unicode数组的index
    glm::vec2 textBorder;   // 句子左下角坐标
    glm::vec2 textSize; // 句子的宽和高(不计算scale的值, 不计算在基准线以下的高度(如果有的话))
    float textScale;    // 句子的缩放程度
    glm::vec3 textColor;    // 颜色
    bool textPressed;   // 鼠标是否按下句子

    Sentence() {}
    Sentence(std::vector<int> index, glm::vec2 tborder, glm::vec2 tsize, float tscale, glm::vec3 tcolor)
        : unicodeIndex(index), textBorder(tborder), textSize(tsize), textScale(tscale), textColor(tcolor)
    {
        countTextSize();
        textPressed = false;
    }

    void countTextSize()
    {
        textSize = glm::vec2(0.0f, 0.0f);
        for (int i = 0; i < unicodeIndex.size(); ++i) {
            Character ch = Characters[unicode[unicodeIndex[i]]];
            textSize.x += (ch.Advance >> 6);
            //textSize.y = max(1.0f, 1.0f);
            //textSize.y = std::max((float)textSize.y, (float)ch.Bearing.y);
            textSize.y = textSize.y > (float)ch.Bearing.y ? textSize.y : (float)ch.Bearing.y;
        }
    }

    void Draw(Shader& shader);

    bool judgeMouseButton(double mousePosX, double mousePosY);

    void process_press();

    void process_release();
};

// 句子变量
//extern Sentence sentence1;
//extern Sentence sentence2;
//extern Sentence sentence3;
extern std::vector<Sentence> sentences;

extern unsigned int textVAO, textVBO;

// 获取前128个ASCII字符的字形纹理
void getAsciiGlyph(std::map<int, Character>& Characters, std::string fontPath, glm::uvec2 pixelSize);
// 获取中文字符的字形纹理
void getUnicodeGlyph(std::map<int, Character>& Characters, std::string fontPath, glm::uvec2 pixelSize);

// 渲染英文文本
void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color);
// 渲染中文文本
void RenderText(Shader& shader, std::vector<int> unicodeIndex, float x, float y, float scale, glm::vec3 color);

#endif