#include "Engine/UI/Widget.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Math/MatrixUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/UI/UIManager.hpp"
#include "Engine/UI/WidgetAnimation.hpp"

Widget::Widget( Transform const& transform, Widget* parentWidget) :
	m_widgetTransform( transform ),
	m_parentWidget( parentWidget ),
	m_isVisible( true )
{
	m_mesh = g_theUIManager->GetUIMesh();

	if( m_parentWidget )
	{
		m_parentWidget->AddChild( this );
	}
}

Widget::Widget( AABB2 screenBounds )
{
	m_mesh = g_theUIManager->GetUIMesh();
	Vec2 dimensions = screenBounds.GetDimensions();
	Vec2 center = screenBounds.GetCenter();

	m_widgetTransform.m_position = center;
	m_widgetTransform.m_scale = dimensions;
	m_widgetTransform.m_scale.z = 1.f;

	m_isVisible = false;
}

Widget::Widget( AABB2 localBounds, Widget* parentWidget )
{
	m_mesh = g_theUIManager->GetUIMesh();
	m_widgetTransform.m_position = localBounds.GetCenter();
	m_widgetTransform.m_scale = Vec3( localBounds.GetDimensions(), 1.f );

	parentWidget->AddChild( this );
}

Widget::Widget( Vec2 const& parentUVs, Vec2 const& parentPercentDimension, Widget* parentWidget, Vec2 const& offset /*= Vec2()*/, Vec2 const& pivot /*= Vec2( 0.5f, 0.5f ) */ )
{
	AABB2 parentAABB2 = parentWidget->GetLocalAABB2();
	AABB2 newBox = parentAABB2.GetInnerBoxWithAlignment( parentUVs, offset, pivot, parentPercentDimension );

	m_widgetTransform.m_position = newBox.GetCenter();
	m_widgetTransform.m_scale = newBox.GetDimensions();

	m_mesh = g_theUIManager->GetUIMesh();

	parentWidget->AddChild( this );
}

