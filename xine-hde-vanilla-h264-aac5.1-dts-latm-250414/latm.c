/*
 * LATM / AAC Parser
 *
 * Copyright (C) 2009 Andreas Öman
 *
 * Based on LATM Parser patch sent to ffmpeg-devel@
 * copyright (c) 2009 Paul Kendall <paul@kcbbs.gen.nz>
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "parser.h"
#include "latm.h"
#include "bitstream.h"

static uint32_t
latm_get_value(GetBitContext *gb)
{
    return get_bits(gb, get_bits(gb, 2) * 8);
}

static int
read_audio_specific_config( latm_private_t *latm, GetBitContext *gb)
{

  int sri, chan;

  latm->aot = get_bits(gb, 5);

  sri = get_bits(gb, 4);

  if(sri == 0xf)
    latm->samplerate = get_bits(gb, 24);
  else
    latm->samplerate = aac_samplerate_tab[sri & 0xf];

  if(latm->samplerate == 0)
    return 0;

  chan = get_bits(gb, 4);

  latm->channels = aac_channels[chan];

  if (latm->aot == 5) { // AOT_SBR
    if (get_bits(gb, 4) == 0xf) {  // extensionSamplingFrequencyIndex
	skip_bits(gb, 24);
    }
    latm->aot = get_bits(gb, 5);	// this is the main object type (i.e. non-extended)
  }

  if(latm->aot != 2)
    return 0;

  unsigned char c0=((latm->aot)<<3)|(sri>>1);
  unsigned char c1=(sri<<7)|(chan<<3);
  latm->extra = (c0)<<8|(c1);


  skip_bits(gb, 1); //framelen_flag
  if(get_bits1(gb))  // depends_on_coder
    skip_bits(gb, 14);

  if(get_bits(gb, 1))  // ext_flag
     skip_bits(gb, 1);  // ext3_flag

}

static int read_stream_mux_config( latm_private_t *latm, GetBitContext *gb)
{

  int audio_mux_version = get_bits(gb, 1);

  latm->audio_mux_version_A = 0;

  if(audio_mux_version)                       // audioMuxVersion
    latm->audio_mux_version_A = get_bits(gb, 1);
  
  if(latm->audio_mux_version_A)
    return 0;

  if(audio_mux_version)
    latm_get_value(gb);                  // taraFullness

  skip_bits(gb, 1);                         // allStreamSameTimeFraming = 1
  skip_bits(gb, 6);                         // numSubFrames = 0
  skip_bits(gb, 4);                         // numPrograms = 0

  // for each program (which there is only on in DVB)
  skip_bits(gb, 3);                         // numLayer = 0
    
  // for each layer (which there is only on in DVB)
  if(!audio_mux_version)
    read_audio_specific_config( latm, gb);
  else {
    return 0;
#if 0
    uint32_t ascLen = latm_get_value(gb);
    abort(); // ascLen -= read_audio_specific_config(filter, gb);
    skip_bits(gb, ascLen);
#endif
  }

  // these are not needed... perhaps
  latm->frame_length_type = get_bits(gb, 3);
  switch(latm->frame_length_type) {
  case 0:
    get_bits(gb, 8);
    break;
  case 1:
    get_bits(gb, 9);
    break;
  case 3:
  case 4:
  case 5:
    get_bits(gb, 6);                 // celp_table_index
    break;
  case 6:
  case 7:
    get_bits(gb, 1);                 // hvxc_table_index
    break;
  }

  if(get_bits(gb, 1)) {                   // other data?
    if(audio_mux_version)
      latm_get_value(gb);              // other_data_bits
    else {
      int esc;
      do {
	esc = get_bits(gb, 1);
	skip_bits(gb, 8);
      } while (esc);
    }
  }

  if(get_bits(gb, 1))                   // crc present?
    skip_bits(gb, 8);                     // config_crc
  latm->configured = 1;

}

/**
 * Parse AAC LATM
 */

//#define LOAS_SYNC_WORD   0x2b7

int parse_latm_audio_mux_element(latm_private_t *latm, uint8_t *data, int muxlength, int *hlen)
{
  GetBitContext gb, out;
  int slot_len, tmp, i;

  init_get_bits(&gb, data+3, (muxlength-3) * 8);

//  if (get_bits(&gb, 11) != LOAS_SYNC_WORD)  return 0;
//  int muxlength = get_bits(&gb, 13) + 3;

  if(!get_bits1(&gb))
    read_stream_mux_config( latm, &gb);

  if(!latm->configured)
    return -1;

  if(latm->frame_length_type != 0)
    return 0;

  slot_len = 0;
  do {
    tmp = get_bits(&gb, 8);
    slot_len += tmp;
  } while (tmp == 255);

  if(slot_len * 8 > get_bits_left(&gb))
    return 0;

  int header_len = get_bits_count(&gb);

  *hlen = muxlength-slot_len;

  uint8_t *buf = (uint8_t*)malloc(slot_len);

  for(i = 0; i < slot_len; i++){
    buf[i] =get_bits(&gb, 8);
  }

  memcpy(data+*hlen, buf, slot_len);

  free(buf);

  return 1;
}
