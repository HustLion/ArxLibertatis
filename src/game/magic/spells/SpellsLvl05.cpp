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

#include "game/magic/spells/SpellsLvl05.h"

#include "core/Application.h"
#include "core/Config.h"
#include "core/Core.h"
#include "core/GameTime.h"
#include "game/Damage.h"
#include "game/Entity.h"
#include "game/EntityManager.h"
#include "game/NPC.h"
#include "game/Player.h"
#include "game/Spells.h"
#include "graphics/particle/Particle.h"
#include "graphics/particle/ParticleEffects.h"
#include "graphics/spells/Spells05.h"
#include "physics/Collisions.h"
#include "scene/GameSound.h"
#include "scene/Interactive.h"


RuneOfGuardingSpell::RuneOfGuardingSpell()
	: SpellBase()
	, tex_p2(NULL)
	, ulCurrentTime(0)
{}

void RuneOfGuardingSpell::Launch()
{
	spells.endByCaster(m_caster, SPELL_RUNE_OF_GUARDING);
	
	ARX_SOUND_PlaySFX(SND_SPELL_RUNE_OF_GUARDING);
	
	m_duration = (m_launchDuration > -1) ? m_launchDuration : 99999999;
	
	m_pos = entities[m_caster]->pos;
	
	tex_p2 = TextureContainer::Load("graph/obj3d/textures/(fx)_tsu_blueting");
	
	m_light = GetFreeDynLight();
	if(lightHandleIsValid(m_light)) {
		EERIE_LIGHT * light = lightHandleGet(m_light);
		
		light->intensity = 0.7f + 2.3f;
		light->fallend = 500.f;
		light->fallstart = 400.f;
		light->rgb = Color3f(1.0f, 0.2f, 0.2f);
		light->pos = m_pos - Vec3f(0.f, 50.f, 0.f);
		light->time_creation = (unsigned long)(arxtime);
		light->duration = 200;
	}
}

void RuneOfGuardingSpell::End() {
	
	endLightDelayed(m_light, 500);
}

void RuneOfGuardingSpell::Update(float timeDelta) {
	
	ulCurrentTime += timeDelta;
	
	if(lightHandleIsValid(m_light)) {
		EERIE_LIGHT * light = lightHandleGet(m_light);
		
		float fa = 1.0f - rnd() * 0.15f;
		light->intensity = 0.7f + 2.3f * fa;
		light->fallend = 350.f;
		light->fallstart = 150.f;
		light->rgb = Color3f(1.0f, 0.2f, 0.2f);
		light->time_creation = (unsigned long)(arxtime);
		light->duration = 200;
	}
	
	Vec3f pos = m_pos + Vec3f(0.f, -20.f, 0.f);
	
	RenderMaterial mat;
	mat.setDepthTest(true);
	mat.setBlendType(RenderMaterial::Additive);
	
	Anglef stiteangle;
	Color3f stitecolor;
	
	float stiteangleb = float(ulCurrentTime) * 0.01f;
	stiteangle.setYaw(0);
	stiteangle.setRoll(0);
	
	stiteangle.setPitch(stiteangleb * 0.1f);
	stitecolor = Color3f(0.4f, 0.4f, 0.6f);
	float scale = std::sin(ulCurrentTime * 0.015f);
	Vec3f stitescale = Vec3f(1.f, -0.1f, 1.f);
	
	Draw3DObject(slight, stiteangle, pos, stitescale, stitecolor, mat);
	
	stiteangle.setPitch(stiteangleb);
	stitecolor = Color3f(0.6f, 0.f, 0.f);
	stitescale = Vec3f(2.f) * (1.f + 0.01f * scale);
	
	Draw3DObject(ssol, stiteangle, pos, stitescale, stitecolor, mat);
	
	stitecolor = Color3f(0.6f, 0.3f, 0.45f);
	stitescale = Vec3f(1.8f) * (1.f + 0.02f * scale);
	
	Draw3DObject(srune, stiteangle, pos, stitescale, stitecolor, mat);
	
	for(int n = 0; n < 4; n++) {
		
		PARTICLE_DEF * pd = createParticle();
		if(!pd) {
			break;
		}
		
		pd->ov = pos + (Vec3f(40.f, 0.f, 40.f) * Vec3f(frand2(), 0.f, frand2()));
		pd->move = Vec3f(0.8f, -4.f, 0.8f) * Vec3f(frand2(), rnd(), frand2());
		pd->scale = Vec3f(-0.1f);
		pd->tolive = Random::get(2600, 3200);
		pd->tc = tex_p2;
		pd->siz = 0.3f;
		pd->rgb = Color3f(.4f, .4f, .6f);
	}
	
	
	
	Sphere sphere = Sphere(m_pos, std::max(m_level * 15.f, 50.f));
	if(CheckAnythingInSphere(sphere, m_caster, CAS_NO_SAME_GROUP | CAS_NO_BACKGROUND_COL | CAS_NO_ITEM_COL| CAS_NO_FIX_COL | CAS_NO_DEAD_COL)) {
		ARX_BOOMS_Add(m_pos);
		LaunchFireballBoom(m_pos, (float)m_level);
		DoSphericDamage(Sphere(m_pos, 30.f * m_level), 4.f * m_level, DAMAGE_AREA, DAMAGE_TYPE_FIRE | DAMAGE_TYPE_MAGICAL, m_caster);
		ARX_SOUND_PlaySFX(SND_SPELL_RUNE_OF_GUARDING_END, &m_pos);
		m_duration = 0;
	}
}

