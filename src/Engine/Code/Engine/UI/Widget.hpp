#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/UI/UIState.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Delegate.hpp"
#include <vector>
#include <string>
#include <map>


struct Vec2;

class RenderContext;
class GPUMesh;
class Texture;
class WidgetAnimation;

enum class eSmoothingFunction;

enum eStates
{
	Normal,
	Hidden,
	Selected,

	NumStates
};

class Widget
{
	friend class WidgetAnimation;
public:
	Widget();
	Widget( AABB2 screenBounds ); //Root Parent Widget
	Widget( AABB2 localBounds, Widget* parentWidget );
	Widget( Transform const& transform, Widget* parentWidget = nullptr );
	Widget( Vec2 const& parentUVs, Vec2 const& parentPercentDimension, Widget* parentWidget, Vec2 const& offset = Vec2(), Vec2 const& pivot = Vec2( 0.5f, 0.5f ) );

	~Widget();

	virtual void Render();

	bool EndAnimation( EventArgs const& args );
	Delegate<EventArgs const&>& StartAnimation( Transform const& finalTransform, float animationTime, eSmoothingFunction smoothingFunction );
	Delegate<EventArgs const&>& StartAnimation( WidgetAnimation const& widgetAnimation );


	//Mutators
	void TransformWidget( Transform const& transform );
	virtual void AddChild( Widget* childWidget );
	void SetTexture( Texture const* texture, Texture const* highlightTexture, Texture const* selectTexture );
	void SetEventToFire( std::string const& eventToFire ) { m_eventToFire = eventToFire; }
	void SetCanDrag( bool canDrag ) { m_canDrag = canDrag; }
	void SetCanHover( bool canHover ) { m_canHover = canHover; }
	void SetCanSelect( bool canSelect ) { m_canSelect = canSelect; }
	void SetIsVisible( bool isVisible ) { m_isVisible = isVisible; }
	void RemoveHoverAndSelected();
	void SetTransform( Transform const& transform ) { m_widgetTransform = transform; }
	void SetPosition( Vec2 const& position ) { m_widgetTransform.m_position = position; }
	void SetText( std::string const& text ) { m_text = text; }
	void SetTextSize( float textSize ) { m_textSize = textSize; }
	void ClearChildren();
	void SetParent( Widget* parentWidget ) { m_parentWidget = parentWidget; }
	void SetTextAlignment( Vec2 const& textAlignment ) { m_textAlignent = textAlignment; }
	void SetTint( Rgba8 const& widgetTint ) { m_tint = widgetTint; }

	//Accessors
	Transform GetTransform() const { return m_widgetTransform; }
	Mat44 GetParentRelativeModelMatrixNoScale() const;
	Mat44 GetParentInverseModelMatrixNoScale() const;
	Mat44 GetRelativeModelMatrixScaleOnlySelf() const;
	Mat44 GetRelativeModelMatrixNoScale() const;
	Mat44 GetInverseModelMatrixNoScale() const;
	Mat44 GetRelativeModelMatrix() const;
	Mat44 GetInverseModelMatrix() const;
	Mat44 GetInverseModelMatrixScaleOnlySelf() const;
	bool IsPointInside( Vec2 const& point ) const;

	std::vector<Widget*> GetChildWidgets();

	void Update( float deltaSeconds, Vec2 const& mousePos );
	bool UpdateHovered( Vec2 const& point );
	void UpdateDrag();
	void CheckInput();
	void UpdateAnimations( float deltaSeconds );

	Vec2 GetWorldTopRight() const;
	Vec2 GetWorldBottomLeft() const;
	AABB2 GetWorldAABB2() const;
	Vec2 GetWorldCenter() const;
	AABB2 GetLocalAABB2() const;
	

	bool GetIsHovered() const { return m_isHovered; }
	bool GetIsSelected() const { return m_isSelected; }

	Widget* GetParentWidget() { return m_parentWidget; }
	void RemoveChildWidget( Widget* childWidget );

	void CleanUpChildren();
	void MarkGarbage() { m_isGarbage = true; }
private:
	void FireSelectedEvents();
	void FireSelectEvents();
	void FireHoverEvents();
	void FireReleaseEvents();

	bool GetIsGarbage() const { return m_isGarbage; }

protected:
	bool m_isGarbage;
	Widget* m_parentWidget = nullptr;
	std::vector<Widget*> m_childWidgets;

	//Assume 1x1 Square
	Transform m_widgetTransform;
	Rgba8 m_tint = Rgba8::WHITE;
	GPUMesh* m_mesh = nullptr;
	Texture const* m_texture = nullptr;
	Texture const* m_highlightTexture = nullptr;
	Texture const* m_selectedTexture = nullptr;
	
	std::string m_eventToFire;
	//properties
	std::string m_text;
	Vec2 m_textAlignent = Vec2( 0.5f, 0.5f );
	float m_textSize = 1.f;
	bool m_isVisible = false;
	bool m_isHovered = false;
	bool m_isSelected = false;
	bool m_wasSelected = false;
	bool m_canDrag = false;
	bool m_canSelect = true;
	bool m_canHover= true;
	
	Vec2 const s_invalidMousePosition = Vec2( -9999.f, -9999.f );
	Vec2 m_mouseOffset = s_invalidMousePosition;
	Vec2 m_currentMousePosition = s_invalidMousePosition;

	UIState* m_currentState = nullptr;
	std::map< std::string, UIState > m_states;

public:
	EventArgs m_selectArgs;
	EventArgs m_selectedArgs;
	EventArgs m_releaseArgs;
	EventArgs m_hoverArgs;

	Delegate<EventArgs const&> m_selectDelegate;
	Delegate<EventArgs const&> m_selectedDelegate;
	Delegate<EventArgs const&> m_releaseDelegate;
	Delegate<EventArgs const&> m_hoverDelegate;

	bool m_isAnimationDone = false;
	WidgetAnimation* m_currentWidgetAnimation = nullptr;
};