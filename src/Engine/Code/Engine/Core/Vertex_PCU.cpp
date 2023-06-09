#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Renderer/BufferAttribute.hpp"


BufferAttribute const Vertex_PCU::LAYOUT[] ={
	BufferAttribute( "POSITION",  BUFFER_FORMAT_VEC3,      		offsetof( Vertex_PCU, position ) ),
	BufferAttribute( "COLOR",     BUFFER_FORMAT_R8G8B8A8_UNORM, 	offsetof( Vertex_PCU, tint ) ),
	BufferAttribute( "TEXCOORD",        BUFFER_FORMAT_VEC2,      		offsetof( Vertex_PCU, uvTexCoords ) ),
	BufferAttribute() // end - terminator element; 
};

void Vertex_PCU::TransformVertexArray( int NUM_SHIP_VERTS, Vertex_PCU* shipVerts, float scale, float orientationDegrees, Vec2 position )
{
	for( int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; vertIndex++ )
	{
		shipVerts[vertIndex].position =  TransformPosition3DXY( shipVerts[vertIndex].position, scale, orientationDegrees, position );
	}

}

void Vertex_PCU::TransformVertexArray( std::vector<Vertex_PCU*>& shipVerts, float scale, float orientationDegrees, Vec2 position )
{
	TransformVertexArray( static_cast<int>(shipVerts.size()), shipVerts[0], scale, orientationDegrees, position );
}

void Vertex_PCU::AppendVerts( std::vector<Vertex_PCU>& masterVertexList, std::vector<Vertex_PCU>& vertexesToAppend )
{
	for( int vertexIndex = 0; vertexIndex < vertexesToAppend.size(); vertexIndex++ )
	{
		masterVertexList.push_back( vertexesToAppend[vertexIndex] );
	}
}



void Vertex_PCU::AppendVertsCapsule2D( std::vector<Vertex_PCU>& masterVertexList, const Capsule2& capsule, const Rgba8& color )
{
	Vec2 center = capsule.GetCenter();
	Vec2 fullDimensions = Vec2( capsule.GetBoneLength(), capsule.radius*2.f );
	float orientationDegrees = (capsule.endPosition - capsule.startPosition).GetAngleDegrees();
	//TODO CHANGE
	OBB2 innerBox = OBB2( center, fullDimensions, orientationDegrees );

	Vertex_PCU::AppendVertsOBB2D( masterVertexList, innerBox, color );

	//Add semicircle at end of capsule
	float orientationMinus90Degrees = orientationDegrees - 90.f;
	float increment = 180.f/32.f;
	float currentOrientation = 0.f;
	for( currentOrientation = orientationMinus90Degrees; currentOrientation < orientationMinus90Degrees + 180.f; currentOrientation += increment )
	{
		Vec2 firstVertex = Vec2::MakeFromPolarDegrees( currentOrientation, capsule.radius ) + capsule.endPosition;
		Vec2 secondVertex = Vec2::MakeFromPolarDegrees( currentOrientation+increment, capsule.radius ) + capsule.endPosition;

		masterVertexList.push_back( Vertex_PCU( Vec3( capsule.endPosition ), color, Vec2( 0.f, 0.f ) ) );
		masterVertexList.push_back( Vertex_PCU( Vec3( firstVertex ), color, Vec2( 0.f, 0.f ) ) );
		masterVertexList.push_back( Vertex_PCU( Vec3( secondVertex ), color, Vec2( 0.f, 0.f ) ) );
	}

	//Add semicircle at start of capsule
	for( currentOrientation; currentOrientation < orientationMinus90Degrees + 360.f; currentOrientation += increment )
	{
		Vec2 firstVertex = Vec2::MakeFromPolarDegrees( currentOrientation, capsule.radius ) + capsule.startPosition;
		Vec2 secondVertex = Vec2::MakeFromPolarDegrees( currentOrientation+increment, capsule.radius ) + capsule.startPosition;

		masterVertexList.push_back( Vertex_PCU( Vec3( capsule.startPosition ), color, Vec2( 0.f, 0.f ) ) );
		masterVertexList.push_back( Vertex_PCU( Vec3( firstVertex ), color, Vec2( 0.f, 0.f ) ) );
		masterVertexList.push_back( Vertex_PCU( Vec3( secondVertex ), color, Vec2( 0.f, 0.f ) ) );
	}
}


