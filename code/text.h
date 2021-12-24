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

// (����)������
class Sentence {
public:
    std::vector<int> unicodeIndex;  // ������ÿ������unicode�����index
    glm::vec2 textBorder;   // �������½�����
    glm::vec2 textSize; // ���ӵĿ�͸�(������scale��ֵ, �������ڻ�׼�����µĸ߶�(����еĻ�))
    float textScale;    // ���ӵ����ų̶�
    glm::vec3 textColor;    // ��ɫ
    bool textPressed;   // ����Ƿ��¾���

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

// ���ӱ���
//extern Sentence sentence1;
//extern Sentence sentence2;
//extern Sentence sentence3;
extern std::vector<Sentence> sentences;

extern unsigned int textVAO, textVBO;

// ��ȡǰ128��ASCII�ַ�����������
void getAsciiGlyph(std::map<int, Character>& Characters, std::string fontPath, glm::uvec2 pixelSize);
// ��ȡ�����ַ�����������
void getUnicodeGlyph(std::map<int, Character>& Characters, std::string fontPath, glm::uvec2 pixelSize);

// ��ȾӢ���ı�
void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color);
// ��Ⱦ�����ı�
void RenderText(Shader& shader, std::vector<int> unicodeIndex, float x, float y, float scale, glm::vec3 color);

#endif