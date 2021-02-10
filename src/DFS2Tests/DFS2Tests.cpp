#include "pch.h"
#include "CppUnitTest.h"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/vec2.hpp"
#include "Game/Entity.hpp"

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

		//Test OBB2 Scale

		//Test Transforming Widget

		//Test RenderWidget
	};
}
