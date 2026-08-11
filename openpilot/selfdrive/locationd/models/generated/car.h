#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_err_fun(double *nom_x, double *delta_x, double *out_724502583953982588);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_4730759327747131033);
void car_H_mod_fun(double *state, double *out_3922762455048601662);
void car_f_fun(double *state, double dt, double *out_2598143491706081670);
void car_F_fun(double *state, double dt, double *out_6652314106784119942);
void car_h_25(double *state, double *unused, double *out_7473650352994810823);
void car_H_25(double *state, double *unused, double *out_980929370136063178);
void car_h_24(double *state, double *unused, double *out_6055745251191332763);
void car_H_24(double *state, double *unused, double *out_3158143793743213151);
void car_h_30(double *state, double *unused, double *out_702815126644459887);
void car_H_30(double *state, double *unused, double *out_3499262328643311805);
void car_h_26(double *state, double *unused, double *out_7705577830437525666);
void car_H_26(double *state, double *unused, double *out_4285455339896863779);
void car_h_27(double *state, double *unused, double *out_6530140500093148388);
void car_H_27(double *state, double *unused, double *out_5722856399827255022);
void car_h_29(double *state, double *unused, double *out_5704488394305305532);
void car_H_29(double *state, double *unused, double *out_4009493672957703989);
void car_h_28(double *state, double *unused, double *out_6914930305955557583);
void car_H_28(double *state, double *unused, double *out_1072905344111826585);
void car_h_31(double *state, double *unused, double *out_9067498541078495882);
void car_H_31(double *state, double *unused, double *out_3659247237663512303);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}