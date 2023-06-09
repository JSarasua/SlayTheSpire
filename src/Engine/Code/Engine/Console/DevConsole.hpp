#pragma once
#include <vector>
#include <string>
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/EventSystem.hpp"

class RenderContext;
class Camera;
class InputSystem;
class Clock;


struct ColoredLine
{
public:
	ColoredLine( const Rgba8& textColor, const std::string& devConsolePrintString );

public:
	Rgba8 m_textColor;
	std::string m_devConsolePrintString;
};


class DevConsole
{
public:
	DevConsole();
	~DevConsole() {}

	void Startup();
	void BeginFrame();
	void Update();
	void EndFrame();
	void Shutdown();

	void HandleKeyStroke( unsigned char keyStroke );
	void HandleSpecialKeyStroke ( unsigned char keyStroke ); //Delete, pg up, pg down, etc.

	void ErrorString( std::string const& devConsolePrintString );
	bool GuaranteeOrError( bool condition, std::string const& devConsolePrintString );
	void PrintString( Rgba8 const& textColor, std::string const& devConsolePrintString );
	void Render( RenderContext& renderer, Camera const& camera, float lineHeight ) const;

	void SetIsOpen( bool isOpen );
	bool IsOpen() const {return m_isOpen;}

	void AutoCompleteCommand();

	static bool InvalidCommand( EventArgs const* args );
	static bool ListCommands( EventArgs const* args );

private:
	void ClampScrollIndex();
	void EraseSelectedChars();
	void ClampCurrentLine();
	void ResetSelection();
	void ExecuteString( std::string const& commandToExecute );

private:
	std::vector<ColoredLine> m_coloredLines;
	std::vector<ColoredLine> m_commandHistory;
	ColoredLine m_currentColoredLine;
	ColoredLine m_caret;
	bool m_isCaretRendering = true;
	float m_caretTimer = 0.f;
	bool m_isOpen = false;
	int m_currentCharIndex = 0;
	int m_selectedChars = 0;
	int m_beginSelect = 0;
	int m_endSelect = 0;
	int m_currentPreviousLineIndex = 0;
	int m_currentScrollIndex = 0;

	bool m_isCommandValid = true;
	bool m_isAutoCompleting = false;

	Clock* m_devConsoleClock = nullptr;
};