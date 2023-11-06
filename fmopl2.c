#include "fmopl2.h"


void FMOPL2_DoShiftRegisters(fmopl2_t *chip, int sel)
{
    int j;
    int to = sel;
    int from = sel ^ 1;
    int rot = sel == 0 ? 1 : 0;
#define CH_ROTATE(x) rot ? ((x << 1) | ((x >> 8) & 1)) : x
#define OP_ROTATE(x) rot ? ((x << 1) | ((x >> 17) & 1)) : x
    // channel registers

    // fnum
    for (j = 0; j < 10; j++)
        chip->ch_fnum[j][to] = CH_ROTATE(chip->ch_fnum[j][from]);
    // block
    for (j = 0; j < 3; j++)
        chip->ch_block[j][to] = CH_ROTATE(chip->ch_block[j][from]);
    // kon
    chip->ch_keyon[to] = CH_ROTATE(chip->ch_keyon[from]);
    // connect
    chip->ch_connect[to] = CH_ROTATE(chip->ch_connect[from]);
    // feedback
    for (j = 0; j < 3; j++)
        chip->ch_fb[j][to] = CH_ROTATE(chip->ch_fb[j][from]);
    // multi
    for (j = 0; j < 4; j++)
        chip->op_multi[j][to] = OP_ROTATE(chip->op_multi[j][from]);
    // ksr
    chip->op_ksr[to] = OP_ROTATE(chip->op_ksr[from]);
    // egt
    chip->op_egt[to] = OP_ROTATE(chip->op_egt[from]);
    // connect
    chip->op_vib[to] = OP_ROTATE(chip->op_vib[from]);
    // connect
    chip->op_am[to] = OP_ROTATE(chip->op_am[from]);
    // tl
    for (j = 0; j < 6; j++)
        chip->op_tl[j][to] = OP_ROTATE(chip->op_tl[j][from]);
    // ksl
    for (j = 0; j < 2; j++)
        chip->op_ksl[j][to] = OP_ROTATE(chip->op_ksl[j][from]);
    // ar
    for (j = 0; j < 4; j++)
        chip->op_ar[j][to] = OP_ROTATE(chip->op_ar[j][from]);
    // dr
    for (j = 0; j < 4; j++)
        chip->op_dr[j][to] = OP_ROTATE(chip->op_dr[j][from]);
    // sl
    for (j = 0; j < 4; j++)
        chip->op_sl[j][to] = OP_ROTATE(chip->op_sl[j][from]);
    // rr
    for (j = 0; j < 4; j++)
        chip->op_rr[j][to] = OP_ROTATE(chip->op_rr[j][from]);
    // wf
    for (j = 0; j < 2; j++)
        chip->op_wf[j][to] = OP_ROTATE(chip->op_wf[j][from]);
#undef CH_ROTATE
#undef OP_ROTATE
}

