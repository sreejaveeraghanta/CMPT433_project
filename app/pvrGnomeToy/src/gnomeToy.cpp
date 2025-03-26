/*!
\brief Shows how to use the PVRApi library together with loading models from POD files and rendering them with effects from PFX files.
\file OpenGLESIntroducingPVRUtils.cpp
\author PowerVR by Imagination, Developer Technology Team
\copyright Copyright (c) Imagination Technologies Limited.
*/
#include "hal/lcd.h"
#include "hal/joystick.h"

#include "PVRShell/PVRShell.h"
#include "PVRUtils/OpenGLES/BindingsGles.h"
#include "PVRUtils/PVRUtilsGles.h"

#include <chrono>
#include <stdio.h>

bool testGLError(const char* functionLastCalled)
{
	//	glGetError returns the last error that occurred using OpenGL ES, not necessarily the status of the last called function. The user
	//	has to check after every single OpenGL ES call or at least once every frame. Usually this would be for debugging only, but for this
	//	example it is enabled always

	GLenum lastError = gl::GetError();
	if (lastError != GL_NO_ERROR)
	{
		printf("%s failed (%x).\n", functionLastCalled, lastError);
		return false;
	}
	return true;
}

namespace Uniforms {
enum Enum
{
	WorldViewProjection,
	WorldViewIT,
	LightDirEye,
	AlbedoTexture,
	Count
};

const char* names[] = { "WVPMatrix", "WorldViewIT", "LightDirection", "sTexture" };
} // namespace Uniforms

// Content file names
const char VertexShaderFile[] = "VertShader.vsh"; // Effect file
const char FragmentShaderFile[] = "FragShader.fsh"; // Effect file

const char SceneFileName[] = "GnomeToy.pod"; // POD _scene files

/// <summary>Class implementing the Shell functions.</summary>
class OpenGLESIntroducingPVRUtils : public pvr::Shell
{
	struct DeviceResources
	{
		// pvr::EglContext context;
		EGLDisplay display;
		EGLSurface surface;

		// The Vertex buffer object handle array.
		std::vector<GLuint> vbos;
		std::vector<GLuint> ibos;
		GLuint program;
		std::vector<GLuint> textures;

		// UIRenderer used to display text
		pvr::ui::UIRenderer uiRenderer;

		DeviceResources() : program(0) {}
		~DeviceResources()
		{
			if (program) { gl::DeleteProgram(program); }
			program = 0;
			if (vbos.size())
			{
				gl::DeleteBuffers((GLsizei)vbos.size(), vbos.data());
				vbos.clear();
			}
			if (ibos.size())
			{
				gl::DeleteBuffers((GLsizei)ibos.size(), ibos.data());
				ibos.clear();
			}
			if (textures.size())
			{
				gl::DeleteTextures((GLsizei)textures.size(), textures.data());
				textures.clear();
			}
		}
	};

	std::unique_ptr<DeviceResources> _deviceResources;

	glm::vec3 _clearColor;

	// 3D Model
	pvr::assets::ModelHandle _scene;

	// Projection and Model View matrices
	glm::mat4 _projMtx, _viewMtx;

	// Variables to handle the animation in a time-based manner
	float _frame;

	pvr::utils::VertexConfiguration _vertexConfiguration;

	int32_t _uniformLocations[Uniforms::Count];

	/// <summary>Flag to know whether astc iss upported by the physical device.</summary>
	bool _astcSupported;

	// A 240x240x4 buffer to contains RGBA format pixels
	std::unique_ptr<uint8_t[]> _readPixelsBuffer;

	// Update camera later
	glm::vec3 _cameraPos, _cameraTarget, _cameraUp;

public:
	virtual pvr::Result initApplication();
	virtual pvr::Result initView();
	virtual pvr::Result releaseView();
	virtual pvr::Result quitApplication();
	virtual pvr::Result renderFrame();
	virtual void eventMappedInput(pvr::SimplifiedInput key);

	void renderMesh(uint32_t meshNodeId);
	void setOpenglState();
};