void Vertex_PCU::AppendVertsAABB2D( std::vector<Vertex_PCU>& masterVertexList, const AABB2& aabb, const Rgba8& color, const AABB2& uvs /*= AABB2() */ )
{
	masterVertexList.push_back( Vertex_PCU( Vec3( aabb.mins.x, aabb.mins.y, 0.f ), color, uvs.mins ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( aabb.maxs.x, aabb.mins.y, 0.f ), color, Vec2( uvs.maxs.x, uvs.mins.y ) ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( aabb.maxs.x, aabb.maxs.y, 0.f ), color, uvs.maxs ) );

	masterVertexList.push_back( Vertex_PCU( Vec3( aabb.mins.x, aabb.mins.y, 0.f ), color, uvs.mins ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( aabb.maxs.x, aabb.maxs.y, 0.f ), color, uvs.maxs ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( aabb.mins.x, aabb.maxs.y, 0.f ), color, Vec2( uvs.mins.x, uvs.maxs.y ) ) );
}



void Vertex_PCU::AppendIndexedVertsAABB2D( std::vector<Vertex_PCU>& masterVertexList, std::vector<uint>& masterIndexList, const AABB2& aabb, const Rgba8& color, const AABB2& uvs /*= AABB2() */ )
{
	uint currentIndex = (uint)masterIndexList.size();

	masterVertexList.push_back( Vertex_PCU( Vec3( aabb.mins.x, aabb.mins.y, 0.f ), color, uvs.mins ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( aabb.maxs.x, aabb.mins.y, 0.f ), color, Vec2( uvs.maxs.x, uvs.mins.y ) ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( aabb.maxs.x, aabb.maxs.y, 0.f ), color, uvs.maxs ) );

	masterVertexList.push_back( Vertex_PCU( Vec3( aabb.mins.x, aabb.mins.y, 0.f ), color, uvs.mins ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( aabb.maxs.x, aabb.maxs.y, 0.f ), color, uvs.maxs ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( aabb.mins.x, aabb.maxs.y, 0.f ), color, Vec2( uvs.mins.x, uvs.maxs.y ) ) );


	masterIndexList.push_back( currentIndex );
	masterIndexList.push_back( currentIndex + 1 );
	masterIndexList.push_back( currentIndex + 2 );

	masterIndexList.push_back( currentIndex + 3 );
	masterIndexList.push_back( currentIndex + 4 );
	masterIndexList.push_back( currentIndex + 5 );
}

void Vertex_PCU::AppendVertsAABB2DBorder( std::vector<Vertex_PCU>& masterVertexList, AABB2 const& aabb, float thickness, Rgba8 const& color )
{
	Vec2 bLeft = aabb.mins;
	Vec2 bRight = Vec2( aabb.maxs.x, aabb.mins.y );
	Vec2 tLeft = Vec2( aabb.mins.x, aabb.maxs.y );
	Vec2 tRight = aabb.maxs;
	LineSegment2 bottomLine( bLeft, bRight );
	LineSegment2 topLine( tLeft, tRight );
	LineSegment2 leftLine( bLeft, tLeft );
	LineSegment2 rightLine( bRight, tRight );

	AppendVertsLine2D( masterVertexList, bottomLine, thickness, color );
	AppendVertsLine2D( masterVertexList, topLine, thickness, color );
	AppendVertsLine2D( masterVertexList, leftLine, thickness, color );
	AppendVertsLine2D( masterVertexList, rightLine, thickness, color );
}

