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

class Widget
{
public:
	Widget();
	Widget( GPUMesh* mesh, Transform const& transform, Widget* parentWidget = nullptr );
	~Widget();

	void Render();

	//Mutators
	void TransformWidget( Transform const& transform );
	void AddChild( Widget* childWidget );

	//Accessors
	Mat44 GetRelativeModelMatrix() const;
	Mat44 GetReverseModelMatrix() const;
	bool IsPointInside( Vec2 const& point ) const;

private:
	Widget* m_parentWidget = nullptr;
	std::vector<Widget*> m_childWidgets;

	//Assume 1x1 Square
	Transform m_widgetTransform;
	GPUMesh* m_mesh = nullptr;
	Texture* m_texture = nullptr;
	
	std::string m_eventToFire;
	//properties
};