Vec3f RuneOfGuardingSpell::getPosition() {
	
	return m_pos;
}


LevitateSpell::LevitateSpell()
	: SpellBase()
	, ulCurrentTime(0)
	, m_baseRadius(50.f)
{}

void LevitateSpell::Launch()
{
	spells.endByCaster(m_caster, SPELL_LEVITATE);
	
	if(m_caster == PlayerEntityHandle) {
		m_target = PlayerEntityHandle;
	}
	
	ARX_SOUND_PlaySFX(SND_SPELL_LEVITATE_START, &entities[m_target]->pos);
	
	m_duration = (m_launchDuration > -1) ? m_launchDuration : 2000000000;
	m_hasDuration = true;
	m_fManaCostPerSecond = 1.f;
	
	Vec3f target;
	if(m_target == PlayerEntityHandle) {
		target = player.pos + Vec3f(0.f, 150.f, 0.f);
		m_duration = 200000000;
		player.levitate = true;
	} else {
		target = entities[m_target]->pos;
	}
	
	m_pos = target;
	
	float rhaut = 100.f;
	float hauteur = 80.f;
	
	cone1.Init(m_baseRadius, rhaut, hauteur);
	cone2.Init(m_baseRadius, rhaut * 1.5f, hauteur * 0.5f);
	m_stones.Init(m_baseRadius);
	
	m_snd_loop = ARX_SOUND_PlaySFX(SND_SPELL_LEVITATE_LOOP, &entities[m_target]->pos, 0.7f, ARX_SOUND_PLAY_LOOPED);
	
	m_targets.push_back(m_target);
}

void LevitateSpell::End()
{
	ARX_SOUND_Stop(m_snd_loop);
	ARX_SOUND_PlaySFX(SND_SPELL_LEVITATE_END, &entities[m_target]->pos);
	m_targets.clear();
	
	if(m_target == PlayerEntityHandle)
		player.levitate = false;
}

