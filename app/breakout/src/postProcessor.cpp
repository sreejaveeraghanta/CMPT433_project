#include "postProcessor.h"

#include "PVRUtils/PVRUtilsGles.h"

#include <iostream>

PostProcessor::PostProcessor(Shader shader, unsigned int width, unsigned int height) 
    : PostProcessingShader(shader), Texture(), Width(width), Height(height), Confuse(false), Chaos(false), Shake(false)
{
    // initialize renderbuffer/framebuffer object
    gl::GenFramebuffers(1, &this->MSFBO);
    gl::GenFramebuffers(1, &this->FBO);
    gl::GenRenderbuffers(1, &this->RBO);
    // initialize renderbuffer storage with a multisampled color buffer (don't need a depth/stencil buffer)
    gl::BindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
    gl::BindRenderbuffer(GL_RENDERBUFFER, this->RBO);
    gl::RenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB8, width, height); // allocate storage for render buffer object
    gl::FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->RBO); // attach MS render buffer object to framebuffer
    if (gl::CheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::POSTPROCESSOR: Failed to initialize MSFBO" << std::endl;
    // also initialize the FBO/texture to blit multisampled color-buffer to; used for shader operations (for postprocessing effects)
    gl::BindFramebuffer(GL_FRAMEBUFFER, this->FBO);
    this->Texture.Generate(width, height, NULL);
    gl::FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->Texture.ID, 0); // attach texture to framebuffer as its color attachment
    if (gl::CheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;
    gl::BindFramebuffer(GL_FRAMEBUFFER, 0);
    // initialize render data and uniforms
    this->initRenderData();
    this->PostProcessingShader.SetInteger("scene", 0, true);
    float offset = 1.0f / 300.0f;
    float offsets[9][2] = {
        { -offset,  offset  },  // top-left
        {  0.0f,    offset  },  // top-center
        {  offset,  offset  },  // top-right
        { -offset,  0.0f    },  // center-left
        {  0.0f,    0.0f    },  // center-center
        {  offset,  0.0f    },  // center - right
        { -offset, -offset  },  // bottom-left
        {  0.0f,   -offset  },  // bottom-center
        {  offset, -offset  }   // bottom-right    
    };
    gl::Uniform2fv(gl::GetUniformLocation(this->PostProcessingShader.ID, "offsets"), 9, (float*)offsets);
    int edge_kernel[9] = {
        -1, -1, -1,
        -1,  8, -1,
        -1, -1, -1
    };
    gl::Uniform1iv(gl::GetUniformLocation(this->PostProcessingShader.ID, "edge_kernel"), 9, edge_kernel);
    float blur_kernel[9] = {
        1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
        2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
        1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
    };
    gl::Uniform1fv(gl::GetUniformLocation(this->PostProcessingShader.ID, "blur_kernel"), 9, blur_kernel);    
}

void PostProcessor::BeginRender()
{
    gl::BindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
    gl::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    gl::Clear(GL_COLOR_BUFFER_BIT);
}
void PostProcessor::EndRender()
{
    // now resolve multisampled color-buffer into intermediate FBO to store to texture
    gl::BindFramebuffer(GL_READ_FRAMEBUFFER, this->MSFBO);
    gl::BindFramebuffer(GL_DRAW_FRAMEBUFFER, this->FBO);
    gl::BlitFramebuffer(0, 0, this->Width, this->Height, 0, 0, this->Width, this->Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    gl::BindFramebuffer(GL_FRAMEBUFFER, 0); // binds both READ and WRITE framebuffer to default framebuffer
}

void PostProcessor::Render(float time)
{
    // set uniforms/options
    this->PostProcessingShader.Use();
    this->PostProcessingShader.SetFloat("time", time);
    this->PostProcessingShader.SetInteger("confuse", this->Confuse);
    this->PostProcessingShader.SetInteger("chaos", this->Chaos);
    this->PostProcessingShader.SetInteger("shake", this->Shake);
    // render textured quad
    gl::ActiveTexture(GL_TEXTURE0);
    this->Texture.Bind();	
    gl::BindVertexArray(this->VAO);
    gl::DrawArrays(GL_TRIANGLES, 0, 6);
    gl::BindVertexArray(0);
}

void PostProcessor::initRenderData()
{
    // configure VAO/VBO
    unsigned int VBO;
    float vertices[] = {
        // pos        // tex
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,

        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f
    };
    gl::GenVertexArrays(1, &this->VAO);
    gl::GenBuffers(1, &VBO);

    gl::BindBuffer(GL_ARRAY_BUFFER, VBO);
    gl::BufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    gl::BindVertexArray(this->VAO);
    gl::EnableVertexAttribArray(0);
    gl::VertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    gl::BindBuffer(GL_ARRAY_BUFFER, 0);
    gl::BindVertexArray(0);
}