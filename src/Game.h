#pragma once
#include "FlyFish.h"
#include "structs.h"
#include "SDL.h"
#include "SDL_opengl.h"
#include <vector>

#include "GameItem.h"

class Game
{
public:
	explicit Game(const Window& window);
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game(Game&& other) = delete;
	Game& operator=(Game&& other) = delete;

	~Game();

	void Run();

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
		KeyBoardSpawnNewPillar(e);
	}
	void ProcessKeyUpEvent(const SDL_KeyboardEvent& e)
	{

	}
	void ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e)
	{

	}
	void ProcessMouseDownEvent(const SDL_MouseButtonEvent& e)
	{
		MouseSelectPillar(e);
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
	const float m_PlayerSize{ 20.f };
	ThreeBlade m_PlayerPosition{ m_Window.width / 2+ m_PlayerSize,m_Window.height / 2,0 };
	const float m_NormalPlayerSpeed{ 200.f };
	float m_PlayerSpeed{ m_NormalPlayerSpeed };
	Color4f m_PlayerColor{ 1,1,1,1 };
	//player dimensions to be added
	TwoBlade m_PlayerDirection{ 1,1,0,0,0,0 };
	TwoBlade m_PlayerDirectionRotation{ 0,0,0,0,0,1 };
	Motor m_PlayerMotor{ Motor::Translation(m_PlayerSpeed,m_PlayerDirection) };
	bool m_IsRotating{ false };
	int m_PlayerScore{ 0 };

	// FUNCTIONS
	void InitializeGameEngine();
	void CleanupGameEngine();
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
	//struct pillar
	//{
	//	ThreeBlade position;
	//	Color4f color;
	//	float size;
	//	bool isSelected;
	//};

	std::vector<std::unique_ptr<Pillar>> m_PillarsVec{};
	int m_SelectedPillar{};
	//const Color4f m_SelectedPillarColor{ 0.8f,0.02f,0.5f,1.f };
	//const Color4f m_BasicPillarColor{ 0.2f,0.01f,0.4f,1.f };

	//pillar functions
	void InitPillars();
	void ColorPillars();
	void DrawPillars() const;
	void SpawnPillar();

	//Pickups
	/*struct pickup
	{
		ThreeBlade position;
		int size;
		int points;
	};*/
	std::vector<std::unique_ptr<Pickup>> m_PickupsVec;
	//const Color4f m_PickupColor{ 0.f,1.f,0.f,1.f };

	//pickup functions
	void SpawnPickups();
	void MakeNewPickup();
	void DrawPickups() const;
	void PickupCollision();

	//Game items
	void InitGameItems();
	void DrawGameItems() const;

	std::vector<std::unique_ptr<Barrier>> m_BarrierVec;

	//Keyboard functions
	void KeyboardSpeed(const SDL_KeyboardEvent& e);
	void KeyboardPillar(const SDL_KeyboardEvent& e);
	void KeyboardRotatePillar(const SDL_KeyboardEvent& e);
	void KeyBoardMovePillar(const SDL_KeyboardEvent& e);
	void KeyBoardReflectPlayer(const SDL_KeyboardEvent& e);
	void KeyBoardSpawnNewPillar(const SDL_KeyboardEvent& e);

	//Mouse functions
	void MouseSelectPillar(const SDL_MouseButtonEvent& e);

	//Overlap functions
	template <typename T>
	int CheckOverlapGameItems(const ThreeBlade& pos, int size, const std::vector<std::unique_ptr<T>>& vec)
	{
		int hasHit{ -1 };

		//game items
		for (int i = 0; i < vec.size(); ++i)
		{
			int vecSize = vec[i]->GetSize();
			//if the size is 0, check on size of item
			if (size == 0) size = vecSize;

			ThreeBlade checkPos = { pos[0], pos[1], 0 };

			//if its a barrier, the y coord is overrated
			Barrier* barrierExists = dynamic_cast<Barrier*>(vec[i].get());
			if (barrierExists)
			{
				checkPos = { pos[0], 0, 0 };
			}

			auto bladeDis = vec[i]->GetPos() & checkPos;
			if (abs(bladeDis.Norm()) < static_cast <float>(size / 2 + vecSize / 2))
			{
				hasHit = i;
			}
		}
		//nothing hit
		return hasHit;
	}

	bool DoesOverlapAll(ThreeBlade item, int size);
	int CheckOverlapPillars(ThreeBlade item, int size);
	int CheckOverlapPickups(ThreeBlade item, int size);
	int CheckOverlapBarriers(ThreeBlade item, int size);
	//int CheckBarrierOverlapDir(ThreeBlade item, int size, Barrier* b);
};
