#include "Engine/Physics2D/Physics2D.hpp"
#include "Engine/Physics2D/Collision2D.hpp"
#include "Engine/Physics2D/Collider2D.hpp"
#include "Engine/Physics2D/DiscCollider2D.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Physics2D/Rigidbody2D.hpp"
#include "Engine/Math/Polygon2D.hpp"
#include "Engine/Physics2D/PolygonCollider2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Renderer/DebugRender.hpp"

static float m_fixedTimeFrame = 1.f/120.f;

void Physics2D::Startup()
{
	m_fixedTimeTimer.SetSeconds( m_clock, m_fixedTimeFrame );
	g_theEventSystem->SubscribeToEvent( "set_phyics_update", CONSOLECOMMAND, SetPhysicsUpdate );

	m_fixedTimeFrame = 1.f/120.f;

	memset(m_layerInteractions,~0U, sizeof(m_layerInteractions) );

}

void Physics2D::SetClock( Clock* clock )
{
	m_clock = clock;

	if( nullptr == m_clock )
	{
		m_clock = Clock::GetMaster();
	}
}

void Physics2D::BeginFrame()
{

}

void Physics2D::Update()
{
	while( m_fixedTimeTimer.CheckAndDecrement() )
	{
		AdvanceSimulation( m_fixedTimeFrame );
	}

}

void Physics2D::AdvanceSimulation( float deltaSeconds )
{
	ApplyEffectors();
	MoveRigidbodies( deltaSeconds );
	DetectCollisions();
	ResolveCollisions();
	CleanupDestroyedObjects();
}

void Physics2D::ApplyEffectors()
{
	for( size_t rigidBodyIndex = 0; rigidBodyIndex < m_rigidBodies.size(); rigidBodyIndex++ )
	{
		Rigidbody2D* rb = m_rigidBodies[rigidBodyIndex];
		
		if( nullptr == rb )
		{
			continue;
		}

		Vec2 gravity( 0.f, -1.f );
		gravity *= m_gravity;
		gravity *= rb->GetMass();
		rb->AddForce( gravity );
		rb->ApplyDragForce();
	}
}

void Physics2D::MoveRigidbodies( float deltaSeconds )
{
	for( size_t rigidBodyIndex = 0; rigidBodyIndex < m_rigidBodies.size(); rigidBodyIndex++ )
	{
		if( nullptr == m_rigidBodies[rigidBodyIndex] )
		{
			continue;
		}
		m_rigidBodies[rigidBodyIndex]->Update( deltaSeconds );
	}
}

void Physics2D::DetectCollisions()
{
	for( size_t colliderIndex = 0; colliderIndex < m_colliders.size(); colliderIndex++ )
	{
		if( nullptr == m_colliders[colliderIndex] )
		{
			continue;
		}
		if( !m_colliders[colliderIndex]->m_rigidbody->IsEnabled() )
		{
			continue;
		}
		for( size_t otherColliderIndex = colliderIndex + 1; otherColliderIndex < m_colliders.size(); otherColliderIndex++ )
		{
			if( nullptr == m_colliders[otherColliderIndex] )
			{
				continue;
			}
			if( !m_colliders[otherColliderIndex]->m_rigidbody->IsEnabled() )
			{
				continue;
			}

			Collider2D* myCollider = m_colliders[colliderIndex];
			Collider2D* otherCollider = m_colliders[otherColliderIndex];

			eSimulationMode mySimMode = myCollider->GetSimulationMode();
			eSimulationMode theirSimMode = otherCollider->GetSimulationMode();

			if( mySimMode == STATIC && theirSimMode == STATIC )
			{
				continue;
			}

			Manifold2D manifold;
			bool intersects = myCollider->GetManifold(otherCollider, &manifold );

			Collision2D collision;

			if( myCollider->m_type == COLLIDER2D_POLYGON && otherCollider->m_type == COLLIDER2D_DISC )
			{
				collision.me = otherCollider;
				collision.them = myCollider;
			}
			else
			{
				collision.me = myCollider;
				collision.them = otherCollider;
			}
// 				collision.me = myCollider;
// 				collision.them = otherCollider;
			collision.manifold = manifold;
			collision.colliderId = IntVec2( MinInt( myCollider->m_ID, otherCollider->m_ID), MaxInt( myCollider->m_ID, otherCollider->m_ID ) );


			Rigidbody2D* myRb = myCollider->m_rigidbody;
			Rigidbody2D* theirRB = otherCollider->m_rigidbody;
			uint myLayer = myRb->GetLayer();
			uint theirlayer = theirRB->GetLayer();
			bool doLayersInteract = DoLayersInteract( myLayer, theirlayer );
			if( !intersects || !doLayersInteract )
			{
				RemoveCollision( collision );
			}
			else
			{
				AddCollision( collision );
			}

		}
	}
}

