#include "Engine/UI/Widget.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/Shader.hpp"

Widget::Widget( GPUMesh* mesh, Transform const& transform, Widget* parentWidget) :
	m_mesh( mesh ),
	m_widgetTransform( transform ),
	m_parentWidget( parentWidget )
{}

void Widget::TransformWidget( Transform const& transform )
{
	m_widgetTransform.TransformBy( transform );
}

void Widget::AddChild( Widget* childWidget )
{
	m_childWidgets.push_back( childWidget );
}

void Widget::Render()
{
	//Render Self
	if( nullptr != m_mesh )
	{
		RenderContext* context = m_mesh->m_renderContext;
		if( nullptr != context )
		{
			Mat44 modelMatrix = GetRelativeModelMatrix();
			context->SetModelMatrix( modelMatrix );
			context->BindTexture( m_texture );
			context->BindShader( (Shader*)nullptr );
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

Mat44 Widget::GetReverseModelMatrix() const
{
	Mat44 parentMatrix;
	Mat44 myLocalMatrix = m_widgetTransform.ToMatrix();

	Mat44 currentMatrix;
	currentMatrix = myLocalMatrix;
	Widget* parentWidget = m_parentWidget;
	while( parentWidget )
	{
		currentMatrix.TransformBy( parentWidget->m_widgetTransform.ToMatrix() );
		parentWidget = parentWidget->m_parentWidget;
	}

	return currentMatrix;
}

bool Widget::IsPointInside( Vec2 const& point ) const
{
	Mat44 localSpaceMatrix = GetReverseModelMatrix();
	Vec2 localPosition = localSpaceMatrix.TransformPosition2D( point );

	//Unsure if this works. Need to test. Matrices may be going backwards
	AABB2 widgetBounds = AABB2( Vec2( -0.5f, -0.5f ), Vec2( 0.5f, 0.5f ) );
	return widgetBounds.IsPointInside( localPosition );
}

