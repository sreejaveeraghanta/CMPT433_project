#pragma once

#include "game.h"

#include "PVRShell/PVRShell.h"
#include "PVRUtils/OpenGLES/BindingsGles.h"

class PongShell : public pvr::Shell
{
public:
	virtual pvr::Result initApplication();
	virtual pvr::Result initView();
    virtual pvr::Result renderFrame();
	virtual pvr::Result releaseView();
	virtual pvr::Result quitApplication();
	virtual void eventMappedInput(pvr::SimplifiedInput key);

private:
	EGLDisplay 				   	m_display;
	EGLSurface 					m_surface;
	std::unique_ptr<Game> 		m_pong;
	std::unique_ptr<uint8_t[]>	m_readPixBuf;
};