void Physics2D::RemoveCollision( Collision2D const& collision )
{
	for( size_t collisionIndex = 0; collisionIndex < m_collisions.size(); collisionIndex++ )
	{
		if( m_collisions[collisionIndex].colliderId == collision.colliderId )
		{
			//Call end overlap events
			CallOverlapEndEvents( collision );

			m_collisions[collisionIndex] = m_collisions[ m_collisions.size() - 1];
			m_collisions.pop_back();
			
			break;
		}
	}
}

void Physics2D::RemoveCollisionsWithRigidbody( Rigidbody2D* rb )
{
	Collider2D* col = rb->m_collider;

	for( size_t collisionIndex = 0; collisionIndex < m_collisions.size(); collisionIndex++ )
	{
		Collider2D* first = m_collisions[collisionIndex].me;
		Collider2D* second = m_collisions[collisionIndex].them;

		if( nullptr != first && col == first )
		{
			RemoveCollision( m_collisions[collisionIndex] );
		}

		if( nullptr != second && col == second )
		{
			RemoveCollision( m_collisions[collisionIndex] );
		}
	}
}

void Physics2D::AddCollision( Collision2D collision )
{

	for( size_t collisionIndex = 0; collisionIndex < m_collisions.size(); collisionIndex++ )
	{
		if( m_collisions[collisionIndex].colliderId == collision.colliderId )
		{
			m_collisions[collisionIndex] = collision;
			CallOverlapStayEvents( collision );
			return;
		}
	}

	CallOverlapStartEvents( collision );
	m_collisions.push_back( collision );



}

void Physics2D::CallOverlapStartEvents( Collision2D collision )
{
	bool amITrigger = collision.me->m_isTrigger;
	bool areTheyTrigger = collision.them->m_isTrigger;
	Collider2D* me = collision.me;
	Collider2D* them = collision.them;

	Rigidbody2D* myRB = me->m_rigidbody;
	Rigidbody2D* theirRB = them->m_rigidbody;

	if( amITrigger == true )
	{
		me->m_onTriggerStart.Invoke( collision );
	}
	if( areTheyTrigger == true )
	{
		them->m_onTriggerStart.Invoke( collision );
	}
	if( amITrigger == false && areTheyTrigger == false )
	{
		myRB->m_onOverlapStart.Invoke( collision );
		theirRB->m_onOverlapStart.Invoke( collision );
	}
}

void Physics2D::CallOverlapStayEvents( Collision2D collision )
{
	bool amITrigger = collision.me->m_isTrigger;
	bool areTheyTrigger = collision.them->m_isTrigger;
	Collider2D* me = collision.me;
	Collider2D* them = collision.them;

	Rigidbody2D* myRB = me->m_rigidbody;
	Rigidbody2D* theirRB = them->m_rigidbody;


	if( amITrigger == false && areTheyTrigger == false )
	{
		myRB->m_onOverlapStay.Invoke( collision );
		theirRB->m_onOverlapStay.Invoke( collision );
	}
	else if( amITrigger == true && areTheyTrigger == false )
	{
		me->m_onTriggerStay.Invoke( collision );
		//them->m_onTriggerStay.Invoke( collision );
	}
	else if( amITrigger == false && areTheyTrigger == true )
	{
		//me->m_onTriggerStay.Invoke( collision );
		them->m_onTriggerStay.Invoke( collision );
	}
	else if( amITrigger == true && areTheyTrigger == true )
	{
		me->m_onTriggerStay.Invoke( collision );
		them->m_onTriggerStay.Invoke( collision );
	}

}

