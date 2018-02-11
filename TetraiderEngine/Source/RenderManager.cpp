#include "RenderManager.h"
#include "TetraiderAPI.h"
#include "JsonReader.h"
#include "DebugLineMesh.h"
#include "ShaderProgram.h"
#include "Shader.h"

#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "Sprite.h"
#include "Particle.h"

#include "SDL_image.h"

//#include <glew.h>
//#include <GL/gl.h>
#include <iostream>
#include <fstream>
#include <windows.h>


void ParticleContainer::Update(float deltaTime)
{
	int newparticles = 1;

	for (int i = 0; i < newparticles; ++i) {
		int idx = FindUnusedParticle();
		if (idx > -1) {
			particles[idx].life = maxLife;
			particles[idx].pos.Set(0, 0, 0);
			particles[idx].speed.Set(0, .5f, 0);
		}
	}

	pCount = 0;
	for (int i = 0; i<MaxParticles; i++) {

		ParticleItem& p = particles[i]; 

		if (p.life > 0.0f) {

			// Decrease life
			p.life -= deltaTime;
			if (p.life > 0.0f) {
				p.speed += Gravity * deltaTime * 0.5f;
				p.speed.y = p.speed.y < -9.8f ? -9.8f : p.speed.y;
				p.pos += p.speed * deltaTime;
				p.pos.z = 0.5f;
				p.cameradistance = Vector3D::SquareDistance(p.pos, TETRA_GAME_OBJECTS.GetCamera(1)->GetComponent<Transform>(ComponentType::C_Transform)->GetPosition());

				p.r = 1;
				p.b = 1;
				p.g = 1;
				p.a = 1;

				// Fill the GPU buffer
				positions[4 * pCount] = p.pos.x;
				positions[4 * pCount + 1] = p.pos.y;
				positions[4 * pCount + 2] = p.pos.z;

				positions[4 * pCount + 3] = p.size;

				colors[4 * pCount + 0] = p.r;
				colors[4 * pCount + 1] = p.g;
				colors[4 * pCount + 2] = p.b;
				colors[4 * pCount + 3] = p.a;

			}
			else {
				// Particles that just died will be put at the end of the buffer in SortParticles();
				p.cameradistance = -1.0f;
			}
			++pCount;
		}
	}
}





enum SHADER_LOCATIONS {
	POSITION = 0,
	TEXTURE_COORD,
	P_POS_SIZE,
	P_COLOR,

	PERSP_MATRIX = 10,
	VIEW_MATRIX,
	MODEL_MATRIX,
	NORMAL_MATRIX,

	TINT_COLOR = 20,
	SATURATION_COLOR,
	FRAME_OFFSET,
	FRAME_SIZE,
	TILE
};

RenderManager::RenderManager(int width, int height, std::string title) :
	m_width(width), m_height(height), m_windowTitle(title), m_baseWindowTitle(title),
	m_pCurrentProgram(nullptr), m_debugShaderName("")
{
	_InitWindow(title);
	TETRA_EVENTS.Subscribe(EventType::EVENT_FPS_UPDATE, this);

	particleTest = ParticleContainer();
	particleTest.pCount = 0;
	
	for (int i = 0; i < MaxParticles; i+=4) {
		particleTest.positions[i] = float(i);
		particleTest.positions[i + 1] = float(i);
		particleTest.positions[i + 2] = 0.f;
		particleTest.positions[i + 3] = 1.f;

		particleTest.colors[i] =		1.f;
		particleTest.colors[i + 1] = 0.f;
		particleTest.colors[i + 2] = 0.f;
		particleTest.colors[i + 3] = 1.f;
	}
}

RenderManager::~RenderManager() 
{
	delete particleTest; // REMOVE THIS
	SDL_GL_DeleteContext(m_context);
	SDL_Quit();
}

#pragma region Private Methods

void RenderManager::UPDATE_PARTICLE_TEST(float deltaTime)
{
	particleTest->Update(deltaTime);
}

