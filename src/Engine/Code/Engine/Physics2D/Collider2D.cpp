#include "Engine/Physics2D/Collider2D.hpp"
#include "Engine/Physics2D/DiscCollider2D.hpp"
#include "Engine/Physics2D/PolygonCollider2D.hpp"
#include "Engine/Physics2D/Rigidbody2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Polygon2D.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Math/LineSegment3.hpp"

typedef bool (*collision_check_cb)( Collider2D const*, Collider2D const* );
typedef bool (*manifold_cb)( Collider2D const*, Collider2D const*, Manifold2D* );

static bool DiscVDiscCollisionCheck( Collider2D const* col0, Collider2D const* col1 );
static bool DiscVPolygonCollisionCheck( Collider2D const* col0, Collider2D const* col1 );
static bool PolygonVPolygonCollisionCheck( Collider2D const* col0, Collider2D const* col1 );
static bool DiscVDiscManifold( Collider2D const* col0, Collider2D const* col1, Manifold2D* manifold );
static bool DiscVPolygonManifold( Collider2D const* col0, Collider2D const* col1, Manifold2D* manifold );
static bool PolygonVPolygonManifold( Collider2D const* col0, Collider2D const* col1, Manifold2D* manifold );

static collision_check_cb gCollisionChecks[NUM_COLLIDER_TYPE * NUM_COLLIDER_TYPE] ={
	/*             disc,                         polygon, */
	/*    disc */  DiscVDiscCollisionCheck,      nullptr,
	/* polygon */  DiscVPolygonCollisionCheck,   PolygonVPolygonCollisionCheck
};

static manifold_cb gManifoldChecks[NUM_COLLIDER_TYPE * NUM_COLLIDER_TYPE] ={
	/*             disc,                         polygon, */
	/*    disc */  DiscVDiscManifold,      nullptr,
	/* polygon */  DiscVPolygonManifold,   PolygonVPolygonManifold
};

bool Collider2D::GetManifold( Collider2D const* other, Manifold2D* manifold ) const
{
	eCollider2DType myType = m_type;
	eCollider2DType otherType = other->m_type;

	AABB2 const& myBounds = GetBounds();
	AABB2 const& theirBounds = other->GetBounds();
	if( !DoAABBsOverlap2D( myBounds, theirBounds ) )
	{
		return false;
	}

	if( myType <= otherType ) {
		int idx = otherType * NUM_COLLIDER_TYPE + myType;
		manifold_cb check = gManifoldChecks[idx];
		return check( this, other, manifold );
	}
	else {
		// flip the types when looking into the index.
		int idx = myType * NUM_COLLIDER_TYPE + otherType;
		manifold_cb check = gManifoldChecks[idx];
		return check( other, this, manifold );
	}
}

bool Collider2D::Intersects( Collider2D const* other ) const
{
	eCollider2DType myType = m_type;
	eCollider2DType otherType = other->m_type;

	AABB2 myBounds = GetBounds();
	AABB2 theirBounds = other->GetBounds();
	if( !DoAABBsOverlap2D( myBounds, theirBounds ) )
	{
		return false;
	}

	if( myType <= otherType ) {
		int idx = otherType * NUM_COLLIDER_TYPE + myType;
		collision_check_cb check = gCollisionChecks[idx];
		return check( this, other );
	}
	else {
		// flip the types when looking into the index.
		int idx = myType * NUM_COLLIDER_TYPE + otherType;
		collision_check_cb check = gCollisionChecks[idx];
		return check( other, this );
	}
}

float Collider2D::GetMass() const
{
	if( nullptr == m_rigidbody )
	{
		return 1.f;
	}
	else
	{
		return m_rigidbody->GetMass();
	}
}

eSimulationMode Collider2D::GetSimulationMode() const
{
	return m_rigidbody->GetSimulationMode();
}

PhysicsMaterial Collider2D::GetPhysicsMaterial() const
{
	return m_physicsMaterial;
}

float Collider2D::GetFriction() const
{
	return m_physicsMaterial.m_friction;
}

float Collider2D::GetResitution() const
{
	return m_physicsMaterial.m_restitution;
}

float Collider2D::GetBounceWith( Collider2D const* other ) const
{
	float myRestitution = m_physicsMaterial.m_restitution;
	float theirResitution = other->m_physicsMaterial.m_restitution;

	//Calculating Restitution by the produce. May want to change to allow for Min, Max, etc.
	return myRestitution * theirResitution;
}

float Collider2D::GetFrictionWith( Collider2D const* other ) const
{
	float myfriction = m_physicsMaterial.m_friction;
	float theirFriction = other->m_physicsMaterial.m_friction;

	return myfriction * theirFriction;
}

