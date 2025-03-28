#include "spriteRenderer.h"

#include "PVRUtils/OpenGLES/BindingsGles.h"

SpriteRenderer::SpriteRenderer() {}

SpriteRenderer::SpriteRenderer(Shader shader)
{
    m_shader = shader;
    initRenderData();
}

SpriteRenderer::~SpriteRenderer()
{
    gl::DeleteVertexArrays(1, &m_quadVAO);
}

void SpriteRenderer::drawSprite(Texture2D texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color)
{
    // prepare transformations
    m_shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move origin of rotation to center of quad
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // move origin back

    model = glm::scale(model, glm::vec3(size, 1.0f)); // last scale

    m_shader.setMatrix4("model", model);

    // render textured quad
    m_shader.setVector3f("spriteColor", color);

    gl::ActiveTexture(GL_TEXTURE0);
    texture.bind();

    gl::BindVertexArray(m_quadVAO);
    gl::DrawArrays(GL_TRIANGLES, 0, 6);
    gl::BindVertexArray(0);
}

void SpriteRenderer::initRenderData()
{
    // configure VAO/VBO
    unsigned int VBO;
    float vertices[] = { 
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    gl::GenVertexArrays(1, &m_quadVAO);
    gl::GenBuffers(1, &VBO);

    gl::BindBuffer(GL_ARRAY_BUFFER, VBO);
    gl::BufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    gl::BindVertexArray(m_quadVAO);
    gl::EnableVertexAttribArray(0);
    gl::VertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    gl::BindBuffer(GL_ARRAY_BUFFER, 0);
    gl::BindVertexArray(0);
}