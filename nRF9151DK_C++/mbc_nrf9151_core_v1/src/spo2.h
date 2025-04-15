#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <stdint.h>

#define FreqS 25    // Sampling frequency
#define BUFFER_SIZE (FreqS * 4)
#define MA4_SIZE 4  // DO NOT CHANGE

#ifndef min
#define min(x,y) ((x) < (y) ? (x) : (y))
#endif

// Lookup table for SpO2 calculation
extern const uint8_t uch_spo2_table[184];

// Buffers for internal use (IR and RED signals)
extern int32_t an_x[BUFFER_SIZE]; // IR buffer
extern int32_t an_y[BUFFER_SIZE]; // RED buffer

// Function to compute heart rate and SpO2
void maxim_heart_rate_and_oxygen_saturation(
    uint32_t *pun_ir_buffer,
    int32_t n_ir_buffer_length,
    uint32_t *pun_red_buffer,
    int32_t *pn_spo2,
    int8_t *pch_spo2_valid,
    int32_t *pn_heart_rate,
    int8_t *pch_hr_valid
);

// Utilities for peak detection and sorting
void maxim_find_peaks(
    int32_t *pn_locs,
    int32_t *n_npks,
    int32_t *pn_x,
    int32_t n_size,
    int32_t n_min_height,
    int32_t n_min_distance,
    int32_t n_max_num
);

void maxim_peaks_above_min_height(
    int32_t *pn_locs,
    int32_t *n_npks,
    int32_t *pn_x,
    int32_t n_size,
    int32_t n_min_height
);

void maxim_remove_close_peaks(
    int32_t *pn_locs,
    int32_t *pn_npks,
    int32_t *pn_x,
    int32_t n_min_distance
);

void maxim_sort_ascend(
    int32_t *pn_x,
    int32_t n_size
);

void maxim_sort_indices_descend(
    int32_t *pn_x,
    int32_t *pn_indx,
    int32_t n_size
);

#endif // ALGORITHM_H