/// <summary>Code in initApplication() will be called by Shell once per run, before the rendering context is created.
/// Used to initialize variables that are not dependent on it(e.g.external modules, loading meshes, etc.).If the rendering
/// context is lost, initApplication() will not be called again.</summary>
/// <returns>Result::Success if no error occurred.</returns>
pvr::Result OpenGLESIntroducingPVRUtils::initApplication()
{
	LCD_init();
	// Joystick_init();
	_readPixelsBuffer = std::make_unique<uint8_t[]>(LCD_WIDTH * LCD_HEIGHT * 4);

	// Load the scene
	_scene = pvr::assets::loadModel(*this, SceneFileName);

	// The cameras are stored in the file. We check it contains at least one.
	if (_scene->getNumCameras() == 0) { throw pvr::InvalidDataError("The scene does not contain a camera"); }
	// We check the _scene contains at least one light
	if (_scene->getNumLights() == 0) { throw pvr::InvalidDataError("The scene does not contain a light"); }

	// Ensure that all meshes use an indexed triangle list
	for (uint32_t i = 0; i < _scene->getNumMeshes(); ++i)
	{
		if (_scene->getMesh(i).getPrimitiveType() != pvr::PrimitiveTopology::TriangleList || _scene->getMesh(i).getFaces().getDataSize() == 0)
		{
			throw pvr::InvalidDataError(" The meshes in the scene must use an indexed triangle list");
			return pvr::Result::UnknownError;
		}
	}

	// Initialize variables used for the animation
	_frame = 0;

	return pvr::Result::Success;
}

/// <summary>Code in quitApplication() will be called by pvr::Shell once per run, just before exiting the program.</summary>
/// <returns>Result::Success if no error occurred</returns>.
pvr::Result OpenGLESIntroducingPVRUtils::quitApplication()
{
	// Joystick_deinit();
	LCD_deinit();
	_scene.reset();
	return pvr::Result::Success;
}

