#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Core/EngineCommon.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

//Change TEXT to L without parens if it throws error
static TCHAR const* WND_CLASS_NAME = TEXT( "Simple Window Class" );

//-----------------------------------------------------------------------------------------------
// Handles Windows (Win32) messages/events; i.e. the OS is trying to tell us something happened.
// This function is called by Windows whenever we ask it for notifications
//
// #SD1ToDo: We will move this function to a more appropriate place later on...
//
static LRESULT CALLBACK WindowsMessageHandlingProcedure( HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam )
{
	Window* window = (Window*) ::GetWindowLongPtr(windowHandle, GWLP_USERDATA);

	switch( wmMessageCode )
	{
		// App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
	case WM_CLOSE:
	{
		window->m_eventSystem->FireEvent("quit", CONSOLECOMMAND, nullptr);
		return 0; // "Consumes" this message (tells Windows "okay, we handled it")
	}
	// Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
	case WM_KEYDOWN:
	{
		unsigned char asKey = (unsigned char)wParam;
		if( g_theConsole->IsOpen() )
		{
			if( asKey == TILDE_KEY || asKey == LCTRL_KEY || asKey == RCTRL_KEY || asKey == CTRL_KEY || asKey == SHIFT_KEY ) // tilde
			{
				window->m_input->HandleKeyDown( asKey );
			}
			else if( asKey == LEFT_KEY || asKey == RIGHT_KEY || asKey == UP_KEY || asKey == DOWN_KEY )
			{
				g_theConsole->HandleKeyStroke(asKey);
			}
			else if( asKey == DEL_KEY || asKey == PGUP_KEY || asKey == PGDOWN_KEY || asKey == HOME_KEY || asKey == END_KEY )
			{
				g_theConsole->HandleSpecialKeyStroke(asKey);
			}
		}
		else
		{
			window->m_input->HandleKeyDown( asKey );
		}

		break;
	}

	// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
	case WM_KEYUP:
	{
		unsigned char asKey = (unsigned char)wParam;
		window->m_input->HandleKeyUp( asKey );
		break;
	}
	case WM_CHAR:
	{
		unsigned char asKey = (unsigned char)wParam;
		g_theConsole->HandleKeyStroke( asKey );
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	{
		bool leftButtonDown = wParam & MK_LBUTTON;
		bool middleButtonDown = wParam & MK_MBUTTON;
		bool rightButtonDown = wParam & MK_RBUTTON;

		// tell input system - this should be similar to how you 
		// handle keyboard input, but now for however many mouse buttons you want to 
		// track;  For example, this one just tracks the 3. 
		g_theInput->UpdateMouseButtonState( leftButtonDown, middleButtonDown, rightButtonDown );

		// break, don't return - let windows still do its default thing even though we're intercepting the message
		break;
	}
	case WM_MOUSEWHEEL:
	{
		short wheelDeltaAmount = GET_WHEEL_DELTA_WPARAM( wParam );
		//DWORD scrollFixedPoint = (DWORD)wParam >> 16; // shift away low word part, leaving only the highword
		float scrollAmount = (float)wheelDeltaAmount / WHEEL_DELTA; // convert to a numeric value
		g_theInput->AddMouseWheelScrollAmount( scrollAmount ); // let the input system know that the mouse wheel has moved - again, interface design is up to you - this is just a suggestion
		break;
	}
	case  WM_ACTIVATE:
	{

		if( wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE)
		{
			g_theInput->SetScreenActive();
		}
		else if( wParam == WA_INACTIVE )
		{
			g_theInput->SetScreenInactive();
		}
		break;
	}
// 	case WM_COMMAND:
// 	{
// 		unsigned char asKey = (unsigned char)wParam;
// 		g_theConsole->HandleKeyStroke( asKey );
// 	}
	}


	// Send back to Windows any unhandled/unconsumed messages we want other apps to see (e.g. play/pause in music apps, etc.)
	return ::DefWindowProc( windowHandle, wmMessageCode, wParam, lParam );
}

static void RegisterWindowClass()
{
	// Define a window style/class
	WNDCLASSEX windowClassDescription;
	memset( &windowClassDescription, 0, sizeof( windowClassDescription ) );
	windowClassDescription.cbSize = sizeof( windowClassDescription );
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast<WNDPROC>(WindowsMessageHandlingProcedure); // Register our Windows message-handling function
	windowClassDescription.hInstance = GetModuleHandle( NULL );
	windowClassDescription.hIcon = NULL;						//Where you can embed an icon
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = WND_CLASS_NAME;
	::RegisterClassEx( &windowClassDescription );
}

static void UnregisterWindowClass()
{
	::UnregisterClass( WND_CLASS_NAME, GetModuleHandle( NULL));
}

Window::Window()
{
	RegisterWindowClass();
}



Window::~Window()
{
	UnregisterWindowClass();
}


bool Window::Open( std::string const& title, float clientAspect, float ratioOfHeight /*= 0.9f*/ )
{

	// #SD1ToDo: Add support for fullscreen mode (requires different window style flags than windowed mode)
	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	// Get desktop rect, dimensions, aspect
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect( desktopWindowHandle, &desktopRect );
	float desktopWidth = (float)(desktopRect.right - desktopRect.left);
	float desktopHeight = (float)(desktopRect.bottom - desktopRect.top);
	float desktopAspect = desktopWidth / desktopHeight;

	// Calculate maximum client size (as some % of desktop size)
	//constexpr float maxClientFractionOfDesktop = 0.90f;
	float clientWidth = desktopWidth * ratioOfHeight;
	float clientHeight = desktopHeight * ratioOfHeight;
	if( clientAspect > desktopAspect )
	{
		// Client window has a wider aspect than desktop; shrink client height to match its width
		clientHeight = clientWidth / clientAspect;
	}
	else
	{
		// Client window has a taller aspect than desktop; shrink client width to match its height
		clientWidth = clientHeight * clientAspect;
	}
	m_clientHeight = (unsigned int)clientHeight;
	m_clientWidth = (unsigned int)clientWidth;

	// Calculate client rect bounds by centering the client area
	float clientMarginX = 0.5f * (desktopWidth - clientWidth);
	float clientMarginY = 0.5f * (desktopHeight - clientHeight);
	RECT clientRect;
	clientRect.left = (int)clientMarginX;
	clientRect.right = clientRect.left + (int)clientWidth;
	clientRect.top = (int)clientMarginY;
	clientRect.bottom = clientRect.top + (int)clientHeight;

	// Calculate the outer dimensions of the physical window, including frame et. al.
	RECT windowRect = clientRect;
	AdjustWindowRectEx( &windowRect, windowStyleFlags, FALSE, windowStyleExFlags );

	WCHAR windowTitle[1024];
	MultiByteToWideChar( GetACP(), 0, title.c_str(), -1, windowTitle, sizeof( windowTitle ) / sizeof( windowTitle[0] ) );
	HWND hwnd = CreateWindowEx(
		windowStyleExFlags,
		WND_CLASS_NAME,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		(HINSTANCE) ::GetModuleHandle(NULL),
		NULL );

	ShowWindow( hwnd, SW_SHOW );
	SetForegroundWindow( hwnd );
	SetFocus( hwnd );

	::SetWindowLongPtr( hwnd, GWLP_USERDATA, (LONG_PTR)this );

	if( nullptr == hwnd )
	{
		return false;
	}

	HCURSOR cursor = LoadCursor( NULL, IDC_ARROW );
	SetCursor( cursor );

	m_hwnd = (void*)hwnd;
	return true;
}

void Window::Close()
{
	HWND hwnd = (HWND)m_hwnd;
	if( nullptr == hwnd )
	{
		return;
	}

	::DestroyWindow(hwnd);
	m_hwnd = nullptr;
}

//-----------------------------------------------------------------------------------------------
// Processes all Windows messages (WM_xxx) for this app that have queued up since last frame.
// For each message in the queue, our WindowsMessageHandlingProcedure (or "WinProc") function
//	is called, telling us what happened (key up/down, minimized/restored, gained/lost focus, etc.)
//
// #SD1ToDo: We will move this function to a more appropriate place later on...
//
void Window::BeginFrame()
{
	MSG queuedMessage;
	for( ;; )
	{
		const BOOL wasMessagePresent = PeekMessage( &queuedMessage, NULL, 0, 0, PM_REMOVE );
		if( !wasMessagePresent )
		{
			break;
		}

		TranslateMessage( &queuedMessage );
		DispatchMessage( &queuedMessage ); // This tells Windows to call our "WindowsMessageHandlingProcedure" (a.k.a. "WinProc") function
	}
}

void Window::EndFrame()
{

}

void Window::ToggleBorder()
{
	if( m_isbordered )
	{
		m_isbordered = false;
		SetWindowLongPtr( (HWND)m_hwnd, GWL_STYLE, WS_VISIBLE | WS_POPUP );
	}
	else
	{
		m_isbordered = true;
		SetWindowLongPtr( (HWND)m_hwnd, GWL_STYLE, WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED | WS_VISIBLE );
	}

}

void Window::SetInputSystem( InputSystem* input )
{
	m_input = input;
}

void Window::SetEventSystem( EventSystem* eventSystem )
{
	m_eventSystem = eventSystem;
}

unsigned int Window::GetClientWidth()
{
	return m_clientWidth;
}

unsigned int Window::GetClientHeight()
{
	return m_clientHeight;
}
