typedef struct ECG ECG;

typedef enum ECGChannel
{
	ECG_CHANNEL_I,
	ECG_CHANNEL_II,
	ECG_CHANNEL_III,
	ECG_CHANNEL_V1,
	ECG_CHANNEL_V2,
	ECG_CHANNEL_V3,
	ECG_CHANNEL_V4,
	ECG_CHANNEL_V5,
	ECG_CHANNEL_V6,
	ECG_CHANNEL_AVR,
	ECG_CHANNEL_AVL,
	ECG_CHANNEL_AVF,
	ECG_CHANNEL_X
} ECGChannel;

ECG *              ecg_open                               (const char         *filename);

void               ecg_cache                              (ECG                *ecg,
                                                           int                 first_frame,
                                                           int                 last_frame);

void               ecg_clear_cache                        (ECG                *ecg);

float              ecg_get_magnitude                      (ECG                *ecg,
                                                           ECGChannel          channel,
                                                           int                 frame);

int                ecg_get_frames_count                   (ECG                *ecg);

float              ecg_get_frame_rate                     (ECG                *ecg);

void               ecg_close                              (ECG                *ecg);

