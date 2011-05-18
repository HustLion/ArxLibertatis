/*
===========================================================================
ARX FATALIS GPL Source Code
Copyright (C) 1999-2010 Arkane Studios SA, a ZeniMax Media company.

This file is part of the Arx Fatalis GPL Source Code ('Arx Fatalis Source Code'). 

Arx Fatalis Source Code is free software: you can redistribute it and/or modify it under the terms of the GNU General Public 
License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

Arx Fatalis Source Code is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied 
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Arx Fatalis Source Code.  If not, see 
<http://www.gnu.org/licenses/>.

In addition, the Arx Fatalis Source Code is also subject to certain additional terms. You should have received a copy of these 
additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Arx 
Fatalis Source Code. If not, please request a copy in writing from Arkane Studios at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing Arkane Studios, c/o 
ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.
===========================================================================
*/

#include "audio/openal/OpenALBackend.h"

#include "audio/openal/OpenALSource.h"
#include "audio/openal/OpenALUtils.h"
#include "audio/AudioGlobal.h"
#include "audio/AudioEnvironment.h"

namespace audio {

#undef ALError
#define ALError LogError

OpenALBackend::OpenALBackend() : device(NULL), context(NULL),
#ifdef HAVE_OPENAL_EFX
	hasEFX(false), effectEnabled(false),
#endif
	rolloffFactor(1.f) {
	
}

OpenALBackend::~OpenALBackend() {
	
	sources.clear();
	
	if(context) {
		alcDestroyContext(context);
	}
	
	ALenum error = alcGetError(device);
	if(error != AL_NO_ERROR) {
		LogError << "error destroying OpenAL context: " << error;
	}
	
	if(device) {
		if(alcCloseDevice(device) == ALC_FALSE) {
			LogError << "error closing device";
		}
	}
	
}

aalError OpenALBackend::init(bool enableEffects) {
	
	if(device) {
		return AAL_ERROR_INIT;
	}
	
	
	// clear error
	alGetError();
	
	// Create OpenAL interface
	device = alcOpenDevice(NULL);
	if(!device) {
		LogError << "error opening device: " << alcGetError(NULL);
		return AAL_ERROR_SYSTEM;
	}
	
	context = alcCreateContext(device, NULL);
	if(!context) {
		LogError << "error creating OpenAL context: " << alcGetError(device);
	}
	alcMakeContextCurrent(context);
	
#ifdef HAVE_OPENAL_EFX
	hasEFX = enableEffects && alcIsExtensionPresent(device, "ALC_EXT_EFX");
	if(enableEffects && !hasEFX) {
		LogWarning << "cannot enable effects, missing the EFX extension";
	}
	if(hasEFX) {
		alGenEffects = (LPALGENEFFECTS)alGetProcAddress("alGenEffects");
		alDeleteEffects = (LPALDELETEEFFECTS)alGetProcAddress("alDeleteEffects");
		alEffectf = (LPALEFFECTF)alGetProcAddress("alEffectf");
		arx_assert(alGenEffects && alDeleteEffects && alEffectf);
	}
#else
	ARX_UNUSED(enableEffects);
#endif
	
	alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
	
	AL_CHECK_ERROR("initializing")
	LogInfo << "Using " << alGetString(AL_RENDERER) << " " << alGetString(AL_VERSION) << (hasEFX ? " with" : " without") << " EFX";
	
	LogDebug << "AL extensions: " << alGetString(AL_EXTENSIONS);
	LogDebug << "ALC extensions: " << alcGetString(device, ALC_EXTENSIONS);
	
	return AAL_OK;
}

aalError OpenALBackend::updateDeferred() {
	
	// Nothing to do here.
	
	return AAL_OK;
}

Source * OpenALBackend::createSource(SampleId sampleId, const Channel & channel) {
	
	SampleId s_id = getSampleId(sampleId);
	
	if(!_sample.isValid(s_id)) {
		return NULL;
	}
	
	Sample * sample = _sample[s_id];
	
	OpenALSource * orig = NULL;
	for(size_t i = 0; i < sources.size(); i++) {
		if(sources[i] && sources[i]->getSample() == sample) {
			orig = (OpenALSource*)sources[i];
			break;
		}
	}
	
	OpenALSource * source = new OpenALSource(sample);
	
	size_t index = sources.add(source);
	if(index == (size_t)INVALID_ID) {
		delete source;
		return NULL;
	}
	
	SourceId id = (index << 16) | s_id;
	if(source->init(id, orig, channel)) {
		sources.remove(index);
		return NULL;
	}
	
	source->setRolloffFactor(rolloffFactor);
	
	return source;
}

Source * OpenALBackend::getSource(SourceId sourceId) {
	
	size_t index = ((sourceId >> 16) & 0x0000ffff);
	if(!sources.isValid(index)) {
		return NULL;
	}
	
	Source * source = sources[index];
	
	SampleId sample = getSampleId(sourceId);
	if(!_sample.isValid(sample) || source->getSample() != _sample[sample]) {
		return NULL;
	}
	
	arx_assert(source->getId() == sourceId);
	
	return source;
}

aalError OpenALBackend::setRolloffFactor(float factor) {
	
	rolloffFactor = factor;
	for(size_t i = 0; i < sources.size(); i++) {
		if(sources[i]) {
			sources[i]->setRolloffFactor(rolloffFactor);
		}
	}
	
	return AAL_OK;
}

aalError OpenALBackend::setListenerPosition(const Vec3f & position) {
	
	alListener3f(AL_POSITION, position.x, position.y, position.z);
	AL_CHECK_ERROR("setting listener posiotion")
	
	return AAL_OK;
}

aalError OpenALBackend::setListenerOrientation(const Vec3f & front, const Vec3f & up) {
	
	ALfloat orientation[] = {front.x, front.y, front.z, -up.x, -up.y, -up.z};
	alListenerfv(AL_ORIENTATION, orientation);
	AL_CHECK_ERROR("setting listener orientation")
	
	return AAL_OK;
}

Backend::source_iterator OpenALBackend::sourcesBegin() {
	return (source_iterator)sources.begin();
}

Backend::source_iterator OpenALBackend::sourcesEnd() {
	return (source_iterator)sources.end();
}

Backend::source_iterator OpenALBackend::deleteSource(source_iterator it) {
	arx_assert(it >= sourcesBegin() && it < sourcesEnd());
	return (source_iterator)sources.remove((ResourceList<OpenALSource>::iterator)it);
}

aalError OpenALBackend::setUnitFactor(float factor) {
	
#ifdef HAVE_OPENAL_EFX
	if(hasEFX) {
		alListenerf(AL_METERS_PER_UNIT, factor);
		AL_CHECK_ERROR("setting unit factor")
	}
#endif
	
	const float speedOfSoundMetersPerSecond = 343.3f; // Default for OpenAL
	alSpeedOfSound(speedOfSoundMetersPerSecond / factor);
	AL_CHECK_ERROR("scaling speed of sound to unit factor")
	
	return AAL_OK;
}

#ifdef HAVE_OPENAL_EFX

aalError OpenALBackend::setReverbEnabled(bool enable) {
	
	ARX_UNUSED(enable);
	
	// TODO implement reverb
	
	return AAL_OK;
}

aalError OpenALBackend::setRoomRolloffFactor(float factor) {
	
	if(!effectEnabled) {
		return AAL_ERROR_INIT;
	}
	
	float rolloff = clamp(factor, 0.f, 10.f);
	
	return setEffect(AL_REVERB_ROOM_ROLLOFF_FACTOR, rolloff);
}

aalError OpenALBackend::setListenerEnvironment(const Environment & env) {
	
	if(!effectEnabled) {
		return AAL_ERROR_INIT;
	}
	
	// TODO implement reverb - not all properties are set, some may be wrong
	
	setEffect(AL_REVERB_DIFFUSION, env.diffusion);
	setEffect(AL_REVERB_AIR_ABSORPTION_GAINHF, env.absorption * -100.f);
	setEffect(AL_REVERB_LATE_REVERB_GAIN, clamp(env.reverb_volume, 0.f, 1.f));
	setEffect(AL_REVERB_LATE_REVERB_DELAY, clamp(env.reverb_delay, 0.f, 100.f) * 0.001f);
	setEffect(AL_REVERB_DECAY_TIME, clamp(env.reverb_decay, 100.f, 20000.f) * 0.001f);
	setEffect(AL_REVERB_DECAY_HFRATIO, clamp(env.reverb_hf_decay / env.reverb_decay, 0.1f, 2.f));
	setEffect(AL_REVERB_REFLECTIONS_GAIN, clamp(env.reflect_volume, 0.f, 1.f));
	setEffect(AL_REVERB_REFLECTIONS_DELAY, clamp(env.reflect_delay, 0.f, 300.f) * 0.001F);
	
	return AAL_OK;
}

aalError OpenALBackend::setEffect(ALenum type, float val) {
	
	alEffectf(effect, type, val);
	AL_CHECK_ERROR("setting effect var");
	
	return AAL_OK;
}

#else // HAVE_OPENAL_EFX

aalError OpenALBackend::setReverbEnabled(bool enable) {
	ARX_UNUSED(enable);
	return AAL_ERROR_SYSTEM;
}

aalError OpenALBackend::setRoomRolloffFactor(float factor) {
	ARX_UNUSED(factor);
	return AAL_ERROR_SYSTEM;
}

aalError OpenALBackend::setListenerEnvironment(const Environment & env) {
	ARX_UNUSED(env);
	return AAL_ERROR_SYSTEM;
}

#endif // HAVE_OPENAL_EFX

} // namespace audio
