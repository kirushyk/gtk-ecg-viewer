#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <stdint.h>
#include "ecg-parser.h"
#include "tlc5000.bin.h"
#include "tlc5000.ecg.h"

typedef enum ECGFormat
{
    ECG_TYPE_TLC5000_BIN,
    ECG_TYPE_TLC5000_ECG
} ECGFormat;

struct ECG
{
    ECGFormat                      format;
    void                          *priv;
};

ECG * ecg_open(const char *filename)
{
    int length = strlen(filename);
    if (length >= 4)
    {
        if (strcasecmp(filename + length - 4, ".bin") == 0)
        {
            ECG *ecg = malloc(sizeof(ECG));
            ecg->format = ECG_TYPE_TLC5000_BIN;
            ecg->priv = tlc5000_bin_open(filename);
            return ecg;
        }
        else if (strcasecmp(filename + length - 4, ".ecg") == 0)
        {
            ECG *ecg = malloc(sizeof(ECG));
            ecg->format = ECG_TYPE_TLC5000_ECG;
            ecg->priv = tlc5000_ecg_open(filename);
            return ecg;
        }
    }

    fprintf(stderr, "Unknown file format: %s", filename);
    return NULL;

}

void ecg_cache(ECG *ecg, int first_frame, int last_frame)
{
    if (ecg == NULL)
        return;

    if (ecg->priv == NULL)
        return;

    switch (ecg->format)
    {
    case ECG_TYPE_TLC5000_BIN:
        tlc5000_bin_cache(ecg->priv, first_frame, last_frame);
        break;

    case ECG_TYPE_TLC5000_ECG:
        tlc5000_ecg_cache(ecg->priv, first_frame, last_frame);
        break;

    default:
        break;
    }
}

void ecg_clear_cache(ECG *ecg)
{
    if (ecg == NULL)
        return;

    if (ecg->priv == NULL)
        return;

    switch (ecg->format)
    {
    case ECG_TYPE_TLC5000_BIN:
        tlc5000_bin_clear_cache(ecg->priv);
        break;

    case ECG_TYPE_TLC5000_ECG:
        tlc5000_ecg_clear_cache(ecg->priv);
        break;

    default:
        break;
    }
}


float ecg_get_magnitude(ECG *ecg, ECGChannel channel, int frame)
{
    if (ecg == NULL)
        return 0.0f;

    if (ecg->priv == NULL)
        return 0.0f;

    switch (ecg->format)
    {
    case ECG_TYPE_TLC5000_BIN:
        return tlc5000_bin_get_magnitude(ecg->priv, channel, frame);

    case ECG_TYPE_TLC5000_ECG:
        return tlc5000_ecg_get_magnitude(ecg->priv, channel, frame);

    default:
        return 0.0f;
    }

    return 0.0f;
}

int ecg_get_frames_count(ECG *ecg)
{
    if (ecg == NULL)
        return 0;

    if (ecg->priv == NULL)
        return 0;

    switch (ecg->format)
    {
    case ECG_TYPE_TLC5000_BIN:
        return tlc5000_bin_get_frames_count(ecg->priv);

    case ECG_TYPE_TLC5000_ECG:
        return tlc5000_ecg_get_frames_count(ecg->priv);

    default:
        return 0;
    }

    return 0;
}

float ecg_get_frame_rate(ECG *ecg)
{
    if (ecg == NULL)
        return;

    if (ecg->priv == NULL)
        return;

    switch (ecg->format)
    {
    case ECG_TYPE_TLC5000_BIN:
        tlc5000_bin_get_frame_rate(ecg->priv);
        break;

    case ECG_TYPE_TLC5000_ECG:
        tlc5000_ecg_get_frame_rate(ecg->priv);
        break;

    default:
        break;
    }
}

void ecg_close(ECG *ecg)
{
    if (ecg == NULL)
        return;

    if (ecg->priv != NULL)
    {
        switch (ecg->format)
        {
        case ECG_TYPE_TLC5000_BIN:
            tlc5000_bin_close(ecg->priv);
            break;

        case ECG_TYPE_TLC5000_ECG:
            tlc5000_ecg_close(ecg->priv);
            break;

        default:
            break;
        }
    }

    free(ecg);
}

