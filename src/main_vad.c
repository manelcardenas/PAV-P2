#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sndfile.h>
        
#include "vad.h"
#include "vad_docopt.h"
#include "pav_analysis.h"

#define DEBUG_VAD 0x1

int main(int argc, char *argv[]) {
  int verbose = 0; /* To show internal state of vad: verbose = DEBUG_VAD; */

  SNDFILE *sndfile_in, *sndfile_out = 0;
  SF_INFO sf_info;
  FILE *vadfile;
  int n_read = 0, i,rm=0;

  VAD_DATA *vad_data;
  VAD_STATE state, last_state, last_defined_state;

  float *buffer, *buffer_zeros, *bufferaux;
  int frame_size;         /* in samples */
  float frame_duration;   /* in seconds */
  unsigned int t, last_t,last_defined_t; /* in frames */

  char	*input_wav, *output_vad, *output_wav;

  DocoptArgs args = docopt(argc, argv, /* help */ 1, /* version */ "2.0");

  verbose    = args.verbose ? DEBUG_VAD : 0;
  input_wav  = args.input_wav;
  output_vad = args.output_vad;
  output_wav = args.output_wav;

  if (input_wav == 0 || output_vad == 0) {
    fprintf(stderr, "%s\n", args.usage_pattern);
    return -1;
  }

  /* Open input sound file */
  if ((sndfile_in = sf_open(input_wav, SFM_READ, &sf_info)) == 0) {
    fprintf(stderr, "Error opening input file %s (%s)\n", input_wav, strerror(errno));
    return -1;
  }

  if (sf_info.channels != 1) {
    fprintf(stderr, "Error: the input file has to be mono: %s\n", input_wav);
    return -2;
  }

  /* Open vad file */
  if ((vadfile = fopen(output_vad, "wt")) == 0) {
    fprintf(stderr, "Error opening output vad file %s (%s)\n", output_vad, strerror(errno));
    return -1;
  }

  /* Open output sound file, with same format, channels, etc. than input */
  if (output_wav) {
    if ((sndfile_out = sf_open(output_wav, SFM_WRITE, &sf_info)) == 0) {
      fprintf(stderr, "Error opening output wav file %s (%s)\n", output_wav, strerror(errno));
      return -1;
    }
  }

  vad_data = vad_open(sf_info.samplerate);
  /* Allocate memory for buffers */
  frame_size   = vad_frame_size(vad_data);
  buffer       = (float *) malloc(frame_size * sizeof(float));
  buffer_zeros = (float *) malloc(frame_size * sizeof(float));

  bufferaux= (float *) malloc(frame_size * sizeof(float));
  
  for (i=0; i< frame_size; ++i) buffer_zeros[i] = 0.0F;
  
  frame_duration = (float) frame_size/ (float) sf_info.samplerate;
  //last_state = ST_SILENCE; //esto se tendra que modificar creo

  for (t = last_t = 0; ; t++) { /* For each frame ... */
    /* End loop when file has finished (or there is an error) */
    if  ((n_read = sf_read_float(sndfile_in, buffer, frame_size)) != frame_size) break;

    if (sndfile_out != 0) {
      /* TODO: copy all the samples into sndfile_out */
      //sf_write_float(sndfile_out,buffer,frame_size);
    }

    state = vad(vad_data, buffer);
    if (verbose & DEBUG_VAD) vad_show_state(vad_data, stdout);
    last_defined_t = t - last_t; 
    /* TODO: print only SILENCE and VOICE labels */
    /* As it is, it prints UNDEF segments but is should be merge to the proper value */
    if(last_state == ST_VOICE || last_state == ST_SILENCE){
      last_defined_state = last_state;    //creo que es necesario para poder definir el ultimo estado que nos interesa
      //last_defined_t=t-last_t;
    }
    
  

    /*if(last_state == state && state == ST_MbSILENCE){
        last_state=ST_SILENCE;
        last_defined_state = last_state;//no creo que sea necesario
    }
   
    if(last_state == state && state == ST_MbVOICE){
      last_state=ST_VOICE;
      last_defined_state = last_state;
    }*/

    if (state != last_state) {
      if(state == ST_SILENCE){
        if(last_defined_state == ST_SILENCE){
          last_state = ST_SILENCE;
        }
        if(last_defined_state == ST_VOICE){
          last_state = ST_SILENCE;  //podria ser al reves ya que es justo entre final de v a s 
        }
      }
      if(state == ST_VOICE){
        if(last_defined_state == ST_SILENCE){
          last_state = ST_VOICE;
        }
        if(last_defined_state == ST_VOICE){
          last_state = ST_VOICE;
        }
      }
      /*if(state== ST_MbSILENCE && last_defined_t<2){
          
      }*/
      if (t != last_t)
        //fprintf(vadfile, "%.5f\t%.5f\t%s\tLDS: %s\tSTATE: %s\tf.p:%f\tk1:%f\t%f\t%f\n", last_t * frame_duration, t * frame_duration, state2str(last_state),state2str(last_defined_state),state2str(state),vad_data->last_feature,vad_data->k1,*buffer,*buffer_zeros);
        //fprintf(vadfile, "%d\t%d\t%d\tlast_state: %s\tLDS: %s\tSTATE:%s\t%f\n", last_t, t,last_defined_t, state2str(last_state),state2str(last_defined_state),state2str(state),frame_duration);
        fprintf(vadfile, "%.5f\t%.5f\t%s\n", last_t * frame_duration, t * frame_duration, state2str(last_state));
       
    /*if(last_defined_t<2 && last_state != last_defined_state){
      last_state=last_defined_state;////estoy intentando tener en cuenta el tiempo
      //fprintf(vadfile,"hola");
    }*/
      last_state = state;
      last_t = t;
    } 
    if(last_state==ST_SILENCE){
        bufferaux=buffer_zeros;
        //sf_write_float(sndfile_out,buffer_zeros,frame_size);
      }
      else bufferaux=buffer;

    if (sndfile_out != 0) {
      /* TODO: go back and write zeros in silence segments */
     
      sf_write_float(sndfile_out,bufferaux,frame_size);
    }
    rm++;
  }

  state = vad_close(vad_data);
  //state=ST_SILENCE;
  /* TODO: what do you want to print, for last frames? */
  if (t != last_t)
    fprintf(vadfile, "%.5f\t%.5f\t%s\n", last_t * frame_duration, t * frame_duration + n_read / (float) sf_info.samplerate, state2str(state));

  /* clean up: free memory, close open files */
  free(buffer);
  free(buffer_zeros);
  sf_close(sndfile_in);
  fclose(vadfile);
  if (sndfile_out) sf_close(sndfile_out);
  return 0;
}
