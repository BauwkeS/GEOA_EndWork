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
		KeyboardSpeed(e);
		KeyboardPillar(e);
		KeyboardRotatePillar(e);
		KeyBoardMovePillar(e);
		KeyBoardReflectPlayer(e);
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
	TwoBlade m_PlayerDirectionRotation{ 0,0,0,0,0,1 };
	Motor m_PlayerMotor{ Motor::Translation(m_PlayerSpeed,m_PlayerDirection)};
	bool m_IsRotating{ false };

	// FUNCTIONS
	void InitializeGameEngine( );
	void CleanupGameEngine( );
	void PrintGameControls();

	//Player functions
	void DrawPlayer() const;
	void TranslatePlayer(float deltaTime);
	void CheckWindowCollision();
	void CheckGameCollision();
	void VisualizeEnergy();
	void ManageEnergySpeed(float deltaTime);
	void ManageRotation(float deltaTime);
	void MovePlayer(float deltaTime);
	void ReflectPlayer();

	//Window boundaries
	OneBlade m_LeftWindow{ 0,1,0,0 };
	OneBlade m_RightWindow{ m_Window.width,-1,0,0 };
	OneBlade m_DownWindow{ 0,0,1,0 };
	OneBlade m_UpWindow{ m_Window.height,0,-1,0 };
	const std::vector<OneBlade> m_WindowBoundaries{ m_LeftWindow,m_RightWindow,m_DownWindow,m_UpWindow };

	//Pillar
	struct pillar
	{
		ThreeBlade position;
		Color4f color;
		float size;
		bool isSelected;
	};

	std::vector<pillar> m_PillarsVec{};
	int m_SelectedPillar{};

	//pillar functions
	void InitPillars();
	void ColorPillars();
	void DrawPillars() const;


	//Keyboard functions
	void KeyboardSpeed(const SDL_KeyboardEvent& e);
	void KeyboardPillar(const SDL_KeyboardEvent& e);
	void KeyboardRotatePillar(const SDL_KeyboardEvent& e);
	void KeyBoardMovePillar(const SDL_KeyboardEvent& e);
	void KeyBoardReflectPlayer(const SDL_KeyboardEvent& e);
};
