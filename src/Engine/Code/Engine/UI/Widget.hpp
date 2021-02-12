#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/Transform.hpp"
#include <vector>
#include <string>

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
	void SetTexture( Texture* texture, Texture* highlightTexture, Texture* selectTexture );

	//Accessors
	Mat44 GetParentRelativeModelMatrixNoScale() const;
	Mat44 GetParentInverseModelMatrixNoScale() const;
	Mat44 GetRelativeModelMatrixNoScale() const;
	Mat44 GetInverseModelMatrixNoScale() const;
	Mat44 GetRelativeModelMatrix() const;
	Mat44 GetInverseModelMatrix() const;
	bool IsPointInside( Vec2 const& point ) const;
	void UpdateHovered( Vec2 const& point );
	void CheckInput();
	void SetEventToFire( std::string const& eventToFire ) { m_eventToFire = eventToFire; }

	bool GetIsHovered() const { return m_isHovered; }
	bool GetIsSelected() const { return m_isSelected; }

private:
	Widget* m_parentWidget = nullptr;
	std::vector<Widget*> m_childWidgets;

	//Assume 1x1 Square
	Transform m_widgetTransform;
	GPUMesh* m_mesh = nullptr;
	Texture* m_texture = nullptr;
	Texture* m_highlightTexture = nullptr;
	Texture* m_selectedTexture = nullptr;
	
	std::string m_eventToFire;
	//properties

	bool m_isVisible = false;
	bool m_isHovered = false;
	bool m_isSelected = false;
};