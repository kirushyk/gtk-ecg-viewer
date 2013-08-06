#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include "ecg-parser.h"
#define CACHED_CHANNELS_COUNT 9

struct ECG
{
	FILE                          *file;
	int                            first_cached_frame;
	int                            last_cached_frame;
	int                            frames_count;
	uint16_t                      *magnitudes;
};

ECG * ecg_open(const char *filename)
{
	ECG *ecg = malloc(sizeof(ECG));
	ecg->file = fopen(filename, "rb");
	if (ecg->file)
	{
		fseek(ecg->file, 0, SEEK_END);
		ecg->frames_count = ftell(ecg->file) / 512 * 36;
		ecg->first_cached_frame = -1;
		ecg->last_cached_frame = -1;
		ecg->magnitudes = NULL;
	}
	else
	{
		free(ecg);
		ecg = NULL;
	}
	return ecg;
}

void ecg_cache(ECG *ecg, int first_frame, int last_frame)
{
	if (ecg == NULL)
		return;

	int frame;
	if ((last_frame < first_frame) ||
	    (first_frame > ecg->frames_count) ||
	    (last_frame > ecg->frames_count) ||
	    (first_frame < 0) ||
	    (last_frame < 0))
		return;
	free(ecg->magnitudes);
	ecg->first_cached_frame = first_frame;
	ecg->last_cached_frame = last_frame;
	ecg->magnitudes = malloc((last_frame - first_frame + 1) * CACHED_CHANNELS_COUNT * sizeof(uint16_t));
	for (frame = first_frame; frame <= last_frame; frame++)
	{
		unsigned char buffer[14];
		int chunk_number = frame / 36;
		int chunk_position = frame - chunk_number * 36;
		fseek(ecg->file, chunk_number * 512 + 8 + chunk_position * 14, SEEK_SET);
		fread(&buffer, 1, 14, ecg->file);

		ecg->magnitudes[(frame - first_frame) * CACHED_CHANNELS_COUNT + 0] = ((buffer[0] & 0x0F) << 8) + buffer[1];
		ecg->magnitudes[(frame - first_frame) * CACHED_CHANNELS_COUNT + 1] = ((buffer[0] & 0xF0) << 4) + buffer[2];

		ecg->magnitudes[(frame - first_frame) * CACHED_CHANNELS_COUNT + 2] = ((buffer[3] & 0x0F) << 8) + buffer[4];
		ecg->magnitudes[(frame - first_frame) * CACHED_CHANNELS_COUNT + 3] = ((buffer[3] & 0xF0) << 4) + buffer[5];

		ecg->magnitudes[(frame - first_frame) * CACHED_CHANNELS_COUNT + 4] = ((buffer[6] & 0x0F) << 8) + buffer[7];
		ecg->magnitudes[(frame - first_frame) * CACHED_CHANNELS_COUNT + 5] = ((buffer[6] & 0xF0) << 4) + buffer[8];

		ecg->magnitudes[(frame - first_frame) * CACHED_CHANNELS_COUNT + 6] = ((buffer[9] & 0x0F) << 8) + buffer[10];
		ecg->magnitudes[(frame - first_frame) * CACHED_CHANNELS_COUNT + 7] = ((buffer[9] & 0xF0) << 4) + buffer[11];
		ecg->magnitudes[(frame - first_frame) * CACHED_CHANNELS_COUNT + 8] = buffer[12];
	}
}

void ecg_clear_cache(ECG *ecg)
{
	if (ecg == NULL)
		return;

	free(ecg->magnitudes);
	ecg->magnitudes = NULL;
	ecg->first_cached_frame = -1;
	ecg->last_cached_frame = -1;
}

