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

#ifndef ARX_GAME_MAGIC_SPELLS_SPELLSLVL02_H
#define ARX_GAME_MAGIC_SPELLS_SPELLSLVL02_H

#include "game/magic/Spell.h"

#include "graphics/particle/ParticleSystem.h"

class HealSpell : public SpellBase {
public:
	bool CanLaunch();
	void Launch();
	void End();
	void Update(float framedelay);
	
private:
	Vec3f m_pos;
	LightHandle m_light;
	ParticleSystem m_particles;
	long m_currentTime;
};

class DetectTrapSpell : public SpellBase {
public:
	void Launch();
	void End();
	void Update(float timeDelta);
};

class ArmorSpell : public SpellBase {
public:
	void Launch();
	void End();
	void Update(float timeDelta);
	
	Vec3f getPosition();
};

class LowerArmorSpell : public SpellBase {
public:
	LowerArmorSpell();
	
	void Launch();
	void End();
	void Update(float timeDelta);
	
	Vec3f getPosition();
	
private:
	long m_longinfo_lower_armor;
};

class HarmSpell : public SpellBase {
public:
	HarmSpell();
	
	void Launch();
	void End();
	void Update(float timeDelta);
	
private:
	LightHandle m_light;
	DamageHandle m_damage;
	float m_pitch;
};

#endif // ARX_GAME_MAGIC_SPELLS_SPELLSLVL02_H
