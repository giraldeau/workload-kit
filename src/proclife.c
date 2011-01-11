#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <alsa/asoundlib.h>
#define NUM_PROCESSES 10
#define NUM_SAMPLES 8000

const int rate = 8000; 
const int bits = sizeof( unsigned char ) * 8 ; 
const int channels = 1; 

void rename_process( int currentId, char **argv) 
{
	char newName[80] ; 
	int argvSize ; 
	sprintf( newName, "Trace Child %d" , currentId) ;
	argvSize = strlen( argv[ 0 ] );
	strncpy( argv[0] , newName, argvSize ) ;
	
}

void allocate_memory()
{
	int *dataDump;
	int i;
 	/* Allocate some memory */ 
	for( i = 10 ; i >= 0; --i)
	{
		dataDump = (int *)malloc( i << 16 ); 
		usleep( 100); 
		free( dataDump);
	}
}

void wait_for_sync(int currentId)
{
	/* Sleep to offset yourself */ 
	{
		printf( "i = %d, sleeping for %f seconds\n", currentId, currentId+.5 ); 
                usleep( currentId*1000000 + 500 );
	}
}


/* Access some files */ 
void write_to_file( unsigned char *soundByte, int samples, int currentId ) 
{
	int j = 0;
	char fileName[80]; 
	sprintf( fileName, "/tmp/tempFile%d.tmp" , currentId);
	FILE *f = fopen(fileName, "w");
	for( j = 0; j < samples; j++) 
	{
		fprintf( f, "%d, %d\n", j,soundByte[j] ); 
	}
	fclose( f) ;
}

void set_up_sound( unsigned char *soundByte, int samples, int currentId )
{
	int i;
/* set up sound */
	for( i = 0; i < samples ; i++) 
	{
		float soundVal = 127 * sin(110.0 * ( currentId + 1 ) *
				       2.0 * 3.141592 / (float)rate * (float)i ) ; 
		soundByte[i] = soundVal + 127; 
	}
}

/*
 * Play some audio
 * inspired by 
 * http://www.oreilly.de/catalog/multilinux/excerpt/ch14-05.htm
 * http://www.equalarea.com/paul/alsa-audio.html
 * http://www.alsa-project.org/alsa-doc/alsa-lib/_2test_2pcm__min_8c-example.html#a9
 */
void play_sound( unsigned char *soundByte, int samples )
{

	int status = 0; 
	snd_pcm_t *playback_handle = NULL;
	snd_pcm_hw_params_t *hw_params = NULL;

	if ((status = snd_pcm_open (&playback_handle, "default", SND_PCM_STREAM_PLAYBACK, 0 )) < 0) {
		fprintf (stderr, "cannot open audio device %s (%s)\n", 
			 "default",
			 snd_strerror (status));
	}
	else if ((status = snd_pcm_set_params(playback_handle,
                               SND_PCM_FORMAT_U8,
                               SND_PCM_ACCESS_RW_INTERLEAVED,
                               1,
                               8000,
                               0,
                               500000)) < 0) 
	{
 		printf("Playback open error: %s\n", snd_strerror(status));
	}
	else
	{	
		printf( "Opened sound card\n" );
	        snd_pcm_wait( playback_handle, -1 ) ;

		if ((status = snd_pcm_writei (playback_handle, soundByte, samples)) != samples) {
			fprintf (stderr, "write to audio interface failed (%s)\n",
				 snd_strerror (status));
		}
	}
	snd_pcm_hw_params_free (hw_params);
	if( playback_handle != NULL )
		snd_pcm_close (playback_handle);
}

void call_ls()
{
	execl ("/bin/ls", "ls", "-1", (char *)0);
}

int main(int argc, char **argv)
{
        pid_t pids[NUM_PROCESSES];
        int i;
	int samples = NUM_SAMPLES;
	unsigned char soundByte[NUM_SAMPLES]; 

	
	/* Make some forks */
        for (i = NUM_PROCESSES-1; i >= 0; --i) 
	{
		char childName[80];
		sprintf( childName, "trace-child_%d" , i);
		strncpy( argv[0], childName , strlen(argv[0])); 
                pids[i] = fork();
		if( pids[i] == 0)
		{
			
			wait_for_sync( i );
			rename_process( i , argv );
			set_up_sound( soundByte, samples, i ) ;
			allocate_memory();
			write_to_file( soundByte, samples, i ) ;
			play_sound( soundByte, samples ) ;
			printf( "Done!\n") ;
	                _exit(0) ;
		}
        }	
	/* parent section */
	set_up_sound( soundByte, samples, 0 ) ;
	allocate_memory();
	write_to_file( soundByte, samples, 0 ) ;
	play_sound( soundByte, samples ) ;
	printf( "Done!\n") ;
        for (i =  NUM_PROCESSES - 1 ; i >= 0; --i)
	{	
		printf( "Waiting for process %d\n" , i);
                waitpid(pids[i], NULL, 0);
		printf( "Done!\n" );
	}

        return 0;
}
