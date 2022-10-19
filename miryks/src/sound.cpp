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

	wav_file load_wav_file(RES);

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
		RES res = bsa_find_more("sound\\fx\\npc\\draugr\\aware\\npc_draugr_aware_01.wav", 0x8);
		bsa_read(res);

		wav_file wav = load_wav_file(res);

		ALenum format;
		if(wav.channels == 1 && wav.bits_per_sample == 8)
			format = AL_FORMAT_MONO8;
		else if(wav.channels == 1 && wav.bits_per_sample == 16)
			format = AL_FORMAT_MONO16;
		else if(wav.channels == 2 && wav.bits_per_sample == 8)
			format = AL_FORMAT_STEREO8;
		else if(wav.channels == 2 && wav.bits_per_sample == 16)
			format = AL_FORMAT_STEREO16;
			
		ALuint buffer;
		alGenBuffers(1, &buffer);
		alBufferData(buffer, format, wav.sound_data, wav.size_of_data, wav.sample_rate);

		ALuint source;
		alGenSources(1, &source);
		alSourcef(source, AL_GAIN, 1);
		alSourcef(source, AL_PITCH, 1);

		alSourcei(source, AL_BUFFER, buffer);
    	alSourcePlay(source);

		ALint state = AL_PLAYING;

		/*while(state == AL_PLAYING)
		{
			alGetSourcei(source, AL_SOURCE_STATE, &state);
		}*/

	};

	wav_file load_wav_file(RES res)
	{
		wav_file wav;

		memcpy(&wav.buffer[0], res->buf, 4);
		printf(" RIFF is %.4s \n", wav.buffer);

		memcpy(&wav.filesize, res->buf + 4, 4);
		printf(" size of file %i \n", wav.filesize);

		memcpy(&wav.WAVE[0], res->buf + 8, 4);
		printf(" WAVE is %.4s \n", wav.WAVE);

		memcpy(&wav.fmt0[0], res->buf + 12, 4);
		printf(" fmt/0 is %.4s \n", wav.fmt0);

		memcpy(&wav.fmt_data_chunk, res->buf + 16, 4);
		printf(" fmt data chunk is %i \n", wav.fmt_data_chunk);

		memcpy(&wav.pcm, res->buf + 20, 2);
		printf(" pcm is %i \n", wav.pcm);

		memcpy(&wav.channels, res->buf + 22, 2);
		printf(" channels is %i \n", wav.channels);

		memcpy(&wav.sample_rate, res->buf + 24, 4);
		printf(" sample_rate is %i \n", wav.sample_rate);

		memcpy(&wav.conversion, res->buf + 28, 4);
		printf(" conversion is %i \n", wav.conversion);

		memcpy(&wav.dafaq, res->buf + 32, 2);
		printf(" dafaq is %i \n", wav.dafaq);

		memcpy(&wav.bits_per_sample, res->buf + 34, 2);
		printf(" bits_per_sample is %i \n", wav.bits_per_sample);

		memcpy(&wav.data_chunk_header, res->buf + 36, 4);
		printf(" data_chunk_header is %.4s \n", wav.data_chunk_header);

		memcpy(&wav.size_of_data, res->buf + 40, 4);
		printf(" size_of_data is %i \n", wav.size_of_data);

		printf("before malloc\n");
		// int calculation
		wav.sound_data = (char *)malloc(wav.size_of_data + 1);
		if (wav.sound_data)
			printf("we got enough %i\n", wav.sound_data);
		printf("after malloc\n");
		memcpy(&wav.sound_data[0], res->buf + 44, wav.size_of_data);
		printf("after memcpy\n");

		return wav;
	}
}