void RenderManager::RENDER_PARTICLES_TEST(const GameObject& camera)
{
	SelectShaderProgram("particle");
	const Camera * cameraComp = camera.GetComponent<Camera>(ComponentType::C_Camera);
	glUseProgram(m_pCurrentProgram->GetProgram());

	//glUniformMatrix4fv(SHADER_LOCATIONS::PERSP_MATRIX, 1, true, (float*)cameraComp->GetCameraMatrix());
	//glUniformMatrix4fv(SHADER_LOCATIONS::VIEW_MATRIX, 1, true, (float*)cameraComp->GetViewMatrix());
	glUniformMatrix4fv(SHADER_LOCATIONS::PERSP_MATRIX, 1, true, (float*)Matrix4x4::Identity4D());
	glUniformMatrix4fv(SHADER_LOCATIONS::VIEW_MATRIX, 1, true, (float*)Matrix4x4::Identity4D());

	glBindBuffer(GL_ARRAY_BUFFER, particleTest->positionsBuffer);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	glBufferSubData(GL_ARRAY_BUFFER, 0, particleTest->pCount * sizeof(GLfloat) * 4, particleTest->positions);

	glBindBuffer(GL_ARRAY_BUFFER, particleTest->colorsBuffer);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	glBufferSubData(GL_ARRAY_BUFFER, 0, particleTest->pCount * sizeof(GLubyte) * 4, particleTest->colors);


	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(SHADER_LOCATIONS::POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, particleTest->mesh.GetVertexBuffer());
	glVertexAttribPointer(SHADER_LOCATIONS::POSITION, 4, GL_FLOAT, GL_FALSE, 0, (void*)0); // <- load it to memory




	// 1rst attribute buffer : vertices
	//glEnableVertexAttribArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, particleTest->vbo);
	//glVertexAttribPointer(
	//	SHADER_LOCATIONS::POSITION, // attribute. No particular reason for 0, but must match the layout in the shader.
	//	3, // size
	//	GL_FLOAT, // type
	//	GL_FALSE, // normalized?
	//	0, // stride
	//	(void*)0 // array buffer offset
	//);



	//// 2nd attribute buffer : texture coords for the particles6
	//glEnableVertexAttribArray(2);
	//glEnableVertexAttribArray(SHADER_LOCATIONS::TEXTURE_COORD);
	//glBindBuffer(GL_ARRAY_BUFFER, particleTest->mesh.GetTextCoordBuffer());
	//glVertexAttribPointer(SHADER_LOCATIONS::TEXTURE_COORD, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0); // <- load it to memory



	// 3rd attribute buffer : positions of particles' centers
	glEnableVertexAttribArray(SHADER_LOCATIONS::P_POS_SIZE);
	glBindBuffer(GL_ARRAY_BUFFER, particleTest->positionsBuffer);
	glVertexAttribPointer(
		SHADER_LOCATIONS::P_POS_SIZE, // attribute. No particular reason for 1, but must match the layout in the shader.
		4, // size : x + y + z + size => 4
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);

	// 4th attribute buffer : particles' colors
	glEnableVertexAttribArray(SHADER_LOCATIONS::P_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, particleTest->colorsBuffer);
	glVertexAttribPointer(
		SHADER_LOCATIONS::P_COLOR, // attribute. No particular reason for 1, but must match the layout in the shader.
		4, // size : r + g + b + a => 4
		GL_UNSIGNED_BYTE, // type
		GL_TRUE, // normalized? *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
		0, // stride
		(void*)0 // array buffer offset
	);


	// These functions are specific to glDrawArrays*Instanced*.
	// The first parameter is the attribute buffer we're talking about.
	// The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
	// http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
	glVertexAttribDivisor(SHADER_LOCATIONS::POSITION, 0); // particles vertices : always reuse the same 4 vertices -> 0
	//glVertexAttribDivisor(1, 0); // texture coords :always reuse the same 2 tex coords
	glVertexAttribDivisor(SHADER_LOCATIONS::P_POS_SIZE, 1); // positions : one per quad (its center) -> 1
	glVertexAttribDivisor(SHADER_LOCATIONS::P_COLOR, 1); // color : one per quad -> 1

	// select the texture to use
	glBindTexture(GL_TEXTURE_2D, particleTest->GetTextureBuffer());

	// Draw the particules !
	// This draws many times a small triangle_strip (which looks like a quad).
	// This is equivalent to :
	// for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4),
	// but faster.
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 6, particleTest->pCount);
}

