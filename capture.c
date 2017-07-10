#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

struct au_header_s {
	unsigned int magic;
	unsigned int data_offset;
	unsigned int data_size;
	unsigned int encoding;
	unsigned int sample_rate;
	unsigned int channels;
};

struct au_header_s file_hdr = {
    .magic = 0x2e736e64,
    .data_offset = 24,
    .data_size = 128*1000,
    .encoding = 3,
    .sample_rate = 44100,
    .channels = 1
};

main (int argc, char *argv[])
{
    int i;
    int err;
    short buf[128];
    snd_pcm_t *capture_handle;
    snd_pcm_hw_params_t *hw_params;

    if ((err = snd_pcm_open (&capture_handle, argv[1],
                             SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        fprintf (stderr, "cannot open audio device %s (%s)\n",
                 argv[1],
                 snd_strerror (err));
        exit (1);
    }
    if ((err = snd_pcm_set_params(capture_handle,
                                  SND_PCM_FORMAT_S16_LE,
                                  SND_PCM_ACCESS_RW_INTERLEAVED,
                                  1,
                                  44100,
                                  0, //int  	soft_resample,
                                  0)) < 0) //unsigned int  	latency
    {
        fprintf (stderr, "cannot set params (%s)\n", snd_strerror(err));
        exit (1);
    }

    if ((err = snd_pcm_prepare (capture_handle)) < 0) {
        fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
                 snd_strerror (err));
        exit (1);
    }

    puts("Read Now!");
    write(fileno(stdout), &file_hdr, sizeof(file_hdr));
    for (i = 0; i < 1000; ++i) {
        if ((err = snd_pcm_readi(capture_handle, buf, 128)) != 128) {
            fprintf (stderr, "read from audio interface failed (%s)\n",
                     snd_strerror (err));
            exit (1);
        } else {
	    write(fileno(stdout), buf, 128);
	}
    }

    snd_pcm_close (capture_handle);
    exit (0);
}
