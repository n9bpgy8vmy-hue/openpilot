#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void live_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_9(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_12(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_35(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_32(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_33(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_H(double *in_vec, double *out_6931801075002631809);
void live_err_fun(double *nom_x, double *delta_x, double *out_109947025622862389);
void live_inv_err_fun(double *nom_x, double *true_x, double *out_4093493871380981869);
void live_H_mod_fun(double *state, double *out_4635958729205586165);
void live_f_fun(double *state, double dt, double *out_5297052292466081641);
void live_F_fun(double *state, double dt, double *out_4909292255207234021);
void live_h_4(double *state, double *unused, double *out_4079096614099624043);
void live_H_4(double *state, double *unused, double *out_5274579456415872996);
void live_h_9(double *state, double *unused, double *out_6622716275922558080);
void live_H_9(double *state, double *unused, double *out_1117411720061095513);
void live_h_10(double *state, double *unused, double *out_222992411176678508);
void live_H_10(double *state, double *unused, double *out_5638156138737734689);
void live_h_12(double *state, double *unused, double *out_7761104972629585702);
void live_H_12(double *state, double *unused, double *out_5895678481463466663);
void live_h_35(double *state, double *unused, double *out_4688539584863275547);
void live_H_35(double *state, double *unused, double *out_8641241513788480372);
void live_h_32(double *state, double *unused, double *out_4740542295649189820);
void live_H_32(double *state, double *unused, double *out_1726377010094364016);
void live_h_13(double *state, double *unused, double *out_3019979000208187376);
void live_H_13(double *state, double *unused, double *out_13296517015306365);
void live_h_14(double *state, double *unused, double *out_6622716275922558080);
void live_H_14(double *state, double *unused, double *out_1117411720061095513);
void live_h_33(double *state, double *unused, double *out_6619996030262357657);
void live_H_33(double *state, double *unused, double *out_6654945555282213640);
void live_predict(double *in_x, double *in_P, double *in_Q, double dt);
}