Widget::~Widget()
{
	if( m_parentWidget )
	{
		m_parentWidget->RemoveChildWidget( this );
		m_parentWidget = nullptr;
	}

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

void Widget::SetTexture( Texture const* texture, Texture const* highlightTexture, Texture const* selectTexture )
{
	m_texture = texture;
	m_highlightTexture = highlightTexture;
	m_selectedTexture = selectTexture;
}

void Widget::RemoveHoverAndSelected()
{
	m_isHovered = false;
	m_isSelected = false;
}

void Widget::ClearChildren()
{
	for( Widget* childWidget : m_childWidgets )
	{
		delete childWidget;
		childWidget = nullptr;
	}

	m_childWidgets.clear();
}

//Used by a child Widget on its parent trying to get its parent's relative matrix
Mat44 Widget::GetParentRelativeModelMatrixNoScale() const
{
	Mat44 parentMatrix;
	Mat44 myLocalMatrix = m_widgetTransform.ToMatrixNoScale();
	if( nullptr != m_parentWidget )
	{
		parentMatrix = m_parentWidget->GetParentRelativeModelMatrixNoScale();
	}
	//Pushing on local matrix to the end of matrix chain
	parentMatrix.TransformBy( myLocalMatrix );

	return parentMatrix;
}

Mat44 Widget::GetParentInverseModelMatrixNoScale() const
{
	if( m_parentWidget )
	{
		Mat44 parentInverse = m_parentWidget->GetInverseModelMatrixNoScale();
		return parentInverse;
	}
	else
	{
		return Mat44();
	}
}

Mat44 Widget::GetRelativeModelMatrixScaleOnlySelf() const
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

Mat44 Widget::GetRelativeModelMatrixNoScale() const
{
	Mat44 parentRelativeMatrixNoScale;
	if( m_parentWidget )
	{
		parentRelativeMatrixNoScale = m_parentWidget->GetParentRelativeModelMatrixNoScale();
	}

	Mat44 myLocalMatrix = m_widgetTransform.ToMatrixNoScale();

	parentRelativeMatrixNoScale.TransformBy( myLocalMatrix );
	return parentRelativeMatrixNoScale;
}

Mat44 Widget::GetInverseModelMatrixNoScale() const
{
	Mat44 parentInverseMatrixNoScale;
	if( m_parentWidget )
	{
		parentInverseMatrixNoScale = m_parentWidget->GetInverseModelMatrixNoScale();
	}

	Mat44 myLocalMatrix = m_widgetTransform.ToMatrixNoScale();
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
			Mat44 modelMatrix = GetRelativeModelMatrixScaleOnlySelf();

			context->SetModelMatrix( modelMatrix );
			context->SetModelTint( m_tint );
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

			if( m_texture )
			{
				context->BindTexture( m_texture );
				context->DrawMesh( m_mesh );
			}


			if( m_text.size() > 0 )
			{
				Mat44 textModelMatrix = GetRelativeModelMatrixNoScale();
				context->SetModelMatrix( textModelMatrix );
				AABB2 textBox = GetLocalAABB2();
				context->DrawAlignedTextAtPosition( m_text.c_str(), textBox, m_textSize, m_textAlignent, m_tint );

			}
			context->SetModelTint( Rgba8::WHITE );
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

bool Widget::EndAnimation( EventArgs const& args )
{
	UNUSED( args );
	m_isAnimationDone = true;
	return false;
}

Delegate<EventArgs const&>& Widget::StartAnimation( Transform const& finalTransform, float animationTime, eSmoothingFunction smoothingFunction )
{
	m_isAnimationDone = false;
	if( m_currentWidgetAnimation )
	{
		delete m_currentWidgetAnimation;
		m_currentWidgetAnimation = nullptr;
	}

	m_currentWidgetAnimation = new WidgetAnimation( this, finalTransform, animationTime, smoothingFunction );
	m_currentWidgetAnimation->m_endAnimationDelegate.SubscribeMethod( this, &Widget::EndAnimation );

	return m_currentWidgetAnimation->m_endAnimationDelegate;
}

Delegate<EventArgs const&>& Widget::StartAnimation( WidgetAnimation const& widgetAnimation )
{
	if( m_currentWidgetAnimation )
	{
		delete m_currentWidgetAnimation;
		m_currentWidgetAnimation = nullptr;
	}

	m_currentWidgetAnimation = new WidgetAnimation( widgetAnimation );
	m_currentWidgetAnimation->m_endAnimationDelegate.SubscribeMethod( this, &Widget::EndAnimation );

	return m_currentWidgetAnimation->m_endAnimationDelegate;
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

Mat44 Widget::GetInverseModelMatrixScaleOnlySelf() const
{
	Mat44 parentInverseNoScale;
	if( m_parentWidget )
	{
		parentInverseNoScale = m_parentWidget->GetInverseModelMatrixNoScale();
	}

	Mat44 myModelMatrix = m_widgetTransform.ToMatrix();
	MatrixInvert( myModelMatrix );
	myModelMatrix.TransformBy( parentInverseNoScale );

	return myModelMatrix;
}

bool Widget::IsPointInside( Vec2 const& point ) const
{
	Mat44 localSpaceMatrix = GetInverseModelMatrixScaleOnlySelf();
	Vec2 localPosition = localSpaceMatrix.TransformPosition2D( point );

	//Unsure if this works. Need to test. Matrices may be going backwards
	AABB2 widgetBounds = AABB2( Vec2( -0.5f, -0.5f ), Vec2( 0.5f, 0.5f ) );
	return widgetBounds.IsPointInside( localPosition );
}

std::vector<Widget*> Widget::GetChildWidgets()
{
	return m_childWidgets;
}

void Widget::Update( float deltaSeconds, Vec2 const& mousePos )
{
	UpdateHovered( mousePos );
	CheckInput();
	UpdateDrag();
	UpdateAnimations( deltaSeconds );
}

bool Widget::UpdateHovered( Vec2 const& point )
{
	m_currentMousePosition = point;
	m_isHovered = IsPointInside( point ) && m_canHover;
	if( m_isHovered )
	{
		FireHoverEvents();
	}
	if( m_isSelected )
	{
		//Mouse Offset is in World space
		if( m_mouseOffset == s_invalidMousePosition )
		{
			m_mouseOffset = m_currentMousePosition - GetWorldCenter();
		}
	}

	bool doesAWidgetHover = false;
	for( int widgetIndex = (int)m_childWidgets.size() - 1; widgetIndex >= 0; widgetIndex-- )
	{
		Widget* childWidget = m_childWidgets[widgetIndex];
		if( nullptr != childWidget )
		{
			if( doesAWidgetHover )
			{
				childWidget->RemoveHoverAndSelected();
			}
			else
			{
				doesAWidgetHover = childWidget->UpdateHovered( point );
			}
		}
	}

	return m_isHovered;
}

void Widget::UpdateDrag()
{
	if( m_isSelected && m_canDrag && m_mouseOffset != s_invalidMousePosition )
	{
		//The new position must be in parent's space without scale
		Mat44 localSpaceMatrix = GetParentInverseModelMatrixNoScale();
		Vec2 position = m_currentMousePosition - m_mouseOffset;
		Vec2 localPosition = localSpaceMatrix.TransformPosition2D( position );
		m_widgetTransform.m_position = localPosition;
	}

	for( Widget* childWidget : m_childWidgets )
	{
		if( nullptr != childWidget )
		{
			childWidget->UpdateDrag();
		}
	}
}

void Widget::CheckInput()
{
	m_wasSelected = m_isSelected;

	KeyButtonState const& leftMouseButton = g_theInput->GetMouseButton(LeftMouseButton);


	if( leftMouseButton.WasJustPressed() && m_isHovered && m_canSelect )
	{
		m_isSelected = true;

		FireSelectEvents();
	}
	if( leftMouseButton.IsPressed() && m_isHovered && m_canSelect )
	{
		FireSelectedEvents();
	}
	if( leftMouseButton.WasJustReleased() )
	{
		m_isSelected = false;
		m_mouseOffset = s_invalidMousePosition;
		

		if( !m_isSelected && m_wasSelected )
		{
			FireReleaseEvents();
		}
	}

	for( Widget* childWidget : m_childWidgets )
	{
		if( nullptr != childWidget )
		{
			childWidget->CheckInput();
		}
	}
}

void Widget::UpdateAnimations( float deltaSeconds )
{
	if( m_currentWidgetAnimation )
	{
		m_currentWidgetAnimation->Update( deltaSeconds );
	}

	if( m_isAnimationDone )
	{
		m_isAnimationDone = false;
		if( m_currentWidgetAnimation )
		{
			delete m_currentWidgetAnimation;
			m_currentWidgetAnimation = nullptr;
		}
	}

	for( Widget* childWidget : m_childWidgets )
	{
		if( nullptr != childWidget )
		{
			childWidget->UpdateAnimations( deltaSeconds );
		}
	}
}

Vec2 Widget::GetWorldTopRight() const
{
	Mat44 invMatrix = GetRelativeModelMatrixScaleOnlySelf();
	Vec2 localTopRight = Vec2( 0.5f, 0.5f );
	Vec2 worldTopRight = invMatrix.TransformPosition2D( localTopRight );

	return worldTopRight;
}

Vec2 Widget::GetWorldBottomLeft() const
{
	Mat44 invMatrix = GetRelativeModelMatrixScaleOnlySelf();
	Vec2 localBottomLeft = Vec2( -0.5f, -0.5f );
	Vec2 worldBottomLeft = invMatrix.TransformPosition2D( localBottomLeft );

	return worldBottomLeft;
}

AABB2 Widget::GetWorldAABB2() const
{
	AABB2 worldAABB = AABB2( GetWorldBottomLeft(), GetWorldTopRight() );

	return worldAABB;
}

Vec2 Widget::GetWorldCenter() const
{
	Mat44 invMatrix = GetRelativeModelMatrixScaleOnlySelf();
	Vec2 localCenter = Vec2( 0.f, 0.f );
	Vec2 worlCenter = invMatrix.TransformPosition2D( localCenter );

	return worlCenter;
}

AABB2 Widget::GetLocalAABB2() const
{
	Mat44 localMatrix = m_widgetTransform.ToMatrixNoTranslation();
	Vec2 localBottomLeft = localMatrix.TransformPosition2D( Vec2( -0.5f, -0.5f ) );
	Vec2 localTopRight = localMatrix.TransformPosition2D( Vec2( 0.5f, 0.5f ) );

	AABB2 localAABB = AABB2( localBottomLeft, localTopRight );
	return localAABB;
}

void Widget::RemoveChildWidget( Widget* childWidget )
{
	for( size_t widgetIndex = 0; widgetIndex < m_childWidgets.size(); widgetIndex++ )
	{
		Widget*& currentWidget = m_childWidgets[widgetIndex];

		if( currentWidget == childWidget )
		{
			Widget* backWidget = m_childWidgets.back();
			currentWidget = backWidget;
			m_childWidgets.pop_back();
			return;
		}
	}
}

void Widget::CleanUpChildren()
{
	int widgetIndex = 0;
	while( widgetIndex < m_childWidgets.size() )
	{
		Widget* childWidget = m_childWidgets[widgetIndex];
		if( childWidget->GetIsGarbage() )
		{
			delete childWidget;
		}
		else
		{
			widgetIndex++;
		}
	}

	for( Widget* childWidget : m_childWidgets )
	{
		childWidget->CleanUpChildren();
	}
}

void Widget::FireSelectedEvents()
{
	Vec2 currentPos = m_widgetTransform.m_position;
	currentPos = GetWorldCenter();
	m_selectedArgs.SetValue( "currentPos", currentPos );
	m_selectedArgs.SetValue( "mousePos", m_currentMousePosition );

	g_theEventSystem->FireEvent( m_eventToFire, NOCONSOLECOMMAND, nullptr );
	g_theEventSystem->FireEvent( m_eventToFire, CONSOLECOMMAND, nullptr );
	m_selectedDelegate.Invoke( m_selectedArgs );
}

void Widget::FireSelectEvents()
{
	Vec2 currentPos = m_widgetTransform.m_position;
	m_selectArgs.SetValue( "currentPos", currentPos );
	m_selectArgs.SetValue( "mousePos", m_currentMousePosition );

	g_theEventSystem->FireEvent( m_eventToFire, NOCONSOLECOMMAND, nullptr );
	g_theEventSystem->FireEvent( m_eventToFire, CONSOLECOMMAND, nullptr );
	m_selectDelegate.Invoke( m_selectArgs );
}

void Widget::FireHoverEvents()
{
	Vec2 currentPos = m_widgetTransform.m_position;
	m_hoverArgs.SetValue( "currentPos", currentPos );
	m_hoverArgs.SetValue( "mousePos", m_currentMousePosition );

	m_hoverDelegate.Invoke( m_hoverArgs );
}

void Widget::FireReleaseEvents()
{
	Vec2 currentPos = m_widgetTransform.m_position;
	m_releaseArgs.SetValue( "currentPos", currentPos );
	m_releaseArgs.SetValue( "mousePos", m_currentMousePosition );

	m_releaseDelegate.Invoke( m_releaseArgs );
}

