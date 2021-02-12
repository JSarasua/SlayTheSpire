#include "Engine/UI/Widget.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Math/MatrixUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"

Widget::Widget( GPUMesh* mesh, Transform const& transform, Widget* parentWidget) :
	m_mesh( mesh ),
	m_widgetTransform( transform ),
	m_parentWidget( parentWidget ),
	m_isVisible( true )
{}

Widget::Widget( GPUMesh* mesh, AABB2 screenBounds ) :
	m_mesh( mesh )
{
	Vec2 dimensions = screenBounds.GetDimensions();
	Vec2 center = screenBounds.GetCenter();

	m_widgetTransform.m_position = center;
	m_widgetTransform.m_scale = dimensions;
	m_widgetTransform.m_scale.z = 1.f;

	m_isVisible = false;
}

Widget::~Widget()
{
	m_parentWidget = nullptr;

	for( Widget* childWidget : m_childWidgets )
	{
		delete childWidget;
	}
	m_childWidgets.clear();
	m_mesh = nullptr;
	m_texture = nullptr;
}

void Widget::TransformWidget( Transform const& transform )
{
	m_widgetTransform.TransformBy( transform );
}

void Widget::AddChild( Widget* childWidget )
{
	childWidget->m_parentWidget = this;
	m_childWidgets.push_back( childWidget );
}

void Widget::SetTexture( Texture* texture, Texture* highlightTexture, Texture* selectTexture )
{
	m_texture = texture;
	m_highlightTexture = highlightTexture;
	m_selectedTexture = selectTexture;
}

Mat44 Widget::GetParentRelativeModelMatrixNoScale() const
{
	Mat44 parentMatrix;
	Mat44 myLocalMatrix = m_widgetTransform.ToMatrixNoScale();
	if( nullptr != m_parentWidget )
	{
		parentMatrix = m_parentWidget->GetRelativeModelMatrix();
	}
	//Pushing on local matrix to the end of maxtrix chain
	parentMatrix.TransformBy( myLocalMatrix );

	return parentMatrix;
}

Mat44 Widget::GetParentInverseModelMatrixNoScale() const
{
	Mat44 myLocalMatrix = m_widgetTransform.ToMatrixNoScale();
	MatrixInvert( myLocalMatrix );

	Mat44 currentMatrix;
	currentMatrix = myLocalMatrix;
	Widget* parentWidget = m_parentWidget;
	while( parentWidget )
	{
		Mat44 parentMatrix = parentWidget->m_widgetTransform.ToMatrixNoScale();
		MatrixInvert( parentMatrix );
		currentMatrix.TransformBy( parentMatrix );
		parentWidget = parentWidget->m_parentWidget;
	}

	return currentMatrix;
}

Mat44 Widget::GetRelativeModelMatrixNoScale() const
{
	Mat44 parentRelativeMatrixNoScale;
	if( m_parentWidget )
	{
		parentRelativeMatrixNoScale = m_parentWidget->GetParentRelativeModelMatrixNoScale();
	}

	Mat44 myLocalMatrix = m_widgetTransform.ToMatrix();

	parentRelativeMatrixNoScale.TransformBy( myLocalMatrix );
	return parentRelativeMatrixNoScale;
}

Mat44 Widget::GetInverseModelMatrixNoScale() const
{
	Mat44 parentInverseMatrixNoScale;
	if( m_parentWidget )
	{
		parentInverseMatrixNoScale = m_parentWidget->GetParentInverseModelMatrixNoScale();
	}

	Mat44 myLocalMatrix = m_widgetTransform.ToMatrix();
	MatrixInvert( myLocalMatrix );

	myLocalMatrix.TransformBy( parentInverseMatrixNoScale );

	return myLocalMatrix;
}

void Widget::Render()
{
	//Render Self
	if( nullptr != m_mesh && m_isVisible )
	{
		RenderContext* context = m_mesh->m_renderContext;
		if( nullptr != context )
		{
			//Mat44 modelMatrix = GetRelativeModelMatrix();
			Mat44 modelMatrix = GetRelativeModelMatrixNoScale();

			context->SetModelMatrix( modelMatrix );
			context->BindShader( (Shader*)nullptr );

			if( m_isSelected )
			{
				context->BindTexture( m_selectedTexture );
				context->DrawMesh( m_mesh );
			}
			else if( m_isHovered )
			{
				context->BindTexture( m_highlightTexture );
				context->DrawMesh( m_mesh );
			}

			context->BindTexture( m_texture );
			context->DrawMesh( m_mesh );
		}
	}

	//Render Children
	for( Widget* childWidget : m_childWidgets )
	{
		if( nullptr != childWidget )
		{
			childWidget->Render();
		}
	}
}

Mat44 Widget::GetRelativeModelMatrix() const
{
	Mat44 parentMatrix;
	Mat44 myLocalMatrix = m_widgetTransform.ToMatrix();
	if( nullptr != m_parentWidget )
	{
		parentMatrix = m_parentWidget->GetRelativeModelMatrix();
	}
	//Pushing on local matrix to the end of maxtrix chain
	parentMatrix.TransformBy( myLocalMatrix );

	return parentMatrix;
}

Mat44 Widget::GetInverseModelMatrix() const
{
	Mat44 myLocalMatrix = m_widgetTransform.ToMatrix();
	MatrixInvert( myLocalMatrix );

	Mat44 currentMatrix;
	currentMatrix = myLocalMatrix;
	Widget* parentWidget = m_parentWidget;
	while( parentWidget )
	{
		Mat44 parentMatrix = parentWidget->m_widgetTransform.ToMatrix();
		MatrixInvert( parentMatrix );
		currentMatrix.TransformBy( parentMatrix );
		parentWidget = parentWidget->m_parentWidget;
	}

	return currentMatrix;
}

bool Widget::IsPointInside( Vec2 const& point ) const
{
	Mat44 localSpaceMatrix = GetInverseModelMatrixNoScale();
	Vec2 localPosition = localSpaceMatrix.TransformPosition2D( point );

	//Unsure if this works. Need to test. Matrices may be going backwards
	AABB2 widgetBounds = AABB2( Vec2( -0.5f, -0.5f ), Vec2( 0.5f, 0.5f ) );
	return widgetBounds.IsPointInside( localPosition );
}

void Widget::UpdateHovered( Vec2 const& point )
{
	m_isHovered = IsPointInside( point );

	for( Widget* childWidget : m_childWidgets )
	{
		if( nullptr != childWidget )
		{
			childWidget->UpdateHovered( point );
		}
	}
}

void Widget::CheckInput()
{
	KeyButtonState const& leftMouseButton = g_theInput->GetMouseButton(LeftMouseButton);


	if( leftMouseButton.WasJustPressed() && m_isHovered )
	{
		g_theEventSystem->FireEvent( m_eventToFire, CONSOLECOMMAND, nullptr );
	}
	if( leftMouseButton.IsPressed() && m_isHovered )
	{
		m_isSelected = true;
	}
	if( leftMouseButton.WasJustReleased() )
	{
		m_isSelected = false;
	}

	for( Widget* childWidget : m_childWidgets )
	{
		if( nullptr != childWidget )
		{
			childWidget->CheckInput();
		}
	}
}

