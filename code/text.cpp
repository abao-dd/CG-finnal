#include "text.h"

// ���ֵ�unicode����
// 0 - 3: �Թ�̽��
// 4 - 7: ��ʼ��Ϸ
// 8 - 11: ��Ϸ˵��
// 12 - 15: �鿴����
// 16 - 17: ����
// 18 - 27: �ӳ����۸��ӵ��Թ���
// 28 - 33: ���������ƶ�
// 34 - 41: ����ƶ������ӽ�
// 42 - 49�������ֽ�������
// 50 - 57����ϲ���ӳ����Թ�
std::vector<int> unicode = {
    36855, 23467, 25506, 38505,
    24320, 22987, 28216, 25103,
    28216, 25103, 35828, 26126,
    26597, 30475, 32593, 26684,
    36820, 22238,
    36867, 20986, 38169, 32508, 22797, 26434, 30340, 36855, 23467, 21543,
    25511, 21046, 20154, 29289, 31227, 21160,
    40736, 26631, 31227, 21160, 35843, 25972, 35270, 35282,
    40736, 26631, 28378, 36718, 36827, 34892, 32553, 25918,
    24685, 21916, 20320, 36867, 20986, 20102, 36855, 23467
};

std::map<int, Character> Characters = {};

// ���ӱ���
std::vector<Sentence> sentences;

unsigned int textVAO, textVBO;

// ��ȡǰ128��ASCII�ַ�����������
void getAsciiGlyph(std::map<int, Character>& Characters, std::string fontPath, glm::uvec2 pixelSize)
{
    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

    // Load font as face
    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

    // ���������С
    FT_Set_Pixel_Sizes(face, pixelSize.x, pixelSize.y);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // Load first 128 characters of ASCII set
    for (unsigned char c = 0; c < 128; c++)
    {
        // Load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<int, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

// ��ȡ�����ַ�����������
void getUnicodeGlyph(std::map<int, Character>& Characters, std::string fontPath, glm::uvec2 pixelSize)
{
    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

    // Load font as face
    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

    // ���������С
    FT_Set_Pixel_Sizes(face, pixelSize.x, pixelSize.y);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // ���ظ���unicode�б������б����Ӧ����������
    for (int i = 0; i < unicode.size(); ++i) {
        // ����unicode�����ȡ�ַ���face�����е�index���������������(glyph)
        int gIndex = FT_Get_Char_Index(face, unicode[i]);
        if (FT_Load_Glyph(face, gIndex, FT_LOAD_RENDER)) {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        // Generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<int, Character>(unicode[i], character));
    }
}

// ��ȾӢ���ı�
void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    // Activate corresponding render state	
    shader.use();
    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        // Update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// ��Ⱦ�����ı�
void RenderText(Shader& shader, std::vector<int> unicodeIndex, float x, float y, float scale, glm::vec3 color)
{
    // Activate corresponding render state	
    shader.use();
    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    // Iterate through all characters
    for (int i = 0; i < unicodeIndex.size(); ++i)
    {
        Character ch = Characters[unicode[unicodeIndex[i]]];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // Update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 }, // ����
            { xpos,     ypos,       0.0, 1.0 }, // ����
            { xpos + w, ypos,       1.0, 1.0 }, // ����

            { xpos,     ypos + h,   0.0, 0.0 }, // ����
            { xpos + w, ypos,       1.0, 1.0 }, // ����
            { xpos + w, ypos + h,   1.0, 0.0 }  // ����
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Sentence��ĳ�Ա��������Ⱦһ�����ľ���
void Sentence::Draw(Shader& shader)
{
    RenderText(shader, unicodeIndex, textBorder.x, textBorder.y, textScale, textColor);
}

// �ж�����Ƿ��ھ�������λ��
bool Sentence::judgeMouseButton(double mousePosX, double mousePosY) {
    return mousePosX >= textBorder.x && mousePosX <= textBorder.x + textSize.x * textScale
        && mousePosY >= textBorder.y
        && mousePosY <= textBorder.y + textSize.y * textScale;
}

// ����������¼�
void Sentence::process_press() {
    textColor = glm::vec3(1.0f, 1.0f, 0.0f);
    textPressed = true;
}

// ����ſ�����¼�
void Sentence::process_release() {
    textColor = glm::vec3(1.0f, 1.0f, 1.0f);
    textPressed = false;
}