void RenderManager::_InitWindow(std::string title)
{
	if (AllocConsole())
	{
		FILE* file;

		freopen_s(&file, "CONOUT$", "wt", stdout);
		freopen_s(&file, "CONOUT$", "wt", stderr);
		freopen_s(&file, "CONOUT$", "wt", stdin);

		SetConsoleTitle("Tetraider Engine");
	}

	SDL_Init(SDL_INIT_VIDEO);

	m_pWindow = SDL_CreateWindow(title.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		m_width, m_height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	m_context = SDL_GL_CreateContext(m_pWindow);

	// Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		std::cout << "SDL Image failed to initialize." << std::endl << "Error: " << IMG_GetError() << std::endl;
	}

	SDL_SetWindowSize(m_pWindow, m_width, m_height);
	glViewport(0, 0, m_width, m_height);
}

std::string RenderManager::_LoadTextFile(std::string fname)
{
	std::string out, line;
	std::ifstream in(fname);
	if (in) {
		std::getline(in, line);
		while (in) {
			out += line + "\n";
			std::getline(in, line);
		}
		return out;
	}
	else {
		std::cout << "Error reading in file: " << fname << std::endl;
		return "";
	}
}

bool RenderManager::_GameObjectHasRenderableComponent(const GameObject & gameObject)
{
	return gameObject.HasComponent(ComponentType::C_Sprite);
}

void RenderManager::_RenderSprite(const Sprite * pSpriteComp)
{
	// Optimize this
	glEnableVertexAttribArray(SHADER_LOCATIONS::POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, pSpriteComp->GetMesh().GetVertexBuffer());
	glVertexAttribPointer(SHADER_LOCATIONS::POSITION, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0); // <- load it to memory

	glEnableVertexAttribArray(SHADER_LOCATIONS::TEXTURE_COORD);
	glBindBuffer(GL_ARRAY_BUFFER, pSpriteComp->GetMesh().GetTextCoordBuffer());
	glVertexAttribPointer(SHADER_LOCATIONS::TEXTURE_COORD, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0); // <- load it to memory

	glUniform2f(SHADER_LOCATIONS::FRAME_OFFSET, pSpriteComp->GetUOffset(), pSpriteComp->GetVOffset());
	glUniform2f(SHADER_LOCATIONS::FRAME_SIZE, pSpriteComp->TileX(), pSpriteComp->TileY());

	//glUniform2f(SHADER_LOCATIONS::TILE, 1, 1); // pSpriteComp->TileX(), pSpriteComp->TileY());

	Vector3D tintColor = pSpriteComp->GetTintColor();
	Vector3D saturationColor = pSpriteComp->GetSaturationColor();
	glUniform4f(SHADER_LOCATIONS::TINT_COLOR, tintColor[0], tintColor[1], tintColor[2], tintColor[3]);
	glUniform4f(SHADER_LOCATIONS::SATURATION_COLOR, saturationColor[0], saturationColor[1], saturationColor[2], saturationColor[3]);

	if (pSpriteComp->GetAlphaMode() == GL_RGBA) {
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.4f);
		glEnable(GL_BLEND);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else {
		glDisable(GL_ALPHA_TEST);
		glEnable(GL_DEPTH_TEST);
	}

	// select the texture to use
	glBindTexture(GL_TEXTURE_2D, pSpriteComp->GetTextureBuffer());

	// draw the mesh
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pSpriteComp->GetMesh().GetFaceBuffer());
	glDrawElements(GL_TRIANGLES, 3 * pSpriteComp->GetMesh().faceCount(), GL_UNSIGNED_INT, 0);
}

