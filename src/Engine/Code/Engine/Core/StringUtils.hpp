#pragma once
//-----------------------------------------------------------------------------------------------
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/vec2.hpp"
#include <string>
#include <vector>

typedef std::vector<std::string> Strings;

//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );
const std::string Stringv( const char* format, va_list args );

Strings GetFileNamesInFolder( std::string const& folderPath, const char* filePattern );


const std::vector<std::string> SplitStringOnDelimeter( const char* text, const char delimeter);

bool SetBoolFromText(const char* text);

//GetValueFromString
bool GetValueFromString( std::string const& stringValue, bool const& defaultValue );
int GetValueFromString( std::string const& stringValue, int const& defaultValue );
float GetValueFromString( std::string const& stringValue, float const& defaultValue );
Vec3 GetValueFromString( std::string const& stringValue, Vec3 const& defaultValue );
Vec2 GetValueFromString( std::string const& stringValue, Vec2 const& defaultValue );
IntVec2 GetValueFromString( std::string const& stringValue, IntVec2 const& defaultValue );
IntRange GetValueFromString( std::string const& stringValue, IntRange const& defaultValue );
FloatRange GetValueFromString( std::string const& stringValue, FloatRange const& defaultValue );
Rgba8 GetValueFromString( std::string const& stringValue, Rgba8 const& defaultValue );
std::string GetValueFromString( std::string const& stringValue, std::string const& );
std::uintptr_t GetValueFromString( std::string const& stringValue, std::uintptr_t const& defaultValue );

//ToString
std::string ToString( bool const& value );
std::string ToString( int const& value );
std::string ToString( float const& value );
std::string ToString( Vec3 const& value );
std::string ToString( Vec2 const& value );
std::string ToString( IntVec2 const& value );
std::string ToString( IntRange const& value );
std::string ToString( FloatRange const& value );
std::string ToString( Rgba8 const& value );
std::string ToString( std::string const& value );
std::string ToString( void *const& value );
std::string ToString( std::uintptr_t const& value );