void Physics2D::CallOverlapEndEvents( Collision2D collision )
{
	bool amITrigger = collision.me->m_isTrigger;
	bool areTheyTrigger = collision.them->m_isTrigger;
	Collider2D* me = collision.me;
	Collider2D* them = collision.them;

	Rigidbody2D* myRB = me->m_rigidbody;
	Rigidbody2D* theirRB = them->m_rigidbody;

	if( amITrigger == true )
	{
		me->m_onTriggerLeave.Invoke( collision );
	}
	if( areTheyTrigger == true )
	{
		them->m_onTriggerLeave.Invoke( collision );
	}
	if( amITrigger == false && areTheyTrigger == false )
	{
		myRB->m_onOverlapStop.Invoke( collision );
		theirRB->m_onOverlapStop.Invoke( collision );
	}

}

void Physics2D::ResolveCollisions()
{
	for( size_t collisionIndex = 0; collisionIndex < m_collisions.size(); collisionIndex++ )
	{
		ResolveCollision(m_collisions[collisionIndex]);
	}

	//m_collisions.clear();
}

void Physics2D::ResolveCollision( Collision2D const& collision )
{
	if( collision.me->m_isTrigger || collision.them->m_isTrigger )
	{
		return;
	}
	Manifold2D manifold = collision.manifold;
	Vec2 normal = manifold.normal;
	Vec2 tangent = normal.GetRotatedMinus90Degrees();
	Vec2 contactPoint = manifold.contactEdge.GetCenter();
	Rigidbody2D* myRigidbody = collision.me->m_rigidbody;
	Rigidbody2D* theirRigidbody = collision.them->m_rigidbody;

	if( nullptr == myRigidbody || nullptr == theirRigidbody )
	{
		return;
	}
	if( nullptr == myRigidbody->m_collider || nullptr == theirRigidbody->m_collider )
	{
		return;
	}

	float penetration = manifold.penetration;
	float myMassRatio = GetMassRatio( myRigidbody, theirRigidbody );
	float theirMassRatio = GetMassRatio( theirRigidbody, myRigidbody );

	Vec2 pushMe = normal * myMassRatio * penetration;
	Vec2 pushThem = -normal * theirMassRatio * penetration;

	//save off velocities before moving for VerletVelocity calculations
	Vec2 myVelocity = myRigidbody->GetImpactVelocityAtPoint( contactPoint );
	Vec2 theirVelocity = theirRigidbody->GetImpactVelocityAtPoint( contactPoint );

	collision.me->Move( pushMe );
	collision.them->Move( pushThem );

	/************************************************************************/
	/* Apply Impulses                                                       */
	/************************************************************************/
	float myMass = myRigidbody->GetMass();
	float theirMass = theirRigidbody->GetMass();
	float myMoment = myRigidbody->CalculateMoment();
	float theirMoment = theirRigidbody->CalculateMoment();

	Vec2 myCoM = collision.me->GetCenterOfMass();
	Vec2 theirCoM = collision.them->GetCenterOfMass();

	//Used if we want to apply impulse closest to the center of mass instead of center. Supposedly makes simulation more stable
	Vec2 myApplyImpulseLocation = manifold.contactEdge.GetNearestPoint( myCoM );
	Vec2 theirApplyImpulseLocation = manifold.contactEdge.GetNearestPoint( theirCoM );

	Vec2 rMeToPoint = contactPoint - myCoM;
	Vec2 rThemToPoint = contactPoint - theirCoM;

	Vec2 rMeToPointPerp = rMeToPoint.GetRotated90Degrees();
	Vec2 rThemToPointPerp = rThemToPoint.GetRotated90Degrees();


	float combinedRestituion = collision.me->GetBounceWith( collision.them );
	float combinedFriction = collision.me->GetFrictionWith( collision.them );

	
	eSimulationMode mySimMode = myRigidbody->GetSimulationMode();
	eSimulationMode theirSimMode = theirRigidbody->GetSimulationMode();

	Vec2 velocityMeThem = theirVelocity - myVelocity;  //vAB
	if( mySimMode == DYNAMIC && (theirSimMode == KINEMATIC || theirSimMode == STATIC) )
	{
		float topPieceOfNormalEquation = 1.f * (1.f + combinedRestituion) * DotProduct2D( velocityMeThem, normal );

		float rAPNormalMoment = (DotProduct2D( rMeToPointPerp, normal ) * DotProduct2D( rMeToPointPerp, normal )) / myMoment;
		//float rBPNormalMoment = (DotProduct2D( rThemToPointPerp, normal ) * DotProduct2D( rThemToPointPerp, normal )) / theirMoment;

		float bottomPieceOfNormalEquation = /*DotProduct2D( normal, normal ) **/ (1.f/myMass /*+ 1.f/theirMass*/) + rAPNormalMoment /*+ rBPNormalMoment*/;
		float normalImpulseMagnitude = topPieceOfNormalEquation / bottomPieceOfNormalEquation;
		Vec2 normalImpulse = normalImpulseMagnitude * normal;

		myRigidbody->ApplyImpulseAt( myApplyImpulseLocation, normalImpulse);
		
		//For recalculating normal
		myVelocity = myRigidbody->GetImpactVelocityAtPoint( contactPoint );
		theirVelocity = theirRigidbody->GetImpactVelocityAtPoint( contactPoint );
		velocityMeThem = theirVelocity - myVelocity;  //vAB

		float topPieceOfTangentEquation = 1.f * (1.f + combinedRestituion) * DotProduct2D( velocityMeThem, tangent );
		float rAPTangentMoment = (DotProduct2D( rMeToPointPerp, tangent ) * DotProduct2D( rMeToPointPerp, tangent )) / myMoment;
		//float rBPTangentMoment = (DotProduct2D( rThemToPointPerp, tangent ) * DotProduct2D( rThemToPointPerp, tangent )) / theirMoment;

		float bottomPieceOfTangentEquation = /*DotProduct2D( tangent, tangent ) **/ (1.f/myMass /*+ 1.f/theirMass*/) + rAPTangentMoment /*+ rBPTangentMoment*/;
		float tangentImpulseMagnitude = topPieceOfTangentEquation / bottomPieceOfTangentEquation;
		Vec2 tangentImpulse = tangentImpulseMagnitude * tangent;


		if( absFloat( tangentImpulseMagnitude ) > absFloat( normalImpulseMagnitude )* combinedFriction )
		{
			tangentImpulse = normalImpulse.GetLength() * tangent * combinedFriction;
		}

		if( GetProjectedLength2D( tangentImpulse, myVelocity ) > 0.f )
		{
			tangentImpulse *= -1.f;
		}

		myRigidbody->ApplyImpulseAt( myApplyImpulseLocation, tangentImpulse);
	}
	else if( (mySimMode == KINEMATIC || mySimMode == STATIC) && theirSimMode == DYNAMIC )
	{
		float topPieceOfNormalEquation = 1.f * (1.f + combinedRestituion) * DotProduct2D( velocityMeThem, normal );

		//float rAPNormalMoment = (DotProduct2D( rMeToPointPerp, normal ) * DotProduct2D( rMeToPointPerp, normal )) / myMoment;
		float rBPNormalMoment = (DotProduct2D( rThemToPointPerp, normal ) * DotProduct2D( rThemToPointPerp, normal )) / theirMoment;

		float bottomPieceOfNormalEquation = /*DotProduct2D( normal, normal ) **/ (/*1.f/myMass*/ + 1.f/theirMass) + /*rAPNormalMoment*/ + rBPNormalMoment;
		float normalImpulseMagnitude = topPieceOfNormalEquation / bottomPieceOfNormalEquation;
		Vec2 normalImpulse = normalImpulseMagnitude * normal;

		theirRigidbody->ApplyImpulseAt( theirApplyImpulseLocation, -normalImpulse );
		
		//For recalculating normal
		myVelocity = myRigidbody->GetImpactVelocityAtPoint( contactPoint );
		theirVelocity = theirRigidbody->GetImpactVelocityAtPoint( contactPoint );
		velocityMeThem = theirVelocity - myVelocity;  //vAB

		float topPieceOfTangentEquation = 1.f * (1.f + combinedRestituion) * DotProduct2D( velocityMeThem, tangent );
		//float rAPTangentMoment = (DotProduct2D( rMeToPointPerp, tangent ) * DotProduct2D( rMeToPointPerp, tangent )) / myMoment;
		float rBPTangentMoment = (DotProduct2D( rThemToPointPerp, tangent ) * DotProduct2D( rThemToPointPerp, tangent )) / theirMoment;

		float bottomPieceOfTangentEquation = /*DotProduct2D( tangent, tangent ) **/ (/*1.f/myMass*/ + 1.f/theirMass) + /*rAPTangentMoment*/ + rBPTangentMoment;
		float tangentImpulseMagnitude = topPieceOfTangentEquation / bottomPieceOfTangentEquation;
		Vec2 tangentImpulse = tangentImpulseMagnitude * tangent;


		if( absFloat( tangentImpulseMagnitude ) > absFloat( normalImpulseMagnitude )* combinedFriction )
		{
			tangentImpulse = normalImpulse.GetLength() * tangent * combinedFriction;
		}

		FloatRange zeroRange = FloatRange(-0.001f, 0.0001f);
		if( zeroRange.IsInRange( myVelocity.GetLength() ) )
		{
			if( GetProjectedLength2D( tangentImpulse, theirVelocity ) < 0.f )
			{
				tangentImpulse *= -1.f;
			}
		}
		else
		{
			if( GetProjectedLength2D( tangentImpulse, myVelocity ) > 0.f )
			{
				tangentImpulse *= -1.f;
			}
		}

		theirRigidbody->ApplyImpulseAt( theirApplyImpulseLocation, -tangentImpulse);
	}

	else if( mySimMode == DYNAMIC && theirSimMode == DYNAMIC )
	{
		float topPieceOfNormalEquation = 1.f * (1.f + combinedRestituion) * DotProduct2D( velocityMeThem, normal );

		float rAPNormalMoment = ( DotProduct2D( rMeToPointPerp, normal ) * DotProduct2D( rMeToPointPerp, normal )) / myMoment;
		float rBPNormalMoment = ( DotProduct2D( rThemToPointPerp, normal ) * DotProduct2D( rThemToPointPerp, normal )) / theirMoment;

		float bottomPieceOfNormalEquation = /*DotProduct2D( normal, normal ) * */( 1.f/myMass + 1.f/theirMass ) + rAPNormalMoment + rBPNormalMoment;
		float normalImpulseMagnitude = topPieceOfNormalEquation / bottomPieceOfNormalEquation;
		Vec2 normalImpulse = normalImpulseMagnitude * normal;

		myRigidbody->ApplyImpulseAt( myApplyImpulseLocation, normalImpulse );
		theirRigidbody->ApplyImpulseAt( theirApplyImpulseLocation, -normalImpulse );

		//For recalculating normal
		myVelocity = myRigidbody->GetImpactVelocityAtPoint( contactPoint );
		theirVelocity = theirRigidbody->GetImpactVelocityAtPoint( contactPoint );
		velocityMeThem = theirVelocity - myVelocity;  //vAB

		float topPieceOfTangentEquation = 1.f * (1.f + combinedRestituion) * DotProduct2D( velocityMeThem, tangent );
		float rAPTangentMoment = ( DotProduct2D( rMeToPointPerp, tangent ) * DotProduct2D( rMeToPointPerp, tangent ) ) / myMoment;
		float rBPTangentMoment = ( DotProduct2D( rThemToPointPerp, tangent ) * DotProduct2D( rThemToPointPerp, tangent ) ) / theirMoment;
		float bottomPieceOfTangentEquation = /*DotProduct2D( tangent, tangent ) **/ (1.f/myMass + 1.f/theirMass) + rAPTangentMoment + rBPTangentMoment;
		float tangentImpulseMagnitude = topPieceOfTangentEquation / bottomPieceOfTangentEquation;
		Vec2 tangentImpulse = tangentImpulseMagnitude * tangent;

		if( absFloat( tangentImpulseMagnitude ) > absFloat( normalImpulseMagnitude )* combinedFriction )
		{
			tangentImpulse = normalImpulse.GetLength() * tangent * combinedFriction;
		}

		if( GetProjectedLength2D( tangentImpulse, myVelocity ) > 0.f )
		{
			tangentImpulse *= -1.f;
		}
		
		myRigidbody->ApplyImpulseAt( myApplyImpulseLocation, tangentImpulse );
		theirRigidbody->ApplyImpulseAt( theirApplyImpulseLocation, -tangentImpulse );
	}
}

