#ifndef _TLC5000_BIN_H_
#   define _TLC5000_BIN_H_
#   include <ecg-parser.h>

typedef struct TLC5000Bin TLC5000Bin;

TLC5000Bin *       tlc5000_bin_open                       (const char         *filename);

void               tlc5000_bin_cache                      (TLC5000Bin         *priv,
                                                           int                 first_frame,
                                                           int                 last_frame);

void               tlc5000_bin_clear_cache                (TLC5000Bin         *priv);

float              tlc5000_bin_get_magnitude              (TLC5000Bin         *priv,
                                                           ECGChannel          channel,
                                                           int                 frame);

int                tlc5000_bin_get_frames_count           (TLC5000Bin         *priv);

float              tlc5000_bin_get_frame_rate             (TLC5000Bin         *priv);

void               tlc5000_bin_close                      (TLC5000Bin         *priv);

#endif