void Vertex_PCU::AppendVertsOBB2D( std::vector<Vertex_PCU>& masterVertexList, const OBB2& obb, const Rgba8& color, const AABB2& uvs /*= AABB2() */ )
{
	Vec2* obbCornerPositions = new Vec2[4];
	obb.GetCornerPositions( obbCornerPositions );

	masterVertexList.push_back( Vertex_PCU( Vec3( obbCornerPositions[0] ), color, uvs.mins ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( obbCornerPositions[1] ), color, Vec2( uvs.maxs.x, uvs.mins.y ) ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( obbCornerPositions[2] ), color, uvs.maxs ) );

	masterVertexList.push_back( Vertex_PCU( Vec3( obbCornerPositions[0] ), color, uvs.mins ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( obbCornerPositions[2] ), color, uvs.maxs ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( obbCornerPositions[3] ), color, Vec2( uvs.mins.x, uvs.maxs.y ) ) );
}


void Vertex_PCU::AppendVertsLine2D( std::vector<Vertex_PCU>& masterVertexList, const LineSegment2& line, float thickness, const Rgba8& color )
{
	Vec2 normalizedDisplacement = (line.endPosition - line.startPosition).GetNormalized();
	float radius = thickness * 0.5f;
	Vec2 fwd = normalizedDisplacement * radius;
	Vec2 leftVec = fwd.GetRotated90Degrees();
	Vec2 endLeft = line.endPosition + fwd + leftVec;
	Vec2 endRight= line.endPosition + fwd - leftVec;
	Vec2 startLeft = line.startPosition - fwd + leftVec;
	Vec2 startRight = line.startPosition - fwd - leftVec;

	masterVertexList.push_back( Vertex_PCU( Vec3( startRight ), color, Vec2( 0.f, 0.f ) ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( endRight ), color, Vec2( 0.f, 0.f ) ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( endLeft ), color, Vec2( 0.f, 0.f ) ) );

	masterVertexList.push_back( Vertex_PCU( Vec3( startLeft ), color, Vec2( 0.f, 0.f ) ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( startRight ), color, Vec2( 0.f, 0.f ) ) );
	masterVertexList.push_back( Vertex_PCU( Vec3( endLeft ), color, Vec2( 0.f, 0.f ) ) );
}


void Vertex_PCU::AppendVertsDisc2D( std::vector<Vertex_PCU>& masterVertexList, const Vec2& discCenter, const float discRadius, const Rgba8& color )
{
	//float orientationMinus90Degrees = orientationDegrees - 90.f;
	float increment = 180.f/32.f;
	for( float currentOrientation = 0.f; currentOrientation < 360.f; currentOrientation += increment )
	{
		Vec2 firstVertex = Vec2::MakeFromPolarDegrees( currentOrientation, discRadius ) + discCenter;
		Vec2 secondVertex = Vec2::MakeFromPolarDegrees( currentOrientation+increment, discRadius ) + discCenter;

		masterVertexList.push_back( Vertex_PCU( Vec3( discCenter ), color, Vec2( 0.f, 0.f ) ) );
		masterVertexList.push_back( Vertex_PCU( Vec3( firstVertex ), color, Vec2( 0.f, 0.f ) ) );
		masterVertexList.push_back( Vertex_PCU( Vec3( secondVertex ), color, Vec2( 0.f, 0.f ) ) );
	}
}

