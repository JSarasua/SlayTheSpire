#include "pch.h"
#include "CppUnitTest.h"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/UI/Widget.hpp"
#include "Engine/Math/Transform.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DFS2Tests
{
	TEST_CLASS(DFS2Tests)
	{
	public:
		float epsilonValue = 0.001f;

		TEST_METHOD( DistanceTest1 )
		{
			float distance0To1 = 1.f;
			float distance0To1Test = GetDistance2D( Vec2( 0.f, 0.f ), Vec2( 1.f, 0.f ) );
			Assert::AreEqual( distance0To1Test, distance0To1, 0.001f );
		}

		TEST_METHOD( Vec2DistanceTest2 )
		{
			Vec2 threeFourVec = Vec2( 3.f, 4.f );
			float threeFourLengthTest = threeFourVec.GetLength();
			float threeFourLength = 5.f;

			Assert::AreEqual( threeFourLength, threeFourLengthTest, epsilonValue );
		}

		TEST_METHOD( PointInDiscTest1 )
		{
			Vec2 pointInDisc0Point5 = Vec2( 0.5f, 0.5f );
			Vec2 discCenter0Point75 = Vec2( 0.75f, 0.75f );
			float discRadius0Point3 = 0.3f;
			float discRadius0Point4 = 0.4f;

			Assert::AreEqual( IsPointInsideDisc2D( pointInDisc0Point5, discCenter0Point75, discRadius0Point3 ), false );
			Assert::AreEqual( IsPointInsideDisc2D( pointInDisc0Point5, discCenter0Point75, discRadius0Point4 ), true );
		}

		AABB2 testScreenBounds = AABB2( -5.f, -5.f, 5.f, 5.f );
		AABB2 uiBounds = AABB2( Vec2( -0.5f, -0.5f ), Vec2( 0.5f, 0.5f ) );
		Widget rootWidget = Widget( nullptr, testScreenBounds );
		
		//Test Transforming Widget]
		TEST_METHOD( WidgetRootTransformTestInside )
		{
			bool rootPointInside = rootWidget.IsPointInside( Vec2( -4.9f, -1.f ) );
			Assert::AreEqual( rootPointInside, true );
		}
		TEST_METHOD( WidgetRootTransformTestOutside )
		{
			bool rootPointOutSide = rootWidget.IsPointInside( Vec2( 5.1f, 0.f ) );
			Assert::AreEqual( rootPointOutSide, false );
		}

		TEST_METHOD( WidgetChildTransformTest1 )
		{
			Vec3 scale = Vec3( 1.f, 1.f, 1.f );
			Transform childTransform = Transform();
			childTransform.m_position = testScreenBounds.GetPointAtUV( Vec2( 0.1f, 0.1f ) );
			childTransform.m_scale = scale;
			Widget* childWidget =  new Widget( nullptr, childTransform, nullptr );
			rootWidget.AddChild( childWidget );

			bool childPointInside = childWidget->IsPointInside( Vec2( -4.49f, -4.49f ) );
			bool childPointOutside = childWidget->IsPointInside( Vec2( -3.49f, -3.49f ) );
			Assert::AreEqual( childPointInside, true );
			Assert::AreNotEqual( childPointOutside, true );
		}

		TEST_METHOD( WidgetChildTransformTest2 )
		{
			Vec3 scale = Vec3( 2.f, 1.f, 1.f );
			Transform childTransform = Transform();
			childTransform.m_position = testScreenBounds.GetPointAtUV( Vec2( 0.6f, 0.5f ) );
			childTransform.m_scale = scale;
			Widget* childWidget =  new Widget( nullptr, childTransform, nullptr );
			rootWidget.AddChild( childWidget );

			bool childPointInside = childWidget->IsPointInside( Vec2( 1.5f, 0.45f ) );
			bool childPointOutside = childWidget->IsPointInside( Vec2( 0.f, 0.f ) );
			Assert::AreEqual( childPointInside, true );
			Assert::AreNotEqual( childPointOutside, true );
		}

		TEST_METHOD( WidgetHoveredCheck )
		{
			Vec3 scale = Vec3( 1.f, 1.f, 1.f );
			Transform childTransform = Transform();
			childTransform.m_position = testScreenBounds.GetPointAtUV( Vec2( 0.1f, 0.1f ) );
			childTransform.m_scale = scale;
			Widget* childWidget =  new Widget( nullptr, childTransform, nullptr );
			rootWidget.AddChild( childWidget );


			childWidget->UpdateHovered( Vec2( -4.49f, -4.49f ) );
			bool childIsHovered = childWidget->GetIsHovered();
			Assert::AreEqual( childIsHovered, true );

			childWidget->UpdateHovered( Vec2( -3.49f, -3.49f ) );
			childIsHovered = childWidget->GetIsHovered();
			Assert::AreNotEqual( childIsHovered, true );
		}
	};
}
