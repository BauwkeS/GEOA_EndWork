#pragma once
#include <utility>

#include "FlyFish.h"
#include "structs.h"
#include "utils.h"

class GameItem
{
protected:
	ThreeBlade m_Position{};
	int m_Size{};
public:
	GameItem(ThreeBlade pos, int size):
	m_Position{std::move(pos)},m_Size{size}{}
	ThreeBlade GetPos() { return m_Position; }
	void SetPos(ThreeBlade newPos) { m_Position = newPos; }
	int GetSize() { return m_Size; }
};

class Pillar : public GameItem
{
	Color4f m_Color{};
	bool m_IsSelected{};
public:
	Pillar(ThreeBlade pos, int size, bool selected=false)
		: GameItem(std::move(pos),size), m_IsSelected{selected}
	{
		if (m_IsSelected) m_Color = m_SelectedPillarColor;
		else m_Color = m_BasicPillarColor;
	}
	//pillar functions
	void ColorPillar();
	void DrawPillar() const;

	bool IsSelected() { return m_IsSelected; }
	void SetSelected(bool value) { m_IsSelected = value; }
private:
	const Color4f m_SelectedPillarColor{ 0.8f,0.02f,0.5f,1.f };
	const Color4f m_BasicPillarColor{ 0.2f,0.01f,0.4f,1.f };
};
