#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "tlc5000.bin.h"

#define CACHED_CHANNELS_COUNT 9

struct TLC5000Bin
{
	FILE                          *file;
	float                          frame_rate;
	int                            first_cached_frame;
	int                            last_cached_frame;
	int                            frames_count;
	uint16_t                      *magnitudes;
};

TLC5000Bin * tlc5000_bin_open(const char *filename)
{
	TLC5000Bin *priv = malloc(sizeof(TLC5000Bin));
	priv->file = fopen(filename, "rb");
	if (priv->file)
	{
		fseek(priv->file, 0, SEEK_END);
		priv->frames_count = ftell(priv->file) / 512 * 36;
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

void tlc5000_bin_cache(TLC5000Bin *priv, int first_frame, int last_frame)
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
		unsigned char buffer[14];
		int chunk_number = frame / 36;
		int chunk_position = frame - chunk_number * 36;
		fseek(priv->file, chunk_number * 512 + 8 + chunk_position * 14, SEEK_SET);
		fread(&buffer, 1, 14, priv->file);

		priv->magnitudes[(frame - first_frame) * CACHED_CHANNELS_COUNT + 0] = ((buffer[0] & 0x0F) << 8) + buffer[1];
		priv->magnitudes[(frame - first_frame) * CACHED_CHANNELS_COUNT + 1] = ((buffer[0] & 0xF0) << 4) + buffer[2];

		priv->magnitudes[(frame - first_frame) * CACHED_CHANNELS_COUNT + 2] = ((buffer[3] & 0x0F) << 8) + buffer[4];
		priv->magnitudes[(frame - first_frame) * CACHED_CHANNELS_COUNT + 3] = ((buffer[3] & 0xF0) << 4) + buffer[5];

		priv->magnitudes[(frame - first_frame) * CACHED_CHANNELS_COUNT + 4] = ((buffer[6] & 0x0F) << 8) + buffer[7];
		priv->magnitudes[(frame - first_frame) * CACHED_CHANNELS_COUNT + 5] = ((buffer[6] & 0xF0) << 4) + buffer[8];

		priv->magnitudes[(frame - first_frame) * CACHED_CHANNELS_COUNT + 6] = ((buffer[9] & 0x0F) << 8) + buffer[10];
		priv->magnitudes[(frame - first_frame) * CACHED_CHANNELS_COUNT + 7] = ((buffer[9] & 0xF0) << 4) + buffer[11];
		priv->magnitudes[(frame - first_frame) * CACHED_CHANNELS_COUNT + 8] = buffer[12];
	}
}

void tlc5000_bin_clear_cache(TLC5000Bin *priv)
{
	if (priv == NULL)
		return;

	free(priv->magnitudes);
	priv->magnitudes = NULL;
	priv->first_cached_frame = -1;
	priv->last_cached_frame = -1;
}

int tlc5000_bin_read(TLC5000Bin *priv, int channel, int frame)
{
	if (priv == NULL)
		return 0;

	unsigned char buffer[4];
	int chunk_number = frame / 36;
	int chunk_position = frame - chunk_number * 36;
	long int position = chunk_number * 512 + 8 + chunk_position * 14 + (channel / 2 * 3);
	// fprintf(stderr, "bin_read: %d %d %d \n", chunk_number, chunk_position, position);
	fseek(priv->file, position, SEEK_SET);
	if (fread(&buffer, 1, 3, priv->file) != 3)
		return 0;
	if (channel == 8)
		return buffer[0];
	return (channel & 1) ?
		((buffer[0] & 0xF0) << 4) + buffer[2] :
		((buffer[0] & 0x0F) << 8) + buffer[1];
}

int tlc5000_bin_get_integer_magnitude(TLC5000Bin *priv, int channel, int frame)
{
	int magnitude = 0;

	if (priv == NULL)
		return magnitude;

	if ((priv->magnitudes == NULL) ||
	    (priv->first_cached_frame == -1) || 
	    (priv->last_cached_frame == -1) ||
	    (frame < priv->first_cached_frame) ||
	    (frame > priv->last_cached_frame))
		magnitude = tlc5000_bin_read(priv, channel, frame);
	else
		magnitude = priv->magnitudes[(frame - priv->first_cached_frame) * CACHED_CHANNELS_COUNT + channel];

	return magnitude;
}

float tlc5000_bin_get_magnitude(TLC5000Bin *priv, ECGChannel channel, int frame)
{
	if (priv == NULL)
		return 0.0f;

	switch (channel)
	{
	case ECG_CHANNEL_I:
		return tlc5000_bin_get_magnitude(priv, ECG_CHANNEL_II, frame) - tlc5000_bin_get_magnitude(priv, ECG_CHANNEL_III, frame);
	case ECG_CHANNEL_II:
		return (tlc5000_bin_get_integer_magnitude(priv, 0, frame) * -2e-3f) + 4.096f;
	case ECG_CHANNEL_III:
		return (tlc5000_bin_get_integer_magnitude(priv, 1, frame) * -2e-3f) + 4.096f;
	case ECG_CHANNEL_V1:
		return (tlc5000_bin_get_integer_magnitude(priv, 7, frame) * 2e-3f) - 4.096f;
	case ECG_CHANNEL_V2:
		return (tlc5000_bin_get_integer_magnitude(priv, 6, frame) * 2e-3f) - 4.096f;
	case ECG_CHANNEL_V3:
		return (tlc5000_bin_get_integer_magnitude(priv, 5, frame) * 2e-3f) - 4.096f;
	case ECG_CHANNEL_V4:
		return (tlc5000_bin_get_integer_magnitude(priv, 4, frame) * 2e-3f) - 4.096f;
	case ECG_CHANNEL_V5:
		return (tlc5000_bin_get_integer_magnitude(priv, 3, frame) * 2e-3f) - 4.096f;
	case ECG_CHANNEL_V6:
		return (tlc5000_bin_get_integer_magnitude(priv, 2, frame) * 2e-3f) - 4.096f;
	case ECG_CHANNEL_AVR:
		return (tlc5000_bin_get_magnitude(priv, ECG_CHANNEL_I, frame) + tlc5000_bin_get_magnitude(priv, ECG_CHANNEL_II, frame)) / -2.0f;
	case ECG_CHANNEL_AVL:
		return tlc5000_bin_get_magnitude(priv, ECG_CHANNEL_I, frame) - tlc5000_bin_get_magnitude(priv, ECG_CHANNEL_II, frame) / -2.0f;
	case ECG_CHANNEL_AVF:
		return tlc5000_bin_get_magnitude(priv, ECG_CHANNEL_II, frame) - tlc5000_bin_get_magnitude(priv, ECG_CHANNEL_I, frame) / -2.0f;
	case ECG_CHANNEL_X:
		return tlc5000_bin_get_integer_magnitude(priv, 8, frame) / 255.0f;
	}
	return 0.0f;
}

int tlc5000_bin_get_frames_count(TLC5000Bin *priv)
{
	if (priv == NULL)
		return 0;

	return priv->frames_count;
}

float tlc5000_bin_get_frame_rate(TLC5000Bin *priv)
{
	if (priv == NULL)
		1.0f;

	return priv->frame_rate;
}

void tlc5000_bin_close(TLC5000Bin *priv)
{
	if (priv == NULL)
		return;

	fclose(priv->file);
	free(priv->magnitudes);
	free(priv);
	return;
}