void RenderManager::_RenderParticles(const Particle * pParticleComp)
{
}

void RenderManager::_RenderGameObject(const GameObject& gameObject)
{
	// Only attempt to draw if the game object has a sprite component and transform component
	if (!gameObject.GetComponent<Transform>(ComponentType::C_Transform) || !_GameObjectHasRenderableComponent(gameObject))
		return;

	Matrix4x4 M = gameObject.GetComponent<Transform>(ComponentType::C_Transform)->GetTransform();
	Matrix4x4 N = Matrix4x4::Transpose3x3(Matrix4x4::Inverse3x3(M));
	glUniformMatrix4fv(SHADER_LOCATIONS::MODEL_MATRIX, 1, true, (float*)M);
	glUniformMatrix4fv(SHADER_LOCATIONS::NORMAL_MATRIX, 1, true, (float*)N);

	// set shader attributes
	if (gameObject.HasComponent(ComponentType::C_Sprite))
		_RenderSprite(gameObject.GetComponent<Sprite>(ComponentType::C_Sprite));
	else if (gameObject.HasComponent(ComponentType::C_Particle))
		_RenderParticles(gameObject.GetComponent<Particle>(ComponentType::C_Particle));
}

void RenderManager::_SelectShaderProgram(const GameObject & gameObject)
{
	std::string shader = "";

	if (gameObject.HasComponent(ComponentType::C_Sprite))
		shader = gameObject.GetComponent<Sprite>(ComponentType::C_Sprite)->Shader();

	SelectShaderProgram(shader == "" ? "default" : shader);
}

void RenderManager::_SetUpCamera(const GameObject & camera)
{
	const Camera * cameraComp = camera.GetComponent<Camera>(ComponentType::C_Camera);
	glUseProgram(m_pCurrentProgram->GetProgram());

	glUniformMatrix4fv(SHADER_LOCATIONS::PERSP_MATRIX, 1, true, (float*)cameraComp->GetCameraMatrix());
	glUniformMatrix4fv(SHADER_LOCATIONS::VIEW_MATRIX, 1, true, (float*)cameraComp->GetViewMatrix());
}

#pragma region Debug

void RenderManager::_SetUpDebug(const GameObject& camera)
{
	SelectShaderProgram(m_debugShaderName);
	_SetUpCamera(camera);
	
	glEnableVertexAttribArray(SHADER_LOCATIONS::POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, TETRA_RESOURCES.GetDebugLineMesh()->GetVertexBuffer());
	glVertexAttribPointer(SHADER_LOCATIONS::POSITION, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0); // <- load it to memory
}

void RenderManager::_RenderDebugCommand(DebugShape shape, const Vector3D & color, const Vector3D& pos, const Vector3D& rot, const Vector3D& scale)
{
	switch (shape) {
	case DebugShape::S_RECT:
		_RenderRect(color, pos, rot, scale);
		break;
	case DebugShape::S_CIRCLE:
		_RenderCircle(color, scale.x / 2.f, pos);
		break;
	case DebugShape::S_LINE:
		_RenderLine(color, pos, rot, scale);
		break;
	case DebugShape::S_CONE:
		_RenderCone(color, pos, rot, scale);
		break;
	}
}

