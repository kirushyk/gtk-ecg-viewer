#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#include "ecg-parser.h"

int
main(int argc, char *argv[]) {
  printf("tlc ecg parser\n");
  char *bvalue = NULL;
  int c;
  
  opterr = 0;
  
  while ((c = getopt (argc, argv, "b:")) != -1) {
    switch (c) {
    case 'b':
      bvalue = optarg;
      break;
    case '?':
      if (optopt == 'b') {
        fprintf (stderr, "Option -%c requires an argument.\n", optopt);
      } else if (isprint (optopt)) {
        fprintf (stderr, "Unknown option `-%c'.\n", optopt);
      } else {
        fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
      }
      return 1;
    default:
      abort ();
    }
  }

  if (bvalue == NULL) {
    printf("usage: tlc-ecg-parser -b file.bin\n");
    return 0;
  }

  ECG * ecg = ecg_open(bvalue);
  int frame_count = ecg_get_frames_count(ecg);
  float frame_rate = ecg_get_frame_rate(ecg);
  float dt = 1.0f/frame_rate;
  ecg_cache(ecg, 0, frame_count);

  char *ofilename = (char *)malloc(strlen(bvalue)+5);
  sprintf(ofilename, "%s.csv", bvalue);

  FILE *ofile = fopen(ofilename, "w");
  free(ofilename);

  printf("Parsing\n");
  printf("Frames: %i, frame rate: %f\n", frame_count, frame_rate);
  fprintf(ofile, "t, I, II, III, v1, v2, v3, v4, v5, v6, avr, avl, avf\n");
  for (int i = 0; i < frame_count; i++) {
    //printf("Frame: %i\n", i);
    fprintf(ofile, "%f, ", dt*i);
    for (int j = 0; j < ECG_CHANNEL_AVF; j++) {
      fprintf(ofile, "%f, ", ecg_get_magnitude(ecg, j, i));
    }
    fprintf(ofile, "%f\n", ecg_get_magnitude(ecg, ECG_CHANNEL_AVF, i));
  }
  fclose(ofile);
  ecg_close(ecg);
  return 0;
}