void LevitateSpell::Update(float timeDelta) {
	
	ulCurrentTime += timeDelta;
	
	Vec3f target;

	if(m_target == PlayerEntityHandle) {
		target = player.pos + Vec3f(0.f, 150.f, 0.f);
		player.levitate = true;
	} else {
		target = entities[m_caster]->pos;
	}
	
	m_pos = target;
	
	float coneScale = 0.f;
	int dustParticles = 0;
	
	if(ulCurrentTime < 1000) {
		coneScale = (float) ulCurrentTime / 1000.f;
		dustParticles = 3;
	} else {
		coneScale = 1.f;
		dustParticles = 10;
	}
	
	cone1.Update(timeDelta, m_pos, coneScale);
	cone2.Update(timeDelta, m_pos, coneScale);
	
	m_stones.Update(timeDelta, m_pos);
	
	for(int i = 0; i < dustParticles; i++) {
		createDustParticle();
	}
	
	cone1.Render();
	cone2.Render();
	m_stones.DrawStone();
	
	ARX_SOUND_RefreshPosition(m_snd_loop, entities[m_target]->pos);
}

void LevitateSpell::createDustParticle() {
	
	PARTICLE_DEF * pd = createParticle();
	if(!pd) {
		return;
	}
	
	float a = glm::radians(360.f * rnd());
	pd->ov = m_pos + Vec3f(m_baseRadius * std::cos(a), 0.f, m_baseRadius * std::sin(a));
	float t = fdist(pd->ov, m_pos);
	pd->move = Vec3f((5.f + 5.f * rnd()) * ((pd->ov.x - m_pos.x) / t), 3.f * rnd(),
	                 (5.f + 5.f * rnd()) * ((pd->ov.z - m_pos.z) / t));
	pd->siz = 30.f + 30.f * rnd();
	pd->tolive = 3000;
	pd->timcreation = -(long(arxtime) + 3000l); // TODO WTF
	pd->special = FIRE_TO_SMOKE | FADE_IN_AND_OUT | ROTATING | MODULATE_ROTATION | DISSIPATING;
	pd->fparam = 0.0000001f;
}



CurePoisonSpell::CurePoisonSpell()
	: SpellBase()
	, ulCurrentTime(0)
{}

void CurePoisonSpell::Launch()
{
	if(m_caster == PlayerEntityHandle) {
		m_target = PlayerEntityHandle;
	}
	
	float cure = m_level * 10;
	if(m_target == PlayerEntityHandle) {
		player.poison -= std::min(player.poison, cure);
		ARX_SOUND_PlaySFX(SND_SPELL_CURE_POISON);
	} else if (ValidIONum(m_target)) {
		Entity * io = entities[m_target];
		if(io->ioflags & IO_NPC) {
			io->_npcdata->poisonned -= std::min(io->_npcdata->poisonned, cure);
		}
		ARX_SOUND_PlaySFX(SND_SPELL_CURE_POISON, &io->pos);
	}
	
	m_duration = 3500;
	
	{
	ParticleParams cp;
	cp.m_nbMax = 350;
	cp.m_life = 800;
	cp.m_lifeRandom = 2000;
	cp.m_pos = Vec3f(100, 0, 100);
	cp.m_direction = Vec3f(0.f, -1.f, 0.f);
	cp.m_angle = glm::radians(5.f);
	cp.m_speed = 120;
	cp.m_speedRandom = 84;
	cp.m_gravity = Vec3f(0, -10, 0);
	cp.m_flash = 0;
	cp.m_rotation = 1.0f / (101 - 80);

	cp.m_startSegment.m_size = 8;//6;
	cp.m_startSegment.m_sizeRandom = 8;
	cp.m_startSegment.m_color = Color(20, 205, 20, 245).to<float>();
	cp.m_startSegment.m_colorRandom = Color(50, 50, 50, 10).to<float>();

	cp.m_endSegment.m_size = 6;
	cp.m_endSegment.m_sizeRandom = 4;
	cp.m_endSegment.m_color = Color(5, 20, 5, 0).to<float>();
	cp.m_endSegment.m_colorRandom = Color(0, 40, 0, 0).to<float>();
	cp.m_blendMode = RenderMaterial::Additive;
	cp.m_texture.set("graph/particles/cure_poison", 0, 100); //5
	cp.m_spawnFlags = PARTICLE_CIRCULAR | PARTICLE_BORDER;
	pPS.SetParams(cp);
	}
	
	m_light = GetFreeDynLight();

	if(lightHandleIsValid(m_light)) {
		EERIE_LIGHT * light = lightHandleGet(m_light);
		
		light->intensity = 1.5f;
		light->fallstart = 200.f;
		light->fallend   = 350.f;
		light->rgb = Color3f(0.f, 1.f, 0.0f);
		light->pos = m_pos + Vec3f(0.f, -50.f, 0.f);
		light->time_creation = (unsigned long)(arxtime);
		light->duration = 200;
		light->extras = 0;
	}
}