void RenderManager::_RenderRect(const Vector3D & color, const Vector3D & pos, const Vector3D & rot, const Vector3D & scale)
{
	glUniform4f(SHADER_LOCATIONS::SATURATION_COLOR, color.x, color.y, color.z, color.w);

	float halfWidth = scale.x / 2.f,
		halfHeight = scale.y / 2.f;

	// square base matrix
	Matrix4x4 Base = Matrix4x4::Translate(pos) * Matrix4x4::Rotate(rot.z, ZAXIS);
	Matrix4x4 SideBase = Matrix4x4::Rotate(90.f, ZAXIS) * Matrix4x4::Scale(scale.y, 0.f, 0.f);
	Matrix4x4 TopBotScale = Matrix4x4::Scale(scale.x, 0.f, 0.f);

	Matrix4x4 Left = Base
		* Matrix4x4::Translate(Vector3D(-halfWidth, 0, 0))
		* SideBase;
	glUniformMatrix4fv(SHADER_LOCATIONS::MODEL_MATRIX, 1, true, (float*)Left);
	glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);

	Matrix4x4 Right = Base
		* Matrix4x4::Translate(Vector3D(halfWidth, 0, 0))
		* SideBase;
	glUniformMatrix4fv(SHADER_LOCATIONS::MODEL_MATRIX, 1, true, (float*)Right);
	glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);

	Matrix4x4 Top = Base
		* Matrix4x4::Translate(Vector3D(0, halfHeight, 0))
		* TopBotScale;
	glUniformMatrix4fv(SHADER_LOCATIONS::MODEL_MATRIX, 1, true, (float*)Top);
	glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);

	Matrix4x4 Bottom = Base
		* Matrix4x4::Translate(Vector3D(0, -halfHeight, 0))
		* TopBotScale;
	glUniformMatrix4fv(SHADER_LOCATIONS::MODEL_MATRIX, 1, true, (float*)Bottom);
	glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
}

void RenderManager::_RenderCircle(const Vector3D & color, float radius, const Vector3D & pos)
{
	glUniform4f(SHADER_LOCATIONS::SATURATION_COLOR, color.x, color.y, color.z, color.w);

	Matrix4x4 ArcMatrix;
	int max = 32;
	float degreeAmt = 360.f / float(max);
	Vector3D AXIS_Z = Vector3D(0, 0, 1);
	Vector3D a = Vector3D(radius, 0, 0);
	Vector3D b = Matrix4x4::Rotate(degreeAmt, AXIS_Z) * a;
	float lineLength = Vector3D::Distance(a, b);
	// circle base matrix
	Matrix4x4 Base = Matrix4x4::Translate(Vector3D(radius, 0, 0))
		* Matrix4x4::Rotate(90.f, ZAXIS)
		* Matrix4x4::Scale(lineLength, 0, 0);
	Matrix4x4 Position = Matrix4x4::Translate(pos);
	for (int i = 0; i < max; ++i) {
		ArcMatrix = Position
			* Matrix4x4::Rotate(degreeAmt * float(i), AXIS_Z)
			* Base;

		glUniformMatrix4fv(SHADER_LOCATIONS::MODEL_MATRIX, 1, true, (float*)ArcMatrix);
		glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
	}
}

void RenderManager::_RenderLine(const Vector3D & color, const Vector3D & pos, const Vector3D & rot, const Vector3D & scale)
{
	glUniform4f(SHADER_LOCATIONS::SATURATION_COLOR, color.x, color.y, color.z, color.w);

	Matrix4x4 model = Matrix4x4::Translate(pos) * 
		Matrix4x4::Rotate(rot.z, ZAXIS) *
		Matrix4x4::Scale(scale.x) * 
		Matrix4x4::Translate(Vector3D(.5f, 0, 0));

	glUniformMatrix4fv(SHADER_LOCATIONS::MODEL_MATRIX, 1, true, (float*)model);
	glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
}

