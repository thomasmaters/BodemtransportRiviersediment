/*
 * DepthProfiler.cpp
 *
 *  Created on: 2 Jul 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */
namespace Controller
{
template <std::size_t N, typename T>
DepthProfiler<N, T>::DepthProfiler()
{
	Matrix<480,3,T> input1({{0.0,4.0,0},
		{0.0965,4.03425,0},
		{0.19199999999999998,4.0840000000000005,0},
		{0.2955,4.144749999999999,0},
		{0.4159999999999999,4.212,0},
		{0.5625,4.28125,0},
		{0.744,4.348,0},
		{0.9694999999999999,4.40775,0},
		{1.2479999999999998,4.455999999999999,0},
		{1.5885000000000005,4.48825,0},
		{2.0,4.5,0},
		{2.5199999999999996,4.4815,0},
		{3.16,4.4319999999999995,0},
		{3.8899999999999997,4.360499999999999,0},
		{4.68,4.276,0},
		{5.5,4.1875,0},
		{6.319999999999999,4.104,0},
		{7.109999999999999,4.0344999999999995,0},
		{7.839999999999999,3.988,0},
		{8.48,3.973499999999999,0},
		{9.0,4.0,0},
		{9.38,4.0825000000000005,0},
		{9.64,4.220000000000001,0},
		{9.809999999999999,4.3975,0},
		{9.919999999999998,4.6,0},
		{10.0,4.8125,0},
		{10.080000000000002,5.02,0},
		{10.189999999999998,5.2075,0},
		{10.36,5.359999999999999,0},
		{10.620000000000001,5.4625,0},
		{11.0,5.5,0},
		{11.5155,5.4647499999999996,0},
		{12.144,5.367999999999999,0},
		{12.8585,5.223249999999999,0},
		{13.632,5.044,0},
		{14.4375,4.84375,0},
		{15.247999999999998,4.636,0},
		{16.036499999999997,4.4342500000000005,0},
		{16.775999999999996,4.251999999999999,0},
		{17.4395,4.1027499999999995,0},
		{18.0,4.0,0},
		{18.4665,3.937,0},
		{18.872,3.8960000000000004,0},
		{19.2255,3.8740000000000006,0},
		{19.536,3.8679999999999994,0},
		{19.8125,3.875,0},
		{20.064,3.8920000000000003,0},
		{20.2995,3.9159999999999995,0},
		{20.527999999999995,3.943999999999999,0},
		{20.758499999999998,3.973,0},
		{21.0,4.0,0},
		{21.218,4.03425,0},
		{21.384,4.0840000000000005,0},
		{21.516,4.144749999999999,0},
		{21.631999999999998,4.212,0},
		{21.75,4.28125,0},
		{21.888,4.348,0},
		{22.064,4.40775,0},
		{22.296,4.455999999999999,0},
		{22.601999999999997,4.48825,0},
		{23.0,4.5,0},
		{23.52,4.486,0},
		{24.159999999999997,4.4479999999999995,0},
		{24.89,4.3919999999999995,0},
		{25.679999999999996,4.324,0},
		{26.5,4.25,0},
		{27.320000000000004,4.176,0},
		{28.11,4.108,0},
		{28.839999999999996,4.052,0},
		{29.48,4.013999999999999,0},
		{30.0,4.0,0},
		{30.38,4.014,0},
		{30.64,4.0520000000000005,0},
		{30.81,4.108,0},
		{30.92,4.176,0},
		{31.0,4.25,0},
		{31.080000000000005,4.324,0},
		{31.189999999999998,4.3919999999999995,0},
		{31.359999999999996,4.447999999999999,0},
		{31.62,4.486,0},
		{32.0,4.5,0},
		{32.519999999999996,4.486,0},
		{33.160000000000004,4.4479999999999995,0},
		{33.89,4.3919999999999995,0},
		{34.67999999999999,4.324,0},
		{35.5,4.25,0},
		{36.32000000000001,4.176,0},
		{37.11,4.108,0},
		{37.839999999999996,4.052,0},
		{38.480000000000004,4.013999999999999,0},
		{39.0,4.0,0},
		{39.379999999999995,4.014,0},
		{39.63999999999999,4.0520000000000005,0},
		{39.81,4.108,0},
		{39.92,4.176,0},
		{40.0,4.25,0},
		{40.08,4.324,0},
		{40.19,4.3919999999999995,0},
		{40.35999999999999,4.447999999999999,0},
		{40.61999999999999,4.486,0},
		{41.0,4.5,0},
		{41.51999999999999,4.4815,0},
		{42.160000000000004,4.4319999999999995,0},
		{42.88999999999999,4.360499999999999,0},
		{43.68,4.276,0},
		{44.5,4.1875,0},
		{45.32,4.104,0},
		{46.11,4.0344999999999995,0},
		{46.83999999999999,3.988,0},
		{47.48,3.973499999999999,0},
		{48.0,4.0,0},
		{48.379999999999995,4.0825000000000005,0},
		{48.64,4.220000000000001,0},
		{48.81,4.3975,0},
		{48.92,4.6,0},
		{49.0,4.8125,0},
		{49.08,5.02,0},
		{49.18999999999999,5.2075,0},
		{49.35999999999999,5.359999999999999,0},
		{49.620000000000005,5.4625,0},
		{50.0,5.5,0},
		{50.51549999999999,5.4647499999999996,0},
		{51.144,5.367999999999999,0},
		{51.8585,5.223249999999999,0},
		{52.632,5.044,0},
		{53.4375,4.84375,0},
		{54.248000000000005,4.636,0},
		{55.036500000000004,4.4342500000000005,0},
		{55.775999999999996,4.251999999999999,0},
		{56.439499999999995,4.1027499999999995,0},
		{57.0,4.0,0},
		{57.466499999999996,3.9324999999999997,0},
		{57.872,3.8800000000000003,0},
		{58.2255,3.8425000000000007,0},
		{58.535999999999994,3.8199999999999994,0},
		{58.8125,3.8125,0},
		{59.06400000000001,3.8200000000000003,0},
		{59.299499999999995,3.8424999999999994,0},
		{59.52799999999999,3.8799999999999994,0},
		{59.758500000000005,3.9324999999999997,0},
		{60.0,4.0,0},
		{60.217999999999996,4.10275,0},
		{60.384,4.252000000000001,0},
		{60.516,4.43425,0},
		{60.632000000000005,4.635999999999999,0},
		{60.75,4.84375,0},
		{60.88799999999999,5.0440000000000005,0},
		{61.06399999999999,5.22325,0},
		{61.29599999999999,5.367999999999999,0},
		{61.60199999999999,5.46475,0},
		{62.0,5.5,0},
		{62.515499999999996,5.4647499999999996,0},
		{63.144000000000005,5.367999999999999,0},
		{63.85849999999999,5.223249999999999,0},
		{64.63199999999999,5.044,0},
		{65.4375,4.84375,0},
		{66.248,4.636,0},
		{67.03649999999999,4.4342500000000005,0},
		{67.776,4.251999999999999,0},
		{68.43950000000001,4.1027499999999995,0},
		{69.0,4.0,0},
		{69.4665,3.937,0},
		{69.87200000000001,3.8960000000000004,0},
		{70.2255,3.8740000000000006,0},
		{70.53599999999999,3.8679999999999994,0},
		{70.8125,3.875,0},
		{71.064,3.8920000000000003,0},
		{71.2995,3.9159999999999995,0},
		{71.52799999999998,3.943999999999999,0},
		{71.7585,3.973,0},
		{72.0,4.0,0},
		{72.21799999999999,4.03425,0},
		{72.384,4.0840000000000005,0},
		{72.51599999999999,4.144749999999999,0},
		{72.63199999999999,4.212,0},
		{72.75,4.28125,0},
		{72.888,4.348,0},
		{73.064,4.40775,0},
		{73.29599999999999,4.455999999999999,0},
		{73.60199999999999,4.48825,0},
		{74.0,4.5,0},
		{74.52,4.4815,0},
		{75.16,4.4319999999999995,0},
		{75.88999999999999,4.360499999999999,0},
		{76.68,4.276,0},
		{77.5,4.1875,0},
		{78.32000000000001,4.104,0},
		{79.11,4.0344999999999995,0},
		{79.84,3.988,0},
		{80.48,3.973499999999999,0},
		{81.0,4.0,0},
		{81.38,4.0825000000000005,0},
		{81.64,4.220000000000001,0},
		{81.81,4.3975,0},
		{81.92000000000002,4.6,0},
		{82.0,4.8125,0},
		{82.08,5.02,0},
		{82.19,5.2075,0},
		{82.35999999999999,5.359999999999999,0},
		{82.61999999999999,5.4625,0},
		{83.0,5.5,0},
		{83.5155,5.4647499999999996,0},
		{84.144,5.367999999999999,0},
		{84.85849999999999,5.223249999999999,0},
		{85.63199999999999,5.044,0},
		{86.4375,4.84375,0},
		{87.248,4.636,0},
		{88.03649999999999,4.4342500000000005,0},
		{88.776,4.251999999999999,0},
		{89.43950000000001,4.1027499999999995,0},
		{90.0,4.0,0},
		{90.4665,3.9324999999999997,0},
		{90.872,3.8800000000000003,0},
		{91.22549999999998,3.8425000000000007,0},
		{91.536,3.8199999999999994,0},
		{91.8125,3.8125,0},
		{92.064,3.8200000000000003,0},
		{92.2995,3.8424999999999994,0},
		{92.52799999999999,3.8799999999999994,0},
		{92.7585,3.9324999999999997,0},
		{93.0,4.0,0},
		{93.21799999999999,4.10275,0},
		{93.38399999999999,4.252000000000001,0},
		{93.516,4.43425,0},
		{93.63199999999999,4.635999999999999,0},
		{93.75,4.84375,0},
		{93.888,5.0440000000000005,0},
		{94.064,5.22325,0},
		{94.29599999999998,5.367999999999999,0},
		{94.60199999999999,5.46475,0},
		{95.0,5.5,0},
		{95.5155,5.4647499999999996,0},
		{96.144,5.367999999999999,0},
		{96.85849999999999,5.223249999999999,0},
		{97.63199999999999,5.044,0},
		{98.4375,4.84375,0},
		{99.24799999999999,4.636,0},
		{100.03649999999999,4.4342500000000005,0},
		{100.77599999999998,4.251999999999999,0},
		{101.4395,4.1027499999999995,0},
		{102.0,4.0,0},
		{102.4665,3.937,0},
		{102.872,3.8960000000000004,0},
		{103.22549999999998,3.8740000000000006,0},
		{103.53599999999999,3.8679999999999994,0},
		{103.8125,3.875,0},
		{104.06400000000001,3.8920000000000003,0},
		{104.29949999999998,3.9159999999999995,0},
		{104.52799999999999,3.943999999999999,0},
		{104.75849999999998,3.973,0},
		{105.0,4.0,0},
		{105.21799999999999,4.03425,0},
		{105.384,4.0840000000000005,0},
		{105.516,4.144749999999999,0},
		{105.632,4.212,0},
		{105.75,4.28125,0},
		{105.888,4.348,0},
		{106.064,4.40775,0},
		{106.29599999999998,4.455999999999999,0},
		{106.602,4.48825,0},
		{107.0,4.5,0},
		{107.51999999999998,4.4815,0},
		{108.16000000000001,4.4319999999999995,0},
		{108.89,4.360499999999999,0},
		{109.67999999999999,4.276,0},
		{110.5,4.1875,0},
		{111.32,4.104,0},
		{112.11,4.0344999999999995,0},
		{112.83999999999997,3.988,0},
		{113.47999999999999,3.973499999999999,0},
		{114.0,4.0,0},
		{114.38,4.0825000000000005,0},
		{114.64,4.220000000000001,0},
		{114.80999999999999,4.3975,0},
		{114.92,4.6,0},
		{115.0,4.8125,0},
		{115.08000000000001,5.02,0},
		{115.19,5.2075,0},
		{115.35999999999999,5.359999999999999,0},
		{115.61999999999999,5.4625,0},
		{116.0,5.5,0},
		{116.51549999999999,5.4647499999999996,0},
		{117.14399999999999,5.367999999999999,0},
		{117.85849999999999,5.223249999999999,0},
		{118.63199999999999,5.044,0},
		{119.4375,4.84375,0},
		{120.24799999999999,4.636,0},
		{121.03650000000002,4.4342500000000005,0},
		{121.77599999999998,4.251999999999999,0},
		{122.4395,4.1027499999999995,0},
		{123.0,4.0,0},
		{123.4665,3.9324999999999997,0},
		{123.872,3.8800000000000003,0},
		{124.2255,3.8425000000000007,0},
		{124.53599999999997,3.8199999999999994,0},
		{124.8125,3.8125,0},
		{125.06400000000004,3.8200000000000003,0},
		{125.29950000000001,3.8424999999999994,0},
		{125.52799999999999,3.8799999999999994,0},
		{125.7585,3.9324999999999997,0},
		{126.0,4.0,0},
		{126.21799999999999,4.10275,0},
		{126.38400000000001,4.252000000000001,0},
		{126.51600000000002,4.43425,0},
		{126.63199999999999,4.635999999999999,0},
		{126.75,4.84375,0},
		{126.888,5.0440000000000005,0},
		{127.06400000000001,5.22325,0},
		{127.29599999999998,5.367999999999999,0},
		{127.602,5.46475,0},
		{128.0,5.5,0},
		{128.51549999999997,5.4647499999999996,0},
		{129.144,5.367999999999999,0},
		{129.8585,5.223249999999999,0},
		{130.63199999999998,5.044,0},
		{131.4375,4.84375,0},
		{132.248,4.636,0},
		{133.0365,4.4342500000000005,0},
		{133.77599999999998,4.251999999999999,0},
		{134.43949999999998,4.1027499999999995,0},
		{135.0,4.0,0},
		{135.4665,3.9324999999999997,0},
		{135.87199999999999,3.8800000000000003,0},
		{136.2255,3.8425000000000007,0},
		{136.536,3.8199999999999994,0},
		{136.8125,3.8125,0},
		{137.064,3.8200000000000003,0},
		{137.2995,3.8424999999999994,0},
		{137.528,3.8799999999999994,0},
		{137.75850000000003,3.9324999999999997,0},
		{138.0,4.0,0},
		{138.218,4.10275,0},
		{138.38400000000001,4.252000000000001,0},
		{138.516,4.43425,0},
		{138.63199999999998,4.635999999999999,0},
		{138.75,4.84375,0},
		{138.888,5.0440000000000005,0},
		{139.06400000000002,5.22325,0},
		{139.296,5.367999999999999,0},
		{139.60199999999998,5.46475,0},
		{140.0,5.5,0},
		{140.51549999999997,5.4647499999999996,0},
		{141.144,5.367999999999999,0},
		{141.85850000000002,5.223249999999999,0},
		{142.632,5.044,0},
		{143.4375,4.84375,0},
		{144.248,4.636,0},
		{145.0365,4.4342500000000005,0},
		{145.77599999999998,4.251999999999999,0},
		{146.4395,4.1027499999999995,0},
		{147.0,4.0,0},
		{147.4665,3.937,0},
		{147.87199999999999,3.8960000000000004,0},
		{148.2255,3.8740000000000006,0},
		{148.536,3.8679999999999994,0},
		{148.8125,3.875,0},
		{149.06400000000002,3.8920000000000003,0},
		{149.2995,3.9159999999999995,0},
		{149.52799999999996,3.943999999999999,0},
		{149.75849999999997,3.973,0},
		{150.0,4.0,0},
		{150.21800000000002,4.03425,0},
		{150.384,4.0840000000000005,0},
		{150.516,4.144749999999999,0},
		{150.63199999999998,4.212,0},
		{150.75,4.28125,0},
		{150.888,4.348,0},
		{151.064,4.40775,0},
		{151.296,4.455999999999999,0},
		{151.60199999999998,4.48825,0},
		{152.0,4.5,0},
		{152.52,4.486,0},
		{153.16,4.4479999999999995,0},
		{153.89,4.3919999999999995,0},
		{154.67999999999998,4.324,0},
		{155.5,4.25,0},
		{156.32,4.176,0},
		{157.11,4.108,0},
		{157.83999999999997,4.052,0},
		{158.48,4.013999999999999,0},
		{159.0,4.0,0},
		{159.38,4.014,0},
		{159.64000000000001,4.0520000000000005,0},
		{159.81,4.108,0},
		{159.92,4.176,0},
		{160.0,4.25,0},
		{160.08,4.324,0},
		{160.19,4.3919999999999995,0},
		{160.35999999999999,4.447999999999999,0},
		{160.61999999999998,4.486,0},
		{161.0,4.5,0},
		{161.52,4.486,0},
		{162.16,4.4479999999999995,0},
		{162.89000000000001,4.3919999999999995,0},
		{163.68,4.324,0},
		{164.5,4.25,0},
		{165.32,4.176,0},
		{166.10999999999999,4.108,0},
		{166.83999999999997,4.052,0},
		{167.48,4.013999999999999,0},
		{168.0,4.0,0},
		{168.371,4.014,0},
		{168.608,4.0520000000000005,0},
		{168.74699999999999,4.108,0},
		{168.82399999999998,4.176,0},
		{168.875,4.25,0},
		{168.936,4.324,0},
		{169.043,4.3919999999999995,0},
		{169.232,4.447999999999999,0},
		{169.53900000000002,4.486,0},
		{170.0,4.5,0},
		{170.648,4.486,0},
		{171.464,4.4479999999999995,0},
		{172.40599999999998,4.3919999999999995,0},
		{173.43200000000002,4.324,0},
		{174.5,4.25,0},
		{175.568,4.176,0},
		{176.594,4.108,0},
		{177.53599999999997,4.052,0},
		{178.35199999999998,4.013999999999999,0},
		{179.0,4.0,0},
		{179.461,4.014,0},
		{179.768,4.0520000000000005,0},
		{179.957,4.108,0},
		{180.064,4.176,0},
		{180.125,4.25,0},
		{180.176,4.324,0},
		{180.253,4.3919999999999995,0},
		{180.39199999999997,4.447999999999999,0},
		{180.629,4.486,0},
		{181.0,4.5,0},
		{181.52,4.486,0},
		{182.16000000000003,4.4479999999999995,0},
		{182.89,4.3919999999999995,0},
		{183.68,4.324,0},
		{184.5,4.25,0},
		{185.32,4.176,0},
		{186.10999999999999,4.108,0},
		{186.83999999999997,4.052,0},
		{187.48,4.013999999999999,0},
		{188.0,4.0,0},
		{188.38,4.014,0},
		{188.64000000000004,4.0520000000000005,0},
		{188.80999999999997,4.108,0},
		{188.92,4.176,0},
		{189.0,4.25,0},
		{189.08,4.324,0},
		{189.18999999999997,4.3919999999999995,0},
		{189.35999999999996,4.447999999999999,0},
		{189.61999999999998,4.486,0},
		{190.0,4.5,0},
		{190.51999999999998,4.486,0},
		{191.16,4.4479999999999995,0},
		{191.89,4.3919999999999995,0},
		{192.67999999999998,4.324,0},
		{193.5,4.25,0},
		{194.32000000000002,4.176,0},
		{195.11,4.108,0},
		{195.84,4.052,0},
		{196.48,4.013999999999999,0},
		{197.0,4.0,0},
		{197.37099999999998,4.014,0},
		{197.60800000000003,4.0520000000000005,0},
		{197.747,4.108,0},
		{197.82399999999998,4.176,0},
		{197.875,4.25,0},
		{197.936,4.324,0},
		{198.043,4.3919999999999995,0},
		{198.23199999999994,4.447999999999999,0},
		{198.539,4.486,0},
		{199.0,4.5,0},
		{199.648,4.4815,0},
		{200.464,4.4319999999999995,0},
		{201.406,4.360499999999999,0},
		{202.432,4.276,0},
		{203.5,4.1875,0},
		{204.568,4.104,0},
		{205.594,4.0344999999999995,0},
		{206.53599999999997,3.988,0},
		{207.35199999999998,3.973499999999999,0}});

	for (std::size_t i = 0; i < 5; ++i) {

        auto start = std::chrono::steady_clock::now();
        addRawPoint(input1);
        auto end = std::chrono::steady_clock::now();
        auto diff = end - start;
        std::cout << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;

        for (std::size_t j = N-1; j > 0; --j)
        {
        	input1.at(j,0) += i % 3 + 1 ;
		}
	}


}

template <std::size_t N, typename T>
DepthProfiler<N, T>::~DepthProfiler()
{
}

template <std::size_t N, typename T>
void DepthProfiler<N, T>::addRawPoint(Matrix<N, 3, T>& matrix)
{
    // Apply filters and stuff.
    addProcessedPoint(matrix);
}

template <std::size_t N, typename T>
void DepthProfiler<N, T>::addProcessedPoint(Matrix<N, 3, T>& matrix)
{
    try
    {
        BottomProfile<N, 3, T> bottom_profile;
        bottom_profile.raw_data_          = matrix;
        bottom_profile.average_transport_ = 0;

        // Calculate ax^3 + bx^2 + cx + d
        auto afgeleide                             = calculateDerivative(matrix);
        std::vector<std::size_t> peaks_and_valleys = findPeaksAndValleys(afgeleide, 1);
        std::vector<std::pair<std::size_t, std::size_t>> waves = classifyWave(afgeleide, peaks_and_valleys);

        for (std::size_t i = 0; i < waves.size(); ++i)
        {
            Dune dune;
            dune.size_index_  = waves[i].second - waves[i].first;
            dune.start_index_ = waves[i].first;
            dune.start_x_     = matrix.at(dune.start_index_, 0);
            dune.end_x_		  = matrix.at(waves[i].second, 0);

            // TODO: change this magic number.
            if (dune.size_index_ > 60)  // We have a big wave, just skip it. .
            {
                continue;
            }
            if (dune.start_index_ + 60 > N)  // Our wave doesn't fit in matrix, this is really bad coding.
            {
                continue;
            }
            Matrix<60, 2, float> single_dune_matrix(matrix, dune.start_index_, 0);

            // Translate x values so they start from 0. (This helps the Gauss-Newton algorithm find a result.)
            shiftWaveToZero(single_dune_matrix, dune.size_index_ + 1);

            Matrix<4, 1, T> params({ 1, 1, 1, 1 });
            gaussNewton(single_dune_matrix, params, dune.size_index_);
            dune.signature_ = params;
            dune.surface_area_ =
                calculateArea(params, single_dune_matrix.at(0, 0), single_dune_matrix.at(dune.size_index_, 0));
            bottom_profile.dunes_.push_back(dune);
        }

        // Calculate the transport!
        if (depth_data_.size() > 0)
        {
            getTransportOverTime(depth_data_.back(), bottom_profile);
        }

        depth_data_.push_back(bottom_profile);
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}

template <std::size_t N, typename T>
template <std::size_t H>
T DepthProfiler<N, T>::DepthProfiler::newtonsMethod(const Matrix<H, 1, T>& params,
                                                    T inital_guess,
                                                    T x_shift,
                                                    float precision) const
{
    T guess     = inital_guess;
    T new_guess = 0;

    for (std::size_t i = 0; i < MAX_ITERATIONS; ++i)
    {
        T y  = baseFunction(guess, params, x_shift);
        T yp = baseDerivative(guess, params, x_shift);

        if (std::abs(yp) < EPSILON)
        {
            throw std::runtime_error("Determinat ~= 0");
        }

        new_guess = guess - y / yp;
        if (std::abs(new_guess - guess) <= precision * abs(new_guess))
        {
            return new_guess;
        }

        guess = new_guess;
    }
    throw std::runtime_error("Took to many iterations.");
}

template <std::size_t N, typename T>
template <std::size_t H>
T DepthProfiler<N, T>::newtonsMethod(const Matrix<H, 1, T>& params,
                                     const Matrix<H, 1, T>& params2,
									 const std::pair<float,float>& solution_space,
                                     T inital_guess,
                                     T x_shift,
                                     float precision) const
{
    T guess     = inital_guess;
    T new_guess = 0;

    for (std::size_t i = 0; i < MAX_ITERATIONS; ++i)
    {
        // Keeps in considuration that the second function is shifted along the x axis.
        // Only downside is that it increases the complexity.
        T y  = baseFunction(guess, params) - baseFunction(guess + x_shift, params2);
        T yp = baseDerivative(guess, params) - baseDerivative(guess + x_shift, params2);

        if (std::abs(yp) < EPSILON)
        {
            throw std::runtime_error("Determinat ~= 0");
        }

        new_guess = guess - y / yp;
        if(new_guess < solution_space.first || new_guess > solution_space.second)
        {
        	new_guess = solution_space.first + (solution_space.second - solution_space.first) / 1.95;
        	continue;
        }
        else if (std::abs(new_guess - guess) <= precision * abs(new_guess))
        {
            return new_guess;
        }

        guess = new_guess;
    }
    throw std::runtime_error("Took to many iterations.");
}

template <std::size_t N, typename T>
template <std::size_t H, std::size_t W, std::size_t aH>
void DepthProfiler<N, T>::gaussNewton(const Matrix<H, W, T>& inputs,
                                      Matrix<aH, 1, T>& params,
                                      std::size_t iterations,
                                      const float precision) const
{
    assert(H >= W);

    Matrix<H, 1, T> residuals;  // Residual matrix
    Matrix<H, aH, T> jacobian;  // Jacobian matrix
    Matrix<1, 1, T> input;      // Single row input

    float last_deviation = 0;

    for (std::size_t i = 0; i < MAX_ITERATIONS; ++i)
    {
        float total_deviation = 0;

        for (std::size_t j = 0; j < iterations; ++j)
        {
            input.at(0, 0) = inputs.at(j, 0);

            // r_i(beta)= y_i - f(x_i, \beta).
            residuals.at(j, 0) = inputs.at(j, 1) - baseFunction(input, params);
            // Add the square difference of the last derived parameter and the wanted value to the total deviation.
            total_deviation += residuals.at(j, 0) * residuals.at(j, 0);

            for (std::size_t k = 0; k < aH; ++k)
            {
                jacobian.at(j, k) = calculateDerivative(input, params, k);
            }
        }

        // Mean deviation over all the inputs.
        total_deviation /= H;

        // TODO: Maybe change this check to only check for the current deviation |total_deviation| < precision
        // If the deviation is since last iteration is smaller then precision, we have found the best optimal value.
        if (std::abs(total_deviation - last_deviation) < precision)
        {
            break;
        }

        // beta^(s+1) = beta^s - (Jf.T * Jf)^-1 * Jf.T * r(beta^s)
        auto jacobijnTranspose = jacobian.transpose();
        auto delta             = (jacobijnTranspose * jacobian).inverse() * jacobijnTranspose * residuals;
        last_deviation         = total_deviation;

        params += delta;

        if (std::isnan(total_deviation))
        {
            throw std::runtime_error("NaN is found");
        }
        if (i + 1 == MAX_ITERATIONS)
        {
            throw std::runtime_error("Took to many iterations");
        }

        //            std::cout << "Iteration: " << i << " " << total_deviation << std::endl;
    }
}

template <std::size_t N, typename T>
template <std::size_t H, std::size_t aH>
T DepthProfiler<N, T>::baseFunction(const Matrix<H, 1, T>& input, const Matrix<aH, 1, T>& params) const
{
    T x = input.at(0, 0);

    return baseFunction(x, params);
}

template <std::size_t N, typename T>
template <std::size_t H>
T DepthProfiler<N, T>::baseFunction(T x, const Matrix<H, 1, T>& params, T h) const
{
    if (h == 0)
    {
        return params.at(0, 0) * std::pow(x, 3) + params.at(1, 0) * std::pow(x, 2) + params.at(2, 0) * x +
               params.at(3, 0);
    }
    else
    {
        T a = params.at(0, 0);
        T b = params.at(1, 0);
        T c = params.at(2, 0);
        T d = params.at(3, 0);

        return (-a) * std::pow(h, 3) + 3 * a * std::pow(h, 2) * x - 3 * a * h * std::pow(x, 2) + a * std::pow(x, 3) +
               b * std::pow(h, 2) - 2 * b * h * x + b * std::pow(x, 2) - c * h + c * x + d;
    }
}

template <std::size_t N, typename T>
template <std::size_t H>
T DepthProfiler<N, T>::baseIntegral(T x, const Matrix<H, 1, T>& params, T h) const
{
    if (h == 0)
    {
        return ((1.0 / 4) * params.at(0, 0) * std::pow(x, 4)) + ((1.0 / 3) * params.at(1, 0) * std::pow(x, 3)) +
               ((1.0 / 2) * params.at(2, 0) * std::pow(x, 2)) + params.at(3, 0) * x;
    }
    else
    {
        T a = params.at(0, 0);
        T b = params.at(1, 0);
        T c = params.at(2, 0);
        T d = params.at(3, 0);

        // http://www.wolframalpha.com/input/?i=a*h%5E3%2B3*a*h%5E2*x-3*a*h*x%5E2%2Ba*x%5E3%2Bb*h%5E2-2*b*h*x%2Bb*x%5E2%2Bc*x-c*h%2Bd
        return a * std::pow(h, 3) + 3 / 2 * a * std::pow(h, 2) * std::pow(x, 2) - a * h * std::pow(x, 3) +
               1 / 4 * a * std::pow(x, 4) + b * std::pow(h, 2) * x - b * h * std::pow(x, 2) +
               1 / 3 * b * std::pow(x, 3) - c * h * x + 1 / 2 * c * std::pow(x, 2) + d * x;
    }
}

template <std::size_t N, typename T>
template <std::size_t H>
T DepthProfiler<N, T>::baseDerivative(T x, const Matrix<H, 1, T>& params, T h) const
{
    if (h == 0)
    {
        return x * (3 * params.at(0, 0) * x + 2 * params.at(1, 0)) + params.at(2, 0);
    }
    else
    {
        return (h - x) * (3 * params.at(0, 0) * (h - x) - 2 * params.at(1, 0)) + params.at(2, 0);
    }
}

template <std::size_t N, typename T>
template <std::size_t H, std::size_t aH>
T DepthProfiler<N, T>::calculateDerivative(const Matrix<H, 1, T>& input,
                                           const Matrix<aH, 1, T>& params,
                                           std::size_t param_number) const
{
    // Make copy of the current parameters.
    Matrix<aH, 1, T> params1(params);
    Matrix<aH, 1, T> params2(params);

    // Use central difference  to get derivative.
    params1.at(param_number, 0) -= STEP_SIZE;
    params2.at(param_number, 0) += STEP_SIZE;

    T p1 = baseFunction(input, params1);
    T p2 = baseFunction(input, params2);

    return (p2 - p1) / (2 * STEP_SIZE);
}

template <std::size_t N, typename T>
template <std::size_t H, std::size_t W>
Matrix<H - 1, 2, T> DepthProfiler<N, T>::calculateDerivative(Matrix<H, W, T>& matrix) const
{
    Matrix<H - 1, 2, T> result;

    for (std::size_t i = 0; i < H - 1; ++i)
    {
        result[i][0] = matrix[i][0];
        result[i][1] = (matrix[i + 1][1] - matrix[i][1]) / (matrix[i + 1][0] - matrix[i][0]);
    }
    return result;
}

template <std::size_t N, typename T>
void DepthProfiler<N, T>::getTransportForAllData() const
{
    if (depth_data_.size() < 2)
    {
        return;
    }

    for (std::size_t i = 0; i < depth_data_.size() - 1; ++i)
    {
        getTransportOverTime(depth_data_.at(i), depth_data_.at(i + 1));
    }
}

template <std::size_t N, typename T>
template <std::size_t H, std::size_t W>
void DepthProfiler<N, T>::getTransportOverTime(const BottomProfile<H, W, T>& profile_first,
                                               BottomProfile<H, W, T>& profile_second) const
{
    std::vector<std::pair<Dune, Dune>> simular_dunes = profile_first.getSimularDune(profile_second, 0.01);
    std::cout << "Found: " << simular_dunes.size() << " simular dunes." << std::endl;
    if (simular_dunes.size() > 0)
    {
        float total_transport       = 0;
        std::size_t calculatedDunes = 0;
        for (const auto& i : simular_dunes)
        {
            try
            {
                total_transport += calculateMovedArea(i.first, i.second);
                calculatedDunes++;
            }
            catch (std::exception& e)
            {
                continue;
            }
        }
        profile_second.average_transport_ = total_transport / calculatedDunes;
    }
    else
    {
        profile_second.average_transport_ = 0;
    }
    std::cout << "Average transport: " << profile_second.average_transport_ << std::endl;
}

template <std::size_t N, typename T>
float DepthProfiler<N, T>::calculateMovedArea(const Dune& dune_left, const Dune& dune_right) const
{
    // Calculate the how much our dune has moved across the x axis.
    T x_diff = dune_right.start_x_ - dune_left.start_x_;
    T max_guess = (dune_left.end_x_ - dune_right.start_x_);
    T inital_guess = max_guess / 2;
//    std::cout << "Inital guess: " << inital_guess << " rs: " << dune_right.start_x_ << " re: " << dune_right.end_x_ << " ls: " << dune_left.start_x_ << " le: " << dune_left.end_x_ << std::endl;
    auto solution_space = std::make_pair(0, max_guess + x_diff);

    // Calculate intersection between the dunes.
    T root = newtonsMethod(dune_left.signature_, dune_right.signature_, solution_space, inital_guess, x_diff);

    // Calculate how much of the dune has stayed in place
    float area_of_a  = calculateArea(dune_left.signature_, root, (dune_left.end_x_ - dune_left.start_x_));
    float area_of_b  = calculateArea(dune_right.signature_, 0, root + x_diff);
    float moved_area = dune_right.surface_area_ - (area_of_a + area_of_b);


//    std::cout << "DuneLeft area: " << area_of_a << " DuneRight area: " << area_of_b << " DuneRight surface: " << dune_right.surface_area_ << std::endl;
//    std::cout << "Duneleft: " << dune_left.signature_ << " Dune right: " << dune_right.signature_ << std::endl;
//    std::cout << "Moved area: " << moved_area << std::endl;

    return moved_area;
}

template <std::size_t N, typename T>
T DepthProfiler<N, T>::calculateArea(const Matrix<4, 1, T>& params, T start, T end) const
{
    T y1 = baseIntegral(start, params);
    T y2 = baseIntegral(end, params);
    return std::abs(y2 - y1);
}


template <std::size_t N, typename T>
template <std::size_t H, std::size_t W>
std::vector<std::size_t> DepthProfiler<N, T>::findPeaksAndValleys(const Matrix<H, W, T>& derivative,
                                                                  std::size_t minimal_x_diff) const
{
    std::vector<std::size_t> result;
    float last_found_x = 0;
    for (std::size_t i = 0; i < H - 1; ++i)
    {
        if (((derivative[i][1] < 0 && derivative[i + 1][1] > 0) || (derivative[i][1] > 0 && derivative[i + 1][1] < 0)) &&
            abs(derivative[i][0] - last_found_x) > minimal_x_diff)
        {
            last_found_x = derivative[i][0];
            result.push_back(i + 1);
        }
    }
//    std::cout << "Found: " << result.size() << " peaks or valleys." << std::endl;
    return result;
}

template <std::size_t N, typename T>
template <std::size_t H, std::size_t W>
std::vector<std::pair<std::size_t, std::size_t>>
DepthProfiler<N, T>::classifyWave(const Matrix<H, W, T>& afgeleide,
                                  const std::vector<std::size_t>& peaks_and_valleys) const
{
    std::vector<std::pair<std::size_t, std::size_t>> result;
    for (std::size_t i = 0; i < peaks_and_valleys.size() - 2; ++i)
    {
        // Find a valley -> peak -> valley order.
        if (!isPeak(afgeleide, peaks_and_valleys[i]) && isPeak(afgeleide, peaks_and_valleys[i + 1]) &&
            !isPeak(afgeleide, peaks_and_valleys[i + 2]))
        {
            result.push_back(std::make_pair(peaks_and_valleys[i], peaks_and_valleys[i + 2]));
//            std::cout << "Wave from: " << peaks_and_valleys[i] << " to: " << peaks_and_valleys[i+2]
//				   << "   from_x: " << afgeleide.at(peaks_and_valleys[i],0) << " to_x: " << afgeleide.at(peaks_and_valleys[i+2],0) << std::endl;
        }
    }
    std::cout << "Found: " << result.size() << " Waves" << std::endl;
    return result;
}

template <std::size_t N, typename T>
template <std::size_t H, std::size_t W>
void DepthProfiler<N, T>::shiftWaveToZero(Matrix<H, W, T>& matrix, std::size_t dune_size) const
{
    T start_x = matrix.at(0, 0);
    for (std::size_t i = 0; i < dune_size + 1; ++i)
    {
        matrix.at(i, 0) -= start_x;
    }
}

template <std::size_t N, typename T>
template <std::size_t H, std::size_t W>
bool DepthProfiler<N, T>::isPeak(const Matrix<H, W, T>& derivative, std::size_t index) const
{
    assert(index + 2 < H);
    return derivative.at(index, 1) > derivative.at(index + 1, 1) &&
           derivative.at(index + 1, 1) > derivative.at(index + 2, 1);
}

}