void Physics2D::CleanupDestroyedObjects()
{
	for( int colliderIndex = 0; colliderIndex < m_colliders.size(); colliderIndex++ )
	{
		if( nullptr == m_colliders[colliderIndex] )
		{
			continue;
		}

		bool isGarbage = m_colliders[colliderIndex]->m_isGarbage;
		if( isGarbage )
		{
			delete m_colliders[colliderIndex];
			m_colliders[colliderIndex] = nullptr;
		}
	}

	for( int rigidBodyIndex = 0; rigidBodyIndex < m_rigidBodies.size(); rigidBodyIndex++ )
	{
		if( nullptr == m_colliders[rigidBodyIndex] )
		{
			continue;
		}

		bool isGarbage = m_rigidBodies[rigidBodyIndex]->m_isGarbage;
		if( isGarbage )
		{
			delete m_rigidBodies[rigidBodyIndex];
			m_rigidBodies[rigidBodyIndex] = nullptr;
		}
	}
}

void Physics2D::EndFrame()
{
}


float Physics2D::GetMassRatio( Rigidbody2D* me, Rigidbody2D* them ) const
{
	float myMass = me->GetMass();
	float theirMass = them->GetMass();

	eSimulationMode mySimMode = me->GetSimulationMode();
	eSimulationMode theirSimMode = them->GetSimulationMode();
	if( mySimMode == STATIC )
	{
		return 0.f;
	}
	else if( theirSimMode == STATIC )
	{
		return 1.f;
	}
	else if( mySimMode == DYNAMIC && theirSimMode == KINEMATIC )
	{
		return 1.f;
	}
	else if( mySimMode == KINEMATIC && theirSimMode == DYNAMIC )
	{
		return 0.f;
	}
	else
	{
		return theirMass/(myMass + theirMass);
	}
}

