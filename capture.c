#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <netinet/in.h>
#include <alsa/asoundlib.h>

#define BUF_SIZE	8000

struct au_header_s {
	uint32_t magic;
	uint32_t hdr_size;
	uint32_t data_size;
	uint32_t encoding;
	uint32_t sample_rate;
	uint32_t channels;
};


int main (int argc, char *argv[])
{
    int i;
    int ordi;
    int err;
    struct au_header_s au;
    uint16_t buf[BUF_SIZE];
    snd_pcm_t *capture_handle;

    if ((err = snd_pcm_open (&capture_handle, argv[1],
                             SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        fprintf (stderr, "cannot open audio device %s (%s)\n",
                 argv[1],
                 snd_strerror (err));
        exit (1);
    }
    if ((err = snd_pcm_set_params(capture_handle,
                                  SND_PCM_FORMAT_S16_BE,
                                  SND_PCM_ACCESS_RW_INTERLEAVED,
                                  1,
                                  8000,
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

    au.magic = htonl(0x2e736e64);
    au.hdr_size = htonl(18);
    au.data_size = htonl(BUF_SIZE*1000);
    au.encoding = htonl(3);
    au.sample_rate = htonl(8000);
    au.channels = htonl(1);

    puts("Read Now!");
    write(fileno(stdout), &au, sizeof(au));
    for (i = 0; i < 1000; ++i) {
        if ((err = snd_pcm_readi(capture_handle, buf, BUF_SIZE)) != BUF_SIZE) {
            fprintf (stderr, "read from audio interface failed (%s)\n",
                     snd_strerror (err));
            exit (1);
        } else {
	  //  for (ordi = 0; ordi < BUF_SIZE; ordi++)
          //      buf[ordi] = htons(buf[ordi]); 
	    write(fileno(stdout), buf, BUF_SIZE);
	}
    }

    snd_pcm_close (capture_handle);
    exit (0);
}
