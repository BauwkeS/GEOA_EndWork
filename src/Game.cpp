#include <iostream>
#include <algorithm>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_ttf.h>
#include <chrono>
#include "Game.h"
#include <stdio.h>      
#include <stdlib.h>     
#include <time.h>

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

	//init own items
	InitPillars();
	PrintGameControls();

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

void Game::PrintGameControls()
{
	//print controls to use
	std::cout << "-----GAME CONTROLS ------\n";
	std::cout << "- S: Press to activate/Deactivate speed up (only possible with enough energy)\n";
	std::cout << "- E: Switch selection to a random other pillar\n";
	std::cout << "- R: Rotate around the selected pillar\n";
	std::cout << "- W: Reflect around the selected pillar\n";
	std::cout << "- Arrow keys: Move the selected pillar\n";
	std::cout << "-------------------------\n";
	std::cout << "- P: Spawn a new pillar on the player\n";
	std::cout << "-------------------------\n";
}

void Game::DrawPlayer() const
{
	utils::SetColor(m_PlayerColor);
	utils::FillRect(m_PlayerPosition[0], m_PlayerPosition[1], m_PlayerSize, m_PlayerSize);
}

void Game::TranslatePlayer(float deltaTime)
{
	//Constantly move your player around
	m_PlayerMotor = Motor::Translation(m_PlayerSpeed * deltaTime, m_PlayerDirection);
	m_PlayerPosition = (m_PlayerMotor * m_PlayerPosition * ~m_PlayerMotor).Grade3();
}

void Game::CheckWindowCollision()
{
	//get the player center point for easy collision loop
	auto centerOfPlayer = m_PlayerPosition;
	centerOfPlayer[0] += m_PlayerSize / 2;
	centerOfPlayer[1] += m_PlayerSize / 2;

	//when you hit the boundary, reflect on it
	for (auto& boundary : m_WindowBoundaries)
	{
		if (abs(boundary & centerOfPlayer) < m_PlayerSize / 2)
		{
			//depending on if you're rotating or translating, update the correct direction
			if (m_IsRotating) m_PlayerDirectionRotation = -m_PlayerDirectionRotation;
			else m_PlayerDirection = (boundary * m_PlayerDirection * ~boundary).Grade2();
		}
	}

}

void Game::CheckGameCollision()
{
	//check collision with other game objects

	PickupCollision();
}

void Game::VisualizeEnergy()
{
	//Change color of the square depending on the amount of energy
	m_PlayerColor =
		Color4f{
			(100.f - m_PlayerPosition[2]) / 100.f,
			m_PlayerPosition[2] / 100.f,
			0,
			1 };
}

void Game::ManageEnergySpeed(float deltaTime)
{
	//Manage the energy in different situations
	constexpr float energySpeed{ 40.f }; //how fast you get new energy

	//if going OVER normal speed -> take energy OTHERWISE -> give energy
	if (m_PlayerSpeed > m_NormalPlayerSpeed) m_PlayerPosition[2] -= deltaTime * energySpeed;
	else m_PlayerPosition[2] += deltaTime * energySpeed;

	//if you have 0 energy -> revert to normal speed
	if (m_PlayerPosition[2] <= 0) m_PlayerSpeed = m_NormalPlayerSpeed;

	//cap energy
	if (m_PlayerPosition[2] >= 100) m_PlayerPosition[2] = 100;
}

void Game::ManageRotation(float deltaTime)
{
	//rotate around the selected Pillar

	//translation to the selected pillar
	Motor translator{ Motor::Translation(m_PillarsVec[m_SelectedPillar].position.VNorm(),
		TwoBlade(m_PillarsVec[m_SelectedPillar].position[0], m_PillarsVec[m_SelectedPillar].position[1], 0, 0, 0, 0)) };

	//rotation
	const float rotSpeed = m_PlayerSpeed / 3 * deltaTime;
	Motor rotation{ Motor::Rotation(rotSpeed,m_PlayerDirectionRotation) };

	//full rotation & translation around the pillar
	Motor rotTranslations{ translator * rotation * ~translator };
	m_PlayerPosition = (rotTranslations * m_PlayerPosition * ~rotTranslations).Grade3();

}

void Game::MovePlayer(float deltaTime)
{
	if (m_IsRotating) ManageRotation(deltaTime);
	else TranslatePlayer(deltaTime);
}