void Vertex_PCU::AppendVertsCube( std::vector<Vertex_PCU>& masterVertexList, float cubeHalfHeight )
{
	float c = cubeHalfHeight;

	Vec2 bLeft(0.f, 0.f);
	Vec2 tRight(1.f, 1.f);
	Vec2 tLeft(0.f, 1.f);
	Vec2 bRight(1.f, 0.f);


	Vertex_PCU cubeVerts[36] =
	{
		//Front Quad
		Vertex_PCU(Vec3( -c, -c, c ), Rgba8::WHITE,		bLeft),
		Vertex_PCU(Vec3( c, -c, c ), Rgba8::WHITE,		bRight),
		Vertex_PCU(Vec3( c, c, c ), Rgba8::WHITE,		tRight),

		Vertex_PCU(Vec3( -c, -c, c ), Rgba8::WHITE,		bLeft),
		Vertex_PCU(Vec3( c, c, c ), Rgba8::WHITE,		tRight),
		Vertex_PCU(Vec3( -c, c, c ), Rgba8::WHITE,		tLeft),

		//Right Quad
		Vertex_PCU( Vec3( c, -c, c ), Rgba8::WHITE,		bLeft ),
		Vertex_PCU( Vec3( c, -c, -c ), Rgba8::WHITE,	bRight ),
		Vertex_PCU( Vec3( c, c, -c ), Rgba8::WHITE,		tRight ),

		Vertex_PCU( Vec3( c, -c, c ), Rgba8::WHITE,		bLeft ),
		Vertex_PCU( Vec3( c, c, -c ), Rgba8::WHITE,		tRight ),
		Vertex_PCU( Vec3( c, c, c ), Rgba8::WHITE,		tLeft ),

		//Back Quad
		Vertex_PCU( Vec3( c, -c, -c ), Rgba8::WHITE,	bLeft ),
		Vertex_PCU( Vec3( -c, -c, -c ), Rgba8::WHITE,	bRight ),
		Vertex_PCU( Vec3( -c, c, -c ), Rgba8::WHITE,	tRight ),

		Vertex_PCU( Vec3( c, -c, -c ), Rgba8::WHITE,	bLeft ),
		Vertex_PCU( Vec3( -c, c, -c ), Rgba8::WHITE,	tRight ),
		Vertex_PCU( Vec3( c, c, -c ), Rgba8::WHITE,		tLeft ),

		//Left Quad
		Vertex_PCU( Vec3( -c, -c, -c ), Rgba8::WHITE,	bLeft ),
		Vertex_PCU( Vec3( -c, -c, c ), Rgba8::WHITE,	bRight ),
		Vertex_PCU( Vec3( -c, c, c ), Rgba8::WHITE,	tRight ),

		Vertex_PCU( Vec3( -c, -c, -c ), Rgba8::WHITE,	bLeft ),
		Vertex_PCU( Vec3( -c, c, c ), Rgba8::WHITE,		tRight ),
		Vertex_PCU( Vec3( -c, c, -c ), Rgba8::WHITE,	tLeft ),

		//Top Quad
		Vertex_PCU( Vec3( -c, c, c ), Rgba8::WHITE,		bLeft ),
		Vertex_PCU( Vec3( c, c, c ), Rgba8::WHITE,		bRight ),
		Vertex_PCU( Vec3( c, c, -c ), Rgba8::WHITE,		tRight ),

		Vertex_PCU( Vec3( -c, c, c ), Rgba8::WHITE,		bLeft ),
		Vertex_PCU( Vec3( c, c, -c ), Rgba8::WHITE,		tRight ),
		Vertex_PCU( Vec3( -c, c, -c ), Rgba8::WHITE,	tLeft ),

		//Bottom Quad
		Vertex_PCU( Vec3( -c, -c, -c ), Rgba8::WHITE,	bLeft ),
		Vertex_PCU( Vec3( c, -c, -c ), Rgba8::WHITE,	bRight ),
		Vertex_PCU( Vec3( c, -c, c ), Rgba8::WHITE,		tRight ),

		Vertex_PCU( Vec3( -c, -c, -c ), Rgba8::WHITE,	bLeft ),
		Vertex_PCU( Vec3( c, -c, c ), Rgba8::WHITE,		tRight ),
		Vertex_PCU( Vec3( -c, -c, c ), Rgba8::WHITE,	tLeft )
	};

	for( int vertexIndex = 0; vertexIndex < 36; vertexIndex++ )
	{
		masterVertexList.push_back( cubeVerts[vertexIndex] );
	}

}