void CurePoisonSpell::End() {
	
}

void CurePoisonSpell::Update(float timeDelta) {
	
	ulCurrentTime += timeDelta;
	
	m_pos = entities[m_target]->pos;
	
	if(m_target == PlayerEntityHandle)
		m_pos.y += 200;
	
	unsigned long ulCalc = m_duration - ulCurrentTime ;
	arx_assert(ulCalc <= LONG_MAX);
	long ff = 	static_cast<long>(ulCalc);

	if(ff < 1500) {
		pPS.m_parameters.m_spawnFlags = PARTICLE_CIRCULAR;
		pPS.m_parameters.m_gravity = Vec3f_ZERO;

		std::list<Particle *>::iterator i;

		for(i = pPS.listParticle.begin(); i != pPS.listParticle.end(); ++i) {
			Particle * pP = *i;

			if(pP->isAlive()) {
				pP->fColorEnd.a = 0;

				if(pP->m_age + ff < pP->m_timeToLive) {
					pP->m_age = pP->m_timeToLive - ff;
				}
			}
		}
	}

	pPS.SetPos(m_pos);
	pPS.Update(timeDelta);

	if(!lightHandleIsValid(m_light))
		m_light = GetFreeDynLight();

	if(lightHandleIsValid(m_light)) {
		EERIE_LIGHT * light = lightHandleGet(m_light);
		
		light->intensity = 2.3f;
		light->fallstart = 200.f;
		light->fallend   = 350.f;
		light->rgb = Color3f(0.4f, 1.f, 0.4f);
		light->pos = m_pos + Vec3f(0.f, -50.f, 0.f);
		light->duration = 200;
		light->time_creation = (unsigned long)(arxtime);
		light->extras = 0;
	}
	
	pPS.Render();
}


RepelUndeadSpell::RepelUndeadSpell()
	: SpellBase()
	, m_yaw(0.f)
	, tex_p2(NULL)
{}

void RepelUndeadSpell::Launch()
{
	spells.endByCaster(m_caster, SPELL_REPEL_UNDEAD);
	
	if(m_caster == PlayerEntityHandle) {
		m_target = PlayerEntityHandle;
	}
	
	ARX_SOUND_PlaySFX(SND_SPELL_REPEL_UNDEAD, &entities[m_target]->pos);
	if(m_target == PlayerEntityHandle) {
		m_snd_loop = ARX_SOUND_PlaySFX(SND_SPELL_REPEL_UNDEAD_LOOP, &entities[m_target]->pos, 1.f, ARX_SOUND_PLAY_LOOPED);
	}
	
	m_duration = (m_launchDuration > -1) ? m_launchDuration : 20000000;
	m_hasDuration = true;
	m_fManaCostPerSecond = 1.f;
	
	m_pos = player.pos;
	m_yaw = 0.f;
	tex_p2 = TextureContainer::Load("graph/obj3d/textures/(fx)_tsu_blueting");
}

void RepelUndeadSpell::End() {
	ARX_SOUND_Stop(m_snd_loop);
	
	endLightDelayed(m_light, 500);
}

