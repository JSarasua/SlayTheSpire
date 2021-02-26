#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/UI/UIState.hpp"
#include <vector>
#include <string>
#include <map>


struct Vec2;

class RenderContext;
class GPUMesh;
class Texture;


enum eStates
{
	Normal,
	Hidden,
	Selected,

	NumStates
};

class Widget
{
public:
	Widget();
	Widget( GPUMesh* mesh, AABB2 screenBounds ); //Root Parent Widget
	Widget( GPUMesh* mesh, Transform const& transform, Widget* parentWidget = nullptr );
	~Widget();

	void Render();

	//Mutators
	void TransformWidget( Transform const& transform );
	void AddChild( Widget* childWidget );
	void SetTexture( Texture const* texture, Texture const* highlightTexture, Texture const* selectTexture );
	void SetEventToFire( std::string const& eventToFire ) { m_eventToFire = eventToFire; }
	void SetCanDrag( bool canDrag ) { m_canDrag = canDrag; }
	void SetCanHover( bool canHover ) { m_canHover = canHover; }
	void SetCanSelect( bool canSelect ) { m_canSelect = canSelect; }
	void RemoveHoverAndSelected();
	void SetPosition( Vec2 const& position ) { m_widgetTransform.m_position = position; }
	void SetText( std::string const& text ) { m_text = text; }
	void SetTextSize( float textSize ) { m_textSize = textSize; }
	void ClearChildren();

	//Accessors
	Mat44 GetParentRelativeModelMatrixNoScale() const;
	Mat44 GetParentInverseModelMatrixNoScale() const;
	Mat44 GetRelativeModelMatrixScaleOnlySelf() const;
	Mat44 GetInverseModelMatrixNoScale() const;
	Mat44 GetRelativeModelMatrix() const;
	Mat44 GetInverseModelMatrix() const;
	Mat44 GetInverseModelMatrixScaleOnlySelf() const;
	bool IsPointInside( Vec2 const& point ) const;
	bool UpdateHovered( Vec2 const& point );
	void UpdateDrag();
	void CheckInput();

	Vec2 GetWorldTopRight() const;
	Vec2 GetWorldBottomLeft() const;
	AABB2 GetWorldAABB2() const;
	Vec2 GetWorldCenter() const;
	AABB2 GetLocalAABB2() const;
	

	bool GetIsHovered() const { return m_isHovered; }
	bool GetIsSelected() const { return m_isSelected; }

private:
	Widget* m_parentWidget = nullptr;
	std::vector<Widget*> m_childWidgets;

	//Assume 1x1 Square
	Transform m_widgetTransform;
	GPUMesh* m_mesh = nullptr;
	Texture const* m_texture = nullptr;
	Texture const* m_highlightTexture = nullptr;
	Texture const* m_selectedTexture = nullptr;
	
	std::string m_eventToFire;
	//properties
	std::string m_text;
	float m_textSize = 1.f;
	bool m_isVisible = false;
	bool m_isHovered = false;
	bool m_isSelected = false;
	bool m_canDrag = false;
	bool m_canSelect = true;
	bool m_canHover= true;
	
	Vec2 const s_invalidMousePosition = Vec2( -9999.f, -9999.f );
	Vec2 m_mouseOffset = s_invalidMousePosition;
	Vec2 m_currentMousePosition = s_invalidMousePosition;

	UIState* m_currentState = nullptr;
	std::map< std::string, UIState > m_states;
};