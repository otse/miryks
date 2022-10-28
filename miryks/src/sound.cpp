#include <AL/al.h>
#include <AL/alext.h>
#include <miryks/sound.h>

namespace miryks
{
	struct wav_file
	{
		char buffer[4] = {'\0'};
		uint32_t filesize;
		char WAVE[4];
		char fmt0[5];
		uint32_t fmt_data_chunk;
		uint16_t pcm;
		uint16_t channels;
		uint32_t sample_rate;
		uint32_t conversion;
		uint16_t dafaq;
		uint16_t bits_per_sample;
		char data_chunk_header[4];
		uint32_t size_of_data;
		char *sound_data;
	};

	struct save {
		ALuint buffer;
	};

	wav_file load_wav_file(RES);

	static ALuint make_or_get(const char *path) {
		static std::map<const char *, save> saves;
		auto find = saves.find(path);
		if (find == saves.end())
		{
			save save;

			RES res = bsa_find_more(path, 0x8);
			if (!res)
			{
				printf(" bsa find more no such wav \n");
				return 0;
			}
			bsa_read(res);
			wav_file wav = load_wav_file(res);

			ALenum format;
			if (wav.channels == 1 && wav.bits_per_sample == 8)
				format = AL_FORMAT_MONO8;
			else if (wav.channels == 1 && wav.bits_per_sample == 16)
				format = AL_FORMAT_MONO16;
			else if (wav.channels == 2 && wav.bits_per_sample == 8)
				format = AL_FORMAT_STEREO8;
			else if (wav.channels == 2 && wav.bits_per_sample == 16)
				format = AL_FORMAT_STEREO16;

			alGenBuffers(1, &save.buffer);
			alBufferData(save.buffer, format, wav.sound_data, wav.size_of_data, wav.sample_rate);
			
			saves.emplace(path, save);
			return save.buffer;
		}
		else
		{
			save &save = find->second;
			return save.buffer;
		}
	}

	sound::sound(const char *path)
	{
		buffer = make_or_get(path);
		alGenSources(1, &source);
		alSourcei(source, AL_BUFFER, buffer);
	}

	void sound::play()
	{
		alSourcef(source, AL_GAIN, 1);
		alSourcef(source, AL_PITCH, 1);
		alSourcePlay(source);
	}

	bool sound::is_playing()
	{
		ALint state;
		alGetSourcei(source, AL_SOURCE_STATE, &state);
		return state == AL_PLAYING;
	}

	void sounds_init()
	{
		printf(" sounds init \n");

		ALCdevice *device = alcOpenDevice(NULL);
		ALCcontext *context = alcCreateContext(device, NULL);
		alcMakeContextCurrent(context);

		alListenerf(AL_GAIN, 1);
		/*ALfloat listenerOri[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};

		alListener3f(AL_POSITION, 0, 0, 1.0f);
		alListener3f(AL_VELOCITY, 0, 0, 0);
		alListenerfv(AL_ORIENTATION, listenerOri);*/

		// SF_INFO sfinfo;
		//sound aware("sound\\fx\\npc\\draugr\\aware\\npc_draugr_aware_01.wav");
		//aware.play();
		sound menu("sound\\fx\\ui\\ui_menu_ok.wav");
		menu.play();
	};

	wav_file load_wav_file(RES res)
	{
		wav_file wav;

		memcpy(&wav.buffer[0], res->buf, 4);
		memcpy(&wav.filesize, res->buf + 4, 4);
		memcpy(&wav.WAVE[0], res->buf + 8, 4);
		memcpy(&wav.fmt0[0], res->buf + 12, 4);
		memcpy(&wav.fmt_data_chunk, res->buf + 16, 4);
		memcpy(&wav.pcm, res->buf + 20, 2);
		memcpy(&wav.channels, res->buf + 22, 2);
		memcpy(&wav.sample_rate, res->buf + 24, 4);
		memcpy(&wav.conversion, res->buf + 28, 4);
		memcpy(&wav.dafaq, res->buf + 32, 2);
		memcpy(&wav.bits_per_sample, res->buf + 34, 2);
		memcpy(&wav.data_chunk_header, res->buf + 36, 4);
		memcpy(&wav.size_of_data, res->buf + 40, 4);

		wav.sound_data = (char *)malloc(wav.size_of_data + 1);
		memcpy(&wav.sound_data[0], res->buf + 44, wav.size_of_data);

		return wav;
	}
}