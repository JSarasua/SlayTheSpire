#include "pch.h"
#include "CppUnitTest.h"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Entity.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DFS2Tests
{
	TEST_CLASS(DFS2Tests)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			float distance0To1 = 1.f;
			float distance0To1Test = GetDistance2D( Vec2( 0.f, 0.f ), Vec2( 1.f, 0.f ) );
			bool isAlmostEqual0To1Test = AlmostEqualsFloat( distance0To1Test, distance0To1 );
			Assert::AreEqual( isAlmostEqual0To1Test, true );

		}
	};
}
