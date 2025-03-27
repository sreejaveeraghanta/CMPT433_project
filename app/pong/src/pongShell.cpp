#include "pongShell.h"

#include "util.h"
#include "hal/lcd.h"
#include "hal/spi.h"
#include "hal/joystick.h"

#include "PVRUtils/PVRUtilsGles.h"

#include <iostream>

pvr::Result PongShell::initApplication()
{
    debugLog("initApplication");

	// Init HAL modules
	LCD_init();
	Joystick_init();

	// Construct game
	m_pong = std::make_unique<Game>(LCD_WIDTH, LCD_HEIGHT);

	// Allocate a pixel buffer to read from GPU memory later
	m_readPixBuf = std::make_unique<uint8_t[]>(LCD_WIDTH * LCD_HEIGHT * 4);

    return pvr::Result::Success;
}

pvr::Result PongShell::initView()
{
    debugLog("initView");

	// Since we are not using Linux graphics stack, we have to create a custom display and PbufferSurface
	PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT = reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(
        egl::internal::getEglExtensionFunction("eglGetPlatformDisplayEXT")
    );
    if (!eglGetPlatformDisplayEXT)
	{
        std::cout << "eglGetPlatformDisplayEXT not found";
    }
    m_display = eglGetPlatformDisplayEXT(EGL_PLATFORM_SURFACELESS_MESA, EGL_DEFAULT_DISPLAY, NULL);

	int major, minor;
	if (egl::Initialize(m_display, &major, &minor) == EGL_FALSE)
	{
        fprintf(stderr, "Failed to get EGL version! Error: %d\n", egl::GetError());
        egl::Terminate(m_display);
    }

	if (egl::BindAPI(EGL_OPENGL_ES_API) == EGL_FALSE)
	{
		fprintf(stderr, "Bind API error\n");
        egl::Terminate(m_display);
	}

	const EGLint configAttribs[] =
	{
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
        EGL_BLUE_SIZE, 5,
        EGL_GREEN_SIZE, 6,
        EGL_RED_SIZE, 5,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, EGL_NONE
	};

	const EGLint pbufferAttribs[] =
	{
		EGL_WIDTH,  LCD_WIDTH,
		EGL_HEIGHT, LCD_HEIGHT,
		EGL_NONE,
	};

    EGLint numConfigs;
    EGLConfig config;
    if (!egl::ChooseConfig(m_display, configAttribs, &config, 1, &numConfigs))
	{
        fprintf(stderr, "Failed to get EGL config! Error \n");
        egl::Terminate(m_display);
    }

    m_surface = egl::CreatePbufferSurface(m_display, config, pbufferAttribs);
    if (m_surface == EGL_NO_SURFACE)
	{
        fprintf(stderr, "Failed to create EGL surface! Error\n");
        egl::Terminate(m_display);
    }

	const EGLint contextAttribs[] =
	{
		EGL_CONTEXT_MAJOR_VERSION, 3,
		EGL_CONTEXT_MINOR_VERSION, 0,
		EGL_NONE
	};

    EGLContext context = egl::CreateContext(m_display, config, EGL_NO_CONTEXT, contextAttribs);
    if (context == EGL_NO_CONTEXT)
	{
        fprintf(stderr, "Failed to create EGL context! Error\n");
        egl::DestroySurface(m_display, m_surface);
        egl::Terminate(m_display);
    }

    egl::MakeCurrent(m_display, m_surface, m_surface, context);
	EGLint errNum = egl::GetError();
	if (errNum != EGL_SUCCESS)
	{
		fprintf(stderr, "MakeCurrent error %d\n", errNum);
		egl::DestroySurface(m_display, m_surface);
		egl::Terminate(m_display);
	}

	// Init game
	m_pong->init();

    return pvr::Result::Success;
}

pvr::Result PongShell::renderFrame()
{
	double frameMs = static_cast<double>(getFrameTime());
	double dt = frameMs / 1000.0;
	debugLog("FPS: %.1f", 1000.0 / frameMs);

	// Update game states
	m_pong->processInput(dt);
	m_pong->update(dt);
	m_pong->render();

	// Transfer frame from GPU to system memory
	GLint colorFormat;
    gl::GetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &colorFormat);
    GLint colorType;
    gl::GetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, &colorType);
	gl::ReadPixels(0, 0, LCD_WIDTH, LCD_HEIGHT, colorFormat, colorType, m_readPixBuf.get());

	// Convert read pixels to a format that can be sent to LCD
    convertRGBAtoRGB565(m_readPixBuf.get(), static_cast<uint16_t*>(LCD_getFrameBufferAddress()), LCD_WIDTH, LCD_HEIGHT);
	swapShortByteOrder(static_cast<uint16_t*>(LCD_getFrameBufferAddress()), LCD_WIDTH, LCD_HEIGHT);

	// Display frame buffer on the LCD
	LCD_displayFrame();

    return pvr::Result::Success;
}

pvr::Result PongShell::releaseView()
{
    debugLog("releaseView");
    return pvr::Result::Success;
}

pvr::Result PongShell::quitApplication()
{
    debugLog("quitApplication");

	// Deinit HAL modules
	Joystick_deinit();
	LCD_deinit();

	return pvr::Result::Success;
}

// Press Q in terminal to close the application
void PongShell::eventMappedInput(pvr::SimplifiedInput key)
{
	switch (key)
	{
		case pvr::SimplifiedInput::ActionClose:
            exitShell();
			break;
		default:
			break;
    }
}

std::unique_ptr<pvr::Shell> pvr::newDemo() { return std::make_unique<PongShell>(); }