void RenderManager::_RenderCone(const Vector3D & color, const Vector3D & pos, const Vector3D & rot, const Vector3D & arcWidthAndRadius)
{
	float arcWidth = arcWidthAndRadius.x;
	float radius = arcWidthAndRadius.y;

	glUniform4f(SHADER_LOCATIONS::SATURATION_COLOR, color.x, color.y, color.z, color.w);

	Matrix4x4 ArcMatrix;
	int max = 32;
	float degreeAmt = arcWidth / float(max);
	float offset = rot.z - (arcWidth / 2.f) + degreeAmt/2.f;
	//max += int(rot.z);
	Vector3D AXIS_Z = Vector3D(0, 0, 1);
	Vector3D a = Vector3D(radius, 0, 0);
	Vector3D b = Matrix4x4::Rotate(degreeAmt, AXIS_Z) * a;
	float lineLength = Vector3D::Distance(a, b);
	// Draw cone arc
	// circle base matrix
	Matrix4x4 Base = Matrix4x4::Translate(Vector3D(radius, 0, 0))
		* Matrix4x4::Rotate(90.f, ZAXIS)
		* Matrix4x4::Scale(lineLength, 0, 0);
	Matrix4x4 Position = Matrix4x4::Translate(pos);
	for (int i = 0; i < max; ++i) {
		ArcMatrix = Position
			* Matrix4x4::Rotate(degreeAmt * float(i) + offset, AXIS_Z)
			* Base;

		glUniformMatrix4fv(SHADER_LOCATIONS::MODEL_MATRIX, 1, true, (float*)ArcMatrix);
		glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
	}
	

	// Draw cone lines
	Matrix4x4 model = Matrix4x4::Translate(pos) *
		Matrix4x4::Rotate(rot.z + (arcWidth / 2.f), ZAXIS) *
		Matrix4x4::Scale(radius) *
		Matrix4x4::Translate(Vector3D(.5f, 0, 0));

	glUniformMatrix4fv(SHADER_LOCATIONS::MODEL_MATRIX, 1, true, (float*)model);
	glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);

	model = Matrix4x4::Translate(pos) *
		Matrix4x4::Rotate(rot.z - (arcWidth / 2.f), ZAXIS) *
		Matrix4x4::Scale(radius) *
		Matrix4x4::Translate(Vector3D(.5f, 0, 0));

	glUniformMatrix4fv(SHADER_LOCATIONS::MODEL_MATRIX, 1, true, (float*)model);
	glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
}

#pragma endregion

#pragma endregion

bool RenderManager::Init()
{
	// GLEW: get function bindings (if possible)
	glewInit();
	if (!GLEW_VERSION_2_0) {
		std::cout << "Needs OpenGL version 2.0 or better" << std::endl;
		return false;
	}
	return true;
}