float Physics2D::GetFixedDeltaTime() const
{
	return m_fixedTimeFrame;
}

void Physics2D::SetFixedDeltaTime( float frameTimeSeconds )
{
	m_fixedTimeFrame = frameTimeSeconds;
}

void Physics2D::EnableLayerInteraction( uint layerIdx0, uint layerIdx1 )
{
	m_layerInteractions[layerIdx0] &= ( 1 << layerIdx1 );
	m_layerInteractions[layerIdx1] &= ( 1 << layerIdx0 );
}

void Physics2D::DisableLayerInteraction( uint layerIdx0, uint layerIdx1 )
{
	m_layerInteractions[layerIdx0] &= ~( 1 << layerIdx1 );
	m_layerInteractions[layerIdx1] &= ~( 1 << layerIdx0 );
}

bool Physics2D::DoLayersInteract( uint layerIdx0, uint layerIdx1 )
{
	return ( ( m_layerInteractions[layerIdx0] & ( 1 << layerIdx1 ) ) != 0 );
}

Rigidbody2D* Physics2D::CreateRigidBody()
{
	Rigidbody2D* rb = new Rigidbody2D();
	rb->m_system = this;
	m_rigidBodies.push_back(rb);
	return rb;
}

void Physics2D::DestroyRigidBody( Rigidbody2D* rb )
{
	rb->m_isGarbage = true;

	rb->m_collider->m_isGarbage = true;;
	rb->m_collider = nullptr;
	
}

