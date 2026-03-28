#ifndef SYNTHESIZER_H
#define SYNTHESIZER_H

#include <cstdint>
#include <fstream>

#define SAMPLE_RATE 16000

int16_t decode_adpcm_nibble(uint8_t nibble);
void write_wav_header(std::ofstream& f, int num, int rate);

#endif
