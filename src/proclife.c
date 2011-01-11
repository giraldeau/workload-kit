/*
 * Proclife. 
 * This program should run through the majority of states a process
 * can enter. 
 * 
 * This file is part of Proclife. Proclife is free software: you can
 * redistribute it and/or modify it under the terms of the GNU 
 * General Public License as published by the Free Software Foundation, 
 * version 2.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


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
		usleep( 100); 
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

int main(int argc, char **argv)
{
        pid_t pids[NUM_PROCESSES];
        int i;
	int samples = NUM_SAMPLES;
	char name_backup[80];
	unsigned char soundByte[NUM_SAMPLES]; 
	strncpy( name_backup, argv[0], 80);
	
	/* Make some forks */
        for (i = NUM_PROCESSES-1; i >= 0; --i) 
	{
		char childName[80];
		sprintf( childName, "trace-child_%d" , i);
		strncpy( argv[0], childName , strlen(argv[0])); 
                pids[i] = fork();
		strncpy( argv[0] , name_backup,strlen(name_backup));
		if( pids[i] == 0)
		{
			
			wait_for_sync( i );
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
	usleep( 250000 );
	{
		char *args[] = {"/bin/ls", "-r", "-t", "-l", (char *) 0 };
		execv("/bin/ls", args);
	}
	/* should not get here. */
	
        return 0;
}

