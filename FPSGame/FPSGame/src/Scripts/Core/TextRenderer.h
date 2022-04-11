#pragma once

#include <glad.h>
#include <map>
#include <ft2build.h>
#include "GLM/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

#include FT_FREETYPE_H

class TextRenderer
{
public:
	TextRenderer(const char* font, unsigned short int fontSize, glm::vec2 screenSize): m_Font(font), m_FontSize(fontSize), m_ScreenSize(screenSize), m_Initialized(false)
	{
		m_Initialized = Init();
	}

	bool Init()
	{
        if (m_Initialized) 
        {
            std::cout << "Text Renderer Already Has Been Intialized!" << std::endl;
            return true; 
        }

		// compile and setup the shader
		// ----------------------------
		m_Shader = new Shader(R"(D:\OddStoneGames\FPSGame\FPSGame\FPSGame\src\Scripts\Shaders\text.vs)", 
                              R"(D:\OddStoneGames\FPSGame\FPSGame\FPSGame\src\Scripts\Shaders\text.fs)");
		glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_ScreenSize.x), 0.0f, static_cast<float>(m_ScreenSize.y));
		m_Shader->use();
		glUniformMatrix4fv(glGetUniformLocation(m_Shader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // FreeType
        // --------
        FT_Library ft;
        // All functions return a value different than 0 whenever an error occurred
        if (FT_Init_FreeType(&ft))
        {
            std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
            return false;
        }

        // find path to font
        std::string font_name = m_Font;
        if (font_name.empty())
        {
            std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
            return false;
        }

        // load font as face
        FT_Face face;
        if (FT_New_Face(ft, m_Font, 0, &face)) {
            std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
            return false;
        }
        else {
            // set size to load glyphs as
            FT_Set_Pixel_Sizes(face, 0, m_FontSize);

            // disable byte-alignment restriction
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            // load first 128 characters of ASCII set
            for (unsigned char c = 0; c < 128; c++)
            {
                // Load character glyph 
                if (FT_Load_Char(face, c, FT_LOAD_RENDER))
                {
                    std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                    continue;
                }
                // generate texture
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
                // set texture options
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                // now store character for later use
                Character character = {
                    texture,
                    glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                    static_cast<unsigned int>(face->glyph->advance.x)
                };
                Characters.insert(std::pair<char, Character>(c, character));
            }
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        // destroy FreeType once we're finished
        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        // configure VAO/VBO for texture quads
        // -----------------------------------
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
	}

    /// <summary>
    /// Renders A Line Of Text.
    /// </summary>
    /// <param name="text">The Text To Render</param>
    /// <param name="position">Position Of Text in Screen Space Coordinates</param>
    /// <param name="scale">Scale Of Text</param>
    /// <param name="color">Color Of Text</param>
    void RenderText(std::string text, glm::vec2 position, float scale, glm::vec3 color)
    {
        if (!m_Initialized)
        {
            std::cout << "Cannot Render Text Because Text Renderer is Not Initialized" << std::endl;
        }
        // activate corresponding render state	
        m_Shader->use();
        glUniform3f(glGetUniformLocation(m_Shader->ID, "textColor"), color.x, color.y, color.z);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        // iterate through all characters
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            Character ch = Characters[*c];

            float xpos = position.x + ch.Bearing.x * scale;
            float ypos = position.y - (ch.Size.y - ch.Bearing.y) * scale;

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;
            // update VBO for each character
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };
            // render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            position.x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

private:
	/// Holds all state information relevant to a character as loaded using FreeType
	struct Character {
		unsigned int TextureID; // ID handle of the glyph texture
		glm::ivec2   Size;      // Size of glyph
		glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
		unsigned int Advance;   // Horizontal offset to advance to next glyph
	};

    bool m_Initialized;
	glm::vec2 m_ScreenSize;
	const char* m_Font;
    unsigned short int m_FontSize;
    Shader* m_Shader;
	std::map<GLchar, Character> Characters;
	unsigned int VAO, VBO;
};