void Vertex_PCU::AppendIndexedVertsCube( std::vector<Vertex_PCU>& masterVertexList, std::vector<uint>& masterIndexList, float cubeHalfHeight /*= 1.f */ )
{

	float c = cubeHalfHeight;

	Vec2 bLeft( 0.f, 0.f );
	Vec2 tRight( 1.f, 1.f );
	Vec2 tLeft( 0.f, 1.f );
	Vec2 bRight( 1.f, 0.f );

	Vertex_PCU cubeVerts[24] =
	{
		//Front Quad
		Vertex_PCU( Vec3( -c, -c, c ), Rgba8::WHITE,		bLeft ),	//0
		Vertex_PCU( Vec3( c, -c, c ), Rgba8::WHITE,		bRight ),		//1
		Vertex_PCU( Vec3( c, c, c ), Rgba8::WHITE,		tRight ),		//2

		//Vertex_PCU( Vec3( -c, -c, c ), Rgba8::WHITE,		bLeft ),	//0
		//Vertex_PCU( Vec3( c, c, c ), Rgba8::WHITE,		tRight ),	//2
		Vertex_PCU( Vec3( -c, c, c ), Rgba8::WHITE,		tLeft ),		//3

		//Right Quad
		Vertex_PCU( Vec3( c, -c, c ), Rgba8::WHITE,		bLeft ),		//4
		Vertex_PCU( Vec3( c, -c, -c ), Rgba8::WHITE,	bRight ),		//5
		Vertex_PCU( Vec3( c, c, -c ), Rgba8::WHITE,		tRight ),		//6

		//Vertex_PCU( Vec3( c, -c, c ), Rgba8::WHITE,		bLeft ),	//4
		//Vertex_PCU( Vec3( c, c, -c ), Rgba8::WHITE,		tRight ),	//6
		Vertex_PCU( Vec3( c, c, c ), Rgba8::WHITE,		tLeft ),		//7

		//Back Quad
		Vertex_PCU( Vec3( c, -c, -c ), Rgba8::WHITE,	bLeft ),		//8
		Vertex_PCU( Vec3( -c, -c, -c ), Rgba8::WHITE,	bRight ),		//9
		Vertex_PCU( Vec3( -c, c, -c ), Rgba8::WHITE,	tRight ),		//10

		//Vertex_PCU( Vec3( c, -c, -c ), Rgba8::WHITE,	bLeft ),		//8
		//Vertex_PCU( Vec3( -c, c, -c ), Rgba8::WHITE,	tRight ),		//10
		Vertex_PCU( Vec3( c, c, -c ), Rgba8::WHITE,		tLeft ),		//11

		//Left Quad
		Vertex_PCU( Vec3( -c, -c, -c ), Rgba8::WHITE,	bLeft ),		//12
		Vertex_PCU( Vec3( -c, -c, c ), Rgba8::WHITE,	bRight ),		//13
		Vertex_PCU( Vec3( -c, c, c ), Rgba8::WHITE,	tRight ),			//14

		//Vertex_PCU( Vec3( -c, -c, -c ), Rgba8::WHITE,	bLeft ),		//12
		//Vertex_PCU( Vec3( -c, c, c ), Rgba8::WHITE,		tRight ),	//14
		Vertex_PCU( Vec3( -c, c, -c ), Rgba8::WHITE,	tLeft ),		//15

		//Top Quad
		Vertex_PCU( Vec3( -c, c, c ), Rgba8::WHITE,		bLeft ),		//16
		Vertex_PCU( Vec3( c, c, c ), Rgba8::WHITE,		bRight ),		//17
		Vertex_PCU( Vec3( c, c, -c ), Rgba8::WHITE,		tRight ),		//18

		//Vertex_PCU( Vec3( -c, c, c ), Rgba8::WHITE,		bLeft ),	//16
		//Vertex_PCU( Vec3( c, c, -c ), Rgba8::WHITE,		tRight ),	//18
		Vertex_PCU( Vec3( -c, c, -c ), Rgba8::WHITE,	tLeft ),		//19

		//Bottom Quad
		Vertex_PCU( Vec3( -c, -c, -c ), Rgba8::WHITE,	bLeft ),		//20
		Vertex_PCU( Vec3( c, -c, -c ), Rgba8::WHITE,	bRight ),		//21
		Vertex_PCU( Vec3( c, -c, c ), Rgba8::WHITE,		tRight ),		//22

		//Vertex_PCU( Vec3( -c, -c, -c ), Rgba8::WHITE,	bLeft ),		//20
		//Vertex_PCU( Vec3( c, -c, c ), Rgba8::WHITE,		tRight ),	//22
		Vertex_PCU( Vec3( -c, -c, c ), Rgba8::WHITE,	tLeft )			//23
	};

	for( int vertexIndex = 0; vertexIndex < 24; vertexIndex++ )
	{
		masterVertexList.push_back( cubeVerts[vertexIndex] );
	}

	uint cubeIndices[36] =
	{
		//Front Quad
		0,
		1,
		2,
		 
		0,
		2,
		3,
		 
		//Right Quad
		4,
		5,
		6,
		 
		4,
		6,
		7,
		 
		//Back Quad
		8,
		9,
		10,
		  
		8 ,
		10,
		11,
		  
		//Left Quad
		12,
		13,
		14,
		  
		12,
		14,
		15,
		  
		//Top Quad
		16,
		17,
		18,
		  
		16,
		18,
		19,
		  
		//Bottom Quad
		20,
		21,
		22,
		  
		20,
		22,
		23
	};

	for( int indicesIndex = 0; indicesIndex < 36; indicesIndex++ )
	{
		masterIndexList.push_back( cubeIndices[indicesIndex] );
	}
}



