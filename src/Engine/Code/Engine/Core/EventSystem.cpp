#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"



void EventSystem::SubscribeToEvent( const std::string& eventName, eEventType eventType, EventCallbackFunctionPtrType functionToCall )
{
	EventSubscription* newSubscription = new EventSubscription();
	newSubscription->m_eventName = eventName;
	newSubscription->m_eventType = eventType;
	newSubscription->m_callbackfunctionPtr = functionToCall;

	m_eventSubscriptions.push_back(newSubscription);
}

void EventSystem::UnsubscribeToEvent( const std::string& eventName )
{
	for( int eventSystemIndex = 0; eventSystemIndex < m_eventSubscriptions.size(); eventSystemIndex++ )
	{
		if( !m_eventSubscriptions[eventSystemIndex] )
		{
			continue;
		}
		const std::string& name = m_eventSubscriptions[eventSystemIndex]->m_eventName;
		if( name == eventName )
		{
			delete m_eventSubscriptions[eventSystemIndex];
			m_eventSubscriptions[eventSystemIndex] = nullptr;
		}
	}
}

void EventSystem::FireEvent( const std::string& stringToCall, eEventType eventType, const EventArgs* args )
{

	bool wasAnEventCalled = false;
	for( int eventSystemIndex = 0; eventSystemIndex < m_eventSubscriptions.size(); eventSystemIndex++ )
	{
		if( !m_eventSubscriptions[eventSystemIndex] )
		{
			continue;
		}
		if( m_eventSubscriptions[eventSystemIndex]->m_eventType != eventType )
		{
			continue;
		}

		if( m_eventSubscriptions[eventSystemIndex]->m_eventName == stringToCall )
		{
			EventCallbackFunctionPtrType functionToCall = m_eventSubscriptions[eventSystemIndex]->m_callbackfunctionPtr;
			if( nullptr != functionToCall )
			{
				bool isEventUsed = functionToCall( args );
				wasAnEventCalled = true;
				if( isEventUsed )
				{
					return;
				}
			}
			else
			{
				bool isEventUsed = m_eventSubscriptions[eventSystemIndex]->m_delegate.Invoke( *args );
				wasAnEventCalled = true;
				if( isEventUsed )
				{
					return;
				}
			}
			

		}
	}

	if( !wasAnEventCalled )
	{
		EventArgs NoCallArgs;
		NoCallArgs.SetValue("stringToCall", stringToCall);
		FireEvent( "NoCall", NOCONSOLECOMMAND, &NoCallArgs );
	}
}

std::string EventSystem::GetFirstEventStartingWith( const std::string& subString, eEventType eventType )
{
	for( size_t eventIndex = 0; eventIndex < m_eventSubscriptions.size(); eventIndex++ )
	{
		EventSubscription* eventSub = m_eventSubscriptions[eventIndex];
		if( eventSub->m_eventType != eventType )
		{
			continue;
		}

		std::string eventName = eventSub->m_eventName;
		if( 0 == eventName.compare( 0, subString.length(), subString ) )
		{
			return eventName;
		}
	}
	return std::string();
}

EventArgs EventSystem::GetEvents()
{
	EventArgs args;
	for( size_t eventIndex = 0; eventIndex < m_eventSubscriptions.size(); eventIndex++ )
	{
		std::string eventName = m_eventSubscriptions[eventIndex]->m_eventName;
		args.SetValue(eventName, eventName);
	}
	return args;
}

