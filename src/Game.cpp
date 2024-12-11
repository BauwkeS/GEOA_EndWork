#include <iostream>
#include <algorithm>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_ttf.h>
#include <chrono>
#include "Game.h"

#include "utils.h"
#include "structs.h"

Game::Game(const Window& window)
	: m_Window{ window }
	, m_Viewport{ 0,0,window.width,window.height }
	, m_pWindow{ nullptr }
	, m_pContext{ nullptr }
	, m_Initialized{ false }
	, m_MaxElapsedSeconds{ 0.1f }
{
	InitializeGameEngine();
}

Game::~Game()
{
	CleanupGameEngine();
}

void Game::InitializeGameEngine()
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "BaseGame::Initialize( ), error when calling SDL_Init: " << SDL_GetError() << std::endl;
		return;
	}

	// Use OpenGL 2.1
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	// Create window
	m_pWindow = SDL_CreateWindow(
		m_Window.title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		int(m_Window.width),
		int(m_Window.height),
		SDL_WINDOW_OPENGL);
	if (m_pWindow == nullptr)
	{
		std::cerr << "BaseGame::Initialize( ), error when calling SDL_CreateWindow: " << SDL_GetError() << std::endl;
		return;
	}

	// Create OpenGL context 
	m_pContext = SDL_GL_CreateContext(m_pWindow);
	if (m_pContext == nullptr)
	{
		std::cerr << "BaseGame::Initialize( ), error when calling SDL_GL_CreateContext: " << SDL_GetError() << std::endl;
		return;
	}

	// Set the swap interval for the current OpenGL context,
	// synchronize it with the vertical retrace
	if (m_Window.isVSyncOn)
	{
		if (SDL_GL_SetSwapInterval(1) < 0)
		{
			std::cerr << "BaseGame::Initialize( ), error when calling SDL_GL_SetSwapInterval: " << SDL_GetError() << std::endl;
			return;
		}
	}
	else
	{
		SDL_GL_SetSwapInterval(0);
	}

	// Set the Projection matrix to the identity matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set up a two-dimensional orthographic viewing region.
	glOrtho(0, m_Window.width, 0, m_Window.height, -1, 1); // y from bottom to top

	// Set the viewport to the client window area
	// The viewport is the rectangular region of the window where the image is drawn.
	glViewport(0, 0, int(m_Window.width), int(m_Window.height));

	// Set the Modelview matrix to the identity matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Enable color blending and use alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Initialize SDL_ttf
	if (TTF_Init() == -1)
	{
		std::cerr << "BaseGame::Initialize( ), error when calling TTF_Init: " << TTF_GetError() << std::endl;
		return;
	}

	m_Initialized = true;
}

void Game::Run()
{
	if (!m_Initialized)
	{
		std::cerr << "BaseGame::Run( ), BaseGame not correctly initialized, unable to run the BaseGame\n";
		std::cin.get();
		return;
	}

	// Main loop flag
	bool quit{ false };

	// Set start time
	std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

	//The event loop
	SDL_Event e{};
	while (!quit)
	{
		// Poll next event from queue
		while (SDL_PollEvent(&e) != 0)
		{
			// Handle the polled event
			switch (e.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				this->ProcessKeyDownEvent(e.key);
				break;
			case SDL_KEYUP:
				this->ProcessKeyUpEvent(e.key);
				break;
			case SDL_MOUSEMOTION:
				e.motion.y = int(m_Window.height) - e.motion.y;
				this->ProcessMouseMotionEvent(e.motion);
				break;
			case SDL_MOUSEBUTTONDOWN:
				e.button.y = int(m_Window.height) - e.button.y;
				this->ProcessMouseDownEvent(e.button);
				break;
			case SDL_MOUSEBUTTONUP:
				e.button.y = int(m_Window.height) - e.button.y;
				this->ProcessMouseUpEvent(e.button);
				break;
			}
		}

		if (!quit)
		{
			// Get current time
			std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();

			// Calculate elapsed time
			float elapsedSeconds = std::chrono::duration<float>(t2 - t1).count();

			// Update current time
			t1 = t2;

			// Prevent jumps in time caused by break points
			elapsedSeconds = std::min(elapsedSeconds, m_MaxElapsedSeconds);

			// Call the BaseGame object 's Update function, using time in seconds (!)
			this->Update(elapsedSeconds);

			// Draw in the back buffer
			this->Draw();

			// Update screen: swap back and front buffer
			SDL_GL_SwapWindow(m_pWindow);
		}
	}
}