int ecg_read(ECG *ecg, size_t channel, size_t frame) 
{
	if (ecg == NULL)
		return 0;

	unsigned char buffer[4];
	int chunk_number = frame / 36;
	int chunk_position = frame - chunk_number * 36;
	fseek(ecg->file, chunk_number * 512 + 8 + chunk_position * 14 + (channel / 2 * 3), SEEK_SET);
	if (fread(&buffer, 1, 3, ecg->file) != 3)
		return 0;
	if (channel == 8)
		return buffer[0];
	return (channel & 1) ?
		((buffer[0] & 0xF0) << 4) + buffer[2] :
		((buffer[0] & 0x0F) << 8) + buffer[1];

}

int ecg_get_integer_magnitude(ECG *ecg, int channel, int frame)
{
	int magnitude = 0;

	if (ecg == NULL)
		return magnitude;

	if ((ecg->magnitudes == NULL) ||
	    (ecg->first_cached_frame == -1) || 
	    (ecg->last_cached_frame == -1) ||
	    (frame < ecg->first_cached_frame) ||
	    (frame > ecg->last_cached_frame))
		magnitude = ecg_read(ecg, channel, frame);
	else
		magnitude = ecg->magnitudes[(frame - ecg->first_cached_frame) * CACHED_CHANNELS_COUNT + channel];

	return magnitude;
}

float ecg_get_magnitude(ECG *ecg, ECGChannel channel, int frame)
{
	if (ecg == NULL)
		return 0.0f;

	switch (channel)
	{
	case ECG_CHANNEL_I:
		return ecg_get_magnitude(ecg, ECG_CHANNEL_II, frame) - ecg_get_magnitude(ecg, ECG_CHANNEL_III, frame);
	case ECG_CHANNEL_II:
		return (ecg_get_integer_magnitude(ecg, 0, frame) * -2e-3f) + 4.096f;
	case ECG_CHANNEL_III:
		return (ecg_get_integer_magnitude(ecg, 1, frame) * -2e-3f) + 4.096f;
	case ECG_CHANNEL_V1:
		return (ecg_get_integer_magnitude(ecg, 7, frame) * 2e-3f) - 4.096f;
	case ECG_CHANNEL_V2:
		return (ecg_get_integer_magnitude(ecg, 6, frame) * 2e-3f) - 4.096f;
	case ECG_CHANNEL_V3:
		return (ecg_get_integer_magnitude(ecg, 5, frame) * 2e-3f) - 4.096f;
	case ECG_CHANNEL_V4:
		return (ecg_get_integer_magnitude(ecg, 4, frame) * 2e-3f) - 4.096f;
	case ECG_CHANNEL_V5:
		return (ecg_get_integer_magnitude(ecg, 3, frame) * 2e-3f) - 4.096f;
	case ECG_CHANNEL_V6:
		return (ecg_get_integer_magnitude(ecg, 2, frame) * 2e-3f) - 4.096f;
	case ECG_CHANNEL_AVR:
		return (ecg_get_magnitude(ecg, ECG_CHANNEL_I, frame) + ecg_get_magnitude(ecg, ECG_CHANNEL_II, frame)) / -2.0f;
	case ECG_CHANNEL_AVL:
		return ecg_get_magnitude(ecg, ECG_CHANNEL_I, frame) - ecg_get_magnitude(ecg, ECG_CHANNEL_II, frame) / -2.0f;
	case ECG_CHANNEL_AVF:
		return ecg_get_magnitude(ecg, ECG_CHANNEL_II, frame) - ecg_get_magnitude(ecg, ECG_CHANNEL_I, frame) / -2.0f;
	case ECG_CHANNEL_X:
		return ecg_get_integer_magnitude(ecg, 8, frame) / 255.0f;
	}
	return 0.0f;
}

int ecg_get_frames_count(ECG *ecg)
{
	if (ecg == NULL)
		return 0;

	return ecg->frames_count;
}

float ecg_get_frame_rate(ECG *ecg)
{
	return 200.0f;
}

void ecg_close(ECG *ecg)
{
	if (ecg == NULL)
		return;

	free(ecg->magnitudes);
	fclose(ecg->file);
	free(ecg);
	return;
}