void Vertex_PCU::AppendIndexedVertsSphere( std::vector<Vertex_PCU>& masterVertexList, std::vector<uint>& masterIndexList, float sphereRadius /*= 1.f */ )
{
	//first point is top
	//last point is bottom
	//rotating counter clockwise
	//top piece is triangles
	//Afterwards is quads


	int numOfVerticalSlices = 64;		//like an orange slice, change based on theta
	int numOfHorizontalSlices = 64;		//like latitude lines, change based on phi
	float thetaIncrements = 360.f / (float)numOfVerticalSlices;
	float phiIncrements = 180.f / (float)numOfHorizontalSlices;

	Vec3 topPoint = Vec3::MakeFromSphericalDegrees( 0.f, 90.f, 1.f );
	Vec2 topUV = Vec2( .5f, 1.f );
	Vec3 bottomPoint = Vec3::MakeFromSphericalDegrees( 0.f, -90.f, 1.f );
	Vec2 bottomUV = Vec2( 0.5f, 0.f );

	std::vector<Vec3> vertexList;
	std::vector<Vec2> uvList;
	vertexList.push_back( topPoint );
	uvList.push_back( topUV );

	for( float latitudeDegrees = 90.f - phiIncrements; latitudeDegrees > -90.f; latitudeDegrees -= phiIncrements )
	{

		for( float longitudeDegrees = 360.f; longitudeDegrees >= 0.f; longitudeDegrees -= thetaIncrements )
		{
			Vec3 currentPoint = Vec3::MakeFromSphericalDegrees( longitudeDegrees, latitudeDegrees, sphereRadius );
			vertexList.push_back( currentPoint );

 			float u = RangeMap( 360.f, 0.f, 0.f, 1.f, longitudeDegrees );
 			float v = RangeMap( -90.f, 90.f, 0.f, 1.f, latitudeDegrees );
			uvList.push_back( Vec2( u, v ) );
		}
	}
	vertexList.push_back( bottomPoint );
	uvList.push_back( bottomUV );


	std::vector<uint> indicesList;
	//Top of sphere
	for( int topIndices = 1; topIndices < numOfVerticalSlices + 1; topIndices++ )
	{
			indicesList.push_back( 0 );
			indicesList.push_back( topIndices );
			indicesList.push_back( topIndices + 1 );
	}

	//Body of Sphere
	//reduce by 2 the number of vertical slices for the top and bottom of the sphere
	for( int latitudeIndex = 0; latitudeIndex < numOfHorizontalSlices - 2; latitudeIndex++ )
	{
		for( int longitudeIndex = 0; longitudeIndex < numOfVerticalSlices; longitudeIndex++ )
		{

			int topLeft = latitudeIndex * (numOfVerticalSlices + 1) + longitudeIndex + 1;
			int topRight = topLeft + 1;
			int bottomLeft = (latitudeIndex + 1) * (numOfVerticalSlices + 1) + longitudeIndex + 1;
			int bottomRight = bottomLeft + 1;

			//left quad triangle
			indicesList.push_back( topLeft );
			indicesList.push_back( bottomLeft );
			indicesList.push_back( topRight );

			//right quad triangle
			indicesList.push_back( topRight );
			indicesList.push_back( bottomLeft );
			indicesList.push_back( bottomRight );
		}
	}

	//Bottom of Sphere
	for( uint bottomIndices = ((uint)vertexList.size() - 1) - numOfVerticalSlices; bottomIndices < ((uint)vertexList.size() - 1); bottomIndices++ )
	{
		if( bottomIndices == ((uint)vertexList.size() - 2) )
		{
			indicesList.push_back( bottomIndices );
			indicesList.push_back( (uint)vertexList.size() - 1 );
			indicesList.push_back( bottomIndices - (numOfVerticalSlices - 1) );
		}
		else
		{
			indicesList.push_back( bottomIndices );
			indicesList.push_back( (uint)vertexList.size() - 1 );
			indicesList.push_back( bottomIndices + 1 );
		}

	}

	for( size_t vertexIndex = 0; vertexIndex < vertexList.size(); vertexIndex++ )
	{
		masterVertexList.push_back( Vertex_PCU( vertexList[vertexIndex], Rgba8::WHITE, uvList[vertexIndex] ) );
	}
 //	masterVertexList.insert( masterVertexList.end(), vertexList.begin(), vertexList.end() );
 	masterIndexList.insert( std::end(masterIndexList), std::begin(indicesList), std::end(indicesList) );
	
}

