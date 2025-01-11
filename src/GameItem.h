#pragma once
#include <utility>
#include <memory>

#include "FlyFish.h"
#include "structs.h"
#include "utils.h"

class GameItem
{
protected:
	ThreeBlade m_Position{};
	int m_Width{};
	int m_Height{};
	Color4f m_Color{};
public:
	virtual ~GameItem() = default;

	GameItem(ThreeBlade pos, int width, int height):
		m_Position{ std::move(pos) }, m_Width{ width }, m_Height{height} {}
	ThreeBlade GetPos() { return m_Position; }
	void SetPos(ThreeBlade newPos) { m_Position = newPos; }
	void SetColor(Color4f col) { m_Color = col; }
	int GetHeight() { return m_Height; }
	int GetWidth() { return m_Width; }
	virtual void Draw() const;
};

class Pillar : public GameItem
{
	bool m_IsSelected{};
public:
	Pillar(ThreeBlade pos, int size, bool selected=false)
		: GameItem(std::move(pos),size, size), m_IsSelected{selected}
	{
		if (m_IsSelected) m_Color = m_SelectedPillarColor;
		else m_Color = m_BasicPillarColor;
	}
	//pillar functions
	void ColorPillar();
	bool IsSelected() { return m_IsSelected; }
	void SetSelected(bool value) { m_IsSelected = value; ColorPillar(); }
	int GetSize() { return m_Width; }

private:
	const Color4f m_SelectedPillarColor{ 0.8f,0.02f,0.5f,1.f };
	const Color4f m_BasicPillarColor{ 0.2f,0.01f,0.4f,1.f };
};

class Pickup : public GameItem
{
	int m_Points{};
	const Color4f m_PickupColor{ 0.f,1.f,0.f,1.f };

public:
	Pickup(ThreeBlade pos, int size,int points)
	: GameItem(pos,size, size), m_Points{points}
	{
		m_Color = m_PickupColor;
	}
	void Draw() const override;
	int GetPoints() { return m_Points; }
	int GetSize() { return m_Width; }
};

class Barrier : public GameItem
{
	const Color4f m_BarrierColor{ 1,0,0,1 };

public:
	Barrier(ThreeBlade pos, int width, int height)
		: GameItem(pos, width, height)
	{
		m_Color = m_BarrierColor;
	}
	void Draw() const override;
};