void FMOPL2_Clock(fmopl2_t *chip)
{
    chip->mclk1 = !chip->input.mclk;
    chip->mclk2 = chip->input.mclk;

    chip->reset1 = !chip->input.ic;
    chip->io_rd = !chip->input.rd;
    chip->io_wr = !chip->input.wr;
    chip->io_cs = !chip->input.cs;
    chip->io_a0 = chip->input.address & 1;

    if (chip->mclk1)
    {
        chip->prescaler_reset_l[0] = (chip->prescaler_reset_l[1] << 1) | chip->reset1;
        if (chip->prescaler_reset)
            chip->prescaler_cnt[0] = 0;
        else
            chip->prescaler_cnt[0] = (chip->prescaler_cnt[1] + 1) & 3;

        chip->prescaler_l1[0] = !chip->prescaler_reset && chip->prescaler_cnt[1] == 1;
        chip->prescaler_l2[0] = chip->prescaler_cnt[1] == 3;
    }
    if (chip->mclk2)
    {
        chip->prescaler_reset_l[1] = chip->prescaler_reset_l[0];
        chip->prescaler_cnt[1] = chip->prescaler_cnt[0];
        chip->prescaler_l1[1] = chip->prescaler_l1[0];
        chip->prescaler_l2[1] = chip->prescaler_l2[0];
    }

    chip->prescaler_reset = !(chip->prescaler_reset_l[1] & 2) && chip->reset1;

    chip->clk1 = chip->prescaler_l1[1];
    chip->clk2 = chip->prescaler_l2[1];

    if (chip->clk1)
    {
        if (chip->fsm_reset || chip->fsm_cnt1_of)
            chip->fsm_cnt1[0] = 0;
        else
            chip->fsm_cnt1[0] = (chip->fsm_cnt1[1] + 1) & 7;
        if (chip->fsm_reset || chip->fsm_cnt2_of)
            chip->fsm_cnt2[0] = 0;
        else
            chip->fsm_cnt2[0] = (chip->fsm_cnt2[1] + chip->fsm_cnt1_of) & 3;

        chip->fsm_reset_l[0] = (chip->fsm_reset_l[1] << 1) | chip->reset1;

        chip->fsm_l1[0] = !chip->fsm_sel[8] && !chip->fsm_sel[9] && (chip->fsm_cnt & 8) == 0;

        chip->fsm_l2[0] = chip->fsm_sel[10];

        chip->fsm_l3[0] = (chip->fsm_l3[1] << 1) | chip->fsm_sel[12];

        chip->fsm_l4[0] = (chip->fsm_l4[1] << 1) | (chip->fsm_cnt & 16 != 0);

        chip->fsm_l5[0] = (chip->fsm_l5[1] << 1) | (chip->fsm_cnt & 8 != 0);

        chip->fsm_l6[0] = (chip->fsm_l6[1] << 1) | (chip->fsm_cnt & 16 != 0);
    }
    if (chip->clk2)
    {
        chip->fsm_cnt1[1] = chip->fsm_cnt1[0];
        chip->fsm_cnt2[1] = chip->fsm_cnt2[0];
        chip->fsm_reset_l[1] = chip->fsm_reset_l[0];
        chip->fsm_l1[1] = chip->fsm_l1[0];
        chip->fsm_l2[1] = chip->fsm_l2[0];
        chip->fsm_l3[1] = chip->fsm_l3[0];
        chip->fsm_l4[1] = chip->fsm_l4[0];
        chip->fsm_l5[1] = chip->fsm_l5[0];
        chip->fsm_l6[1] = chip->fsm_l6[0];
    }

    chip->fsm_reset = !(chip->fsm_reset_l[1] & 2) && chip->reset1;
    chip->fsm_cnt1_of = (chip->fsm_cnt1[1] & 5) == 5;
    chip->fsm_cnt2_of = chip->fsm_cnt1_of && (chip->fsm_cnt2[1] & 2) != 0;

    chip->fsm_cnt = (chip->fsm_cnt2[1] << 3) | chip->fsm_cnt1[1];

    chip->fsm_sel[0] = chip->fsm_cnt == 20 && chip->rhythm;
    chip->fsm_sel[1] = chip->fsm_cnt == 19 && chip->rhythm;
    chip->fsm_sel[2] = chip->fsm_cnt == 18 && chip->rhythm;
    chip->fsm_sel[3] = chip->fsm_cnt == 17 && chip->rhythm;
    chip->fsm_sel[4] = chip->fsm_cnt == 16 && chip->rhythm;
    chip->fsm_sel[5] = chip->fsm_cnt == 20 && chip->rhythm;
    chip->fsm_sel[6] = chip->fsm_cnt == 19 && chip->rhythm;
    chip->fsm_sel[7] = (chip->fsm_cnt & 5) == 4;
    chip->fsm_sel[8] = chip->fsm_cnt == 16;
    chip->fsm_sel[9] = (chip->fsm_cnt & 29) == 5;
    chip->fsm_sel[10] = chip->fsm_cnt == 16;
    chip->fsm_sel[11] = chip->fsm_cnt == 11;
    chip->fsm_sel[12] = chip->fsm_cnt == 20;

    chip->fsm_mc = !(chip->fsm_sel[7] || (chip->fsm_cnt & 2) != 0);

    chip->fsm_out[0] = ((chip->connect_l[1] & 2) != 0 || chip->fsm_sel[0] || chip->fsm_sel[1] || chip->fsm_mc) && !chip->fsm_sel[2];

    chip->fsm_out[1] = chip->fsm_mc && !chip->fsm_sel[3] && !chip->fsm_sel[4];

    chip->fsm_out[2] = !chip->fsm_mc && !chip->fsm_sel[5] && !chip->fsm_sel[6];

    chip->fsm_out[3] = chip->fsm_l1[1] && 1;

    chip->fsm_out[4] = chip->fsm_l2[1];

    chip->fsm_out[5] = chip->fsm_sel[10];

    chip->fsm_out[6] = chip->fsm_sel[11];

    chip->fsm_out[7] = chip->fsm_sel[12];

    chip->fsm_out[8] = (chip->fsm_l3[1] & 1) != 0;

    chip->fsm_out[9] = (chip->fsm_l3[1] & 2) != 0;

    chip->fsm_out[10] = (chip->fsm_l3[1] & 2) != 0;

    chip->fsm_out[11] = (chip->fsm_l4[1] & 2) != 0 && chip->rhythm;

    chip->fsm_out[12] = (chip->fsm_l5[1] & 4) != 0;

    chip->fsm_out[13] = (chip->fsm_l6[1] & 4) != 0;

    chip->fsm_out[14] = !(chip->fsm_out[12] || (chip->fsm_cnt & 16) != 0);

    chip->fsm_out[15] = !(chip->fsm_out[12] || chip->fsm_out[13]);

    chip->io_read0 = !chip->reset1 && chip->io_cs && chip->io_rd && !chip->io_a0;
    chip->io_read1 = !chip->reset1 && chip->io_cs && chip->io_rd && chip->io_a0;
    chip->io_write = !chip->reset1 && chip->io_cs && chip->io_wr;
    chip->io_write0 = !chip->reset1 && chip->io_cs && chip->io_wr && !chip->io_a0;
    chip->io_write1 = !chip->reset1 && chip->io_cs && chip->io_wr && !chip->io_a1;
    chip->io_dir = chip->io_cs && chip->io_rd;

    if (chip->io_dir)
    {
        chip->data_o = chip->io_data;
        chip->data_z = 0;
    }
    else
        chip->data_z = 1;

    if (!chip->io_dir)
        chip->io_data = chip->input.data_i;

    if (chip->io_write)
        chip->data_latch = chip->io_data;

    if (chip->write0)
        chip->write0_sr = 0;
    else if (chip->io_write0)
        chip->write0_sr = 1;

    if (chip->write1)
        chip->write1_sr = 0;
    else if (chip->io_write1)
        chip->write1_sr = 1;

    if (chip->mclk1)
    {
        chip->write0_latch[1] = chip->write0_latch[0];
        chip->write1_latch[1] = chip->write1_latch[0];
    }
    if (chip->mclk2)
    {
        chip->write0_latch[0] = chip->write0_sr;
        chip->write0_latch[2] = chip->write0_latch[1];

        chip->write1_latch[0] = chip->write1_sr;
        chip->write1_latch[2] = chip->write1_latch[1];
    }

    if (chip->clk1)
    {
        chip->write0_latch[4] = chip->write0_latch[3];
        chip->write1_latch[4] = chip->write1_latch[3];
    }
    if (chip->clk2)
    {
        chip->write0_latch[3] = chip->write0_latch[2];
        chip->write0_latch[5] = chip->write0_latch[4];

        chip->write1_latch[3] = chip->write1_latch[2];
        chip->write1_latch[5] = chip->write1_latch[4];
    }

    chip->write0 = chip->write0_latch[5];
    chip->write1 = chip->write1_latch[5];

    if (chip->write0)
    {
        chip->reg_sel1 = chip->data_latch == 1;
        chip->reg_sel2 = chip->data_latch == 2;
        chip->reg_sel3 = chip->data_latch == 3;
        chip->reg_sel4 = chip->data_latch == 4;
        chip->reg_sel8 = chip->data_latch == 8;
        chip->reg_selbd = chip->data_latch == 0xbd;
    }

    chip->reg_sel4_wr = chip->write1 && chip->reg_sel4 && (chip->data_latch & 128) == 0;
    chip->reg_sel4_rst = (chip->write1 && chip->reg_sel4 && (chip->data_latch & 128) != 0) || chip->reset1;

    if (chip->reset1)
    {
        chip->reg_test = 0;
        chip->reg_timer1 = 0;
        chip->reg_timer2 = 0;
        chip->reg_notesel = 0;
        chip->reg_csm = 0;
        chip->rhythm = 0;
        chip->reg_rh_kon = 0;
        chip->reg_da = 0;
        chip->reg_dv = 0;
    }
    else if (chip->write1)
    {
        if (chip->reg_sel1)
            chip->reg_test = chip->data_latch & 255;
        if (chip->reg_sel2)
            chip->reg_timer1 = chip->data_latch & 255;
        if (chip->reg_sel3)
            chip->reg_timer2 = chip->data_latch & 255;
        if (chip->reg_sel8)
        {
            chip->reg_notesel = (chip->data_latch & 64) != 0;
            chip->reg_csm = (chip->data_latch & 128) != 0;
        }
        if (chip->reg_selbd)
        {
            chip->reg_rh_kon = chip->data_latch & 31;
            chip->rhythm = (chip->data_latch & 32) != 0;
            chip->reg_dv = (chip->data_latch & 64) != 0;
            chip->reg_da = (chip->data_latch & 128) != 0;
        }
    }

    if (chip->reset1)
    {
        chip->reg_t1_mask = 0;
        chip->reg_t2_mask = 0;
        chip->reg_t1_start = 0;
        chip->reg_t2_start = 0;
        chip->reg_mode_b3 = 0;
        chip->reg_mode_b4 = 0;
    }
    else if (chip->reg_sel4_wr)
    {
        chip->reg_t1_mask = (chip->data_latch & 64) != 0;
        chip->reg_t2_mask = (chip->data_latch & 32) != 0;
        chip->reg_t1_start = (chip->data_latch & 1) != 0;
        chip->reg_t2_start = (chip->data_latch & 2) != 0;
        chip->reg_mode_b3 = (chip->data_latch & 8) != 0;
        chip->reg_mode_b4 = (chip->data_latch & 16) != 0;
    }

    if (chip->reg_sel4_rst || chip->reg_t1_mask)
        chip->t1_status = 0;
    else if (chip->tm_w1)
        chip->t1_status = 1;

    if (chip->reg_sel4_rst || chip->reg_t2_mask)
        chip->t2_status = 0;
    else if (chip->tm_w1)
        chip->t2_status = 1;

    if (chip->reg_sel4_rst || chip->reg_mode_b4)
        chip->unk_status1 = 0;
    else if (0)
        chip->unk_status1 = 1;

    chip->unk_status2 = 0;

    if (chip->clk1)
    {
        int value = chip->t1_load ? chip->reg_timer1 : chip->t1_cnt[1];
        value += ((chip->t1_start_l[1] & 1) != 0 && chip->tm_w1) || (chip->reg_test & 2) != 0;
        chip->t1_of[0] = (value & 256) != 0;
        chip->t1_cnt[0] = (chip->t1_start_l[1] & 1) == 0 ? 0 : (value & 255);
        
        value = (chip->t2_of[1] || (chip->t2_start_l[1] & 3) == 1) ? chip->reg_timer2 : chip->t2_cnt[1];
        value += ((chip->t2_start_l[1] & 1) != 0 && chip->tm_w1) || (chip->reg_test & 2) != 0;
        chip->t2_of[0] = (value & 256) != 0;
        chip->t2_cnt[0] = (chip->t2_start_l[1] & 1) == 0 ? 0 : (value & 255);

        chip->t1_start_l[0] = (chip->t1_start_l[1] << 1) | chip->t1_start;
        chip->t2_start_l[0] = (chip->t2_start_l[1] << 1) | chip->reg_t2_start;
    }
    if (chip->clk2)
    {
        chip->t1_cnt[1] = chip->t1_cnt[0];
        chip->t1_of[1] = chip->t1_of[0];
        chip->t2_cnt[1] = chip->t2_cnt[0];
        chip->t2_of[1] = chip->t2_of[0];

        chip->t1_start_l[1] = chip->t1_start_l[0];
        chip->t2_start_l[1] = chip->t2_start_l[0];
    }

    chip->t1_load = (chip->t1_of[1] || (chip->t1_start_l[1] & 3) == 1);

    if (chip->clk1)
        chip->timer_st_load_l = chip->fsm_out[8];
    chip->timer_st_load = chip->fsm_out[8] && !chip->timer_st_load_l;

    if (chip->timer_st_load)
        chip->t1_start = chip->reg_t1_start;

    if (chip->clk1)
        chip->csm_load_l = chip->fsm_out[10];
    chip->csm_load = chip->fsm_out[10] && !chip->csm_load_l;

    if (chip->csm_load)
        chip->csm_kon = chip->reg_csm && chip->t1_load;

    chip->rh_sel0 = chip->rhythm && chip->fsm_out[5];

    if (chip->clk1)
    {
        chip->rh_sel[0] = (chip->rh_sel[1] << 1) | chip->rh_sel0;
    }
    if (chip->clk2)
    {
        chip->rh_sel[1] = cihp->rh_sel[0];
    }

    chip->keyon_comb = chip->keyon || chip->csm_kon
        || (chip->rh_sel0 && (chip->reg_rh_kon & 16) != 0) // bd0
        || ((chip->rh_sel[1] & 1) != 0 && (chip->reg_rh_kon & 1) != 0) // hh
        || ((chip->rh_sel[1] & 2) != 0 && (chip->reg_rh_kon & 4) != 0) // tom
        || ((chip->rh_sel[1] & 4) != 0 && (chip->reg_rh_kon & 16) != 0) // bd1
        || ((chip->rh_sel[1] & 8) != 0 && (chip->reg_rh_kon & 8) != 0) // sd
        || ((chip->rh_sel[1] & 16) != 0 && (chip->reg_rh_kon & 2) != 0); // tc

    if (chip->reset1)
        chip->address = 0;
    else if ((chip->data_latch & 0xe0) != 0 && chip->write0)
        chip->address = chip->data_latch;

    if (chip->write0)
        chip->address_valid = (chip->data_latch & 0xe0) != 0;

    if (chip->reset1)
        chip->data = 0;
    else if (chip->address_valid && chip->write1)
        chip->data = chip->data_latch;

    if (chip->clk1)
    {
        chip->address_valid_l[0] = chip->address_valid || chip->address_valid2;

        if (chip->fsm_out[8] || chip->slot_cnt1_of)
            chip->slot_cnt1[0] = 0;
        else
            chip->slot_cnt1[0] = (chip->slot_cnt1[1] + 1) & 7;

        if (chip->fsm_out[8] || (chip->slot_cnt1_of && (chip->slot_cnt2[1] & 2) != 0))
            chip->slot_cnt2[0] = 0;
        else
            chip->slot_cnt2[0] = (chip->slot_cnt2[1] + chip->slot_cnt1_of) & 3;
    }
    if (chip->clk2)
    {
        chip->address_valid_l[1] = chip->address_valid_l[0];

        chip->slot_cnt1[1] = chip->slot_cnt1[0];
        chip->slot_cnt2[1] = chip->slot_cnt2[0];
    }
    chip->address_valid2 = chip->address_valid_l[1] && !chip->write0;

    chip->slot_cnt1_of = (chip->slot_cnt1[1] & 5) == 5;

    chip->slot_cnt = (chip->slot_cnt2[1] << 3) | chip->slot_cnt1[1];
    
    chip->addr_sel = chip->slot_cnt & 1;

    chip->addr_sel |= (((chip->slot_cnt >> 1) + chip->addr_add) & 7) << 1;

    if (!chip->sel_ch)
        chip->addr_sel |= chip->slot_cnt & 16;

    chip->addr_match = chip->addr_sel == (chip->address & 31) && chip->address_valid2;

    chip->sel_20 = (chip->address & 0xe0) == 0x20 && chip->addr_match;
    chip->sel_40 = (chip->address & 0xe0) == 0x40 && chip->addr_match;
    chip->sel_60 = (chip->address & 0xe0) == 0x60 && chip->addr_match;
    chip->sel_80 = (chip->address & 0xe0) == 0x80 && chip->addr_match;
    chip->sel_e0 = (chip->address & 0xe0) == 0xe0 && chip->addr_match && (chip->reg_test & 32) != 0;

    chip->sel_a0 = (chip->address & 0xe0) == 0xa0 && chip->addr_match;
    chip->sel_b0 = (chip->address & 0xe0) == 0xb0 && chip->addr_match;
    chip->sel_c0 = (chip->address & 0xe0) == 0xc0 && chip->addr_match;

    chip->sel_ch = (chip->address & 0xe0) == 0xa0 || (chip->address & 0xe0) == 0xb0 || (chip->address & 0xe0) == 0xc0;

    chip->addr_add = chip->sel_ch && ((chip->address & 8) != 0 || (chip->address & 6) == 6);

    if (chip->clk1)
    {
        int i;
        FMOPL2_DoShiftRegisters(chip, 0);

        if (chip->reset1)
        {
            for (i = 0; i < 10; i++)
                chip->ch_fnum[i][0] &= ~1;
            for (i = 0; i < 3; i++)
                chip->ch_block[i][0] &= ~1;
            chip->ch_keyon[0] &= ~1;
            chip->ch_connect[0] &= ~1;
            for (i = 0; i < 3; i++)
                chip->ch_fb[i][0] &= ~1;

            for (i = 0; i < 4; i++)
                chip->op_multi[i][0] &= ~1;
            chip->op_ksr[0] &= ~1;
            chip->op_egt[0] &= ~1;
            chip->op_vib[0] &= ~1;
            chip->op_am[0] &= ~1;
            for (i = 0; i < 6; i++)
                chip->op_tl[i][0] &= ~1;
            for (i = 0; i < 2; i++)
                chip->op_ksl[i][0] &= ~1;
            for (i = 0; i < 4; i++)
                chip->op_ar[i][0] &= ~1;
            for (i = 0; i < 4; i++)
                chip->op_dr[i][0] &= ~1;
            for (i = 0; i < 4; i++)
                chip->op_sl[i][0] &= ~1;
            for (i = 0; i < 4; i++)
                chip->op_rr[i][0] &= ~1;
            for (i = 0; i < 2; i++)
                chip->op_wf[i][0] &= ~1;
        }
        else
        {
            if (chip->sel_a0)
            {
                for (i = 0; i < 8; i++)
                    chip->ch_fnum[i][0] &= ~1;

                for (i = 0; i < 8; i++)
                    chip->ch_fnum[i][0] |= (chip->data >> i) & 1;
            }
            if (chip->sel_b0)
            {
                for (i = 8; i < 10; i++)
                    chip->ch_fnum[i][0] &= ~1;
                for (i = 0; i < 3; i++)
                    chip->ch_block[i][0] &= ~1;
                chip->ch_keyon[0] &= ~1;

                for (i = 8; i < 10; i++)
                    chip->ch_fnum[i][0] |= (chip->data >> (i - 8)) & 1;
                for (i = 0; i < 3; i++)
                    chip->ch_block[i][0] |= (chip->data >> (i + 2)) & 1;
                chip->ch_keyon[0] |= (chip->data >> 5) & 1;
            }
            if (chip->sel_c0)
            {
                chip->ch_connect[0] &= ~1;
                for (i = 0; i < 3; i++)
                    chip->ch_fb[i][0] &= ~1;

                chip->ch_connect[0] |= (chip->data >> 0) & 1;
                for (i = 0; i < 3; i++)
                    chip->ch_fb[i][0] |= (chip->data >> (i + 1)) & 1;
            }
            if (chip->sel_20)
            {
                for (i = 0; i < 4; i++)
                    chip->op_multi[i][0] &= ~1;
                chip->op_ksr[0] &= ~1;
                chip->op_egt[0] &= ~1;
                chip->op_vib[0] &= ~1;
                chip->op_am[0] &= ~1;

                for (i = 0; i < 4; i++)
                    chip->op_multi[i][0] |= (chip->data >> i) & 1;
                chip->op_ksr[0] |= (chip->data >> 4) & 1;
                chip->op_egt[0] |= (chip->data >> 5) & 1;
                chip->op_vib[0] |= (chip->data >> 6) & 1;
                chip->op_am[0] |= (chip->data >> 7) & 1;
            }
            if (chip->sel_40)
            {
                for (i = 0; i < 6; i++)
                    chip->op_tl[i][0] &= ~1;
                for (i = 0; i < 2; i++)
                    chip->op_ksl[i][0] &= ~1;

                for (i = 0; i < 6; i++)
                    chip->op_tl[i][0] |= (chip->data >> i) & 1;
                for (i = 0; i < 2; i++)
                    chip->op_ksl[i][0] |= (chip->data >> (i + 6)) & 1;
            }
            if (chip->sel_60)
            {
                for (i = 0; i < 4; i++)
                    chip->op_ar[i][0] &= ~1;
                for (i = 0; i < 4; i++)
                    chip->op_dr[i][0] &= ~1;

                for (i = 0; i < 4; i++)
                    chip->op_ar[i][0] |= (chip->data >> (i + 4)) & 1;
                for (i = 0; i < 4; i++)
                    chip->op_dr[i][0] |= (chip->data >> i) & 1;
            }
            if (chip->sel_80)
            {
                for (i = 0; i < 4; i++)
                    chip->op_sl[i][0] &= ~1;
                for (i = 0; i < 4; i++)
                    chip->op_rr[i][0] &= ~1;

                for (i = 0; i < 4; i++)
                    chip->op_sl[i][0] |= (chip->data >> (i + 4)) & 1;
                for (i = 0; i < 4; i++)
                    chip->op_rr[i][0] |= (chip->data >> i) & 1;
            }
            if (chip->sel_e0)
            {
                for (i = 0; i < 2; i++)
                    chip->op_wf[i][0] &= ~1;

                for (i = 0; i < 2; i++)
                    chip->op_wf[i][0] |= (chip->data >> i) & 1;
            }
        }
    }
    if (chip->clk2)
    {
        FMOPL2_DoShiftRegisters(chip, 1);
    }

    {
        int shift = 0;
        int i;

        if (chip->fsm_out[13])
            shift = 8;
        else if (chip->fsm_out[12])
            shift = 5;
        else if (chip->fsm_out[15])
            shift = 2;

        chip->block = 0;
        chip->fnum = 0;
        for (i = 0; i < 3; i++)
            chip->block |= ((chip->ch_block[i][1] >> shift) & 1) << i;
        for (i = 0; i < 10; i++)
            chip->fnum |= ((chip->ch_fnum[i][1] >> shift) & 1) << i;
        chip->keyon = (chip->ch_keyon[1] >> shift) & 1;
        chip->connect = (chip->ch_connect[1] >> shift) & 1;

        if (chip->fsm_out[13])
            shift = 5;
        else if (chip->fsm_out[12])
            shift = 2;
        else if (chip->fsm_out[15])
            shift = 8;
        for (i = 0; i < 3; i++)
            chip->fb |= ((chip->ch_fb[i][1] >> shift) & 1) << i;

        chip->multi = 0;
        chip->tl = 0;
        chip->ksl = 0;
        chip->ar = 0;
        chip->dr = 0;
        chip->sl = 0;
        chip->rr = 0;
        chip->wf = 0;

        for (i = 0; i < 4; i++)
            chip->multi |= ((chip->op_multi[i][1] >> 17) & 1) << i;

        chip->ksr = (chip->op_ksr[1] >> 17) & 1;
        chip->egt = (chip->op_egt[1] >> 17) & 1;
        chip->vib = (chip->op_vib[1] >> 17) & 1;
        chip->am = (chip->op_am[1] >> 17) & 1;

        for (i = 0; i < 4; i++)
            chip->multi |= ((chip->op_multi[i][1] >> 17) & 1) << i;

        for (i = 0; i < 6; i++)
            chip->tl |= ((chip->op_tl[i][1] >> 17) & 1) << i;

        for (i = 0; i < 2; i++)
            chip->ksl |= ((chip->op_ksl[i][1] >> 17) & 1) << i;

        for (i = 0; i < 4; i++)
            chip->ar |= ((chip->op_ar[i][1] >> 17) & 1) << i;

        for (i = 0; i < 4; i++)
            chip->dr |= ((chip->op_dr[i][1] >> 17) & 1) << i;

        for (i = 0; i < 4; i++)
            chip->sl |= ((chip->op_sl[i][1] >> 17) & 1) << i;

        for (i = 0; i < 4; i++)
            chip->rr |= ((chip->op_rr[i][1] >> 17) & 1) << i;

        for (i = 0; i < 2; i++)
            chip->wf |= ((chip->op_wf[i][1] >> 17) & 1) << i;

    }

    if (chip->clk1)
    {
        chip->connect_l[0] = (chip->connect_l[1] << 1) | chip->connect;
        chip->fb_l[0][0] = chip->fb;
        chip->fb_l[1][0] = chip->fb_l[0][1];
    }
    if (chip->clk2)
    {
        chip->connect_l[1] = chip->connect_l[0];
        chip->fb_l[0][1] = chip->fb_l[0][0];
        chip->fb_l[1][1] = chip->fb_l[1][0];
    }
}