void Vertex_PCU::AppendVerts4Points( std::vector<Vertex_PCU>& masterVertexList, std::vector<uint>& masterIndexList, Vec3 const& p0, Vec3 const& p1, Vec3 const& p2, Vec3 const& p3, Rgba8 const& color, AABB2 const& uvs /*= AABB2() */ )
{
	uint currentIndex = (uint)masterIndexList.size();

	masterVertexList.push_back( Vertex_PCU( p0, color, uvs.mins ) );
	masterVertexList.push_back( Vertex_PCU( p1, color, Vec2( uvs.maxs.x, uvs.mins.y ) ) );
	masterVertexList.push_back( Vertex_PCU( p2, color, uvs.maxs ) );

	masterVertexList.push_back( Vertex_PCU( p0, color, uvs.mins ) );
	masterVertexList.push_back( Vertex_PCU( p2, color, uvs.maxs ) );
	masterVertexList.push_back( Vertex_PCU( p3, color, Vec2( uvs.mins.x, uvs.maxs.y ) ) );


	masterIndexList.push_back( currentIndex );
	masterIndexList.push_back( currentIndex + 1 );
	masterIndexList.push_back( currentIndex + 2 );

	masterIndexList.push_back( currentIndex + 3 );
	masterIndexList.push_back( currentIndex + 4 );
	masterIndexList.push_back( currentIndex + 5);
}

Vertex_PCU::Vertex_PCU( const Vertex_PCU& copy )
	: position( copy.position )
	, tint( copy.tint )
	, uvTexCoords( copy.uvTexCoords )
{
}


Vertex_PCU::Vertex_PCU( const Vec3& initialPosition, const Rgba8& initialTint, const Vec2& initialuvTexCoords )
	: position( initialPosition )
	, tint( initialTint )
	, uvTexCoords( initialuvTexCoords )
{
}