void RepelUndeadSpell::Update(float timeDelta) {
	
	ARX_UNUSED(timeDelta);
	
	Vec3f pos = entities[m_target]->pos;
	
	float rot;
	if(m_target == PlayerEntityHandle) {
		rot = player.angle.getPitch();
	} else {
		rot = entities[m_target]->angle.getPitch();
	}
	
	m_pos = pos;
	m_yaw = rot;
	
	RenderMaterial mat;
	mat.setDepthTest(true);
	mat.setBlendType(RenderMaterial::Additive);
	
	Anglef  eObjAngle;

	eObjAngle.setPitch(m_yaw);
	eObjAngle.setYaw(0);
	eObjAngle.setRoll(0);

	float vv = 1.f + (std::sin(arxtime.get_updated() * ( 1.0f / 1000 ))); 
	vv *= ( 1.0f / 2 );
	vv += 1.1f;
	
	Draw3DObject(ssol, eObjAngle, m_pos + Vec3f(0.f, -5.f, 0.f), Vec3f(vv), Color3f(0.6f, 0.6f, 0.8f), mat);
	
	vv *= 100.f;
	
	for(int n = 0; n < 4; n++) {
		
		PARTICLE_DEF * pd = createParticle();
		if(!pd) {
			break;
		}
		
		float dx = -std::sin(frand2() * 360.f) * vv;
		float dz =  std::cos(frand2() * 360.f) * vv;
		pd->ov = m_pos + Vec3f(dx, 0.f, dz);
		pd->move = Vec3f(0.8f * frand2(), -4.f * rnd(), 0.8f * frand2());
		pd->scale = Vec3f(-0.1f);
		pd->tolive = Random::get(2600, 3200);
		pd->tc = tex_p2;
		pd->siz = 0.3f;
		pd->rgb = Color3f(.4f, .4f, .6f);
	}
	
	if(!lightHandleIsValid(m_light)) {
		m_light = GetFreeDynLight();
	}
	
	if(lightHandleIsValid(m_light)) {
		EERIE_LIGHT * light = lightHandleGet(m_light);
		
		light->intensity = 2.3f;
		light->fallend = 350.f;
		light->fallstart = 150.f;
		light->rgb = Color3f(0.8f, 0.8f, 1.f);
		light->pos = m_pos + Vec3f(0.f, -50.f, 0.f);
		light->duration = 200;
		light->time_creation = (unsigned long)(arxtime);
	}
	
	if (m_target == PlayerEntityHandle)
		ARX_SOUND_RefreshPosition(m_snd_loop, entities[m_target]->pos);
}


PoisonProjectileSpell::~PoisonProjectileSpell() {
	
	for(size_t i = 0; i < m_projectiles.size(); i++) {
		CPoisonProjectile * projectile = m_projectiles[i];
		
		endLightDelayed(projectile->lLightId, 2000);
		
		delete projectile;
	}
	m_projectiles.clear();
}

void PoisonProjectileSpell::Launch()
{
	ARX_SOUND_PlaySFX(SND_SPELL_POISON_PROJECTILE_LAUNCH,
	                  &m_caster_pos);
	
	Vec3f srcPos = Vec3f_ZERO;
	float afBeta = 0.f;
	
	Entity * caster = entities[m_caster];
	m_hand_group = caster->obj->fastaccess.primary_attach;

	if(m_hand_group != -1) {
		long group = m_hand_group;
		m_hand_pos = caster->obj->vertexlist3[group].v;
	}
	
	if(m_caster == PlayerEntityHandle) {

		afBeta = player.angle.getPitch();

		if(m_hand_group != -1) {
			srcPos = m_hand_pos;
		} else {
			srcPos = player.pos;
		}
	} else {
		afBeta = entities[m_caster]->angle.getPitch();

		if(m_hand_group != -1) {
			srcPos = m_hand_pos;
		} else {
			srcPos = entities[m_caster]->pos;
		}
	}
	
	srcPos += angleToVectorXZ(afBeta) * 90.f;
	
	long level = std::max(long(m_level), 1l);
	size_t uiNumber = std::min(5L, level);
	
	for(size_t i = 0; i < uiNumber; i++) {
		CPoisonProjectile * projectile = new CPoisonProjectile();
		
		m_projectiles.push_back(projectile);
	}
	
	m_duration = 8000ul;
	
	long lMax = 0;

	for(size_t i = 0; i < m_projectiles.size(); i++) {
		CPoisonProjectile * projectile = m_projectiles[i];
		
		projectile->Create(srcPos, afBeta + frand2() * 10.0f);
		long lTime = m_duration + Random::get(0, 5000);
		projectile->SetDuration(lTime);
		lMax = std::max(lMax, lTime);

		projectile->lLightId = GetFreeDynLight();

		if(lightHandleIsValid(projectile->lLightId)) {
			EERIE_LIGHT * light = lightHandleGet(projectile->lLightId);
			
			light->intensity		= 2.3f;
			light->fallend		= 250.f;
			light->fallstart		= 150.f;
			light->rgb = Color3f::green;
			light->pos = projectile->eSrc;
			light->time_creation	= (unsigned long)(arxtime);
			light->duration		= 200;
		}
	}
	
	m_duration = lMax + 1000;
}