void Game::ReflectPlayer()
{
	//distance
	//try to - the 2 distances to get it and get the distance between 2 points to reflect that

	//translation to the selected pillar
	//Motor translator{ Motor::Translation(m_PillarsVec[m_SelectedPillar].position.VNorm(),
	//	TwoBlade(m_PillarsVec[m_SelectedPillar].position[0], m_PillarsVec[m_SelectedPillar].position[1], 0, 0, 0, 0)) };

	//std::cout << "Player was at: x[" << m_PlayerPosition[0] << "], y[" << m_PlayerPosition[1] << "]\n";
	//full reflection around the pillar
	auto powerLevel = m_PlayerPosition[2];
	m_PlayerPosition = (m_PillarsVec[m_SelectedPillar].position * m_PlayerPosition * ~m_PillarsVec[m_SelectedPillar].position).Grade3();
	m_PlayerPosition[2] = powerLevel;
	//std::cout << "Player is NOW at: x[" << m_PlayerPosition[0] << "], y[" << m_PlayerPosition[1] << "]\n";

	//check if the distance is away from the main screen, if so put it at the most far away point
	if (m_PlayerPosition[0] - m_PlayerSize >= m_Window.width) m_PlayerPosition[0] = m_Window.width - m_PlayerSize;
	else if (m_PlayerPosition[0] <= 0) m_PlayerPosition[0] = 0;
	else if (m_PlayerPosition[1] + m_PlayerSize >= m_Window.height) m_PlayerPosition[1] = m_Window.height - m_PlayerSize;
	else if (m_PlayerPosition[1] <= 0) m_PlayerPosition[1] = 0;

}

void Game::InitPillars()
{
	//pillar 1
	pillar pillar1;
	pillar1.position = ThreeBlade{ m_Window.width / 4 * 1,m_Window.height / 3 * 2,1 };
	pillar1.size = 30.f;
	pillar1.isSelected = true;
	m_PillarsVec.emplace_back(pillar1);

	//pillar 2
	pillar pillar2;
	pillar2.position = ThreeBlade{ m_Window.width / 4 * 3,m_Window.height / 3 * 1,1 };
	pillar2.size = 15.f;
	pillar2.isSelected = false;
	m_PillarsVec.emplace_back(pillar2);

	//assign colors to the pillars
	m_SelectedPillar = 0;
	ColorPillars();
}

void Game::ColorPillars()
{
	//Make the selected pillar pink and the others purple
	for (auto& p : m_PillarsVec)
	{
		if (p.isSelected == true) p.color = m_SelectedPillarColor;
		else p.color = m_BasicPillarColor;
	}
}

void Game::DrawPillars() const
{
	//draw all the pillars on screen
	for (auto p : m_PillarsVec)
	{
		utils::SetColor(p.color);
		utils::FillRect(p.position[0], p.position[1], p.size, p.size);
	}
}

void Game::SpawnPillar()
{
	//Add a pillar when pressing a button to where the player is
	// -> make this an amount of points to pay for later

	pillar p;
	p.position = m_PlayerPosition;
	p.size = static_cast<float>((rand() % 20) + 10);
	if (DoesOverlapAll(p.position, p.size)) std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n";
	p.isSelected = false;
	p.color = m_BasicPillarColor;
	m_PillarsVec.emplace_back(p);
}

void Game::SpawnPickups()
{
	//always have at least 1 pickup there
	if (m_PickupsVec.empty()) MakeNewPickup();
	//add pickups when points are above 1 and 5
	if (m_PlayerScore >= 1 && m_PickupsVec.size() <= 1) MakeNewPickup();
	if (m_PlayerScore >= 5) MakeNewPickup();
}

void Game::MakeNewPickup()
{
	//never have more than 3 pickups
	if (m_PickupsVec.size() < 3)
	{
		//Make a new pickup and add it to the vector
		pickup p;
		p.size = (rand() % 3 + 1) * 10;
		int maxWidth = static_cast<int>(m_Window.width - static_cast<float>(p.size));
		int maxHeight = static_cast<int>(m_Window.height - static_cast<float>(p.size));
		p.position = { static_cast<float>(rand() % maxWidth), static_cast<float>(rand() % maxHeight),0 };
		p.points = abs((p.size / 10) - 4); //pickup points according to size
		//std::cout << "pickup point spawned has: " << p.points << std::endl;
		m_PickupsVec.emplace_back(p);
	}
}

void Game::DrawPickups() const
{
	//draw all the pickups on screen
	//Pickups are only visible when you have more than 30% of energy
	if (m_PlayerPosition[2] >= 30)
	{
		utils::SetColor(m_PickupColor);
		for (auto p : m_PickupsVec)
		{
			utils::FillEllipse(p.position[0], p.position[1], p.size, p.size);
		}
	}
}

void Game::PickupCollision()
{
	//get the player center point for easy collision loop
	auto centerOfPlayer = m_PlayerPosition;
	centerOfPlayer[0] += m_PlayerSize / 2;
	centerOfPlayer[1] += m_PlayerSize / 2;

	//when hit -> remove pickup and absorb points
	int pickupHit = CheckOverlapPickups(centerOfPlayer, m_PlayerSize);
	if (pickupHit >= 0)
	{
		m_PlayerScore += m_PickupsVec[pickupHit].points;
		std::cout << "New player score: " << m_PlayerScore << std::endl;
		m_PickupsVec.erase(m_PickupsVec.begin() + pickupHit);
	}
}

