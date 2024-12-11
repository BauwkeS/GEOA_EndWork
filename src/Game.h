#pragma once
#include "FlyFish.h"
#include "structs.h"
#include "SDL.h"
#include "SDL_opengl.h"
#include <vector>

class Game
{
public:
	explicit Game( const Window& window );
	Game( const Game& other ) = delete;
	Game& operator=( const Game& other ) = delete;
	Game(Game&& other) = delete;
	Game& operator=(Game&& other) = delete;

	~Game();

	void Run( );

	void Update(float elapsedSec);

	void Draw() const;

	// Event handling
	void ProcessKeyDownEvent(const SDL_KeyboardEvent& e)
	{
		//when pressing S => enable or disable speed
		if (e.keysym.sym == SDLK_s)
		{
			//start speeding if you have some energy
			if(m_PlayerSpeed == m_NormalPlayerSpeed)
			{
				if (m_PlayerPosition[2] >= 10) m_PlayerSpeed *= 2.f;
			}
			else m_PlayerSpeed = m_NormalPlayerSpeed; //stop speeding
		}
	}
	void ProcessKeyUpEvent(const SDL_KeyboardEvent& e)
	{

	}
	void ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e)
	{
		
	}
	void ProcessMouseDownEvent(const SDL_MouseButtonEvent& e)
	{
		
	}
	void ProcessMouseUpEvent(const SDL_MouseButtonEvent& e)
	{
		
	}

	const Rectf& GetViewPort() const
	{
		return m_Viewport;
	}

private:
	// DATA MEMBERS
	// The window properties
	const Window m_Window;
	const Rectf m_Viewport;
	// The window we render to
	SDL_Window* m_pWindow;
	// OpenGL context
	SDL_GLContext m_pContext;
	// Init info
	bool m_Initialized;
	// Prevent timing jumps when debugging
	const float m_MaxElapsedSeconds;

	//Player
	ThreeBlade m_PlayerPosition{ m_Window.width/2,m_Window.height/2,0 };
	const float m_PlayerSize{ 20.f };
	const float m_NormalPlayerSpeed{ 200.f };
	float m_PlayerSpeed{ m_NormalPlayerSpeed };
	Color4f m_PlayerColor{ 1,1,1,1 };
	//player dimensions to be added
	TwoBlade m_PlayerDirection{ 1,1,0,0,0,0 };

	// FUNCTIONS
	void InitializeGameEngine( );
	void CleanupGameEngine( );

	//Player functions
	void DrawPlayer() const;
	void TranslatePlayer(float deltaTime);
	void CheckWindowCollision();
	void VisualizeEnergy();
	void ManageEnergySpeed(float deltaTime);


	//Pillar
	struct pillar
	{
		ThreeBlade position;
		Color4f color;
		float size;
		bool isSelected;
	};

	std::vector<pillar> m_PillarsVec{};

	//pillar functions
	void InitPillars();
	void ColorPillars();
	void DrawPillars() const;
};