void PoisonProjectileSpell::End() {
	
	for(size_t i = 0; i < m_projectiles.size(); i++) {
		CPoisonProjectile * projectile = m_projectiles[i];
		
		endLightDelayed(projectile->lLightId, 2000);
		
		delete projectile;
	}
	m_projectiles.clear();
}

void PoisonProjectileSpell::Update(float timeDelta) {
	
	for(size_t i = 0; i < m_projectiles.size(); i++) {
		CPoisonProjectile * projectile = m_projectiles[i];
		
		projectile->Update(timeDelta);
	}
	
	for(size_t i = 0; i < m_projectiles.size(); i++) {
		CPoisonProjectile * projectile = m_projectiles[i];
		
		projectile->Render();
		
		if(lightHandleIsValid(projectile->lLightId)) {
			EERIE_LIGHT * light = lightHandleGet(projectile->lLightId);
			
			light->intensity	= 2.3f * projectile->lightIntensityFactor;
			light->fallend	= 250.f;
			light->fallstart	= 150.f;
			light->rgb = Color3f::green;
			light->pos = projectile->eCurPos;
			light->time_creation = (unsigned long)(arxtime);
			light->duration	= 200;
		}

		AddPoisonFog(projectile->eCurPos, m_level + 7);

		if(m_timcreation + 1600 < (unsigned long)(arxtime)) {
			
			DamageParameters damage;
			damage.pos = projectile->eCurPos;
			damage.radius = 120.f;
			float v = 4.f + m_level * ( 1.0f / 10 ) * 6.f ;
			damage.damages = v * ( 1.0f / 1000 ) * framedelay;
			damage.area = DAMAGE_FULL;
			damage.duration = static_cast<long>(framedelay);
			damage.source = m_caster;
			damage.flags = 0;
			damage.type = DAMAGE_TYPE_MAGICAL | DAMAGE_TYPE_POISON;
			DamageCreate(damage);
		}
	}
}

void PoisonProjectileSpell::AddPoisonFog(const Vec3f & pos, float power) {
	
	int iDiv = 4 - config.video.levelOfDetail;
	
	float flDiv = static_cast<float>(1 << iDiv);
	
	arxtime.update();
	
	long count = std::max(1l, checked_range_cast<long>(framedelay / flDiv));
	while(count--) {
		
		if(rnd() * 2000.f >= power) {
			continue;
		}
		
		PARTICLE_DEF * pd = createParticle();
		if(!pd) {
			return;
		}
		
		float speed = 1.f;
		float fval = speed * 0.2f;
		pd->special = FADE_IN_AND_OUT | ROTATING | MODULATE_ROTATION | DISSIPATING;
		pd->ov = pos + randomVec(-100.f, 100.f);
		pd->scale = Vec3f(8.f, 8.f, 10.f);
		pd->move = Vec3f((speed - rnd()) * fval, (speed - speed * rnd()) * (1.f / 15),
		                 (speed - rnd()) * fval);
		pd->tolive = Random::get(4500, 9000);
		pd->tc = TC_smoke;
		pd->siz = (80.f + rnd() * 160.f) * (1.f / 3);
		pd->rgb = Color3f(rnd() * (1.f / 3), 1.f, rnd() * 0.1f);
		pd->fparam = 0.001f;
	}
}
