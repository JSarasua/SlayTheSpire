#pragma once
#include "Engine/Renderer/SpriteDefinition.hpp"
#include <vector>

class Texture;
struct IntVec2;
struct Vec2;


class SpriteSheet
{
public:
	explicit SpriteSheet( const Texture& texture, const IntVec2& simpleGridLayout );

	const Texture&			GetTexture() const		{ return m_texture; }
	int						GetNumSprites() const	{ return (int) m_spriteDefs.size(); }
	const SpriteDefinition&	GetSpriteDefinition( int spriteIndex ) const;
	const IntVec2&			GetSpriteSheetDimensions() const { return m_spriteSheetDimensions; }
	const int				GetSpriteIndex(const IntVec2& spriteCoords) const;
	void					GetSpriteUVs( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex ) const;


protected:
	const Texture&					m_texture;
	std::vector<SpriteDefinition>	m_spriteDefs;
	IntVec2							m_spriteSheetDimensions;
};