void Collider2D::SetRestitution( float newRestitution )
{
	float restitution = Clampf( newRestitution, 0.f, 1.f );
	m_physicsMaterial.m_restitution = restitution;
}

void Collider2D::SetFriction( float newFriction )
{
	m_physicsMaterial.m_friction = newFriction;
}

Collider2D::Collider2D( float restitution /*= 1.f */ )
{
	m_physicsMaterial.m_restitution = restitution;
}

AABB2 Collider2D::GetBounds() const
{
	return m_bounds;
}

void Collider2D::Move( Vec2 const& translator )
{
	if( nullptr == m_rigidbody )
	{
		return;
	}

	m_rigidbody->Translate( translator );
}

Collider2D::~Collider2D()
{
	
}

static bool DiscVDiscCollisionCheck( Collider2D const* col0, Collider2D const* col1 )
{
	DiscCollider2D const* disc0 = (DiscCollider2D const*)col0;
	DiscCollider2D const* disc1 = (DiscCollider2D const*)col1;

	Vec2 const& disc0Center = disc0->m_worldPosition + disc0->m_localPosition;
	float disc0Radius = disc0->m_radius;
	Vec2 const& disc1Center = disc1->m_worldPosition + disc1->m_localPosition;
	float disc1Radius = disc1->m_radius;

	bool isColliding = DoDiscsOverlap( disc0Center, disc0Radius, disc1Center, disc1Radius );

	return isColliding;
}

static bool DiscVPolygonCollisionCheck( Collider2D const* col0, Collider2D const* col1 )
{
	DiscCollider2D const* disc = (DiscCollider2D const*)col0;
	PolygonCollider2D const* poly = (PolygonCollider2D const*)col1;

	Vec2 const& discCenter = disc->m_worldPosition + disc->m_localPosition;
	float discRadius = disc->m_radius;
	Polygon2D const& polygon = poly->m_polygon;

	if( DoDiscAndAABBOverlap2D( discCenter, discRadius, poly->GetBounds() ) )
	{
		bool isColliding = DoPolygonAndDiscOverlap2D( polygon, discCenter, discRadius );
		return isColliding;
	}
	else
	{
		return false;
	}

}


static bool PolygonVPolygonCollisionCheck( Collider2D const* col0, Collider2D const* col1 )
{
	Vec2 origin = Vec2( 0.f, 0.f );

	PolygonCollider2D const* polyCol0 = (PolygonCollider2D const*)col0;
	PolygonCollider2D const* polyCol1 = (PolygonCollider2D const*)col1;

	Polygon2D const& poly0 = polyCol0->GetPolygon();
	Polygon2D const& poly1 = polyCol1->GetPolygon();


	//Create First Simplex
	Polygon2D simplex;
	Polygon2D::CreateInitialGJKSimplex( poly0, poly1,  &simplex );

	if( simplex.Contains( origin ) )
	{
		return true;
	}

	while( Polygon2D::EvolveGJK( poly0, poly1, &simplex ) )
	{
		if( simplex.Contains( origin ) )
		{
			return true;
		}
	}

	return false;
}


