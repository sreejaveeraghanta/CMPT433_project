#include "uiRenderer.h"

#include <glm/glm.hpp>

UIRenderer::UIRenderer() {}

UIRenderer::UIRenderer(int width, int height)
{
	using namespace pvr::ui;
    m_renderer.init(width, height, true, false);
    m_renderer.getDefaultTitle()->setText("Pong");
	m_renderer.getDefaultTitle()->setScale(0.5f, 0.5f);
	m_renderer.getDefaultTitle()->commitUpdates();
	m_score1 = m_renderer.createText();
	m_score2 = m_renderer.createText();
	m_score1->setPixelOffset(-70.0f, 70.0f);
	m_score2->setPixelOffset(+70.0f, 70.0f);
	m_score1->setScale(0.6f, 0.6f);
	m_score2->setScale(0.6f, 0.6f);
}

void UIRenderer::render(int player1Score, int player2Score)
{
	using namespace pvr::ui;
	m_renderer.beginRendering();
	m_renderer.getDefaultTitle()->render();
	m_score1->setText(std::to_string(player1Score));
	m_score2->setText(std::to_string(player2Score));
	m_score1->commitUpdates();
	m_score2->commitUpdates();
	m_score1->render();
	m_score2->render();
	m_renderer.endRendering();
}
