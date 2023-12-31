/*
 * Audio and Video frame extraction
 * Copyright (c) 2003 Fabrice Bellard.
 * Copyright (c) 2003 Michael Niedermayer.
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
//#include "avcodec.h"
//#include "mpegvideo.h"
//#include "mpegaudio.h"
#include "parser.h"

AVCodecParser *av_first_parser = NULL;

void hde_av_register_codec_parser(AVCodecParser *parser)
{
    parser->next = av_first_parser;
    av_first_parser = parser;
}

AVCodecParserContext *hde_av_parser_init(int codec_id)
{
    AVCodecParserContext *s;
    AVCodecParser *parser;
    int ret;

    if(codec_id == CODEC_ID_NONE)
        return NULL;

    for(parser = av_first_parser; parser != NULL; parser = parser->next) {
        if (parser->codec_ids[0] == codec_id ||
            parser->codec_ids[1] == codec_id ||
            parser->codec_ids[2] == codec_id ||
            parser->codec_ids[3] == codec_id ||
            parser->codec_ids[4] == codec_id)
            goto found;
    }
    return NULL;
 found:
    s = av_mallocz(sizeof(AVCodecParserContext));
    if (!s)
        return NULL;
    s->parser = parser;
    s->priv_data = av_mallocz(parser->priv_data_size);
    if (!s->priv_data) {
        av_free(s);
        return NULL;
    }
    if (parser->parser_init) {
        ret = parser->parser_init(s);
        if (ret != 0) {
            av_free(s->priv_data);
            av_free(s);
            return NULL;
        }
    }
    s->fetch_timestamp=1;
    return s;
}

/**
 *
 * @param buf           input
 * @param buf_size      input length, to signal EOF, this should be 0 (so that the last frame can be output)
 * @param pts           input presentation timestamp
 * @param dts           input decoding timestamp
 * @param poutbuf       will contain a pointer to the first byte of the output frame
 * @param poutbuf_size  will contain the length of the output frame
 * @return the number of bytes of the input bitstream used
 *
 * Example:
 * @code
 *   while(in_len){
 *       len = av_parser_parse(myparser, AVCodecContext, &data, &size,
 *                                       in_data, in_len,
 *                                       pts, dts);
 *       in_data += len;
 *       in_len  -= len;
 *
 *       if(size)
 *          decode_frame(data, size);
 *   }
 * @endcode
 */
int hde_av_parser_parse(AVCodecParserContext *s,
                    AVCodecContext *avctx,
                    uint8_t **poutbuf, int *poutbuf_size,
                    const uint8_t *buf, int buf_size,
                    int64_t pts, int64_t dts)
{
    int index, i, k;
    uint8_t dummy_buf[FF_INPUT_BUFFER_PADDING_SIZE];

    if (buf_size == 0) {
        /* padding is always necessary even if EOF, so we add it here */
        memset(dummy_buf, 0, sizeof(dummy_buf));
        buf = dummy_buf;
    } else {
        /* add a new packet descriptor */
        k = (s->cur_frame_start_index + 1) & (AV_PARSER_PTS_NB - 1);
        s->cur_frame_start_index = k;
        s->cur_frame_offset[k] = s->cur_offset;
        s->cur_frame_pts[k] = pts;
        s->cur_frame_dts[k] = dts;
        if(s->cur_frame_pts[k]>0x10000000)
//            printf("Invalid pts?\n");
s->cur_frame_pts[k] = AV_NOPTS_VALUE;
        /* fill first PTS/DTS */
        if (s->fetch_timestamp){
            s->fetch_timestamp=0;
            s->last_pts = pts;
            s->last_dts = dts;
            s->cur_frame_pts[k] =
            s->cur_frame_dts[k] = AV_NOPTS_VALUE;
        }
    }

    /* WARNING: the returned index can be negative */
    index = s->parser->parser_parse(s, avctx, poutbuf, poutbuf_size, buf, buf_size);
//av_log(NULL, AV_LOG_DEBUG, "parser: in:%"PRId64", %"PRId64", out:%"PRId64", %"PRId64", in:%d out:%d id:%d\n", pts, dts, s->last_pts, s->last_dts, buf_size, *poutbuf_size, avctx->codec_id);
    /* update the file pointer */
    if (*poutbuf_size) {
        /* fill the data for the current frame */
        s->frame_offset = s->last_frame_offset;
	if(s->delay_pts) {
		s->pts = s->curr_pts;
		s->dts = s->curr_dts;
		s->curr_pts = s->last_pts;
		s->curr_dts = s->last_dts;
	} else {
		s->pts = s->last_pts;
		s->dts = s->last_dts;
	} // if
        /* offset of the next frame */
        s->last_frame_offset = s->cur_offset + index;
        /* find the packet in which the new frame starts. It
           is tricky because of MPEG video start codes
           which can begin in one packet and finish in
           another packet. In the worst case, an MPEG
           video start code could be in 4 different
           packets. */
        k = s->cur_frame_start_index;
        for(i = 0; i < AV_PARSER_PTS_NB; i++) {
            if (s->last_frame_offset >= s->cur_frame_offset[k])
                break;
            k = (k - 1) & (AV_PARSER_PTS_NB - 1);
        }

        s->last_pts = s->cur_frame_pts[k];
        s->last_dts = s->cur_frame_dts[k];
        /* some parsers tell us the packet size even before seeing the first byte of the next packet,
           so the next pts/dts is in the next chunk */
        if(index == buf_size){
            s->fetch_timestamp=1;
        }
    }
    if (index < 0)
        index = 0;
    s->cur_offset += index;
    return index;
}