bool DiscVDiscManifold( Collider2D const* col0, Collider2D const* col1, Manifold2D* manifold )
{
	DiscCollider2D const* disc0 = (DiscCollider2D const*)col0;
	DiscCollider2D const* disc1 = (DiscCollider2D const*)col1;

	Vec2 const& disc0Center = disc0->m_worldPosition + disc0->m_localPosition;
	float disc0Radius = disc0->m_radius;
	Vec2 const& disc1Center = disc1->m_worldPosition + disc1->m_localPosition;
	float disc1Radius = disc1->m_radius;

	Vec2 normal = disc0Center - disc1Center;
	float penetration = (disc0Radius + disc1Radius) - GetDistance2D(disc0Center, disc1Center);

	if( penetration < 0.f )
	{
		return false;
	}
	
	normal.Normalize();
	Vec2 contact = disc0Center - normal*(disc0Radius - 0.5f*penetration);

	manifold->normal = normal;
	manifold->penetration = penetration;
	manifold->contactEdge = LineSegment2( contact, contact );

	return true;
}
bool DiscVPolygonManifold( Collider2D const* col0, Collider2D const* col1, Manifold2D* manifold )
{
	DiscCollider2D const* disc = (DiscCollider2D const*)col0;
	PolygonCollider2D const* poly = (PolygonCollider2D const*)col1;

	Vec2 const& discCenter = disc->m_worldPosition + disc->m_localPosition;
	float discRadius = disc->m_radius;
	Polygon2D const& polygon = poly->m_polygon;

	Vec2 closestPoint = polygon.GetClosestPoint( discCenter );
	Vec2 normal = discCenter - closestPoint;
	normal.Normalize();
	float distanceToCollision = GetDistance2D(discCenter, closestPoint);
	float penetration = discRadius - distanceToCollision;

	if( penetration < 0.f )
	{
		return false;
	}

	if( polygon.Contains( discCenter ) )
	{
		normal *= -1.f;
		penetration = discRadius + distanceToCollision;
	}
	Vec2 contactPoint = discCenter - normal*( discRadius - 0.5f*penetration );


	manifold->contactEdge = LineSegment2( contactPoint, contactPoint );
	manifold->normal = normal;
	manifold->penetration = penetration;

	return true;
	//TODO if on edge of polygon
}
bool PolygonVPolygonManifold( Collider2D const* col0, Collider2D const* col1, Manifold2D* manifold )
{
	Vec2 origin = Vec2( 0.f, 0.f );

	PolygonCollider2D const* polyCol0 = (PolygonCollider2D const*)col0;
	PolygonCollider2D const* polyCol1 = (PolygonCollider2D const*)col1;

	Polygon2D const& poly0 = polyCol0->GetPolygon();
	Polygon2D const& poly1 = polyCol1->GetPolygon();

	Polygon2D simplex;
	//Create First Simplex
	if( !Polygon2D::GetGJKContainingSimplex( poly0, poly1, &simplex ) )
	{
		return false;
	}
	

	while(Polygon2D::ExpandPenetration( poly0, poly1, &simplex ) ){}

	LineSegment2 closestEdge;
	size_t closestEdgeIndex = 0;
	simplex.GetClosestEdge( &closestEdge, origin, &closestEdgeIndex );
	Vec2 closestPoint = closestEdge.GetNearestPoint( origin );
	Vec2 normal = origin - closestPoint;
	normal.Normalize();
	float penetration = GetDistance2D( origin, closestPoint );

	if( penetration < 0.01f )
	{
		return false;
	}

	//To get all points on the reference, make an infinite line with two points on the line
	Vec2 refPoint = poly1.GetFurthestPointInDirection( normal );
	Vec2 tangent = normal.GetRotated90Degrees();
	Vec2 pointOnTangentLine = refPoint + tangent;
	
	LineSegment2 referenceEdge;
	Polygon2D::GetRangeNearInfiniteLine( &referenceEdge, refPoint, pointOnTangentLine, poly1, 0.01f );
	
	//Early out check if poly1 is only hitting poly0 with 1 point
	LineSegment2 contactEdge;
	if( referenceEdge.startPosition.IsAlmostEqual( referenceEdge.endPosition ) )
	{
		contactEdge.startPosition = referenceEdge.startPosition - (penetration * normal);
		contactEdge.endPosition = contactEdge.startPosition;
	}
	else
	{
		Polygon2D::GetGJKContactEdgeFromPoly( &contactEdge, referenceEdge, normal, poly0 );
	}
	//Now have infinite line segment with furthestPoint
	
	
	Vec2 contactPoint = refPoint;
	manifold->contactEdge = contactEdge;
	manifold->normal = normal;
	manifold->penetration = penetration;

	
//DEBUG
// 	DebugAddWorldPoint( contactEdge.startPosition, 0.1f, Rgba8::RED, 0.f, DEBUG_RENDER_ALWAYS );
// 	DebugAddWorldPoint( contactEdge.endPosition, 0.1f, Rgba8::RED, 0.f, DEBUG_RENDER_ALWAYS );
//	DebugAddWorldArrow( LineSegment3( contactPoint, contactPoint + normal ), Rgba8::GREEN, 0.f, DEBUG_RENDER_ALWAYS );
// 	DebugAddWorldArrow( LineSegment3( contactEdge.startPosition, contactEdge.startPosition + normal ), Rgba8::GREEN, 0.f, DEBUG_RENDER_ALWAYS );
// 	DebugAddWorldArrow( LineSegment3( contactEdge.endPosition, contactEdge.endPosition + normal ), Rgba8::GREEN, 0.f, DEBUG_RENDER_ALWAYS );
// 	DebugAddWorldBillboardTextf( contactPoint, Vec2(), Rgba8::RED, "Pen: %.2f", penetration );
// 	DebugAddWorldLine( LineSegment3( referenceEdge.startPosition, referenceEdge.endPosition ), Rgba8::RED, 0.f, DEBUG_RENDER_ALWAYS );
// 	DebugAddWorldLine( LineSegment3( contactEdge.startPosition, contactEdge.endPosition ), Rgba8::BLUE, 0.f, DEBUG_RENDER_ALWAYS );
	return true;
}