DiscCollider2D* Physics2D::CreateDiscCollider( Vec2 localPosition, float radius )
{
	DiscCollider2D* dc = new DiscCollider2D(localPosition, Vec2(0.f, 0.f), radius );
	dc->m_system = this;
	dc->m_ID = m_nextColliderID;
	m_nextColliderID++;

	m_colliders.push_back(dc);
	return dc;
}

PolygonCollider2D* Physics2D::CreatePolygonCollider( Polygon2D const& poly, Vec2 localPosition )
{
	Vec2 worldPosition = poly.GetCenterOfMass();
	PolygonCollider2D* pc = new PolygonCollider2D(localPosition, worldPosition, poly );
	pc->m_ID =  m_nextColliderID;
	m_nextColliderID++;

	m_colliders.push_back( pc );
	return pc;
}

void Physics2D::DestroyCollider( Collider2D* collider )
{
	collider->m_isGarbage = true;
}

void Physics2D::SetSceneGravity( float newGravity )
{
	m_gravity = newGravity;
}

float Physics2D::GetSceneGravity() const
{
	return m_gravity;
}

bool Physics2D::SetPhysicsUpdate( const EventArgs* args )
{
	if( nullptr != args )
	{
		float argValue = args->GetValue(std::string("hz"),120.f);

		m_fixedTimeFrame = 1.f/argValue;
	}

	return true;
	
}

