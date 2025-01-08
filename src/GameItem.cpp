#include "GameItem.h"

void GameItem::Draw() const
{
	utils::SetColor(m_Color);
	utils::FillRect(m_Position[0], m_Position[1], m_Size, m_Size);
}

void Pillar::ColorPillar()
{
	if (m_IsSelected) m_Color = m_SelectedPillarColor;
	else m_Color = m_BasicPillarColor;
}

void Pickup::Draw() const
{
	utils::SetColor(m_Color);
	utils::FillEllipse(m_Position[0], m_Position[1], m_Size, m_Size);
}

