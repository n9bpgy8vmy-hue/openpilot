#include "pose.h"

namespace {
#define DIM 18
#define EDIM 18
#define MEDIM 18
typedef void (*Hfun)(double *, double *, double *);
const static double MAHA_THRESH_4 = 7.814727903251177;
const static double MAHA_THRESH_10 = 7.814727903251177;
const static double MAHA_THRESH_13 = 7.814727903251177;
const static double MAHA_THRESH_14 = 7.814727903251177;

/******************************************************************************
 *                      Code generated with SymPy 1.14.0                      *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_8660653318681785761) {
   out_8660653318681785761[0] = delta_x[0] + nom_x[0];
   out_8660653318681785761[1] = delta_x[1] + nom_x[1];
   out_8660653318681785761[2] = delta_x[2] + nom_x[2];
   out_8660653318681785761[3] = delta_x[3] + nom_x[3];
   out_8660653318681785761[4] = delta_x[4] + nom_x[4];
   out_8660653318681785761[5] = delta_x[5] + nom_x[5];
   out_8660653318681785761[6] = delta_x[6] + nom_x[6];
   out_8660653318681785761[7] = delta_x[7] + nom_x[7];
   out_8660653318681785761[8] = delta_x[8] + nom_x[8];
   out_8660653318681785761[9] = delta_x[9] + nom_x[9];
   out_8660653318681785761[10] = delta_x[10] + nom_x[10];
   out_8660653318681785761[11] = delta_x[11] + nom_x[11];
   out_8660653318681785761[12] = delta_x[12] + nom_x[12];
   out_8660653318681785761[13] = delta_x[13] + nom_x[13];
   out_8660653318681785761[14] = delta_x[14] + nom_x[14];
   out_8660653318681785761[15] = delta_x[15] + nom_x[15];
   out_8660653318681785761[16] = delta_x[16] + nom_x[16];
   out_8660653318681785761[17] = delta_x[17] + nom_x[17];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_1461130683285097740) {
   out_1461130683285097740[0] = -nom_x[0] + true_x[0];
   out_1461130683285097740[1] = -nom_x[1] + true_x[1];
   out_1461130683285097740[2] = -nom_x[2] + true_x[2];
   out_1461130683285097740[3] = -nom_x[3] + true_x[3];
   out_1461130683285097740[4] = -nom_x[4] + true_x[4];
   out_1461130683285097740[5] = -nom_x[5] + true_x[5];
   out_1461130683285097740[6] = -nom_x[6] + true_x[6];
   out_1461130683285097740[7] = -nom_x[7] + true_x[7];
   out_1461130683285097740[8] = -nom_x[8] + true_x[8];
   out_1461130683285097740[9] = -nom_x[9] + true_x[9];
   out_1461130683285097740[10] = -nom_x[10] + true_x[10];
   out_1461130683285097740[11] = -nom_x[11] + true_x[11];
   out_1461130683285097740[12] = -nom_x[12] + true_x[12];
   out_1461130683285097740[13] = -nom_x[13] + true_x[13];
   out_1461130683285097740[14] = -nom_x[14] + true_x[14];
   out_1461130683285097740[15] = -nom_x[15] + true_x[15];
   out_1461130683285097740[16] = -nom_x[16] + true_x[16];
   out_1461130683285097740[17] = -nom_x[17] + true_x[17];
}
void H_mod_fun(double *state, double *out_3433969196483414380) {
   out_3433969196483414380[0] = 1.0;
   out_3433969196483414380[1] = 0.0;
   out_3433969196483414380[2] = 0.0;
   out_3433969196483414380[3] = 0.0;
   out_3433969196483414380[4] = 0.0;
   out_3433969196483414380[5] = 0.0;
   out_3433969196483414380[6] = 0.0;
   out_3433969196483414380[7] = 0.0;
   out_3433969196483414380[8] = 0.0;
   out_3433969196483414380[9] = 0.0;
   out_3433969196483414380[10] = 0.0;
   out_3433969196483414380[11] = 0.0;
   out_3433969196483414380[12] = 0.0;
   out_3433969196483414380[13] = 0.0;
   out_3433969196483414380[14] = 0.0;
   out_3433969196483414380[15] = 0.0;
   out_3433969196483414380[16] = 0.0;
   out_3433969196483414380[17] = 0.0;
   out_3433969196483414380[18] = 0.0;
   out_3433969196483414380[19] = 1.0;
   out_3433969196483414380[20] = 0.0;
   out_3433969196483414380[21] = 0.0;
   out_3433969196483414380[22] = 0.0;
   out_3433969196483414380[23] = 0.0;
   out_3433969196483414380[24] = 0.0;
   out_3433969196483414380[25] = 0.0;
   out_3433969196483414380[26] = 0.0;
   out_3433969196483414380[27] = 0.0;
   out_3433969196483414380[28] = 0.0;
   out_3433969196483414380[29] = 0.0;
   out_3433969196483414380[30] = 0.0;
   out_3433969196483414380[31] = 0.0;
   out_3433969196483414380[32] = 0.0;
   out_3433969196483414380[33] = 0.0;
   out_3433969196483414380[34] = 0.0;
   out_3433969196483414380[35] = 0.0;
   out_3433969196483414380[36] = 0.0;
   out_3433969196483414380[37] = 0.0;
   out_3433969196483414380[38] = 1.0;
   out_3433969196483414380[39] = 0.0;
   out_3433969196483414380[40] = 0.0;
   out_3433969196483414380[41] = 0.0;
   out_3433969196483414380[42] = 0.0;
   out_3433969196483414380[43] = 0.0;
   out_3433969196483414380[44] = 0.0;
   out_3433969196483414380[45] = 0.0;
   out_3433969196483414380[46] = 0.0;
   out_3433969196483414380[47] = 0.0;
   out_3433969196483414380[48] = 0.0;
   out_3433969196483414380[49] = 0.0;
   out_3433969196483414380[50] = 0.0;
   out_3433969196483414380[51] = 0.0;
   out_3433969196483414380[52] = 0.0;
   out_3433969196483414380[53] = 0.0;
   out_3433969196483414380[54] = 0.0;
   out_3433969196483414380[55] = 0.0;
   out_3433969196483414380[56] = 0.0;
   out_3433969196483414380[57] = 1.0;
   out_3433969196483414380[58] = 0.0;
   out_3433969196483414380[59] = 0.0;
   out_3433969196483414380[60] = 0.0;
   out_3433969196483414380[61] = 0.0;
   out_3433969196483414380[62] = 0.0;
   out_3433969196483414380[63] = 0.0;
   out_3433969196483414380[64] = 0.0;
   out_3433969196483414380[65] = 0.0;
   out_3433969196483414380[66] = 0.0;
   out_3433969196483414380[67] = 0.0;
   out_3433969196483414380[68] = 0.0;
   out_3433969196483414380[69] = 0.0;
   out_3433969196483414380[70] = 0.0;
   out_3433969196483414380[71] = 0.0;
   out_3433969196483414380[72] = 0.0;
   out_3433969196483414380[73] = 0.0;
   out_3433969196483414380[74] = 0.0;
   out_3433969196483414380[75] = 0.0;
   out_3433969196483414380[76] = 1.0;
   out_3433969196483414380[77] = 0.0;
   out_3433969196483414380[78] = 0.0;
   out_3433969196483414380[79] = 0.0;
   out_3433969196483414380[80] = 0.0;
   out_3433969196483414380[81] = 0.0;
   out_3433969196483414380[82] = 0.0;
   out_3433969196483414380[83] = 0.0;
   out_3433969196483414380[84] = 0.0;
   out_3433969196483414380[85] = 0.0;
   out_3433969196483414380[86] = 0.0;
   out_3433969196483414380[87] = 0.0;
   out_3433969196483414380[88] = 0.0;
   out_3433969196483414380[89] = 0.0;
   out_3433969196483414380[90] = 0.0;
   out_3433969196483414380[91] = 0.0;
   out_3433969196483414380[92] = 0.0;
   out_3433969196483414380[93] = 0.0;
   out_3433969196483414380[94] = 0.0;
   out_3433969196483414380[95] = 1.0;
   out_3433969196483414380[96] = 0.0;
   out_3433969196483414380[97] = 0.0;
   out_3433969196483414380[98] = 0.0;
   out_3433969196483414380[99] = 0.0;
   out_3433969196483414380[100] = 0.0;
   out_3433969196483414380[101] = 0.0;
   out_3433969196483414380[102] = 0.0;
   out_3433969196483414380[103] = 0.0;
   out_3433969196483414380[104] = 0.0;
   out_3433969196483414380[105] = 0.0;
   out_3433969196483414380[106] = 0.0;
   out_3433969196483414380[107] = 0.0;
   out_3433969196483414380[108] = 0.0;
   out_3433969196483414380[109] = 0.0;
   out_3433969196483414380[110] = 0.0;
   out_3433969196483414380[111] = 0.0;
   out_3433969196483414380[112] = 0.0;
   out_3433969196483414380[113] = 0.0;
   out_3433969196483414380[114] = 1.0;
   out_3433969196483414380[115] = 0.0;
   out_3433969196483414380[116] = 0.0;
   out_3433969196483414380[117] = 0.0;
   out_3433969196483414380[118] = 0.0;
   out_3433969196483414380[119] = 0.0;
   out_3433969196483414380[120] = 0.0;
   out_3433969196483414380[121] = 0.0;
   out_3433969196483414380[122] = 0.0;
   out_3433969196483414380[123] = 0.0;
   out_3433969196483414380[124] = 0.0;
   out_3433969196483414380[125] = 0.0;
   out_3433969196483414380[126] = 0.0;
   out_3433969196483414380[127] = 0.0;
   out_3433969196483414380[128] = 0.0;
   out_3433969196483414380[129] = 0.0;
   out_3433969196483414380[130] = 0.0;
   out_3433969196483414380[131] = 0.0;
   out_3433969196483414380[132] = 0.0;
   out_3433969196483414380[133] = 1.0;
   out_3433969196483414380[134] = 0.0;
   out_3433969196483414380[135] = 0.0;
   out_3433969196483414380[136] = 0.0;
   out_3433969196483414380[137] = 0.0;
   out_3433969196483414380[138] = 0.0;
   out_3433969196483414380[139] = 0.0;
   out_3433969196483414380[140] = 0.0;
   out_3433969196483414380[141] = 0.0;
   out_3433969196483414380[142] = 0.0;
   out_3433969196483414380[143] = 0.0;
   out_3433969196483414380[144] = 0.0;
   out_3433969196483414380[145] = 0.0;
   out_3433969196483414380[146] = 0.0;
   out_3433969196483414380[147] = 0.0;
   out_3433969196483414380[148] = 0.0;
   out_3433969196483414380[149] = 0.0;
   out_3433969196483414380[150] = 0.0;
   out_3433969196483414380[151] = 0.0;
   out_3433969196483414380[152] = 1.0;
   out_3433969196483414380[153] = 0.0;
   out_3433969196483414380[154] = 0.0;
   out_3433969196483414380[155] = 0.0;
   out_3433969196483414380[156] = 0.0;
   out_3433969196483414380[157] = 0.0;
   out_3433969196483414380[158] = 0.0;
   out_3433969196483414380[159] = 0.0;
   out_3433969196483414380[160] = 0.0;
   out_3433969196483414380[161] = 0.0;
   out_3433969196483414380[162] = 0.0;
   out_3433969196483414380[163] = 0.0;
   out_3433969196483414380[164] = 0.0;
   out_3433969196483414380[165] = 0.0;
   out_3433969196483414380[166] = 0.0;
   out_3433969196483414380[167] = 0.0;
   out_3433969196483414380[168] = 0.0;
   out_3433969196483414380[169] = 0.0;
   out_3433969196483414380[170] = 0.0;
   out_3433969196483414380[171] = 1.0;
   out_3433969196483414380[172] = 0.0;
   out_3433969196483414380[173] = 0.0;
   out_3433969196483414380[174] = 0.0;
   out_3433969196483414380[175] = 0.0;
   out_3433969196483414380[176] = 0.0;
   out_3433969196483414380[177] = 0.0;
   out_3433969196483414380[178] = 0.0;
   out_3433969196483414380[179] = 0.0;
   out_3433969196483414380[180] = 0.0;
   out_3433969196483414380[181] = 0.0;
   out_3433969196483414380[182] = 0.0;
   out_3433969196483414380[183] = 0.0;
   out_3433969196483414380[184] = 0.0;
   out_3433969196483414380[185] = 0.0;
   out_3433969196483414380[186] = 0.0;
   out_3433969196483414380[187] = 0.0;
   out_3433969196483414380[188] = 0.0;
   out_3433969196483414380[189] = 0.0;
   out_3433969196483414380[190] = 1.0;
   out_3433969196483414380[191] = 0.0;
   out_3433969196483414380[192] = 0.0;
   out_3433969196483414380[193] = 0.0;
   out_3433969196483414380[194] = 0.0;
   out_3433969196483414380[195] = 0.0;
   out_3433969196483414380[196] = 0.0;
   out_3433969196483414380[197] = 0.0;
   out_3433969196483414380[198] = 0.0;
   out_3433969196483414380[199] = 0.0;
   out_3433969196483414380[200] = 0.0;
   out_3433969196483414380[201] = 0.0;
   out_3433969196483414380[202] = 0.0;
   out_3433969196483414380[203] = 0.0;
   out_3433969196483414380[204] = 0.0;
   out_3433969196483414380[205] = 0.0;
   out_3433969196483414380[206] = 0.0;
   out_3433969196483414380[207] = 0.0;
   out_3433969196483414380[208] = 0.0;
   out_3433969196483414380[209] = 1.0;
   out_3433969196483414380[210] = 0.0;
   out_3433969196483414380[211] = 0.0;
   out_3433969196483414380[212] = 0.0;
   out_3433969196483414380[213] = 0.0;
   out_3433969196483414380[214] = 0.0;
   out_3433969196483414380[215] = 0.0;
   out_3433969196483414380[216] = 0.0;
   out_3433969196483414380[217] = 0.0;
   out_3433969196483414380[218] = 0.0;
   out_3433969196483414380[219] = 0.0;
   out_3433969196483414380[220] = 0.0;
   out_3433969196483414380[221] = 0.0;
   out_3433969196483414380[222] = 0.0;
   out_3433969196483414380[223] = 0.0;
   out_3433969196483414380[224] = 0.0;
   out_3433969196483414380[225] = 0.0;
   out_3433969196483414380[226] = 0.0;
   out_3433969196483414380[227] = 0.0;
   out_3433969196483414380[228] = 1.0;
   out_3433969196483414380[229] = 0.0;
   out_3433969196483414380[230] = 0.0;
   out_3433969196483414380[231] = 0.0;
   out_3433969196483414380[232] = 0.0;
   out_3433969196483414380[233] = 0.0;
   out_3433969196483414380[234] = 0.0;
   out_3433969196483414380[235] = 0.0;
   out_3433969196483414380[236] = 0.0;
   out_3433969196483414380[237] = 0.0;
   out_3433969196483414380[238] = 0.0;
   out_3433969196483414380[239] = 0.0;
   out_3433969196483414380[240] = 0.0;
   out_3433969196483414380[241] = 0.0;
   out_3433969196483414380[242] = 0.0;
   out_3433969196483414380[243] = 0.0;
   out_3433969196483414380[244] = 0.0;
   out_3433969196483414380[245] = 0.0;
   out_3433969196483414380[246] = 0.0;
   out_3433969196483414380[247] = 1.0;
   out_3433969196483414380[248] = 0.0;
   out_3433969196483414380[249] = 0.0;
   out_3433969196483414380[250] = 0.0;
   out_3433969196483414380[251] = 0.0;
   out_3433969196483414380[252] = 0.0;
   out_3433969196483414380[253] = 0.0;
   out_3433969196483414380[254] = 0.0;
   out_3433969196483414380[255] = 0.0;
   out_3433969196483414380[256] = 0.0;
   out_3433969196483414380[257] = 0.0;
   out_3433969196483414380[258] = 0.0;
   out_3433969196483414380[259] = 0.0;
   out_3433969196483414380[260] = 0.0;
   out_3433969196483414380[261] = 0.0;
   out_3433969196483414380[262] = 0.0;
   out_3433969196483414380[263] = 0.0;
   out_3433969196483414380[264] = 0.0;
   out_3433969196483414380[265] = 0.0;
   out_3433969196483414380[266] = 1.0;
   out_3433969196483414380[267] = 0.0;
   out_3433969196483414380[268] = 0.0;
   out_3433969196483414380[269] = 0.0;
   out_3433969196483414380[270] = 0.0;
   out_3433969196483414380[271] = 0.0;
   out_3433969196483414380[272] = 0.0;
   out_3433969196483414380[273] = 0.0;
   out_3433969196483414380[274] = 0.0;
   out_3433969196483414380[275] = 0.0;
   out_3433969196483414380[276] = 0.0;
   out_3433969196483414380[277] = 0.0;
   out_3433969196483414380[278] = 0.0;
   out_3433969196483414380[279] = 0.0;
   out_3433969196483414380[280] = 0.0;
   out_3433969196483414380[281] = 0.0;
   out_3433969196483414380[282] = 0.0;
   out_3433969196483414380[283] = 0.0;
   out_3433969196483414380[284] = 0.0;
   out_3433969196483414380[285] = 1.0;
   out_3433969196483414380[286] = 0.0;
   out_3433969196483414380[287] = 0.0;
   out_3433969196483414380[288] = 0.0;
   out_3433969196483414380[289] = 0.0;
   out_3433969196483414380[290] = 0.0;
   out_3433969196483414380[291] = 0.0;
   out_3433969196483414380[292] = 0.0;
   out_3433969196483414380[293] = 0.0;
   out_3433969196483414380[294] = 0.0;
   out_3433969196483414380[295] = 0.0;
   out_3433969196483414380[296] = 0.0;
   out_3433969196483414380[297] = 0.0;
   out_3433969196483414380[298] = 0.0;
   out_3433969196483414380[299] = 0.0;
   out_3433969196483414380[300] = 0.0;
   out_3433969196483414380[301] = 0.0;
   out_3433969196483414380[302] = 0.0;
   out_3433969196483414380[303] = 0.0;
   out_3433969196483414380[304] = 1.0;
   out_3433969196483414380[305] = 0.0;
   out_3433969196483414380[306] = 0.0;
   out_3433969196483414380[307] = 0.0;
   out_3433969196483414380[308] = 0.0;
   out_3433969196483414380[309] = 0.0;
   out_3433969196483414380[310] = 0.0;
   out_3433969196483414380[311] = 0.0;
   out_3433969196483414380[312] = 0.0;
   out_3433969196483414380[313] = 0.0;
   out_3433969196483414380[314] = 0.0;
   out_3433969196483414380[315] = 0.0;
   out_3433969196483414380[316] = 0.0;
   out_3433969196483414380[317] = 0.0;
   out_3433969196483414380[318] = 0.0;
   out_3433969196483414380[319] = 0.0;
   out_3433969196483414380[320] = 0.0;
   out_3433969196483414380[321] = 0.0;
   out_3433969196483414380[322] = 0.0;
   out_3433969196483414380[323] = 1.0;
}
void f_fun(double *state, double dt, double *out_7050980600469623396) {
   out_7050980600469623396[0] = atan2((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), -(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]));
   out_7050980600469623396[1] = asin(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]));
   out_7050980600469623396[2] = atan2(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), -(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]));
   out_7050980600469623396[3] = dt*state[12] + state[3];
   out_7050980600469623396[4] = dt*state[13] + state[4];
   out_7050980600469623396[5] = dt*state[14] + state[5];
   out_7050980600469623396[6] = state[6];
   out_7050980600469623396[7] = state[7];
   out_7050980600469623396[8] = state[8];
   out_7050980600469623396[9] = state[9];
   out_7050980600469623396[10] = state[10];
   out_7050980600469623396[11] = state[11];
   out_7050980600469623396[12] = state[12];
   out_7050980600469623396[13] = state[13];
   out_7050980600469623396[14] = state[14];
   out_7050980600469623396[15] = state[15];
   out_7050980600469623396[16] = state[16];
   out_7050980600469623396[17] = state[17];
}
void F_fun(double *state, double dt, double *out_5108012196123939044) {
   out_5108012196123939044[0] = ((-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*cos(state[0])*cos(state[1]) - sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*cos(state[0])*cos(state[1]) - sin(dt*state[6])*sin(state[0])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_5108012196123939044[1] = ((-sin(dt*state[6])*sin(dt*state[8]) - sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*cos(state[1]) - (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*sin(state[1]) - sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(state[0]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*sin(state[1]) + (-sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) + sin(dt*state[8])*cos(dt*state[6]))*cos(state[1]) - sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(state[0]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_5108012196123939044[2] = 0;
   out_5108012196123939044[3] = 0;
   out_5108012196123939044[4] = 0;
   out_5108012196123939044[5] = 0;
   out_5108012196123939044[6] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(dt*cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) - dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_5108012196123939044[7] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*sin(dt*state[7])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[6])*sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) - dt*sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[7])*cos(dt*state[6])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[8])*sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]) - dt*sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_5108012196123939044[8] = ((dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((dt*sin(dt*state[6])*sin(dt*state[8]) + dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_5108012196123939044[9] = 0;
   out_5108012196123939044[10] = 0;
   out_5108012196123939044[11] = 0;
   out_5108012196123939044[12] = 0;
   out_5108012196123939044[13] = 0;
   out_5108012196123939044[14] = 0;
   out_5108012196123939044[15] = 0;
   out_5108012196123939044[16] = 0;
   out_5108012196123939044[17] = 0;
   out_5108012196123939044[18] = (-sin(dt*state[7])*sin(state[0])*cos(state[1]) - sin(dt*state[8])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_5108012196123939044[19] = (-sin(dt*state[7])*sin(state[1])*cos(state[0]) + sin(dt*state[8])*sin(state[0])*sin(state[1])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_5108012196123939044[20] = 0;
   out_5108012196123939044[21] = 0;
   out_5108012196123939044[22] = 0;
   out_5108012196123939044[23] = 0;
   out_5108012196123939044[24] = 0;
   out_5108012196123939044[25] = (dt*sin(dt*state[7])*sin(dt*state[8])*sin(state[0])*cos(state[1]) - dt*sin(dt*state[7])*sin(state[1])*cos(dt*state[8]) + dt*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_5108012196123939044[26] = (-dt*sin(dt*state[8])*sin(state[1])*cos(dt*state[7]) - dt*sin(state[0])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_5108012196123939044[27] = 0;
   out_5108012196123939044[28] = 0;
   out_5108012196123939044[29] = 0;
   out_5108012196123939044[30] = 0;
   out_5108012196123939044[31] = 0;
   out_5108012196123939044[32] = 0;
   out_5108012196123939044[33] = 0;
   out_5108012196123939044[34] = 0;
   out_5108012196123939044[35] = 0;
   out_5108012196123939044[36] = ((sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_5108012196123939044[37] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-sin(dt*state[7])*sin(state[2])*cos(state[0])*cos(state[1]) + sin(dt*state[8])*sin(state[0])*sin(state[2])*cos(dt*state[7])*cos(state[1]) - sin(state[1])*sin(state[2])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(-sin(dt*state[7])*cos(state[0])*cos(state[1])*cos(state[2]) + sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1])*cos(state[2]) - sin(state[1])*cos(dt*state[7])*cos(dt*state[8])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_5108012196123939044[38] = ((-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (-sin(state[0])*sin(state[1])*sin(state[2]) - cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_5108012196123939044[39] = 0;
   out_5108012196123939044[40] = 0;
   out_5108012196123939044[41] = 0;
   out_5108012196123939044[42] = 0;
   out_5108012196123939044[43] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(dt*(sin(state[0])*cos(state[2]) - sin(state[1])*sin(state[2])*cos(state[0]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*sin(state[2])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(dt*(-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_5108012196123939044[44] = (dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*sin(state[2])*cos(dt*state[7])*cos(state[1]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + (dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[7])*cos(state[1])*cos(state[2]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_5108012196123939044[45] = 0;
   out_5108012196123939044[46] = 0;
   out_5108012196123939044[47] = 0;
   out_5108012196123939044[48] = 0;
   out_5108012196123939044[49] = 0;
   out_5108012196123939044[50] = 0;
   out_5108012196123939044[51] = 0;
   out_5108012196123939044[52] = 0;
   out_5108012196123939044[53] = 0;
   out_5108012196123939044[54] = 0;
   out_5108012196123939044[55] = 0;
   out_5108012196123939044[56] = 0;
   out_5108012196123939044[57] = 1;
   out_5108012196123939044[58] = 0;
   out_5108012196123939044[59] = 0;
   out_5108012196123939044[60] = 0;
   out_5108012196123939044[61] = 0;
   out_5108012196123939044[62] = 0;
   out_5108012196123939044[63] = 0;
   out_5108012196123939044[64] = 0;
   out_5108012196123939044[65] = 0;
   out_5108012196123939044[66] = dt;
   out_5108012196123939044[67] = 0;
   out_5108012196123939044[68] = 0;
   out_5108012196123939044[69] = 0;
   out_5108012196123939044[70] = 0;
   out_5108012196123939044[71] = 0;
   out_5108012196123939044[72] = 0;
   out_5108012196123939044[73] = 0;
   out_5108012196123939044[74] = 0;
   out_5108012196123939044[75] = 0;
   out_5108012196123939044[76] = 1;
   out_5108012196123939044[77] = 0;
   out_5108012196123939044[78] = 0;
   out_5108012196123939044[79] = 0;
   out_5108012196123939044[80] = 0;
   out_5108012196123939044[81] = 0;
   out_5108012196123939044[82] = 0;
   out_5108012196123939044[83] = 0;
   out_5108012196123939044[84] = 0;
   out_5108012196123939044[85] = dt;
   out_5108012196123939044[86] = 0;
   out_5108012196123939044[87] = 0;
   out_5108012196123939044[88] = 0;
   out_5108012196123939044[89] = 0;
   out_5108012196123939044[90] = 0;
   out_5108012196123939044[91] = 0;
   out_5108012196123939044[92] = 0;
   out_5108012196123939044[93] = 0;
   out_5108012196123939044[94] = 0;
   out_5108012196123939044[95] = 1;
   out_5108012196123939044[96] = 0;
   out_5108012196123939044[97] = 0;
   out_5108012196123939044[98] = 0;
   out_5108012196123939044[99] = 0;
   out_5108012196123939044[100] = 0;
   out_5108012196123939044[101] = 0;
   out_5108012196123939044[102] = 0;
   out_5108012196123939044[103] = 0;
   out_5108012196123939044[104] = dt;
   out_5108012196123939044[105] = 0;
   out_5108012196123939044[106] = 0;
   out_5108012196123939044[107] = 0;
   out_5108012196123939044[108] = 0;
   out_5108012196123939044[109] = 0;
   out_5108012196123939044[110] = 0;
   out_5108012196123939044[111] = 0;
   out_5108012196123939044[112] = 0;
   out_5108012196123939044[113] = 0;
   out_5108012196123939044[114] = 1;
   out_5108012196123939044[115] = 0;
   out_5108012196123939044[116] = 0;
   out_5108012196123939044[117] = 0;
   out_5108012196123939044[118] = 0;
   out_5108012196123939044[119] = 0;
   out_5108012196123939044[120] = 0;
   out_5108012196123939044[121] = 0;
   out_5108012196123939044[122] = 0;
   out_5108012196123939044[123] = 0;
   out_5108012196123939044[124] = 0;
   out_5108012196123939044[125] = 0;
   out_5108012196123939044[126] = 0;
   out_5108012196123939044[127] = 0;
   out_5108012196123939044[128] = 0;
   out_5108012196123939044[129] = 0;
   out_5108012196123939044[130] = 0;
   out_5108012196123939044[131] = 0;
   out_5108012196123939044[132] = 0;
   out_5108012196123939044[133] = 1;
   out_5108012196123939044[134] = 0;
   out_5108012196123939044[135] = 0;
   out_5108012196123939044[136] = 0;
   out_5108012196123939044[137] = 0;
   out_5108012196123939044[138] = 0;
   out_5108012196123939044[139] = 0;
   out_5108012196123939044[140] = 0;
   out_5108012196123939044[141] = 0;
   out_5108012196123939044[142] = 0;
   out_5108012196123939044[143] = 0;
   out_5108012196123939044[144] = 0;
   out_5108012196123939044[145] = 0;
   out_5108012196123939044[146] = 0;
   out_5108012196123939044[147] = 0;
   out_5108012196123939044[148] = 0;
   out_5108012196123939044[149] = 0;
   out_5108012196123939044[150] = 0;
   out_5108012196123939044[151] = 0;
   out_5108012196123939044[152] = 1;
   out_5108012196123939044[153] = 0;
   out_5108012196123939044[154] = 0;
   out_5108012196123939044[155] = 0;
   out_5108012196123939044[156] = 0;
   out_5108012196123939044[157] = 0;
   out_5108012196123939044[158] = 0;
   out_5108012196123939044[159] = 0;
   out_5108012196123939044[160] = 0;
   out_5108012196123939044[161] = 0;
   out_5108012196123939044[162] = 0;
   out_5108012196123939044[163] = 0;
   out_5108012196123939044[164] = 0;
   out_5108012196123939044[165] = 0;
   out_5108012196123939044[166] = 0;
   out_5108012196123939044[167] = 0;
   out_5108012196123939044[168] = 0;
   out_5108012196123939044[169] = 0;
   out_5108012196123939044[170] = 0;
   out_5108012196123939044[171] = 1;
   out_5108012196123939044[172] = 0;
   out_5108012196123939044[173] = 0;
   out_5108012196123939044[174] = 0;
   out_5108012196123939044[175] = 0;
   out_5108012196123939044[176] = 0;
   out_5108012196123939044[177] = 0;
   out_5108012196123939044[178] = 0;
   out_5108012196123939044[179] = 0;
   out_5108012196123939044[180] = 0;
   out_5108012196123939044[181] = 0;
   out_5108012196123939044[182] = 0;
   out_5108012196123939044[183] = 0;
   out_5108012196123939044[184] = 0;
   out_5108012196123939044[185] = 0;
   out_5108012196123939044[186] = 0;
   out_5108012196123939044[187] = 0;
   out_5108012196123939044[188] = 0;
   out_5108012196123939044[189] = 0;
   out_5108012196123939044[190] = 1;
   out_5108012196123939044[191] = 0;
   out_5108012196123939044[192] = 0;
   out_5108012196123939044[193] = 0;
   out_5108012196123939044[194] = 0;
   out_5108012196123939044[195] = 0;
   out_5108012196123939044[196] = 0;
   out_5108012196123939044[197] = 0;
   out_5108012196123939044[198] = 0;
   out_5108012196123939044[199] = 0;
   out_5108012196123939044[200] = 0;
   out_5108012196123939044[201] = 0;
   out_5108012196123939044[202] = 0;
   out_5108012196123939044[203] = 0;
   out_5108012196123939044[204] = 0;
   out_5108012196123939044[205] = 0;
   out_5108012196123939044[206] = 0;
   out_5108012196123939044[207] = 0;
   out_5108012196123939044[208] = 0;
   out_5108012196123939044[209] = 1;
   out_5108012196123939044[210] = 0;
   out_5108012196123939044[211] = 0;
   out_5108012196123939044[212] = 0;
   out_5108012196123939044[213] = 0;
   out_5108012196123939044[214] = 0;
   out_5108012196123939044[215] = 0;
   out_5108012196123939044[216] = 0;
   out_5108012196123939044[217] = 0;
   out_5108012196123939044[218] = 0;
   out_5108012196123939044[219] = 0;
   out_5108012196123939044[220] = 0;
   out_5108012196123939044[221] = 0;
   out_5108012196123939044[222] = 0;
   out_5108012196123939044[223] = 0;
   out_5108012196123939044[224] = 0;
   out_5108012196123939044[225] = 0;
   out_5108012196123939044[226] = 0;
   out_5108012196123939044[227] = 0;
   out_5108012196123939044[228] = 1;
   out_5108012196123939044[229] = 0;
   out_5108012196123939044[230] = 0;
   out_5108012196123939044[231] = 0;
   out_5108012196123939044[232] = 0;
   out_5108012196123939044[233] = 0;
   out_5108012196123939044[234] = 0;
   out_5108012196123939044[235] = 0;
   out_5108012196123939044[236] = 0;
   out_5108012196123939044[237] = 0;
   out_5108012196123939044[238] = 0;
   out_5108012196123939044[239] = 0;
   out_5108012196123939044[240] = 0;
   out_5108012196123939044[241] = 0;
   out_5108012196123939044[242] = 0;
   out_5108012196123939044[243] = 0;
   out_5108012196123939044[244] = 0;
   out_5108012196123939044[245] = 0;
   out_5108012196123939044[246] = 0;
   out_5108012196123939044[247] = 1;
   out_5108012196123939044[248] = 0;
   out_5108012196123939044[249] = 0;
   out_5108012196123939044[250] = 0;
   out_5108012196123939044[251] = 0;
   out_5108012196123939044[252] = 0;
   out_5108012196123939044[253] = 0;
   out_5108012196123939044[254] = 0;
   out_5108012196123939044[255] = 0;
   out_5108012196123939044[256] = 0;
   out_5108012196123939044[257] = 0;
   out_5108012196123939044[258] = 0;
   out_5108012196123939044[259] = 0;
   out_5108012196123939044[260] = 0;
   out_5108012196123939044[261] = 0;
   out_5108012196123939044[262] = 0;
   out_5108012196123939044[263] = 0;
   out_5108012196123939044[264] = 0;
   out_5108012196123939044[265] = 0;
   out_5108012196123939044[266] = 1;
   out_5108012196123939044[267] = 0;
   out_5108012196123939044[268] = 0;
   out_5108012196123939044[269] = 0;
   out_5108012196123939044[270] = 0;
   out_5108012196123939044[271] = 0;
   out_5108012196123939044[272] = 0;
   out_5108012196123939044[273] = 0;
   out_5108012196123939044[274] = 0;
   out_5108012196123939044[275] = 0;
   out_5108012196123939044[276] = 0;
   out_5108012196123939044[277] = 0;
   out_5108012196123939044[278] = 0;
   out_5108012196123939044[279] = 0;
   out_5108012196123939044[280] = 0;
   out_5108012196123939044[281] = 0;
   out_5108012196123939044[282] = 0;
   out_5108012196123939044[283] = 0;
   out_5108012196123939044[284] = 0;
   out_5108012196123939044[285] = 1;
   out_5108012196123939044[286] = 0;
   out_5108012196123939044[287] = 0;
   out_5108012196123939044[288] = 0;
   out_5108012196123939044[289] = 0;
   out_5108012196123939044[290] = 0;
   out_5108012196123939044[291] = 0;
   out_5108012196123939044[292] = 0;
   out_5108012196123939044[293] = 0;
   out_5108012196123939044[294] = 0;
   out_5108012196123939044[295] = 0;
   out_5108012196123939044[296] = 0;
   out_5108012196123939044[297] = 0;
   out_5108012196123939044[298] = 0;
   out_5108012196123939044[299] = 0;
   out_5108012196123939044[300] = 0;
   out_5108012196123939044[301] = 0;
   out_5108012196123939044[302] = 0;
   out_5108012196123939044[303] = 0;
   out_5108012196123939044[304] = 1;
   out_5108012196123939044[305] = 0;
   out_5108012196123939044[306] = 0;
   out_5108012196123939044[307] = 0;
   out_5108012196123939044[308] = 0;
   out_5108012196123939044[309] = 0;
   out_5108012196123939044[310] = 0;
   out_5108012196123939044[311] = 0;
   out_5108012196123939044[312] = 0;
   out_5108012196123939044[313] = 0;
   out_5108012196123939044[314] = 0;
   out_5108012196123939044[315] = 0;
   out_5108012196123939044[316] = 0;
   out_5108012196123939044[317] = 0;
   out_5108012196123939044[318] = 0;
   out_5108012196123939044[319] = 0;
   out_5108012196123939044[320] = 0;
   out_5108012196123939044[321] = 0;
   out_5108012196123939044[322] = 0;
   out_5108012196123939044[323] = 1;
}
void h_4(double *state, double *unused, double *out_3112222217623107327) {
   out_3112222217623107327[0] = state[6] + state[9];
   out_3112222217623107327[1] = state[7] + state[10];
   out_3112222217623107327[2] = state[8] + state[11];
}
void H_4(double *state, double *unused, double *out_1718549188555666355) {
   out_1718549188555666355[0] = 0;
   out_1718549188555666355[1] = 0;
   out_1718549188555666355[2] = 0;
   out_1718549188555666355[3] = 0;
   out_1718549188555666355[4] = 0;
   out_1718549188555666355[5] = 0;
   out_1718549188555666355[6] = 1;
   out_1718549188555666355[7] = 0;
   out_1718549188555666355[8] = 0;
   out_1718549188555666355[9] = 1;
   out_1718549188555666355[10] = 0;
   out_1718549188555666355[11] = 0;
   out_1718549188555666355[12] = 0;
   out_1718549188555666355[13] = 0;
   out_1718549188555666355[14] = 0;
   out_1718549188555666355[15] = 0;
   out_1718549188555666355[16] = 0;
   out_1718549188555666355[17] = 0;
   out_1718549188555666355[18] = 0;
   out_1718549188555666355[19] = 0;
   out_1718549188555666355[20] = 0;
   out_1718549188555666355[21] = 0;
   out_1718549188555666355[22] = 0;
   out_1718549188555666355[23] = 0;
   out_1718549188555666355[24] = 0;
   out_1718549188555666355[25] = 1;
   out_1718549188555666355[26] = 0;
   out_1718549188555666355[27] = 0;
   out_1718549188555666355[28] = 1;
   out_1718549188555666355[29] = 0;
   out_1718549188555666355[30] = 0;
   out_1718549188555666355[31] = 0;
   out_1718549188555666355[32] = 0;
   out_1718549188555666355[33] = 0;
   out_1718549188555666355[34] = 0;
   out_1718549188555666355[35] = 0;
   out_1718549188555666355[36] = 0;
   out_1718549188555666355[37] = 0;
   out_1718549188555666355[38] = 0;
   out_1718549188555666355[39] = 0;
   out_1718549188555666355[40] = 0;
   out_1718549188555666355[41] = 0;
   out_1718549188555666355[42] = 0;
   out_1718549188555666355[43] = 0;
   out_1718549188555666355[44] = 1;
   out_1718549188555666355[45] = 0;
   out_1718549188555666355[46] = 0;
   out_1718549188555666355[47] = 1;
   out_1718549188555666355[48] = 0;
   out_1718549188555666355[49] = 0;
   out_1718549188555666355[50] = 0;
   out_1718549188555666355[51] = 0;
   out_1718549188555666355[52] = 0;
   out_1718549188555666355[53] = 0;
}
void h_10(double *state, double *unused, double *out_3363321840709035115) {
   out_3363321840709035115[0] = 9.8100000000000005*sin(state[1]) - state[4]*state[8] + state[5]*state[7] + state[12] + state[15];
   out_3363321840709035115[1] = -9.8100000000000005*sin(state[0])*cos(state[1]) + state[3]*state[8] - state[5]*state[6] + state[13] + state[16];
   out_3363321840709035115[2] = -9.8100000000000005*cos(state[0])*cos(state[1]) - state[3]*state[7] + state[4]*state[6] + state[14] + state[17];
}
void H_10(double *state, double *unused, double *out_7887304865878975139) {
   out_7887304865878975139[0] = 0;
   out_7887304865878975139[1] = 9.8100000000000005*cos(state[1]);
   out_7887304865878975139[2] = 0;
   out_7887304865878975139[3] = 0;
   out_7887304865878975139[4] = -state[8];
   out_7887304865878975139[5] = state[7];
   out_7887304865878975139[6] = 0;
   out_7887304865878975139[7] = state[5];
   out_7887304865878975139[8] = -state[4];
   out_7887304865878975139[9] = 0;
   out_7887304865878975139[10] = 0;
   out_7887304865878975139[11] = 0;
   out_7887304865878975139[12] = 1;
   out_7887304865878975139[13] = 0;
   out_7887304865878975139[14] = 0;
   out_7887304865878975139[15] = 1;
   out_7887304865878975139[16] = 0;
   out_7887304865878975139[17] = 0;
   out_7887304865878975139[18] = -9.8100000000000005*cos(state[0])*cos(state[1]);
   out_7887304865878975139[19] = 9.8100000000000005*sin(state[0])*sin(state[1]);
   out_7887304865878975139[20] = 0;
   out_7887304865878975139[21] = state[8];
   out_7887304865878975139[22] = 0;
   out_7887304865878975139[23] = -state[6];
   out_7887304865878975139[24] = -state[5];
   out_7887304865878975139[25] = 0;
   out_7887304865878975139[26] = state[3];
   out_7887304865878975139[27] = 0;
   out_7887304865878975139[28] = 0;
   out_7887304865878975139[29] = 0;
   out_7887304865878975139[30] = 0;
   out_7887304865878975139[31] = 1;
   out_7887304865878975139[32] = 0;
   out_7887304865878975139[33] = 0;
   out_7887304865878975139[34] = 1;
   out_7887304865878975139[35] = 0;
   out_7887304865878975139[36] = 9.8100000000000005*sin(state[0])*cos(state[1]);
   out_7887304865878975139[37] = 9.8100000000000005*sin(state[1])*cos(state[0]);
   out_7887304865878975139[38] = 0;
   out_7887304865878975139[39] = -state[7];
   out_7887304865878975139[40] = state[6];
   out_7887304865878975139[41] = 0;
   out_7887304865878975139[42] = state[4];
   out_7887304865878975139[43] = -state[3];
   out_7887304865878975139[44] = 0;
   out_7887304865878975139[45] = 0;
   out_7887304865878975139[46] = 0;
   out_7887304865878975139[47] = 0;
   out_7887304865878975139[48] = 0;
   out_7887304865878975139[49] = 0;
   out_7887304865878975139[50] = 1;
   out_7887304865878975139[51] = 0;
   out_7887304865878975139[52] = 0;
   out_7887304865878975139[53] = 1;
}
void h_13(double *state, double *unused, double *out_7518159681466847348) {
   out_7518159681466847348[0] = state[3];
   out_7518159681466847348[1] = state[4];
   out_7518159681466847348[2] = state[5];
}
void H_13(double *state, double *unused, double *out_2115206274746857669) {
   out_2115206274746857669[0] = 0;
   out_2115206274746857669[1] = 0;
   out_2115206274746857669[2] = 0;
   out_2115206274746857669[3] = 1;
   out_2115206274746857669[4] = 0;
   out_2115206274746857669[5] = 0;
   out_2115206274746857669[6] = 0;
   out_2115206274746857669[7] = 0;
   out_2115206274746857669[8] = 0;
   out_2115206274746857669[9] = 0;
   out_2115206274746857669[10] = 0;
   out_2115206274746857669[11] = 0;
   out_2115206274746857669[12] = 0;
   out_2115206274746857669[13] = 0;
   out_2115206274746857669[14] = 0;
   out_2115206274746857669[15] = 0;
   out_2115206274746857669[16] = 0;
   out_2115206274746857669[17] = 0;
   out_2115206274746857669[18] = 0;
   out_2115206274746857669[19] = 0;
   out_2115206274746857669[20] = 0;
   out_2115206274746857669[21] = 0;
   out_2115206274746857669[22] = 1;
   out_2115206274746857669[23] = 0;
   out_2115206274746857669[24] = 0;
   out_2115206274746857669[25] = 0;
   out_2115206274746857669[26] = 0;
   out_2115206274746857669[27] = 0;
   out_2115206274746857669[28] = 0;
   out_2115206274746857669[29] = 0;
   out_2115206274746857669[30] = 0;
   out_2115206274746857669[31] = 0;
   out_2115206274746857669[32] = 0;
   out_2115206274746857669[33] = 0;
   out_2115206274746857669[34] = 0;
   out_2115206274746857669[35] = 0;
   out_2115206274746857669[36] = 0;
   out_2115206274746857669[37] = 0;
   out_2115206274746857669[38] = 0;
   out_2115206274746857669[39] = 0;
   out_2115206274746857669[40] = 0;
   out_2115206274746857669[41] = 1;
   out_2115206274746857669[42] = 0;
   out_2115206274746857669[43] = 0;
   out_2115206274746857669[44] = 0;
   out_2115206274746857669[45] = 0;
   out_2115206274746857669[46] = 0;
   out_2115206274746857669[47] = 0;
   out_2115206274746857669[48] = 0;
   out_2115206274746857669[49] = 0;
   out_2115206274746857669[50] = 0;
   out_2115206274746857669[51] = 0;
   out_2115206274746857669[52] = 0;
   out_2115206274746857669[53] = 0;
}
void h_14(double *state, double *unused, double *out_3485677374259695923) {
   out_3485677374259695923[0] = state[6];
   out_3485677374259695923[1] = state[7];
   out_3485677374259695923[2] = state[8];
}
void H_14(double *state, double *unused, double *out_1364239243739705941) {
   out_1364239243739705941[0] = 0;
   out_1364239243739705941[1] = 0;
   out_1364239243739705941[2] = 0;
   out_1364239243739705941[3] = 0;
   out_1364239243739705941[4] = 0;
   out_1364239243739705941[5] = 0;
   out_1364239243739705941[6] = 1;
   out_1364239243739705941[7] = 0;
   out_1364239243739705941[8] = 0;
   out_1364239243739705941[9] = 0;
   out_1364239243739705941[10] = 0;
   out_1364239243739705941[11] = 0;
   out_1364239243739705941[12] = 0;
   out_1364239243739705941[13] = 0;
   out_1364239243739705941[14] = 0;
   out_1364239243739705941[15] = 0;
   out_1364239243739705941[16] = 0;
   out_1364239243739705941[17] = 0;
   out_1364239243739705941[18] = 0;
   out_1364239243739705941[19] = 0;
   out_1364239243739705941[20] = 0;
   out_1364239243739705941[21] = 0;
   out_1364239243739705941[22] = 0;
   out_1364239243739705941[23] = 0;
   out_1364239243739705941[24] = 0;
   out_1364239243739705941[25] = 1;
   out_1364239243739705941[26] = 0;
   out_1364239243739705941[27] = 0;
   out_1364239243739705941[28] = 0;
   out_1364239243739705941[29] = 0;
   out_1364239243739705941[30] = 0;
   out_1364239243739705941[31] = 0;
   out_1364239243739705941[32] = 0;
   out_1364239243739705941[33] = 0;
   out_1364239243739705941[34] = 0;
   out_1364239243739705941[35] = 0;
   out_1364239243739705941[36] = 0;
   out_1364239243739705941[37] = 0;
   out_1364239243739705941[38] = 0;
   out_1364239243739705941[39] = 0;
   out_1364239243739705941[40] = 0;
   out_1364239243739705941[41] = 0;
   out_1364239243739705941[42] = 0;
   out_1364239243739705941[43] = 0;
   out_1364239243739705941[44] = 1;
   out_1364239243739705941[45] = 0;
   out_1364239243739705941[46] = 0;
   out_1364239243739705941[47] = 0;
   out_1364239243739705941[48] = 0;
   out_1364239243739705941[49] = 0;
   out_1364239243739705941[50] = 0;
   out_1364239243739705941[51] = 0;
   out_1364239243739705941[52] = 0;
   out_1364239243739705941[53] = 0;
}
#include <eigen3/Eigen/Dense>
#include <iostream>

typedef Eigen::Matrix<double, DIM, DIM, Eigen::RowMajor> DDM;
typedef Eigen::Matrix<double, EDIM, EDIM, Eigen::RowMajor> EEM;
typedef Eigen::Matrix<double, DIM, EDIM, Eigen::RowMajor> DEM;

void predict(double *in_x, double *in_P, double *in_Q, double dt) {
  typedef Eigen::Matrix<double, MEDIM, MEDIM, Eigen::RowMajor> RRM;

  double nx[DIM] = {0};
  double in_F[EDIM*EDIM] = {0};

  // functions from sympy
  f_fun(in_x, dt, nx);
  F_fun(in_x, dt, in_F);


  EEM F(in_F);
  EEM P(in_P);
  EEM Q(in_Q);

  RRM F_main = F.topLeftCorner(MEDIM, MEDIM);
  P.topLeftCorner(MEDIM, MEDIM) = (F_main * P.topLeftCorner(MEDIM, MEDIM)) * F_main.transpose();
  P.topRightCorner(MEDIM, EDIM - MEDIM) = F_main * P.topRightCorner(MEDIM, EDIM - MEDIM);
  P.bottomLeftCorner(EDIM - MEDIM, MEDIM) = P.bottomLeftCorner(EDIM - MEDIM, MEDIM) * F_main.transpose();

  P = P + dt*Q;

  // copy out state
  memcpy(in_x, nx, DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
}

// note: extra_args dim only correct when null space projecting
// otherwise 1
template <int ZDIM, int EADIM, bool MAHA_TEST>
void update(double *in_x, double *in_P, Hfun h_fun, Hfun H_fun, Hfun Hea_fun, double *in_z, double *in_R, double *in_ea, double MAHA_THRESHOLD) {
  typedef Eigen::Matrix<double, ZDIM, ZDIM, Eigen::RowMajor> ZZM;
  typedef Eigen::Matrix<double, ZDIM, DIM, Eigen::RowMajor> ZDM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, EDIM, Eigen::RowMajor> XEM;
  //typedef Eigen::Matrix<double, EDIM, ZDIM, Eigen::RowMajor> EZM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, 1> X1M;
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> XXM;

  double in_hx[ZDIM] = {0};
  double in_H[ZDIM * DIM] = {0};
  double in_H_mod[EDIM * DIM] = {0};
  double delta_x[EDIM] = {0};
  double x_new[DIM] = {0};


  // state x, P
  Eigen::Matrix<double, ZDIM, 1> z(in_z);
  EEM P(in_P);
  ZZM pre_R(in_R);

  // functions from sympy
  h_fun(in_x, in_ea, in_hx);
  H_fun(in_x, in_ea, in_H);
  ZDM pre_H(in_H);

  // get y (y = z - hx)
  Eigen::Matrix<double, ZDIM, 1> pre_y(in_hx); pre_y = z - pre_y;
  X1M y; XXM H; XXM R;
  if (Hea_fun){
    typedef Eigen::Matrix<double, ZDIM, EADIM, Eigen::RowMajor> ZAM;
    double in_Hea[ZDIM * EADIM] = {0};
    Hea_fun(in_x, in_ea, in_Hea);
    ZAM Hea(in_Hea);
    XXM A = Hea.transpose().fullPivLu().kernel();


    y = A.transpose() * pre_y;
    H = A.transpose() * pre_H;
    R = A.transpose() * pre_R * A;
  } else {
    y = pre_y;
    H = pre_H;
    R = pre_R;
  }
  // get modified H
  H_mod_fun(in_x, in_H_mod);
  DEM H_mod(in_H_mod);
  XEM H_err = H * H_mod;

  // Do mahalobis distance test
  if (MAHA_TEST){
    XXM a = (H_err * P * H_err.transpose() + R).inverse();
    double maha_dist = y.transpose() * a * y;
    if (maha_dist > MAHA_THRESHOLD){
      R = 1.0e16 * R;
    }
  }

  // Outlier resilient weighting
  double weight = 1;//(1.5)/(1 + y.squaredNorm()/R.sum());

  // kalman gains and I_KH
  XXM S = ((H_err * P) * H_err.transpose()) + R/weight;
  XEM KT = S.fullPivLu().solve(H_err * P.transpose());
  //EZM K = KT.transpose(); TODO: WHY DOES THIS NOT COMPILE?
  //EZM K = S.fullPivLu().solve(H_err * P.transpose()).transpose();
  //std::cout << "Here is the matrix rot:\n" << K << std::endl;
  EEM I_KH = Eigen::Matrix<double, EDIM, EDIM>::Identity() - (KT.transpose() * H_err);

  // update state by injecting dx
  Eigen::Matrix<double, EDIM, 1> dx(delta_x);
  dx  = (KT.transpose() * y);
  memcpy(delta_x, dx.data(), EDIM * sizeof(double));
  err_fun(in_x, delta_x, x_new);
  Eigen::Matrix<double, DIM, 1> x(x_new);

  // update cov
  P = ((I_KH * P) * I_KH.transpose()) + ((KT.transpose() * R) * KT);

  // copy out state
  memcpy(in_x, x.data(), DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
  memcpy(in_z, y.data(), y.rows() * sizeof(double));
}




}
extern "C" {

void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_4, H_4, NULL, in_z, in_R, in_ea, MAHA_THRESH_4);
}
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_10, H_10, NULL, in_z, in_R, in_ea, MAHA_THRESH_10);
}
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_13, H_13, NULL, in_z, in_R, in_ea, MAHA_THRESH_13);
}
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_14, H_14, NULL, in_z, in_R, in_ea, MAHA_THRESH_14);
}
void pose_err_fun(double *nom_x, double *delta_x, double *out_8660653318681785761) {
  err_fun(nom_x, delta_x, out_8660653318681785761);
}
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_1461130683285097740) {
  inv_err_fun(nom_x, true_x, out_1461130683285097740);
}
void pose_H_mod_fun(double *state, double *out_3433969196483414380) {
  H_mod_fun(state, out_3433969196483414380);
}
void pose_f_fun(double *state, double dt, double *out_7050980600469623396) {
  f_fun(state,  dt, out_7050980600469623396);
}
void pose_F_fun(double *state, double dt, double *out_5108012196123939044) {
  F_fun(state,  dt, out_5108012196123939044);
}
void pose_h_4(double *state, double *unused, double *out_3112222217623107327) {
  h_4(state, unused, out_3112222217623107327);
}
void pose_H_4(double *state, double *unused, double *out_1718549188555666355) {
  H_4(state, unused, out_1718549188555666355);
}
void pose_h_10(double *state, double *unused, double *out_3363321840709035115) {
  h_10(state, unused, out_3363321840709035115);
}
void pose_H_10(double *state, double *unused, double *out_7887304865878975139) {
  H_10(state, unused, out_7887304865878975139);
}
void pose_h_13(double *state, double *unused, double *out_7518159681466847348) {
  h_13(state, unused, out_7518159681466847348);
}
void pose_H_13(double *state, double *unused, double *out_2115206274746857669) {
  H_13(state, unused, out_2115206274746857669);
}
void pose_h_14(double *state, double *unused, double *out_3485677374259695923) {
  h_14(state, unused, out_3485677374259695923);
}
void pose_H_14(double *state, double *unused, double *out_1364239243739705941) {
  H_14(state, unused, out_1364239243739705941);
}
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
}

const EKF pose = {
  .name = "pose",
  .kinds = { 4, 10, 13, 14 },
  .feature_kinds = {  },
  .f_fun = pose_f_fun,
  .F_fun = pose_F_fun,
  .err_fun = pose_err_fun,
  .inv_err_fun = pose_inv_err_fun,
  .H_mod_fun = pose_H_mod_fun,
  .predict = pose_predict,
  .hs = {
    { 4, pose_h_4 },
    { 10, pose_h_10 },
    { 13, pose_h_13 },
    { 14, pose_h_14 },
  },
  .Hs = {
    { 4, pose_H_4 },
    { 10, pose_H_10 },
    { 13, pose_H_13 },
    { 14, pose_H_14 },
  },
  .updates = {
    { 4, pose_update_4 },
    { 10, pose_update_10 },
    { 13, pose_update_13 },
    { 14, pose_update_14 },
  },
  .Hes = {
  },
  .sets = {
  },
  .extra_routines = {
  },
};

ekf_lib_init(pose)
