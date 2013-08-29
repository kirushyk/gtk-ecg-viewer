#ifndef _TLC5000_ECG_H_
#	define _TLC5000_ECG_H_
#	include <ecg-parser.h>

typedef struct TLC5000Ecg TLC5000Ecg;

TLC5000Ecg *       tlc5000_ecg_open                       (const char         *filename);

void               tlc5000_ecg_cache                      (TLC5000Ecg         *priv,
                                                           int                 first_frame,
                                                           int                 last_frame);

void               tlc5000_ecg_clear_cache                (TLC5000Ecg         *priv);

float              tlc5000_ecg_get_magnitude              (TLC5000Ecg         *priv,
                                                           ECGChannel          channel,
                                                           int                 frame);

int                tlc5000_ecg_get_frames_count           (TLC5000Ecg         *priv);

float              tlc5000_ecg_get_frame_rate             (TLC5000Ecg         *priv);

void               tlc5000_ecg_close                      (TLC5000Ecg         *priv);

#endif

