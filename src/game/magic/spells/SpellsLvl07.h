/*
 * Copyright 2014 Arx Libertatis Team (see the AUTHORS file)
 *
 * This file is part of Arx Libertatis.
 *
 * Arx Libertatis is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Arx Libertatis is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Arx Libertatis.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ARX_GAME_MAGIC_SPELLS_SPELLSLVL07_H
#define ARX_GAME_MAGIC_SPELLS_SPELLSLVL07_H

#include "game/magic/Spell.h"

#include "graphics/particle/ParticleSystem.h"
#include "graphics/effects/Lightning.h"

class FlyingEyeSpell : public SpellBase {
public:
	FlyingEyeSpell();
	
	bool CanLaunch();
	void Launch();
	void End();
	void Update(float timeDelta);
	
	Vec3f getPosition();
	
private:
	unsigned long m_lastupdate;
};

class FireFieldSpell : public SpellBase {
public:
	FireFieldSpell();
	
	void Launch();
	void End();
	void Update(float timeDelta);
	
	Vec3f getPosition();
	
private:
	Vec3f m_pos;
	LightHandle m_light;
	DamageHandle m_damage;
	
	ParticleSystem pPSStream;
	ParticleSystem pPSStream1;
};

class IceFieldSpell : public SpellBase {
public:
	IceFieldSpell();
	
	void Launch();
	void End();
	void Update(float timeDelta);
	
	Vec3f getPosition();
	
private:
	LightHandle m_light;
	DamageHandle m_damage;
	
	Vec3f m_pos;
	TextureContainer * tex_p1;
	TextureContainer * tex_p2;
	
	static const int iMax = 50;
	int tType[50];
	Vec3f tPos[50];
	Vec3f tSize[50];
	Vec3f tSizeMax[50];
};

class LightningStrikeSpell : public SpellBase {
public:
	void Launch();
	void End();
	void Update(float timeDelta);
	
private:
	CLightning m_lightning;
};

class ConfuseSpell : public SpellBase {
public:
	ConfuseSpell();
	
	void Launch();
	void End();
	void Update(float timeDelta);
	
	Vec3f getPosition();
	
private:
	LightHandle m_light;
	TextureContainer * tex_p1;
	TextureContainer * tex_trail;
	ANIM_USE au;
	Vec3f eCurPos;
};

#endif // ARX_GAME_MAGIC_SPELLS_SPELLSLVL07_H