/// <summary>Code in initView() will be called by Shell upon initialization or after a change
/// in the rendering context. Used to initialize variables that are dependent on the
/// rendering context(e.g.textures, vertex buffers, etc.).</summary>
/// <returns>Result::Success if no error occurred.</returns>
pvr::Result OpenGLESIntroducingPVRUtils::initView()
{
	_deviceResources = std::make_unique<DeviceResources>();

	// _deviceResources->context = pvr::createEglContext();
	// _deviceResources->context->init(getWindow(), getDisplay(), getDisplayAttributes());

	// We don't use the default pvr::EglContext, instead we create our own offscreen display and pbuffersurface
    PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT = reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(
        egl::internal::getEglExtensionFunction("eglGetPlatformDisplayEXT")
    );
    if (!eglGetPlatformDisplayEXT)
	{
        std::cout << "eglGetPlatformDisplayEXT not found";
    }
    _deviceResources->display = eglGetPlatformDisplayEXT(EGL_PLATFORM_SURFACELESS_MESA, EGL_DEFAULT_DISPLAY, NULL);

	int major, minor;
	if (egl::Initialize(_deviceResources->display, &major, &minor) == EGL_FALSE)
	{
        fprintf(stderr, "Failed to get EGL version! Error: %d\n", egl::GetError());
        egl::Terminate(_deviceResources->display);
    }

	if (egl::BindAPI(EGL_OPENGL_ES_API) == EGL_FALSE)
	{
		fprintf(stderr, "Bind API error\n");
        egl::Terminate(_deviceResources->display);
	}

	const EGLint configAttribs[] =
	{
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
        EGL_BLUE_SIZE, 5,
        EGL_GREEN_SIZE, 6,
        EGL_RED_SIZE, 5,
        EGL_ALPHA_SIZE, 8, // Need to keep >= 0 otherwise textures became white noise (why?)
        EGL_DEPTH_SIZE, 8, // For depth test, prioritize redering of objects closer to screen
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
    if (!egl::ChooseConfig(_deviceResources->display, configAttribs, &config, 1, &numConfigs))
	{
        fprintf(stderr, "Failed to get EGL config! Error \n");
        egl::Terminate(_deviceResources->display);
    }

    _deviceResources->surface = egl::CreatePbufferSurface(_deviceResources->display, config, pbufferAttribs);
    if (_deviceResources->surface == EGL_NO_SURFACE)
	{
        fprintf(stderr, "Failed to create EGL surface! Error\n");
        egl::Terminate(_deviceResources->display);
    }

	const EGLint contextAttribs[] =
	{
		EGL_CONTEXT_MAJOR_VERSION, 3,
		EGL_CONTEXT_MINOR_VERSION, 0, // Must be <=3.0 for BeagleY-AI
		EGL_NONE
	};

    EGLContext context = egl::CreateContext(_deviceResources->display, config, EGL_NO_CONTEXT, contextAttribs);
    if (context == EGL_NO_CONTEXT)
	{
        fprintf(stderr, "Failed to create EGL context! Error\n");
        egl::DestroySurface(_deviceResources->display, _deviceResources->surface);
        egl::Terminate(_deviceResources->display);
    }

    egl::MakeCurrent(_deviceResources->display, _deviceResources->surface, _deviceResources->surface, context);
	EGLint errNum = egl::GetError();
	if (errNum != EGL_SUCCESS)
	{
		fprintf(stderr, "MakeCurrent error %d\n", errNum);
		egl::DestroySurface(_deviceResources->display, _deviceResources->surface);
		egl::Terminate(_deviceResources->display);
	}

	/****************END CUSTOM INIT******************/
    
	debugThrowOnApiError("InitView: Initialise Context");
	std::cout << "InitView: Initialise Context" << '\n';

	pvr::utils::appendSingleBuffersFromModel(*_scene, _deviceResources->vbos, _deviceResources->ibos);

	const char* apistring = (const char*)gl::GetString(GL_VERSION);
	std::cout << apistring << '\n';

	_deviceResources->uiRenderer.init(LCD_WIDTH, LCD_HEIGHT, true, false);
	_deviceResources->uiRenderer.getDefaultTitle()->setText("IntroducingPVRUtils");
	glm::vec2 fontSize(0.5, 0.5);
	_deviceResources->uiRenderer.getDefaultTitle()->setScale(fontSize);
	_deviceResources->uiRenderer.getDefaultTitle()->commitUpdates();

	debugThrowOnApiError("InitView: Initialise UIRenderer");
	std::cout << "InitView: Initialise UIRenderer" << '\n';

	static const char* attribs[] = { "inVertex", "inNormal", "inTexCoord" };
	static const uint16_t attribIndices[] = { 0, 1, 2 };

	// Enable or disable gamma correction based on if it is automatically performed on the framebuffer or we need to do it in the shader.
	const char* defines[] = { "FRAMEBUFFER_SRGB" };
	uint32_t numDefines = 1;
	glm::vec3 clearColorLinearSpace(0.0f, 0.45f, 0.41f);
	_clearColor = clearColorLinearSpace;
	if (false) //getBackBufferColorspace() != pvr::ColorSpace::sRGB
	{
		// Gamma correct the clear colour
		_clearColor = pvr::utils::convertLRGBtoSRGB(clearColorLinearSpace);
		// However, OpenGL ES2 should not be gamma corrected because the textures will unfortunately not be correctly read in linear values
		if (pvr::Api::OpenGLES31 > pvr::Api::OpenGLES2) { numDefines = 0; }
	}

	GLuint program = _deviceResources->program = pvr::utils::createShaderProgram(*this, VertexShaderFile, FragmentShaderFile, attribs, attribIndices, 3, defines, numDefines);

	_uniformLocations[Uniforms::WorldViewProjection] = gl::GetUniformLocation(program, Uniforms::names[Uniforms::WorldViewProjection]);
	_uniformLocations[Uniforms::WorldViewIT] = gl::GetUniformLocation(program, Uniforms::names[Uniforms::WorldViewIT]);
	_uniformLocations[Uniforms::LightDirEye] = gl::GetUniformLocation(program, Uniforms::names[Uniforms::LightDirEye]);
	_uniformLocations[Uniforms::AlbedoTexture] = gl::GetUniformLocation(program, Uniforms::names[Uniforms::AlbedoTexture]);

	const pvr::assets::Mesh& mesh = _scene->getMesh(0);
	pvr::utils::VertexBindings_Name vertexBindings[] = { { "POSITION", "inVertex" }, { "NORMAL", "inNormal" }, { "UV0", "inTexCoord" } };
	_vertexConfiguration = createInputAssemblyFromMesh(mesh, vertexBindings, 3);

	debugThrowOnApiError("InitView: Initialise program");
	std::cout << "InitView: Initialise program" << '\n';

	uint32_t i = 0;

	_deviceResources->textures.resize(_scene->getNumMaterials());

	for (auto it = _deviceResources->textures.begin(), end = _deviceResources->textures.end(); it != end; ++it) { *it = 0; }

	_astcSupported = gl::isGlExtensionSupported("GL_KHR_texture_compression_astc_ldr");

	while (i < _scene->getNumMaterials() && _scene->getMaterial(i).defaultSemantics().getDiffuseTextureIndex() != static_cast<uint32_t>(-1))
	{
		// create the texture object
		GLuint texture = 0;

		const pvr::assets::Model::Material& material = _scene->getMaterial(i);

		std::string textureName = _scene->getTexture(material.defaultSemantics().getDiffuseTextureIndex()).getName();
		pvr::assets::helper::getTextureNameWithExtension(textureName, _astcSupported);

		// Load the diffuse texture map
		texture = pvr::utils::textureUpload(*this, textureName.c_str(), false);
		gl::BindTexture(GL_TEXTURE_2D, texture);
		gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		_deviceResources->textures[i] = texture;
		++i;
	}

	// Calculates the projection matrix
	bool isRotated = this->isScreenRotated();
	if (!isRotated)
	{
		_projMtx = glm::perspective(_scene->getCamera(0).getFOV(), static_cast<float>(LCD_WIDTH) / static_cast<float>(LCD_HEIGHT), _scene->getCamera(0).getNear(),
			_scene->getCamera(0).getFar());
	}
	else
	{
		_projMtx = pvr::math::perspective(pvr::Api::OpenGLES3, _scene->getCamera(0).getFOV(), static_cast<float>(LCD_HEIGHT) / static_cast<float>(LCD_WIDTH),
			_scene->getCamera(0).getNear(), _scene->getCamera(0).getFar(), glm::pi<float>() * .5f);
	}
	float fov;

	_scene->getCameraProperties(0, fov, _cameraPos, _cameraTarget, _cameraUp);

	std::cout << "{" 
	<< _cameraPos.x << " " << _cameraPos.y << " " << _cameraPos.z 
	<< "}\n";

	std::cout << "{" 
	<< _cameraTarget.x << " " << _cameraTarget.y << " " << _cameraTarget.z 
	<< "}\n";

	std::cout << "{" 
	<< _cameraUp.x << " " << _cameraUp.y << " " << _cameraUp.z 
	<< "}\n";

	_viewMtx = glm::lookAt(_cameraPos, _cameraTarget, _cameraUp);

	debugThrowOnApiError("InitView: Exit");
	std::cout << "InitView: Exit" << '\n';

	return pvr::Result::Success;
}

void OpenGLESIntroducingPVRUtils::setOpenglState()
{
	gl::DepthMask(GL_TRUE);
	gl::ColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	gl::CullFace(GL_BACK);
	gl::Enable(GL_CULL_FACE);
	gl::FrontFace(GL_CCW);
	gl::Enable(GL_DEPTH_TEST);
	gl::ClearColor(_clearColor.r, _clearColor.g, _clearColor.b, 1.f);
	gl::EnableVertexAttribArray(0);
	gl::EnableVertexAttribArray(1);
	gl::EnableVertexAttribArray(2);
}

/// <summary>Code in releaseView() will be called by Shell when the application quits.</summary>
/// <returns>Result::Success if no error occurred.</returns>
pvr::Result OpenGLESIntroducingPVRUtils::releaseView()
{
	gl::ClearColor(0, 0, 0, 1.f);
	_deviceResources.reset();
	return pvr::Result::Success;
}

void convertRGBAtoRGB565(uint8_t *rgba, uint16_t *rgb565, int width, int height)
{
    int numPixels = width * height;
    for (int i = 0; i < numPixels; i++)
    {
        // Extract red, green, blue components from RGBA.
        uint8_t r = rgba[i * 4 + 0];
        uint8_t g = rgba[i * 4 + 1];
        uint8_t b = rgba[i * 4 + 2];
        // Alpha is ignored in RGB565.

        // Convert 8-bit components to RGB565:
        // Red:   5 bits, Green: 6 bits, Blue: 5 bits.
        uint16_t r5 = (r >> 3) & 0x1F;  // 5 bits
        uint16_t g6 = (g >> 2) & 0x3F;  // 6 bits
        uint16_t b5 = (b >> 3) & 0x1F;  // 5 bits

        // Pack into a 16-bit value in RGB565 format.
        rgb565[i] = (r5 << 11) | (g6 << 5) | (b5);
    }

}

void swapShortByteOrder(uint16_t *rgb565, int width, int height)
{
	int numPixels = width * height;
	// IMPORTANT, need to swap byte order of each uint16_t to before sending to LCD
	for (int i = 0; i < numPixels; i++) {
		rgb565[i] = ((rgb565[i] >> 8) & 0xFF) | ((rgb565[i] << 8) & 0xFF00);
	}
}

void OpenGLESIntroducingPVRUtils::eventMappedInput(pvr::SimplifiedInput key)
{
	switch (key)
	{
		case pvr::SimplifiedInput::ActionClose:
			exitShell();
			break;

		case pvr::SimplifiedInput::Action1:
			if (_frame >= 200.0) { _frame -= 200.0; }
			break;

		case pvr::SimplifiedInput::Left:
			_cameraPos += glm::vec3(-10.0f, 0.0f, 0.0f);
			break;

		case pvr::SimplifiedInput::Right:
			_cameraPos += glm::vec3(10.0f, 0.0f, 0.0f);
			break;

		case pvr::SimplifiedInput::Up:
			_cameraPos += glm::vec3(0.0f, 10.0f, 0.0f);
			break;

		case pvr::SimplifiedInput::Down:
			_cameraPos += glm::vec3(0.0f, -10.0f, 0.0f);
			break;

		default:
			break;
	}
	_viewMtx = glm::lookAt(_cameraPos, _cameraTarget, _cameraUp);
}

/// <summary>Main rendering loop function of the program. The shell will call this function every frame.</summary>
/// <returns>Result::Success if no error occurred.</returns>
pvr::Result OpenGLESIntroducingPVRUtils::renderFrame()
{
	// printf("upX: %.2f, upY:%.2f, upZ: %.2f\n", _cameraUp.x, _cameraUp.y, _cameraUp.z);
	// Control camera with joystick
	// JoystickReading reading = Joystick_read();
	// // printf("X: %d, Y:%d\n", xReading, yReading);
	// if (reading.x > JOYSTICK_HIGH) {
	// 	_cameraUp += glm::vec3(0.5f, 0.0f, 0.0f);
	// } 
	// if (reading.x < JOYSTICK_LOW) {
	// 	_cameraUp += glm::vec3(-0.5f, 0.0f, 0.0f);
	// }

	// if (reading.y > JOYSTICK_HIGH) {
	// 	_cameraUp += glm::vec3(0.0f, 0.5f, 0.0f);
	// } 
	// if (reading.y < JOYSTICK_LOW) {
	// 	_cameraUp += glm::vec3(0.0f, -0.5f, 0.0f);
	// }
	// _viewMtx = glm::lookAt(_cameraPos, _cameraTarget, _cameraUp);

	printf("fps: %.1f, frametime: %u\n", 1000.0 / static_cast<double>(getFrameTime()), getFrameTime());
	debugThrowOnApiError("RenderFrame: Entrance");

	//  Calculates the _frame number to animate in a time-based manner.
	//  get the time in milliseconds.
	pvr::assets::AnimationInstance& animInst = _scene->getAnimationInstance(0);
	_frame += static_cast<float>(getFrameTime());
	if (_frame >= animInst.getTotalTimeInMs()) { _frame = 0; }

	// Sets the _scene animation to this _frame
	animInst.updateAnimation(_frame);

	setOpenglState();

	gl::UseProgram(_deviceResources->program);
	gl::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl::Uniform1i(_uniformLocations[Uniforms::AlbedoTexture], 0);

	// Rendering meshes
	auto start = std::chrono::high_resolution_clock::now();
	for (uint32_t i = 0; i < _scene->getNumMeshNodes(); ++i) { renderMesh(i); }

	_deviceResources->uiRenderer.beginRendering();
	_deviceResources->uiRenderer.getDefaultTitle()->render();
	_deviceResources->uiRenderer.getSdkLogo()->render();
	_deviceResources->uiRenderer.endRendering();
	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    // std::cout << "render time: " << elapsed.count() << " ms\n";

	if (this->shouldTakeScreenshot()) { pvr::utils::takeScreenshot(this->getScreenshotFileName(), this->getWidth(), this->getHeight()); }

    GLint colorFormat;
    gl::GetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &colorFormat);
    GLint colorType;
    gl::GetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, &colorType);
	// Transfer frame from GPU to system memory
	start = std::chrono::high_resolution_clock::now();
	gl::ReadPixels(0, 0, LCD_WIDTH, LCD_HEIGHT, colorFormat, colorType, _readPixelsBuffer.get());
	testGLError("glReadPixels");
	end = std::chrono::high_resolution_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    // std::cout << "ReadPixels time: " << elapsed.count() << " ms\n";

	// Frame data preparation
	start = std::chrono::high_resolution_clock::now();
    convertRGBAtoRGB565(_readPixelsBuffer.get(), static_cast<uint16_t*>(LCD_getFrameBufferAddress()), LCD_WIDTH, LCD_HEIGHT);
 	swapShortByteOrder(static_cast<uint16_t*>(LCD_getFrameBufferAddress()), LCD_WIDTH, LCD_HEIGHT);
	end = std::chrono::high_resolution_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    // std::cout << "convertRGBAtoRGB565 and swapShortByteOrder time: " << elapsed.count() << " ms\n";

	// Data transfer
	start = std::chrono::high_resolution_clock::now();
	LCD_displayFrame();
	end = std::chrono::high_resolution_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    // std::cout << "LCD_displayFrame time: " << elapsed.count() << " ms\n";

	egl::SwapBuffers(_deviceResources->display, _deviceResources->surface);
	debugThrowOnApiError("RenderFrame: Exit");
	return pvr::Result::Success;
}

