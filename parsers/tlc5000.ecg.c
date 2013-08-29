#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "tlc5000.ecg.h"

#define CACHED_CHANNELS_COUNT 8

struct TLC5000Ecg
{
	FILE                          *file;
	float                          frame_rate;
	int                            first_cached_frame;
	int                            last_cached_frame;
	int                            frames_count;
	uint16_t                      *magnitudes;
};

TLC5000Ecg * tlc5000_ecg_open(const char *filename)
{
	TLC5000Ecg *priv = malloc(sizeof(TLC5000Ecg));
	priv->file = fopen(filename, "rb");
	if (priv->file)
	{
		fseek(priv->file, 0, SEEK_END);
		priv->frames_count = ftell(priv->file) / 12;
		priv->first_cached_frame = -1;
		priv->last_cached_frame = -1;
		priv->magnitudes = NULL;
		priv->frame_rate = 200.0f;
	}
	else
	{
		free(priv);
		priv = NULL;
	}
	return priv;
}

void tlc5000_ecg_cache(TLC5000Ecg *priv, int first_frame, int last_frame)
{
	if (priv == NULL)
		return;

	int frame;
	if ((last_frame < first_frame) ||
	    (first_frame > priv->frames_count) ||
	    (last_frame > priv->frames_count) ||
	    (first_frame < 0) ||
	    (last_frame < 0))
		return;
	free(priv->magnitudes);

	priv->first_cached_frame = first_frame;
	priv->last_cached_frame = last_frame;
	priv->magnitudes = malloc((last_frame - first_frame + 1) * CACHED_CHANNELS_COUNT * sizeof(uint16_t));
	for (frame = first_frame; frame <= last_frame; frame++)
	{
		unsigned char buffer[12];
		fseek(priv->file, frame * 12, SEEK_SET);
		fread(&buffer, 1, 12, priv->file);

		priv->magnitudes[(frame - first_frame) * CACHED_CHANNELS_COUNT + 0] = ((buffer[0] & 0x0F) << 8) + buffer[1];
		priv->magnitudes[(frame - first_frame) * CACHED_CHANNELS_COUNT + 1] = ((buffer[0] & 0xF0) << 4) + buffer[2];

		priv->magnitudes[(frame - first_frame) * CACHED_CHANNELS_COUNT + 2] = ((buffer[3] & 0x0F) << 8) + buffer[4];
		priv->magnitudes[(frame - first_frame) * CACHED_CHANNELS_COUNT + 3] = ((buffer[3] & 0xF0) << 4) + buffer[5];

		priv->magnitudes[(frame - first_frame) * CACHED_CHANNELS_COUNT + 4] = ((buffer[6] & 0x0F) << 8) + buffer[7];
		priv->magnitudes[(frame - first_frame) * CACHED_CHANNELS_COUNT + 5] = ((buffer[6] & 0xF0) << 4) + buffer[8];

		priv->magnitudes[(frame - first_frame) * CACHED_CHANNELS_COUNT + 6] = ((buffer[9] & 0x0F) << 8) + buffer[10];
		priv->magnitudes[(frame - first_frame) * CACHED_CHANNELS_COUNT + 7] = ((buffer[9] & 0xF0) << 4) + buffer[11];
	}
}

void tlc5000_ecg_clear_cache(TLC5000Ecg *priv)
{
	if (priv == NULL)
		return;

	free(priv->magnitudes);
	priv->magnitudes = NULL;
	priv->first_cached_frame = -1;
	priv->last_cached_frame = -1;
}

int tlc5000_ecg_read(TLC5000Ecg *priv, int channel, int frame)
{
	if (priv == NULL)
		return 0;

	int value;
	uint8_t buffer[2];

	fseek(priv->file, frame * 24, SEEK_SET);
	if (fread(buffer, 1, 2, priv->file) != 2)
		return 0;

	value = (buffer[1] << 8) + (buffer[0]);
 
	return (channel == 8) ? 0 : value;
}

int tlc5000_ecg_get_integer_magnitude(TLC5000Ecg *priv, int channel, int frame)
{
	int magnitude = 0;

	if (priv == NULL)
		return magnitude;

	magnitude = tlc5000_ecg_read(priv, channel, frame);

	/*	
	if ((priv->magnitudes == NULL) ||
	    (priv->first_cached_frame == -1) || 
	    (priv->last_cached_frame == -1) ||
	    (frame < priv->first_cached_frame) ||
	    (frame > priv->last_cached_frame))
		magnitude = tlc5000_ecg_read(priv, channel, frame);
	else
		magnitude = priv->magnitudes[(frame - priv->first_cached_frame) * CACHED_CHANNELS_COUNT + channel];
	*/

	return magnitude;
}

float tlc5000_ecg_get_magnitude(TLC5000Ecg *priv, ECGChannel channel, int frame)
{
	if (priv == NULL)
		return 0.0f;

	switch (channel)
	{
	case ECG_CHANNEL_I:
		return (tlc5000_ecg_get_integer_magnitude(priv, 0, frame) * -2e-3f) + 4.096f;
	case ECG_CHANNEL_II:
		return (tlc5000_ecg_get_integer_magnitude(priv, 1, frame) * -2e-3f) + 4.096f;
	case ECG_CHANNEL_III:
		return (tlc5000_ecg_get_integer_magnitude(priv, 2, frame) * -2e-3f) + 4.096f;
	case ECG_CHANNEL_V1:
		return (tlc5000_ecg_get_integer_magnitude(priv, 8, frame) * 2e-3f) - 4.096f;
	case ECG_CHANNEL_V2:
		return (tlc5000_ecg_get_integer_magnitude(priv, 7, frame) * 2e-3f) - 4.096f;
	case ECG_CHANNEL_V3:
		return (tlc5000_ecg_get_integer_magnitude(priv, 6, frame) * 2e-3f) - 4.096f;
	case ECG_CHANNEL_V4:
		return (tlc5000_ecg_get_integer_magnitude(priv, 5, frame) * 2e-3f) - 4.096f;
	case ECG_CHANNEL_V5:
		return (tlc5000_ecg_get_integer_magnitude(priv, 4, frame) * 2e-3f) - 4.096f;
	case ECG_CHANNEL_V6:
		return (tlc5000_ecg_get_integer_magnitude(priv, 3, frame) * 2e-3f) - 4.096f;
	case ECG_CHANNEL_AVR:
		return (tlc5000_ecg_get_integer_magnitude(priv, 9, frame) * -2e-3f) + 4.096f;
	case ECG_CHANNEL_AVL:
		return (tlc5000_ecg_get_integer_magnitude(priv, 10, frame) * -2e-3f) + 4.096f;
	case ECG_CHANNEL_AVF:
		return (tlc5000_ecg_get_integer_magnitude(priv, 11, frame) * -2e-3f) + 4.096f;
	case ECG_CHANNEL_X:
		return 0.0f;
	}
	return 0.0f;
}

int tlc5000_ecg_get_frames_count(TLC5000Ecg *priv)
{
	if (priv == NULL)
		return 0;

	return priv->frames_count;
}

float tlc5000_ecg_get_frame_rate(TLC5000Ecg *priv)
{
	if (priv == NULL)
		1.0f;

	return priv->frame_rate;
}

void tlc5000_ecg_close(TLC5000Ecg *priv)
{
	if (priv == NULL)
		return;

	fclose(priv->file);
	free(priv->magnitudes);
	free(priv);
	return;
}