void RenderManager::INIT_PARTICLE_TEST()
{
	Mesh& mesh = *TETRA_RESOURCES.LoadMesh("quad");
	particleTest = new ParticleContainer(mesh);
	particleTest->m_texture = TETRA_RESOURCES.GetTexture("T_Particle.png");
	particleTest->pCount = 0;

	for (int i = 0; i < MaxParticles; i += 4) {
		particleTest->positions[i] = 0.f;
		particleTest->positions[i + 1] = 0.f;
		particleTest->positions[i + 2] = 0.f;
		particleTest->positions[i + 3] = 0.f;

		particleTest->colors[i] = 0.f;
		particleTest->colors[i + 1] = 0.f;
		particleTest->colors[i + 2] = 0.f;
		particleTest->colors[i + 3] = 0.f;
	}



	GLfloat g_vertex_buffer_data[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
	};

	glGenBuffers(1, &particleTest->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, particleTest->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);


	// The VBO containing the positions and sizes of the particles
	glGenBuffers(1, &particleTest->positionsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, particleTest->positionsBuffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	glGenBuffers(1, &particleTest->colorsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, particleTest->colorsBuffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
}

void RenderManager::FrameStart()
{
	// clear frame buffer and z-buffer
	glClearColor(0.0f, 0.0f, 0.0f, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearDepth(1);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void RenderManager::FrameEnd()
{
	SDL_GL_SwapWindow(m_pWindow);
}

#pragma region Window Methods

void RenderManager::Resize(int width, int height)
{
	m_width = width;
	m_height = height;
	glViewport(0, 0, width, height);
}

void RenderManager::HandleEvent(Event * p_event)
{
	if (p_event->Type() == EventType::EVENT_FPS_UPDATE) {
		int fps = (int)p_event->Data<FPSData>()->mFPS;
		SetWindowTitle(m_baseWindowTitle + " ::: FPS: " + std::to_string(fps));
	}
}

void RenderManager::SetWindowWidth(int width)
{
	m_width = width;
	glViewport(0, 0, width, m_height);
}

void RenderManager::SetWindowHeight(int height)
{
	m_height = height;
	glViewport(0, 0, m_width, height);
}

void RenderManager::SetWindowTitle(std::string title)
{
	m_windowTitle = title;
	SDL_SetWindowTitle(m_pWindow, title.c_str());
}

float RenderManager::GetAspectRatio() const
{
	return (float)m_width / (float)m_height;
}

#pragma endregion

void RenderManager::RenderGameObject(const GameObject& camera, const GameObject& go)
{
	_SelectShaderProgram(go);
	_SetUpCamera(camera);
	_RenderGameObject(go);
}

#pragma region Shaders

void RenderManager::LoadShaders()
{
	std::string shaderDir = TETRA_GAME_CONFIG.ShadersDir();

	LoadShaderProgram(shaderDir, m_debugShaderName + ".json");
	LoadShaderProgram(shaderDir, "defaultShader.json"); // TODO: Move this
	LoadShaderProgram(shaderDir, "particleShader.json"); // TODO: Move this
}

void RenderManager::LoadShaderProgram(std::string filePath, std::string fileName)
{
	try {
		json j = JsonReader::OpenJsonFile(filePath + fileName);

		if (j.is_object()) {
			for (json::iterator it = j.begin(); it != j.end(); ++it) {
				std::string programName = it.key();
				ShaderProgram * program = CreateShaderProgram(programName);
				Shader * vShader = CreateVertexShaderFromFile(filePath + ParseString(j, programName, "vertex"));
				Shader * fShader = CreateFragmentShaderFromFile(filePath + ParseString(j, programName, "fragment"));

				program->AttachShader(*vShader);
				program->AttachShader(*fShader);
				program->LinkShaders();
			}
		}
	}
	catch (const json::parse_error& ex) {
		std::cerr << ex.what() << std::endl;
	}
}

ShaderProgram * RenderManager::GetShaderProgram(std::string programName)
{
	return m_shaderPrograms[programName];
}

ShaderProgram * RenderManager::CreateShaderProgram(std::string programName)
{
	ShaderProgram * program = m_shaderPrograms[programName];
	if (program)
		return program;

	program = new ShaderProgram();
	if (program)
		m_shaderPrograms[programName] = program;

	return program;
}

Shader * RenderManager::CreateVertexShader(std::string vertexShaderText)
{
	return new Shader(vertexShaderText, VERTEX_SHADER);
}

Shader * RenderManager::CreateVertexShaderFromFile(std::string fileName)
{
	return CreateVertexShader(_LoadTextFile(fileName));
}

Shader * RenderManager::CreateFragmentShader(std::string fragmentShaderText)
{
	return new Shader(fragmentShaderText, FRAGMENT_SHADER);

}

Shader * RenderManager::CreateFragmentShaderFromFile(std::string fileName)
{
	return CreateFragmentShader(_LoadTextFile(fileName));
}

void RenderManager::SelectShaderProgram(std::string programName)
{
	if (!m_shaderPrograms[programName]) {
		std::cout << "Shader program \"" << programName << "\" does not exist." << std::endl;
		return;
	}

	if (m_pCurrentProgram != m_shaderPrograms[programName])
		m_pCurrentProgram = m_shaderPrograms[programName];
}
#pragma endregion