void Game::CleanupGameEngine()
{
	SDL_GL_DeleteContext(m_pContext);

	SDL_DestroyWindow(m_pWindow);
	m_pWindow = nullptr;

	//Quit SDL subsystems
	TTF_Quit();
	SDL_Quit();

}

void Game::DrawPlayer(Player p) const
{
	utils::SetColor(p.m_PlayerColor);
	utils::FillRect(p.m_PlayerPosition[0], p.m_PlayerPosition[1], p.m_PlayerSize, p.m_PlayerSize);
}

void Game::TranslatePlayer(float deltaTime, Player p)
{
	//Constantly move your player around
	Motor translateBasePosition{ Motor::Translation(p.m_PlayerSpeed*deltaTime,p.m_PlayerDirection) };
	p.m_PlayerPosition = (translateBasePosition * p.m_PlayerPosition * ~translateBasePosition).Grade3();
}

void Game::CheckWindowCollision(Player p)
{
	//Check the boundaries of the window to bounce the player back
	if (p.m_PlayerPosition[0]+p.m_PlayerSize >= m_Window.width) p.m_PlayerDirection[0] = -1;//check right
	else if (p.m_PlayerPosition[0] <= 0) p.m_PlayerDirection[0] = 1; //check left
	else if (p.m_PlayerPosition[1]+p.m_PlayerSize >= m_Window.height) p.m_PlayerDirection[1] = -1;//check up
	else if (p.m_PlayerPosition[1] <= 0) p.m_PlayerDirection[1] = 1; //check down
}

void Game::VisualizeEnergy(Player p)
{
	//Change color of the square depending on the amount of energy
	float energyStatus = p.m_PlayerPosition[2];

	p.m_PlayerColor =
		Color4f{
			(100.f-energyStatus)/100.f,
			energyStatus/100.f,
			0,
			1};
}

void Game::ManageEnergySpeed(float deltaTime, Player p)
{
	//Manage the energy in different situations
	constexpr float energySpeed{ 40.f }; //how fast you get new energy

	//if going OVER normal speed -> take energy OTHERWISE -> give energy
	if (p.m_PlayerSpeed > p.m_NormalPlayerSpeed) p.m_PlayerPosition[2] -= deltaTime * energySpeed;
	else p.m_PlayerPosition[2] += deltaTime * energySpeed;

	//if you have 0 energy -> revert to normal speed
	if (p.m_PlayerPosition[2] <= 0) p.m_PlayerSpeed = p.m_NormalPlayerSpeed;

	//cap energy
	if (p.m_PlayerPosition[2] >= 100) p.m_PlayerPosition[2] = 100;
}
void Game::Update(float elapsedSec)
{
	//---------TESTINGS
	//////translation
	//Motor translator{ Motor::Translation(-400,TwoBlade(1, 1, 0, 0, 0, 0)) };
	//m_Position = (translator * m_Position * ~translator).Grade3();

	////rotation
	//Motor rotation{ Motor::Rotation(-45*elapsedSec,TwoBlade(0, 0, 0 ,0,0,1)) };
	//m_Position = (rotation * m_Position * ~rotation).Grade3();

	//Motor translator2{ Motor::Translation(400,TwoBlade(1, 1, 0, 0, 0, 0)) };
	//m_Position = (translator2 * m_Position * ~translator2).Grade3();
	//----------

	//player functions
	CheckWindowCollision(player1);
	TranslatePlayer(elapsedSec,player1);
	ManageEnergySpeed(elapsedSec,player1);
	VisualizeEnergy(player1);
}

void Game::Draw() const
{
	//clear the screen
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	DrawPlayer(player1);
}