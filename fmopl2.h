#pragma once

typedef struct
{
    int mclk;
    int address;
    int data_i;
    int ic;
    int cs;
    int rd;
    int wr;
} fmopl2_input_t;

typedef struct
{
    fmopl2_input_t input;

    int mclk1;
    int mclk2;
    int clk1;
    int clk2;

    int prescaler_reset_l[2];
    int prescaler_reset;
    int prescaler_cnt[2];
    int prescaler_l1[2];
    int prescaler_l2[2];

    int reset1;

    int fsm_reset_l[2];
    int fsm_reset;
    int fsm_cnt1[2];
    int fsm_cnt2[2];
    int fsm_cnt1_of;
    int fsm_cnt2_of;
    int fsm_sel[13];
    int fsm_cnt;
    int fsm_mc;
    int fsm_ch_out;
    int fsm_do_fb;
    int fsm_load_fb;
    int fsm_l1[2];
    int fsm_l2[2];
    int fsm_l3[2];
    int fsm_l4[2];
    int fsm_l5[2];
    int fsm_l6[2];
    int fsm_out[16];

    int io_rd;
    int io_wr;
    int io_cs;
    int io_a0;

    int io_read0;
    int io_read1;
    int io_write;
    int io_write0;
    int io_write1;
    int io_dir;
    int io_data;

    int data_latch;

    int write0;
    int write0_sr;
    int write0_latch[6];
    int write1;
    int write1_sr;
    int write1_latch[6];

    int reg_sel1;
    int reg_sel2;
    int reg_sel3;
    int reg_sel4;
    int reg_sel8;
    int reg_selbd;
    int reg_test;
    int reg_timer1;
    int reg_timer2;
    int reg_notesel;
    int reg_csm;
    int reg_da;
    int reg_dv;
    int rhythm;
    int reg_rh_kon;
    int reg_sel4_wr;
    int reg_sel4_rst;
    int reg_t1_mask;
    int reg_t2_mask;
    int reg_t1_start;
    int reg_t2_start;
    int reg_mode_b3;
    int reg_mode_b4;

    int t1_cnt[2];
    int t2_cnt[2];
    int t1_of[2];
    int t2_of[2];
    int t1_status;
    int t2_status;
    int unk_status1;
    int unk_status2;
    int timer_st_load_l;
    int timer_st_load;
    int t1_start;
    int t1_start_l[2];
    int t2_start_l[2];
    int t1_load;
    int csm_load_l;
    int csm_load;
    int csm_kon;

    int data_o;
    int data_z;

    int tm_w1;
    int tm_w2;
    int tm_w3;
    int tm_w4;
} fmopl2_t;