void OpenGLESIntroducingPVRUtils::renderMesh(uint32_t meshNodeId)
{
	//  We can build the world view matrix from the camera position, target and an up vector.
	//  A _scene is composed of nodes. There are 3 types of nodes:
	//  - MeshNodes :
	//    references a mesh in the getMesh().
	//    These nodes are at the beginning of the Nodes array.
	//    And there are nNumMeshNode number of them.
	//    This way the .pod format can instantiate several times the same mesh
	//    with different attributes.
	//  - lights
	//  - cameras
	//  To draw a _scene, you must go through all the MeshNodes and draw the referenced meshes.
	glm::mat4 worldView;
	glm::vec3 lightDir3;

	_scene->getLightDirection(0, lightDir3);

	// A _scene is composed of nodes. There are 3 types of nodes:
	// - MeshNodes :
	// references a mesh in the getMesh().
	// These nodes are at the beginning of the Nodes array.
	// And there are nNumMeshNode number of them.
	// This way the .pod format can instantiate several times the same mesh
	// with different attributes.
	// - lights
	// - cameras
	// To draw a _scene, you must go through all the MeshNodes and draw the referenced meshes.

	// Gets the node model matrix
	worldView = _viewMtx * _scene->getWorldMatrix(meshNodeId);

	gl::ActiveTexture(GL_TEXTURE0);
	// Passes the world-view-projection matrix (WVP) to the shader to transform the vertices
	gl::UniformMatrix4fv(_uniformLocations[Uniforms::WorldViewProjection], 1, GL_FALSE, glm::value_ptr(_projMtx * worldView));
	// Passes the inverse-transpose of the world-view-projection matrix (WVP) to the shader to transform the normals
	gl::UniformMatrix4fv(_uniformLocations[Uniforms::WorldViewIT], 1, GL_FALSE, glm::value_ptr(glm::inverseTranspose(worldView)));
	// Passes the eye-space light direction to light the _scene
	gl::Uniform3fv(_uniformLocations[Uniforms::LightDirEye], 1, glm::value_ptr(glm::normalize(glm::mat3(_viewMtx) * lightDir3)));

	const pvr::assets::Model::Node* pNode = &_scene->getMeshNode(meshNodeId);
	// Gets pMesh referenced by the pNode
	const pvr::assets::Mesh* pMesh = &_scene->getMesh(pNode->getObjectId());
	int32_t matId = pNode->getMaterialIndex();

	gl::BindTexture(GL_TEXTURE_2D, _deviceResources->textures[matId]);

	gl::BindBuffer(GL_ARRAY_BUFFER, _deviceResources->vbos[pNode->getObjectId()]);
	gl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, _deviceResources->ibos[pNode->getObjectId()]);

	for (uint32_t i = 0; i < 3; ++i)
	{
		auto& attrib = _vertexConfiguration.attributes[i];
		auto& binding = _vertexConfiguration.bindings[0];
		gl::VertexAttribPointer(attrib.index, attrib.width, pvr::utils::convertToGles(attrib.format), dataTypeIsNormalised(attrib.format), binding.strideInBytes,
			reinterpret_cast<const void*>(static_cast<uintptr_t>(attrib.offsetInBytes)));
	}

	gl::DrawElements(GL_TRIANGLES, pMesh->getNumFaces() * 3, pvr::utils::convertToGles(pMesh->getFaces().getDataType()), 0);
}

/// <summary>This function must be implemented by the user of the shell. The user should return its pvr::Shell object defining the behaviour of the application.</summary>
/// <returns>Return a unique ptr to the demo supplied by the user.</returns>
std::unique_ptr<pvr::Shell> pvr::newDemo() { return std::make_unique<OpenGLESIntroducingPVRUtils>(); }