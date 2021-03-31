#pragma once
#include "Engine/UI/Widget.hpp"
#include "Engine/Math/Transform.hpp"

enum class eSmoothingFunction
{
	SMOOTHSTART2,
	SMOOTHSTART3,
	SMOOTHSTOP2,
	SMOOTHSTOP3,
	SMOOTHSTEP3
};

class WidgetAnimation
{
public:
	WidgetAnimation( Widget* widgetToAnimate, Transform const& finalTransform, float animationTime, eSmoothingFunction smoothingFunction );
	~WidgetAnimation();

	void SetStartingTransform( Transform const& startingTransform );
	void Update( float deltaSeconds );
	void UpdateWidgetTransform();
	Vec3 UpdateVec3( Vec3 const& startVec3, Vec3 const& endVec3 );
	float SmoothFunction( eSmoothingFunction smoothingFunction, float currentTime, float totalTime );
	float SmoothFunctionLerpUsingCurrentTime( float startValue, float endValue);
public:
	Widget* m_widgetToAnimate = nullptr;
	Transform m_startingTransform;
	Transform m_finalTransform;
	float m_currentTime = 0.f;
	float m_animationTime = 0.f;
	eSmoothingFunction m_smoothingFunction = eSmoothingFunction::SMOOTHSTART2;

	Delegate<EventArgs const&> m_endAnimationDelegate;
};