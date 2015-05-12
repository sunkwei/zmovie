#include <stdio.h>
#include <stdlib.h>
#include <webrtc_audio_processing/audio_processing.h>
#include <sndfile.h>
#include <memory.h>

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "usage: %s <wave file name>\n", argv[1]);
		return 1;
	}

	const char *fname = argv[1];

	SF_INFO finfo;
	memset(&finfo, 0, sizeof(finfo));

	SNDFILE *sf = sf_open(fname, SFM_READ, &finfo);
	if (!sf) {
		fprintf(stderr, "ERR: can't open %s\n", fname);
		return -1;
	}

	fprintf(stderr, "INFO: channels=%d, format=%08x, samplerate=%d, frames=%u\n",
			finfo.channels, finfo.format, finfo.samplerate, finfo.frames);

	if (finfo.format != (SF_FORMAT_WAV | SF_FORMAT_PCM_16)) {
		fprintf(stderr, "ERR: only support wav of pcm 16bits\n");
		sf_close(sf);
		return -2;
	}

	webrtc::AudioProcessing *apm = webrtc::AudioProcessing::Create(0);
	fprintf(stderr, "INFO: apm=%p\n", apm);

	apm->set_sample_rate_hz(finfo.samplerate);
	apm->set_num_channels(1, 1);
	apm->voice_detection()->Enable(true);

	short *data = (short*)malloc(finfo.frames * sizeof(short));
	long frames = sf_readf_short(sf, data, finfo.frames);
	fprintf(stderr, "INFO: load %lld frames\n", frames);

	

	sf_close(sf);
	free(data);

	return 0;
}

