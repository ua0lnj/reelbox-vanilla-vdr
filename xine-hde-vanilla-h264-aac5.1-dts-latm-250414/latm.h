/*
 *  Packet parsing functions
 *  Copyright (C) 2007 Andreas �man
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LATM_H_
#define LATM_H_
typedef struct latm_private {

    int configured;
    int audio_mux_version_A;
    int frame_length_type;
    int aot;
    int samplerate;
    int channels;
    int extra;

} latm_private_t;

latm_private_t *latm;

int parse_latm_audio_mux_element(latm_private_t *latm, uint8_t *data, int len, int *hlen);

#endif /* LATM_H_ */
