#include "Engine/UI/WidgetAnimation.hpp"
#include "Engine/Math/MathUtils.hpp"

WidgetAnimation::WidgetAnimation( Widget* widgetToAnimate, Transform const& finalTransform, float animationTime, eSmoothingFunction smoothingFunction ):
	m_widgetToAnimate( widgetToAnimate ),
	m_finalTransform( finalTransform ),
	m_currentTime( 0.f ),
	m_animationTime( animationTime ),
	m_smoothingFunction( smoothingFunction )
{
	m_startingTransform = m_widgetToAnimate->m_widgetTransform;
}

WidgetAnimation::~WidgetAnimation()
{

}

void WidgetAnimation::SetStartingTransform( Transform const& startingTransform )
{
	m_startingTransform = startingTransform;
}

void WidgetAnimation::Update( float deltaSeconds )
{
	m_currentTime += deltaSeconds;
	if( m_currentTime >= m_animationTime )
	{
		m_widgetToAnimate->m_widgetTransform = m_finalTransform;
		m_endAnimationDelegate.Invoke( EventArgs() );
		return;
	}

	UpdateWidgetTransform();
}

void WidgetAnimation::UpdateWidgetTransform()
{
	Vec3 newPosition = UpdateVec3( m_startingTransform.m_position, m_finalTransform.m_position );
	Vec3 newOrientation = UpdateVec3( m_startingTransform.m_rotationPitchRollYawDegrees, m_finalTransform.m_rotationPitchRollYawDegrees );
	Vec3 newScale = UpdateVec3( m_startingTransform.m_scale, m_finalTransform.m_scale );

	Transform& widgetTransform = m_widgetToAnimate->m_widgetTransform;
	widgetTransform.m_position = newPosition;
	widgetTransform.m_rotationPitchRollYawDegrees = newOrientation;
	widgetTransform.m_scale = newScale;
}

Vec3 WidgetAnimation::UpdateVec3( Vec3 const& startVec3, Vec3 const& endVec3 )
{
	Vec3 newVec3;
	newVec3.x = SmoothFunctionLerpUsingCurrentTime( startVec3.x, endVec3.x );
	newVec3.y = SmoothFunctionLerpUsingCurrentTime( startVec3.y, endVec3.y );
	newVec3.z = SmoothFunctionLerpUsingCurrentTime( startVec3.z, endVec3.z );

	return newVec3;
}

float WidgetAnimation::SmoothFunction( eSmoothingFunction smoothingFunction, float currentTime, float totalTime )
{
	float timeFraction = currentTime/totalTime;

	switch( smoothingFunction )
	{
	case eSmoothingFunction::SMOOTHSTART2: return SmoothStart2( timeFraction );
		break;
	case eSmoothingFunction::SMOOTHSTART3: return SmoothStart3( timeFraction );
		break;
	case eSmoothingFunction::SMOOTHSTOP2: return SmoothStop2( timeFraction );
		break;
	case eSmoothingFunction::SMOOTHSTOP3: return SmoothStop3( timeFraction );
		break;
	case eSmoothingFunction::SMOOTHSTEP3: return SmoothStep3( timeFraction );
		break;
	default: return timeFraction;
		break;
	}
}

float WidgetAnimation::SmoothFunctionLerpUsingCurrentTime( float startValue, float endValue )
{
	float smoothFunctionValue = SmoothFunction( m_smoothingFunction, m_currentTime, m_animationTime );
	float lerpedValue = Interpolate( startValue, endValue, smoothFunctionValue );

	return lerpedValue;
}

