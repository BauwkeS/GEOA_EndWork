#pragma once
#include "FlyFish.h"
#include "structs.h"
#include "SDL.h"
#include "SDL_opengl.h"

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
			if(player1.m_PlayerSpeed == player1.m_NormalPlayerSpeed)
			{
				if (player1.m_PlayerPosition[2] >= 10) player1.m_PlayerSpeed *= 2.f;
			}
			else player1.m_PlayerSpeed = player1.m_NormalPlayerSpeed; //stop speeding
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
	struct Player
	{
		ThreeBlade m_PlayerPosition;
		float m_PlayerSize;
		float m_NormalPlayerSpeed;
		float m_PlayerSpeed{ m_NormalPlayerSpeed };
		Color4f m_PlayerColor;
		TwoBlade m_PlayerDirection;

		Player() :
			m_PlayerPosition{ 0,0,0 },
			m_PlayerSize{ 20.f },
			m_NormalPlayerSpeed{ 200.f },
			m_PlayerSpeed{ m_NormalPlayerSpeed },
			m_PlayerColor{ 1,1,1,1 },
			m_PlayerDirection{ 1,1,0,0,0,0 }{}
	};

	Player player1{};

	// FUNCTIONS
	void InitializeGameEngine( );
	void CleanupGameEngine( );

	//Player functions
	void DrawPlayer(Player p) const;
	void TranslatePlayer(float deltaTime, Player p);
	void CheckWindowCollision(Player p);
	void VisualizeEnergy(Player p);
	void ManageEnergySpeed(float deltaTime, Player p);
};
