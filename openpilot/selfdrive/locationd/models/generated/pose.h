#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_err_fun(double *nom_x, double *delta_x, double *out_8660653318681785761);
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_1461130683285097740);
void pose_H_mod_fun(double *state, double *out_3433969196483414380);
void pose_f_fun(double *state, double dt, double *out_7050980600469623396);
void pose_F_fun(double *state, double dt, double *out_5108012196123939044);
void pose_h_4(double *state, double *unused, double *out_3112222217623107327);
void pose_H_4(double *state, double *unused, double *out_1718549188555666355);
void pose_h_10(double *state, double *unused, double *out_3363321840709035115);
void pose_H_10(double *state, double *unused, double *out_7887304865878975139);
void pose_h_13(double *state, double *unused, double *out_7518159681466847348);
void pose_H_13(double *state, double *unused, double *out_2115206274746857669);
void pose_h_14(double *state, double *unused, double *out_3485677374259695923);
void pose_H_14(double *state, double *unused, double *out_1364239243739705941);
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt);
}