void Game::KeyboardSpeed(const SDL_KeyboardEvent& e)
{
	//when pressing S => enable or disable speed
	if (e.keysym.sym == SDLK_s)
	{
		//start speeding if you have some energy
		if (m_PlayerSpeed == m_NormalPlayerSpeed)
		{
			if (m_PlayerPosition[2] >= 10) m_PlayerSpeed *= 2.f;
		}
		else m_PlayerSpeed = m_NormalPlayerSpeed; //stop speeding
	}
}

void Game::KeyboardPillar(const SDL_KeyboardEvent& e)
{
	//when pressing E => switch to another pillar
	if (e.keysym.sym == SDLK_e)
	{
		int newActivePillarIndex{};

		//search for another pillar that is not the active one
		do
		{
			newActivePillarIndex = rand() % static_cast<int>(m_PillarsVec.size());
		} while (newActivePillarIndex == m_SelectedPillar);

		//make every pillar BUT the new one deselected
		for (int i = 0; i < m_PillarsVec.size(); ++i)
		{
			if (i != newActivePillarIndex) m_PillarsVec[i].isSelected = false;
			else m_PillarsVec[i].isSelected = true;
		}

		m_SelectedPillar = newActivePillarIndex;

		//change colors
		ColorPillars();
	}
}

void Game::KeyboardRotatePillar(const SDL_KeyboardEvent& e)
{
	if (e.keysym.sym == SDLK_r)
	{
		m_IsRotating = !m_IsRotating;
	}
}

void Game::KeyBoardMovePillar(const SDL_KeyboardEvent& e)
{
	if (e.keysym.sym == SDLK_UP || e.keysym.sym == SDLK_DOWN || e.keysym.sym == SDLK_RIGHT || e.keysym.sym == SDLK_LEFT)
	{
		//move the selected pillar with arrow functions -> also do a collision check
		float y{ 0 }, x{ 0 };
		if (e.keysym.sym == SDLK_UP) y = 1; //up
		if (e.keysym.sym == SDLK_DOWN) y = -1; //down
		if (e.keysym.sym == SDLK_RIGHT) x = 1; //right
		if (e.keysym.sym == SDLK_LEFT) x = -1; //left

		Motor translation{ Motor::Translation(1,TwoBlade{x,y,0,0,0,0}) };
		m_PillarsVec[m_SelectedPillar].position = (translation * m_PillarsVec[m_SelectedPillar].position * ~translation).Grade3();
	}
}

void Game::KeyBoardReflectPlayer(const SDL_KeyboardEvent& e)
{
	if (e.keysym.sym == SDLK_w)
	{
		ReflectPlayer();
	}
}

void Game::KeyBoardSpawnNewPillar(const SDL_KeyboardEvent& e)
{
	if (e.keysym.sym == SDLK_p)
	{
		SpawnPillar();
	}
}

bool Game::DoesOverlapAll(ThreeBlade pos, int size) const
{
	//check if your item overlaps with anything
	bool hasHit{ false };

	if (CheckOverlapPickups(pos, size) >= 0
		|| CheckOverlapPillars(pos, size)  >= 0) hasHit = true;

	return hasHit;
}

int Game::CheckOverlapPillars(ThreeBlade pos, int size) const
{
	int hasHit{ 0 };

	//pillars
	for (int i = 0; i < m_PillarsVec.size(); ++i)
	{
		auto bladeDis = m_PillarsVec[i].position & ThreeBlade{ pos[0], pos[1],0 };
		if (abs(bladeDis.Norm()) < static_cast <float>(size + m_PillarsVec[i].size / 2))
		{
			return i;
		}
	}
	//nothing hit
	return hasHit-1;
}

int Game::CheckOverlapPickups(ThreeBlade pos, int size) const
{
	int hasHit{ 0 };

	//pickups
	for (int i = 0; i < m_PickupsVec.size(); ++i)
	{
		auto bladeDis = m_PickupsVec[i].position & ThreeBlade{ pos[0], pos[1],0 };
		//std::cout << "abs(bladeDis.Norm(): " << abs(bladeDis.Norm()) << std::endl;
		//std::cout << "size/2 + m_PickupsVec[i].size / 2: " << static_cast <float>(size / 2 + m_PickupsVec[i].size / 2) << std::endl;
		if (abs(bladeDis.Norm()) < static_cast <float>(size / 2 + m_PickupsVec[i].size / 2))
		{
			return i;
		}
	}
	//nothing hit
	return hasHit-1;
}

void Game::Update(float elapsedSec)
{
	CheckWindowCollision();
	CheckGameCollision();

	SpawnPickups();

	MovePlayer(elapsedSec);

	ManageEnergySpeed(elapsedSec);
	VisualizeEnergy();
}

void Game::Draw() const
{
	//clear the screen
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	DrawPillars();
	DrawPickups();
	DrawPlayer();
}