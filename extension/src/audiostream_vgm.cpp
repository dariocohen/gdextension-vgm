#include "audiostream_vgm.h"

#include <climits>

#include <godot_cpp/classes/audio_frame.hpp>
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/classes/time.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/variant/utility_functions.hpp>


// A buffer of about 93ms (at 44100 mix rate)
const int PCM_BUFFER_SIZE = 4096 * 4;

void check_gme_result(int returned_code, String prefix);

AudioStreamVGM::AudioStreamVGM() {
}

Ref<AudioStreamPlayback> AudioStreamVGM::_instantiate_playback() const {
	Ref<AudioStreamPlaybackVGM> playback;
	playback.instantiate();
	playback->setAudioStream(Ref<AudioStreamVGM>(this));
	return playback;
}

void AudioStreamVGM::_bind_methods() {
	// Required by GDCLASS macro
}

#define zeromem(to, count) memset(to, 0, count)

AudioStreamPlaybackVGM::AudioStreamPlaybackVGM()
	: mix_rate(MIN(AudioServer::get_singleton()->get_mix_rate(), 44100)),
	  active(false), last_stats_log(0) {
	// TODO Is locking actually required?
	AudioServer::get_singleton()->lock();
	pcm_buffer = memalloc(PCM_BUFFER_SIZE);
	zeromem(pcm_buffer, PCM_BUFFER_SIZE);
	AudioServer::get_singleton()->unlock();
	UtilityFunctions::print(String("AudioStreamPlaybackVGM will use a mix rate of %d") % (int64_t) mix_rate);
}

AudioStreamPlaybackVGM::~AudioStreamPlaybackVGM() {
	if (pcm_buffer) {
		memfree(pcm_buffer);
		pcm_buffer = NULL;
	}
	if(emu != nullptr) {
		gme_delete(emu);
	}
}

void AudioStreamPlaybackVGM::_bind_methods() {
	// Required by GDCLASS macro
}

double AudioStreamPlaybackVGM::_get_stream_sampling_rate() const {
	return mix_rate;
}

void AudioStreamPlaybackVGM::setAudioStream(Ref<AudioStreamVGM> audioStream) {
	this->audioStream = audioStream;
	if(emu != nullptr) {
		gme_delete(emu);
		emu = nullptr;
	}

	PackedByteArray data = audioStream->get_data();
	auto err = gme_open_data(data.ptr(),data.size(),&emu,mix_rate);
	if(err == 0) {
		err = gme_start_track(emu,0);
	}
	else {
		std::cout << "error opening data\n";
		return;
	}

	gme_info_t* info;
	gme_track_info(emu,&info,0);
	UtilityFunctions::print(String("Loaded vgm named {0}").format(Array::make(info->song)));
}

void AudioStreamPlaybackVGM::_start(double from_pos) {
	gme_start_track(emu,0);
	active = true;
}

void AudioStreamPlaybackVGM::_stop() {
	active = false;
	gme_delete(emu);
}

void AudioStreamPlaybackVGM::_seek(double position) {
	if (position < 0) {
		position = 0;
	}

	// "position" is in seconds, cmp_seek_time expects ms
	gme_seek( emu, position + 1000 );
}

bool AudioStreamPlaybackVGM::_is_playing() const {
	return active;
}

int32_t AudioStreamPlaybackVGM::_mix_resampled(AudioFrame *buffer, int32_t frames) {

	uint64_t start = Time::get_singleton()->get_ticks_usec();

	ERR_FAIL_COND_V(!active, 0);

	// TODO Compute some metrics here to understand typical "frames" value and if we fill the buffer fast enough

	// TODO Consider allocating buffer on stack, it's inexpensive and will accomodate varying "frames" values
	// (but see https://stackoverflow.com/q/24732609/38096)
 
	int bytes_needed = frames * 4; // 2 bytes per sample, stereo

	// TODO What is the max possible value for "frames"?
	if (bytes_needed > PCM_BUFFER_SIZE) {
		String msg = String("bytes_needed ({0}) > PCM_BUFFER_SIZE ({1})").format(Array::make(bytes_needed, PCM_BUFFER_SIZE));
		ERR_FAIL_V_MSG(0, msg);
	}

	// Get the needed number of 16 bits PCM samples
	int play_result = xmp_play_buffer(ctx, pcm_buffer, bytes_needed, 0);
	// gme_play
	if (play_result != 0) {
		check_gme_result(play_result, "playing vgm");
		return 0;
	}

	// Convert samples to Godot format (floats in [-1; 1])
	int16_t *buf = (int16_t *)pcm_buffer;
	for(int i = 0; i < frames; i++) {
		int pos = 2*i;
		float left = float(buf[pos]) / 32767.0;
		float right = float(buf[pos+1]) / 32767.0;
		buffer[i] = { left, right };
	}

	uint64_t end = Time::get_singleton()->get_ticks_usec();
	int elapsed = end - start;

	// Stats bookkeeping
	stats.append(elapsed);
	if (last_stats_log == 0) {
		last_stats_log = end;
	}

	// Log stats once every 5 seconds
	if (end - last_stats_log > 5 * 1000 * 1000) {
		last_stats_log = end;
		int64_t min = INT_MAX;
		int64_t max = 0;
		for (int i=0; i<stats.size(); i++) {
			int item = stats[i];
			min = MIN(min, item);
			max = MAX(max, item);
		}

		// We make the hypothesis that the number of requested frames is constant
		String min_str = String("%3d") % min;
		String max_str = String("%3d") % max;
		UtilityFunctions::print(String(U"During the last 5 seconds, {0} arrays of {1} frames (which is {2}µs) were computed in min {3}µs, max {4}µs")
			.format(Array::make(stats.size(), frames, (int) (frames * 1000 * 1000 / mix_rate), min_str, max_str)));
		stats.clear();
	}

	return frames;
}

void AudioStreamVGM::set_data(const PackedByteArray &p_data) {

	ERR_FAIL_COND_MSG(p_data.is_empty(), "Empty vgm data");

	// We must be stateless (since we are a Resource, so only loaded once),
	// so gme context creation is deferred to AudioStreamPlaybackVGM
	data = p_data;

	ERR_FAIL_COND_MSG(data.is_empty(), "Empty vgm data");
}

const PackedByteArray& AudioStreamVGM::get_data() const {
	return data;
}

String decode_gme_error(int returned_code) {
	switch (returned_code) {
		case -XMP_ERROR_INTERNAL:
			return "internal error";
		case -XMP_ERROR_FORMAT:
			return "unrecognized file format";
		case -XMP_ERROR_LOAD:
			return "error loading file";
		case -XMP_ERROR_DEPACK:
			return "error depacking file";
		case -XMP_ERROR_SYSTEM:
			return "system error";
		case -XMP_ERROR_INVALID:
			return "invalid parameter";
		case -XMP_ERROR_STATE:
			return "invalid player state";
		default:
			return "unknown error";
	}
}

void check_gme_result(int returned_code, String when) {
	if (returned_code == 0) {
		return;
	}

	String err = String("An error occured while {0}: {1}").format(Array::make(when, decode_gme_error(returned_code)));
	UtilityFunctions::printerr(err);
}