/**
 *
 * @return 0 if the output buffer is a subset of the input, 1 if it is allocated and must be freed
 * @deprecated use AVBitstreamFilter
 */
int av_parser_change(AVCodecParserContext *s,
                     AVCodecContext *avctx,
                     uint8_t **poutbuf, int *poutbuf_size,
                     const uint8_t *buf, int buf_size, int keyframe){

    if(s && s->parser->split){
        if((avctx->flags & CODEC_FLAG_GLOBAL_HEADER) || (avctx->flags2 & CODEC_FLAG2_LOCAL_HEADER)){
            int i= s->parser->split(avctx, buf, buf_size);
            buf += i;
            buf_size -= i;
        }
    }

    /* cast to avoid warning about discarding qualifiers */
    *poutbuf= (uint8_t *) buf;
    *poutbuf_size= buf_size;
    if(avctx->extradata){
        if(  (keyframe && (avctx->flags2 & CODEC_FLAG2_LOCAL_HEADER))
            /*||(s->pict_type != I_TYPE && (s->flags & PARSER_FLAG_DUMP_EXTRADATA_AT_NOKEY))*/
            /*||(? && (s->flags & PARSER_FLAG_DUMP_EXTRADATA_AT_BEGIN)*/){
            int size= buf_size + avctx->extradata_size;
            *poutbuf_size= size;
            *poutbuf= av_malloc(size + FF_INPUT_BUFFER_PADDING_SIZE);

            memcpy(*poutbuf, avctx->extradata, avctx->extradata_size);
            memcpy((*poutbuf) + avctx->extradata_size, buf, buf_size + FF_INPUT_BUFFER_PADDING_SIZE);
            return 1;
        }
    }

    return 0;
}

void hde_av_parser_close(AVCodecParserContext *s)
{
    if (s->parser->parser_close)
        s->parser->parser_close(s);
    av_free(s->priv_data);
    av_free(s);
}

/*****************************************************/

/**
 * combines the (truncated) bitstream to a complete frame
 * @returns -1 if no complete frame could be created
 */
int hde_ff_combine_frame(ParseContext *pc, int next, uint8_t **buf, int *buf_size)
{
#if 0
    if(pc->overread){
        printf("overread %d, state:%X next:%d index:%d o_index:%d\n", pc->overread, pc->state, next, pc->index, pc->overread_index);
        printf("%X %X %X %X\n", (*buf)[0], (*buf)[1],(*buf)[2],(*buf)[3]);
    }
#endif

    /* copy overreaded bytes from last frame into buffer */
    for(; pc->overread>0; pc->overread--){
        pc->buffer[pc->index++]= pc->buffer[pc->overread_index++];
    }

    /* flush remaining if EOF */
    if(!*buf_size && next == END_NOT_FOUND){
        next= 0;
    }

    pc->last_index= pc->index;

    /* copy into buffer end return */
    if(next == END_NOT_FOUND){
        pc->buffer= av_fast_realloc(pc->buffer, &pc->buffer_size, (*buf_size) + pc->index + FF_INPUT_BUFFER_PADDING_SIZE);

        memcpy(&pc->buffer[pc->index], *buf, *buf_size);
        pc->index += *buf_size;
        return -1;
    }

    *buf_size=
    pc->overread_index= pc->index + next;

    /* append to buffer */
    if(pc->index){
        pc->buffer= av_fast_realloc(pc->buffer, &pc->buffer_size, next + pc->index + FF_INPUT_BUFFER_PADDING_SIZE);

        memcpy(&pc->buffer[pc->index], *buf, next + FF_INPUT_BUFFER_PADDING_SIZE );
        pc->index = 0;
        *buf= pc->buffer;
    }

    /* store overread bytes */
    for(;next < 0; next++){
        pc->state = (pc->state<<8) | pc->buffer[pc->last_index + next];
        pc->overread++;
    }

#if 0
    if(pc->overread){
        printf("overread %d, state:%X next:%d index:%d o_index:%d\n", pc->overread, pc->state, next, pc->index, pc->overread_index);
        printf("%X %X %X %X\n", (*buf)[0], (*buf)[1],(*buf)[2],(*buf)[3]);
    }
#endif

    return 0;
}

void hde_ff_parse_close(AVCodecParserContext *s)
{
    ParseContext *pc = s->priv_data;

    av_free(pc->buffer);
}

void hde_ff_parse1_close(AVCodecParserContext *s)
{
    ParseContext1 *pc1 = s->priv_data;

    av_free(pc1->pc.buffer);
    av_free(pc1->enc);
}

