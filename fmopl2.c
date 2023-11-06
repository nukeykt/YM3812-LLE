#include "fmopl2.h"


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

    chip->fsm_out[0] = (chip->connect || chip->fsm_sel[0] || chip->fsm_sel[1] || chip->fsm_mc) && !chip->fsm_sel[2];

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
}
