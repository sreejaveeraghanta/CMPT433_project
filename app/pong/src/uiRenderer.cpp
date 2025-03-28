#include "uiRenderer.h"

#include <glm/glm.hpp>

UIRenderer::UIRenderer() {}

UIRenderer::UIRenderer(int width, int height)
{
    m_renderer.init(width, height, true, false);
    m_renderer.getDefaultTitle()->setText("Pong");
	glm::vec2 fontSize(0.5, 0.5);
	m_renderer.getDefaultTitle()->setScale(fontSize);
	m_renderer.getDefaultTitle()->commitUpdates();
}

void UIRenderer::render()
{
	m_renderer.beginRendering();
	m_renderer.getDefaultTitle()->render();
	m_renderer.endRendering();
}
