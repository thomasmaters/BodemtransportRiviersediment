/*
 * DepthProfiler.hpp
 *
 *  Created on: 2 Jul 2018
 *      Author: Thomas Maters
 *		Email : thomasmaters@hotmail.com (TG.Maters@student.han.nl)
 */

#ifndef SRC_DEPTHPROFILER_HPP_
#define SRC_DEPTHPROFILER_HPP_

#include "DataBuffer.hpp"
#include "Dune.hpp"
#include "FileHandler.hpp"
#include "Matrix.hpp"

#include <thread>
#include <valarray>
#include <vector>

#define MAX_ITERATIONS 200
#define STEP_SIZE 1e-5
#define PRECISION 1e-6
#define EPSILON 1e-14

namespace Controller
{
/*
 *
 */
// Datapoints per ping. Type to store the pings as.
template <std::size_t N, typename T = float>
class DepthProfiler
{
    static_assert(std::is_arithmetic<T>::value, "Type must be aritmatic.");

  public:
    DepthProfiler()
    {
    	Matrix<480,3,T> input1({{0,4,0},
    		{1,4,0},
    		{2,4,0},
    		{3,4,0},
    		{4,4,0},
    		{5,4,0},
    		{6,4,0},
    		{7,4,0},
    		{8,4,0},
    		{9,4,0},
    		{10.0,4.0,0},
    		{10.1695,4.03875,0},
    		{10.376,4.1000000000000005,0},
    		{10.6165,4.17625,0},
    		{10.888,4.26,0},
    		{11.1875,4.34375,0},
    		{11.512,4.42,0},
    		{11.8585,4.48125,0},
    		{12.224,4.519999999999999,0},
    		{12.605500000000001,4.52875,0},
    		{13.0,4.5,0},
    		{13.4235,4.41975,0},
    		{13.888,4.288,0},
    		{14.3845,4.11825,0},
    		{14.904,3.9239999999999995,0},
    		{15.4375,3.71875,0},
    		{15.975999999999999,3.5160000000000005,0},
    		{16.5105,3.3292499999999996,0},
    		{17.032,3.1719999999999997,0},
    		{17.5315,3.0577499999999995,0},
    		{18.0,3.0,0},
    		{18.445,3.0010000000000003,0},
    		{18.880000000000003,3.0480000000000005,0},
    		{19.305,3.1319999999999997,0},
    		{19.72,3.2439999999999998,0},
    		{20.125,3.375,0},
    		{20.520000000000003,3.516,0},
    		{20.905,3.658,0},
    		{21.279999999999998,3.792,0},
    		{21.645,3.909,0},
    		{22.0,4.0,0},
    		{22.3315,4.07925,0},
    		{22.631999999999998,4.164,0},
    		{22.9105,4.24975,0},
    		{23.176,4.332,0},
    		{23.4375,4.40625,0},
    		{23.704,4.468,0},
    		{23.984499999999997,4.51275,0},
    		{24.288,4.535999999999999,0},
    		{24.6235,4.53325,0},
    		{25.0,4.5,0},
    		{25.423499999999997,4.41975,0},
    		{25.887999999999998,4.288,0},
    		{26.3845,4.11825,0},
    		{26.904,3.9239999999999995,0},
    		{27.4375,3.71875,0},
    		{27.976,3.5160000000000005,0},
    		{28.510499999999997,3.3292499999999996,0},
    		{29.031999999999996,3.1719999999999997,0},
    		{29.5315,3.0577499999999995,0},
    		{30.0,3.0,0},
    		{30.444999999999997,3.0010000000000003,0},
    		{30.880000000000003,3.0480000000000005,0},
    		{31.305,3.1319999999999997,0},
    		{31.72,3.2439999999999998,0},
    		{32.125,3.375,0},
    		{32.52,3.516,0},
    		{32.905,3.658,0},
    		{33.279999999999994,3.792,0},
    		{33.644999999999996,3.909,0},
    		{34,4,0},
    		{35,4,0},
    		{36,4,0},
    		{37,4,0},
    		{38,4,0},
    		{39,4,0},
    		{40,4,0},
    		{41,4,0},
    		{42,4,0},
    		{43,4,0},
    		{44,4,0},
    		{45,4,0},
    		{46,4,0},
    		{47,4,0},
    		{48,4,0},
    		{49,4,0},
    		{50,4,0},
    		{51,4,0},
    		{52,4,0},
    		{53,4,0},
    		{54,4,0},
    		{55,4,0},
    		{56,4,0},
    		{57,4,0},
    		{58,4,0},
    		{59,4,0},
    		{60,4,0},
    		{61,4,0},
    		{62,4,0},
    		{63,4,0},
    		{64,4,0},
    		{65,4,0},
    		{66,4,0},
    		{67,4,0},
    		{68,4,0},
    		{69,4,0},
    		{70,4,0},
    		{71,4,0},
    		{72,4,0},
    		{73,4,0},
    		{74,4,0},
    		{75,4,0},
    		{76,4,0},
    		{77,4,0},
    		{78,4,0},
    		{79,4,0},
    		{80,4,0},
    		{81,4,0},
    		{82,4,0},
    		{83,4,0},
    		{84,4,0},
    		{85,4,0},
    		{86,4,0},
    		{87,4,0},
    		{88,4,0},
    		{89,4,0},
    		{90,4,0},
    		{91,4,0},
    		{92,4,0},
    		{93,4,0},
    		{94,4,0},
    		{95,4,0},
    		{96,4,0},
    		{97,4,0},
    		{98,4,0},
    		{99,4,0},
    		{100,4,0},
    		{101,4,0},
    		{102,4,0},
    		{103,4,0},
    		{104,4,0},
    		{105,4,0},
    		{106,4,0},
    		{107,4,0},
    		{108,4,0},
    		{109,4,0},
    		{110,4,0},
    		{111,4,0},
    		{112,4,0},
    		{113,4,0},
    		{114,4,0},
    		{115,4,0},
    		{116,4,0},
    		{117,4,0},
    		{118,4,0},
    		{119,4,0},
    		{120,4,0},
    		{121,4,0},
    		{122,4,0},
    		{123,4,0},
    		{124,4,0},
    		{125,4,0},
    		{126,4,0},
    		{127,4,0},
    		{128,4,0},
    		{129,4,0},
    		{130,4,0},
    		{131,4,0},
    		{132,4,0},
    		{133,4,0},
    		{134,4,0},
    		{135,4,0},
    		{136,4,0},
    		{137,4,0},
    		{138,4,0},
    		{139,4,0},
    		{140,4,0},
    		{141,4,0},
    		{142,4,0},
    		{143,4,0},
    		{144,4,0},
    		{145,4,0},
    		{146,4,0},
    		{147,4,0},
    		{148,4,0},
    		{149,4,0},
    		{150,4,0},
    		{151,4,0},
    		{152,4,0},
    		{153,4,0},
    		{154,4,0},
    		{155,4,0},
    		{156,4,0},
    		{157,4,0},
    		{158,4,0},
    		{159,4,0},
    		{160,4,0},
    		{161,4,0},
    		{162,4,0},
    		{163,4,0},
    		{164,4,0},
    		{165,4,0},
    		{166,4,0},
    		{167,4,0},
    		{168,4,0},
    		{169,4,0},
    		{170,4,0},
    		{171,4,0},
    		{172,4,0},
    		{173,4,0},
    		{174,4,0},
    		{175,4,0},
    		{176,4,0},
    		{177,4,0},
    		{178,4,0},
    		{179,4,0},
    		{180,4,0},
    		{181,4,0},
    		{182,4,0},
    		{183,4,0},
    		{184,4,0},
    		{185,4,0},
    		{186,4,0},
    		{187,4,0},
    		{188,4,0},
    		{189,4,0},
    		{190,4,0},
    		{191,4,0},
    		{192,4,0},
    		{193,4,0},
    		{194,4,0},
    		{195,4,0},
    		{196,4,0},
    		{197,4,0},
    		{198,4,0},
    		{199,4,0},
    		{200,4,0},
    		{201,4,0},
    		{202,4,0},
    		{203,4,0},
    		{204,4,0},
    		{205,4,0},
    		{206,4,0},
    		{207,4,0},
    		{208,4,0},
    		{209,4,0},
    		{210,4,0},
    		{211,4,0},
    		{212,4,0},
    		{213,4,0},
    		{214,4,0},
    		{215,4,0},
    		{216,4,0},
    		{217,4,0},
    		{218,4,0},
    		{219,4,0},
    		{220,4,0},
    		{221,4,0},
    		{222,4,0},
    		{223,4,0},
    		{224,4,0},
    		{225,4,0},
    		{226,4,0},
    		{227,4,0},
    		{228,4,0},
    		{229,4,0},
    		{230,4,0},
    		{231,4,0},
    		{232,4,0},
    		{233,4,0},
    		{234,4,0},
    		{235,4,0},
    		{236,4,0},
    		{237,4,0},
    		{238,4,0},
    		{239,4,0},
    		{240,4,0},
    		{241,4,0},
    		{242,4,0},
    		{243,4,0},
    		{244,4,0},
    		{245,4,0},
    		{246,4,0},
    		{247,4,0},
    		{248,4,0},
    		{249,4,0},
    		{250,4,0},
    		{251,4,0},
    		{252,4,0},
    		{253,4,0},
    		{254,4,0},
    		{255,4,0},
    		{256,4,0},
    		{257,4,0},
    		{258,4,0},
    		{259,4,0},
    		{260,4,0},
    		{261,4,0},
    		{262,4,0},
    		{263,4,0},
    		{264,4,0},
    		{265,4,0},
    		{266,4,0},
    		{267,4,0},
    		{268,4,0},
    		{269,4,0},
    		{270,4,0},
    		{271,4,0},
    		{272,4,0},
    		{273,4,0},
    		{274,4,0},
    		{275,4,0},
    		{276,4,0},
    		{277,4,0},
    		{278,4,0},
    		{279,4,0},
    		{280,4,0},
    		{281,4,0},
    		{282,4,0},
    		{283,4,0},
    		{284,4,0},
    		{285,4,0},
    		{286,4,0},
    		{287,4,0},
    		{288,4,0},
    		{289,4,0},
    		{290,4,0},
    		{291,4,0},
    		{292,4,0},
    		{293,4,0},
    		{294,4,0},
    		{295,4,0},
    		{296,4,0},
    		{297,4,0},
    		{298,4,0},
    		{299,4,0},
    		{300,4,0},
    		{301,4,0},
    		{302,4,0},
    		{303,4,0},
    		{304,4,0},
    		{305,4,0},
    		{306,4,0},
    		{307,4,0},
    		{308,4,0},
    		{309,4,0},
    		{310,4,0},
    		{311,4,0},
    		{312,4,0},
    		{313,4,0},
    		{314,4,0},
    		{315,4,0},
    		{316,4,0},
    		{317,4,0},
    		{318,4,0},
    		{319,4,0},
    		{320,4,0},
    		{321,4,0},
    		{322,4,0},
    		{323,4,0},
    		{324,4,0},
    		{325,4,0},
    		{326,4,0},
    		{327,4,0},
    		{328,4,0},
    		{329,4,0},
    		{330,4,0},
    		{331,4,0},
    		{332,4,0},
    		{333,4,0},
    		{334,4,0},
    		{335,4,0},
    		{336,4,0},
    		{337,4,0},
    		{338,4,0},
    		{339,4,0},
    		{340,4,0},
    		{341,4,0},
    		{342,4,0},
    		{343,4,0},
    		{344,4,0},
    		{345,4,0},
    		{346,4,0},
    		{347,4,0},
    		{348,4,0},
    		{349,4,0},
    		{350,4,0},
    		{351,4,0},
    		{352,4,0},
    		{353,4,0},
    		{354,4,0},
    		{355,4,0},
    		{356,4,0},
    		{357,4,0},
    		{358,4,0},
    		{359,4,0},
    		{360,4,0},
    		{361,4,0},
    		{362,4,0},
    		{363,4,0},
    		{364,4,0},
    		{365,4,0},
    		{366,4,0},
    		{367,4,0},
    		{368,4,0},
    		{369,4,0},
    		{370,4,0},
    		{371,4,0},
    		{372,4,0},
    		{373,4,0},
    		{374,4,0},
    		{375,4,0},
    		{376,4,0},
    		{377,4,0},
    		{378,4,0},
    		{379,4,0},
    		{380,4,0},
    		{381,4,0},
    		{382,4,0},
    		{383,4,0},
    		{384,4,0},
    		{385,4,0},
    		{386,4,0},
    		{387,4,0},
    		{388,4,0},
    		{389,4,0},
    		{390,4,0},
    		{391,4,0},
    		{392,4,0},
    		{393,4,0},
    		{394,4,0},
    		{395,4,0},
    		{396,4,0},
    		{397,4,0},
    		{398,4,0},
    		{399,4,0},
    		{400,4,0},
    		{401,4,0},
    		{402,4,0},
    		{403,4,0},
    		{404,4,0},
    		{405,4,0},
    		{406,4,0},
    		{407,4,0},
    		{408,4,0},
    		{409,4,0},
    		{410,4,0},
    		{411,4,0},
    		{412,4,0},
    		{413,4,0},
    		{414,4,0},
    		{415,4,0},
    		{416,4,0},
    		{417,4,0},
    		{418,4,0},
    		{419,4,0},
    		{420,4,0},
    		{421,4,0},
    		{422,4,0},
    		{423,4,0},
    		{424,4,0},
    		{425,4,0},
    		{426,4,0},
    		{427,4,0},
    		{428,4,0},
    		{429,4,0},
    		{430,4,0},
    		{431,4,0},
    		{432,4,0},
    		{433,4,0},
    		{434,4,0},
    		{435,4,0},
    		{436,4,0},
    		{437,4,0},
    		{438,4,0},
    		{439,4,0},
    		{440,4,0},
    		{441,4,0},
    		{442,4,0},
    		{443,4,0}});
    	Matrix<480,3,T> input2({{0,4,0},
    		{1,4,0},
    		{2,4,0},
    		{3,4,0},
    		{4,4,0},
    		{5,4,0},
    		{6,4,0},
    		{7,4,0},
    		{8,4,0},
    		{9,4,0},
    		{10,4,0},
    		{11,4,0},
    		{12.0,4.0,0},
    		{12.1695,4.03875,0},
    		{12.376,4.1000000000000005,0},
    		{12.6165,4.17625,0},
    		{12.888,4.26,0},
    		{13.1875,4.34375,0},
    		{13.512,4.42,0},
    		{13.8585,4.48125,0},
    		{14.224,4.519999999999999,0},
    		{14.605500000000001,4.52875,0},
    		{15.0,4.5,0},
    		{15.4235,4.41975,0},
    		{15.888,4.288,0},
    		{16.3845,4.11825,0},
    		{16.904,3.9239999999999995,0},
    		{17.4375,3.71875,0},
    		{17.976,3.5160000000000005,0},
    		{18.5105,3.3292499999999996,0},
    		{19.032,3.1719999999999997,0},
    		{19.5315,3.0577499999999995,0},
    		{20.0,3.0,0},
    		{20.445,3.0010000000000003,0},
    		{20.880000000000003,3.0480000000000005,0},
    		{21.305,3.1319999999999997,0},
    		{21.72,3.2439999999999998,0},
    		{22.125,3.375,0},
    		{22.520000000000003,3.516,0},
    		{22.905,3.658,0},
    		{23.279999999999998,3.792,0},
    		{23.645,3.909,0},
    		{24.0,4.0,0},
    		{24.3315,4.07925,0},
    		{24.631999999999998,4.164,0},
    		{24.9105,4.24975,0},
    		{25.176,4.332,0},
    		{25.4375,4.40625,0},
    		{25.704,4.468,0},
    		{25.984499999999997,4.51275,0},
    		{26.288,4.535999999999999,0},
    		{26.6235,4.53325,0},
    		{27.0,4.5,0},
    		{27.423499999999997,4.41975,0},
    		{27.887999999999998,4.288,0},
    		{28.3845,4.11825,0},
    		{28.904,3.9239999999999995,0},
    		{29.4375,3.71875,0},
    		{29.976,3.5160000000000005,0},
    		{30.510499999999997,3.3292499999999996,0},
    		{31.031999999999996,3.1719999999999997,0},
    		{31.5315,3.0577499999999995,0},
    		{32.0,3.0,0},
    		{32.44499999999999,3.0010000000000003,0},
    		{32.88,3.0480000000000005,0},
    		{33.305,3.1319999999999997,0},
    		{33.72,3.2439999999999998,0},
    		{34.125,3.375,0},
    		{34.52,3.516,0},
    		{34.905,3.658,0},
    		{35.279999999999994,3.792,0},
    		{35.644999999999996,3.909,0},
    		{36,4,0},
    		{37,4,0},
    		{38,4,0},
    		{39,4,0},
    		{40,4,0},
    		{41,4,0},
    		{42,4,0},
    		{43,4,0},
    		{44,4,0},
    		{45,4,0},
    		{46,4,0},
    		{47,4,0},
    		{48,4,0},
    		{49,4,0},
    		{50,4,0},
    		{51,4,0},
    		{52,4,0},
    		{53,4,0},
    		{54,4,0},
    		{55,4,0},
    		{56,4,0},
    		{57,4,0},
    		{58,4,0},
    		{59,4,0},
    		{60,4,0},
    		{61,4,0},
    		{62,4,0},
    		{63,4,0},
    		{64,4,0},
    		{65,4,0},
    		{66,4,0},
    		{67,4,0},
    		{68,4,0},
    		{69,4,0},
    		{70,4,0},
    		{71,4,0},
    		{72,4,0},
    		{73,4,0},
    		{74,4,0},
    		{75,4,0},
    		{76,4,0},
    		{77,4,0},
    		{78,4,0},
    		{79,4,0},
    		{80,4,0},
    		{81,4,0},
    		{82,4,0},
    		{83,4,0},
    		{84,4,0},
    		{85,4,0},
    		{86,4,0},
    		{87,4,0},
    		{88,4,0},
    		{89,4,0},
    		{90,4,0},
    		{91,4,0},
    		{92,4,0},
    		{93,4,0},
    		{94,4,0},
    		{95,4,0},
    		{96,4,0},
    		{97,4,0},
    		{98,4,0},
    		{99,4,0},
    		{100,4,0},
    		{101,4,0},
    		{102,4,0},
    		{103,4,0},
    		{104,4,0},
    		{105,4,0},
    		{106,4,0},
    		{107,4,0},
    		{108,4,0},
    		{109,4,0},
    		{110,4,0},
    		{111,4,0},
    		{112,4,0},
    		{113,4,0},
    		{114,4,0},
    		{115,4,0},
    		{116,4,0},
    		{117,4,0},
    		{118,4,0},
    		{119,4,0},
    		{120,4,0},
    		{121,4,0},
    		{122,4,0},
    		{123,4,0},
    		{124,4,0},
    		{125,4,0},
    		{126,4,0},
    		{127,4,0},
    		{128,4,0},
    		{129,4,0},
    		{130,4,0},
    		{131,4,0},
    		{132,4,0},
    		{133,4,0},
    		{134,4,0},
    		{135,4,0},
    		{136,4,0},
    		{137,4,0},
    		{138,4,0},
    		{139,4,0},
    		{140,4,0},
    		{141,4,0},
    		{142,4,0},
    		{143,4,0},
    		{144,4,0},
    		{145,4,0},
    		{146,4,0},
    		{147,4,0},
    		{148,4,0},
    		{149,4,0},
    		{150,4,0},
    		{151,4,0},
    		{152,4,0},
    		{153,4,0},
    		{154,4,0},
    		{155,4,0},
    		{156,4,0},
    		{157,4,0},
    		{158,4,0},
    		{159,4,0},
    		{160,4,0},
    		{161,4,0},
    		{162,4,0},
    		{163,4,0},
    		{164,4,0},
    		{165,4,0},
    		{166,4,0},
    		{167,4,0},
    		{168,4,0},
    		{169,4,0},
    		{170,4,0},
    		{171,4,0},
    		{172,4,0},
    		{173,4,0},
    		{174,4,0},
    		{175,4,0},
    		{176,4,0},
    		{177,4,0},
    		{178,4,0},
    		{179,4,0},
    		{180,4,0},
    		{181,4,0},
    		{182,4,0},
    		{183,4,0},
    		{184,4,0},
    		{185,4,0},
    		{186,4,0},
    		{187,4,0},
    		{188,4,0},
    		{189,4,0},
    		{190,4,0},
    		{191,4,0},
    		{192,4,0},
    		{193,4,0},
    		{194,4,0},
    		{195,4,0},
    		{196,4,0},
    		{197,4,0},
    		{198,4,0},
    		{199,4,0},
    		{200,4,0},
    		{201,4,0},
    		{202,4,0},
    		{203,4,0},
    		{204,4,0},
    		{205,4,0},
    		{206,4,0},
    		{207,4,0},
    		{208,4,0},
    		{209,4,0},
    		{210,4,0},
    		{211,4,0},
    		{212,4,0},
    		{213,4,0},
    		{214,4,0},
    		{215,4,0},
    		{216,4,0},
    		{217,4,0},
    		{218,4,0},
    		{219,4,0},
    		{220,4,0},
    		{221,4,0},
    		{222,4,0},
    		{223,4,0},
    		{224,4,0},
    		{225,4,0},
    		{226,4,0},
    		{227,4,0},
    		{228,4,0},
    		{229,4,0},
    		{230,4,0},
    		{231,4,0},
    		{232,4,0},
    		{233,4,0},
    		{234,4,0},
    		{235,4,0},
    		{236,4,0},
    		{237,4,0},
    		{238,4,0},
    		{239,4,0},
    		{240,4,0},
    		{241,4,0},
    		{242,4,0},
    		{243,4,0},
    		{244,4,0},
    		{245,4,0},
    		{246,4,0},
    		{247,4,0},
    		{248,4,0},
    		{249,4,0},
    		{250,4,0},
    		{251,4,0},
    		{252,4,0},
    		{253,4,0},
    		{254,4,0},
    		{255,4,0},
    		{256,4,0},
    		{257,4,0},
    		{258,4,0},
    		{259,4,0},
    		{260,4,0},
    		{261,4,0},
    		{262,4,0},
    		{263,4,0},
    		{264,4,0},
    		{265,4,0},
    		{266,4,0},
    		{267,4,0},
    		{268,4,0},
    		{269,4,0},
    		{270,4,0},
    		{271,4,0},
    		{272,4,0},
    		{273,4,0},
    		{274,4,0},
    		{275,4,0},
    		{276,4,0},
    		{277,4,0},
    		{278,4,0},
    		{279,4,0},
    		{280,4,0},
    		{281,4,0},
    		{282,4,0},
    		{283,4,0},
    		{284,4,0},
    		{285,4,0},
    		{286,4,0},
    		{287,4,0},
    		{288,4,0},
    		{289,4,0},
    		{290,4,0},
    		{291,4,0},
    		{292,4,0},
    		{293,4,0},
    		{294,4,0},
    		{295,4,0},
    		{296,4,0},
    		{297,4,0},
    		{298,4,0},
    		{299,4,0},
    		{300,4,0},
    		{301,4,0},
    		{302,4,0},
    		{303,4,0},
    		{304,4,0},
    		{305,4,0},
    		{306,4,0},
    		{307,4,0},
    		{308,4,0},
    		{309,4,0},
    		{310,4,0},
    		{311,4,0},
    		{312,4,0},
    		{313,4,0},
    		{314,4,0},
    		{315,4,0},
    		{316,4,0},
    		{317,4,0},
    		{318,4,0},
    		{319,4,0},
    		{320,4,0},
    		{321,4,0},
    		{322,4,0},
    		{323,4,0},
    		{324,4,0},
    		{325,4,0},
    		{326,4,0},
    		{327,4,0},
    		{328,4,0},
    		{329,4,0},
    		{330,4,0},
    		{331,4,0},
    		{332,4,0},
    		{333,4,0},
    		{334,4,0},
    		{335,4,0},
    		{336,4,0},
    		{337,4,0},
    		{338,4,0},
    		{339,4,0},
    		{340,4,0},
    		{341,4,0},
    		{342,4,0},
    		{343,4,0},
    		{344,4,0},
    		{345,4,0},
    		{346,4,0},
    		{347,4,0},
    		{348,4,0},
    		{349,4,0},
    		{350,4,0},
    		{351,4,0},
    		{352,4,0},
    		{353,4,0},
    		{354,4,0},
    		{355,4,0},
    		{356,4,0},
    		{357,4,0},
    		{358,4,0},
    		{359,4,0},
    		{360,4,0},
    		{361,4,0},
    		{362,4,0},
    		{363,4,0},
    		{364,4,0},
    		{365,4,0},
    		{366,4,0},
    		{367,4,0},
    		{368,4,0},
    		{369,4,0},
    		{370,4,0},
    		{371,4,0},
    		{372,4,0},
    		{373,4,0},
    		{374,4,0},
    		{375,4,0},
    		{376,4,0},
    		{377,4,0},
    		{378,4,0},
    		{379,4,0},
    		{380,4,0},
    		{381,4,0},
    		{382,4,0},
    		{383,4,0},
    		{384,4,0},
    		{385,4,0},
    		{386,4,0},
    		{387,4,0},
    		{388,4,0},
    		{389,4,0},
    		{390,4,0},
    		{391,4,0},
    		{392,4,0},
    		{393,4,0},
    		{394,4,0},
    		{395,4,0},
    		{396,4,0},
    		{397,4,0},
    		{398,4,0},
    		{399,4,0},
    		{400,4,0},
    		{401,4,0},
    		{402,4,0},
    		{403,4,0},
    		{404,4,0},
    		{405,4,0},
    		{406,4,0},
    		{407,4,0},
    		{408,4,0},
    		{409,4,0},
    		{410,4,0},
    		{411,4,0},
    		{412,4,0},
    		{413,4,0},
    		{414,4,0},
    		{415,4,0},
    		{416,4,0},
    		{417,4,0},
    		{418,4,0},
    		{419,4,0},
    		{420,4,0},
    		{421,4,0},
    		{422,4,0},
    		{423,4,0},
    		{424,4,0},
    		{425,4,0},
    		{426,4,0},
    		{427,4,0},
    		{428,4,0},
    		{429,4,0},
    		{430,4,0},
    		{431,4,0},
    		{432,4,0},
    		{433,4,0},
    		{434,4,0},
    		{435,4,0},
    		{436,4,0},
    		{437,4,0},
    		{438,4,0},
    		{439,4,0},
    		{440,4,0},
    		{441,4,0},
    		{442,4,0},
    		{443,4,0}});

    	addRawPoint(input1);
    	addRawPoint(input2);
    	followWaveOverTime();
//    	calculateRoot();
    }

    virtual ~DepthProfiler()
    {
    }

    void addRawPoint(Matrix<N, 3, T>& matrix)
    {
        // Apply filters and stuff.
        addProcessedPoint(matrix);
    }

    /**
     * Intersection with zero.
     * Using Newtons method.
     * https://en.wikipedia.org/wiki/Newton%27s_method
     */
    template<std::size_t H>
    T calculateRoot(const Matrix<H,1,T>& params, T inital_guess = 1.0, float precision = PRECISION) const
    {
    	std::cout << __PRETTY_FUNCTION__ << std::endl;
    	T guess = inital_guess;
    	T new_guess = 0;

    	for (std::size_t i = 0; i < MAX_ITERATIONS; ++i)
    	{
			T y = baseFunction(guess, params);
			T yp = baseDerivtive(guess, params);

			if(std::abs(yp) < EPSILON)
			{
				throw std::runtime_error("Determinat ~= 0");
			}

			//Wikipedia says its 'guess - y/yp' but that will diverge from the correct answer.
			new_guess = guess + y/yp;
			std::cout << "error: " << std::abs(new_guess - guess) << " guess: " << guess << " y: " << y << std::endl;
			if(std::abs(new_guess - guess) <= precision * abs(new_guess))
			{
				std::cout << "Found intersection at: " << new_guess << std::endl;
		    	return new_guess;
			}

			guess = new_guess;
		}
    	throw std::runtime_error("Took to many iterations.");
    }

    float calculateMovedArea(Dune& aDune, Dune& bDune)
    {
    	Matrix<4,1,T> input({-0.003143,0.007429,0.330888,2.789048});
    	//Where input2 is shifted to the right by 'h'.
    	Matrix<4,1,T> input2({-0.003143,0.007429,0.330888,2.789048});
    	float area_of_b = calculateArea(bDune.signature_, 0, 6);
    	float area_of_a = calculateArea(aDune.signature_, 6, 12);
    	float moved_area = aDune.surface_area_ - (area_of_a + area_of_b);
    	return moved_area;
    }

    float calculateArea(const Matrix<4, 1, T>& params, T start, T end)
    {
    	float y1 = baseDerivtive(start, params);
    	float y2 = baseDerivtive(end, params);
//        float y1 = ((1.0 / 4) * params.at(0, 0) * std::pow(start, 4)) +
//                   ((1.0 / 3) * params.at(1, 0) * std::pow(start, 3)) +
//                   ((1.0 / 2) * params.at(2, 0) * std::pow(start, 2)) + params.at(3, 0) * start;
//        float y2 = ((1.0 / 4) * params.at(0, 0) * std::pow(end, 4)) + ((1.0 / 3) * params.at(1, 0) * std::pow(end, 3)) +
//                   ((1.0 / 2) * params.at(2, 0) * std::pow(end, 2)) + params.at(3, 0) * end;
        return std::abs(y2 - y1);
    }

    void addProcessedPoint(Matrix<N, 3, T>& matrix)
    {
        try
        {
            BottomProfile<N, 3, T> bottom_profile;
            bottom_profile.raw_data_ = matrix;

            // Calculate ax^3 + bx^2 + cx + d
            auto afgeleide                             = calculateAfgeleide(matrix);
            std::vector<std::size_t> peaks_and_valleys = findPeaksAndValleys(afgeleide, 0.001, 1);
            std::vector<std::pair<std::size_t, std::size_t>> waves = classifyWave(afgeleide, peaks_and_valleys);

            for (std::size_t i = 0; i < waves.size(); ++i)
            {
                Dune dune;
                dune.size_  = waves[i].second - waves[i].first;
                dune.start_index_ = waves[i].first;

                // TODO: change this magic number.
                if (dune.size_ > 60)  // Our wave has more then 60 points
                {
                    continue;
                }
                if (dune.start_index_ + 60 > N)  // Our wave doesn't fit in matrix, this is really bad coding.
                {
                    return;
                }
                Matrix<60, 2, float> single_dune_matrix(matrix, dune.start_index_, 0);

                // Translate x values so they start from 0. (This helps the Gausse-Newton algorithme find a result.)
                T start_x = single_dune_matrix.at(0, 0);
                std::cout << "Start x: " << start_x << std::endl;
                for (std::size_t i = 0; i < dune.size_ + 1; ++i)
                {
                    single_dune_matrix.at(i, 0) -= start_x;
                }
                std::cout << single_dune_matrix << std::endl;

                Matrix<4, 1, T> params({ 1, 1, 1, 1 });
                gaussNewton(single_dune_matrix, params, dune.size_);
                dune.signature_ = params;
                dune.surface_area_ = calculateArea(params, single_dune_matrix.at(0, 0), single_dune_matrix.at(dune.size_, 0));
                bottom_profile.dunes_.push_back(dune);
                std::cout << dune.toString() << std::endl;
                std::cout << params << std::endl;
            }

            depth_data_.push_back(bottom_profile);
        }
        catch (std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
    }

    void followWaveOverTime()
    {
    	if(depth_data_.size() < 2)
    	{
    		return;
    	}

    	for (std::size_t i = 0; i < depth_data_.size() - 1; ++i) {
    		depth_data_.at(i).getSimularDune(depth_data_.at(i + 1), 0.01);
		}
    }

    template <std::size_t H, std::size_t W>
    Matrix<H - 1, 2, T> calculateAfgeleide(Matrix<H, W, T>& matrix) const
    {
        Matrix<H - 1, 2, T> result;

        // TODO: Do we get the correct matrix values?
        for (std::size_t i = 0; i < H - 1; ++i)
        {
            result[i][0] = matrix[i][0];
            result[i][1] = (matrix[i + 1][1] - matrix[i][1]) / (matrix[i + 1][0] - matrix[i][0]);
        }
        return result;
    }

    template <std::size_t H, std::size_t W>
    std::vector<std::size_t> findPeaksAndValleys(const Matrix<H, W, T>& afgeleide,
                                                 float afweiking,
                                                 std::size_t minimal_x_diff = 0) const
    {
        std::vector<std::size_t> result;
        float last_found_x = 0;
        for (std::size_t i = 0; i < H - 1; ++i)
        {
            if (((afgeleide[i][1] < 0 && afgeleide[i + 1][1] > 0) ||
                 (afgeleide[i][1] > 0 && afgeleide[i + 1][1] < 0)) &&
                abs(afgeleide[i][0] - last_found_x) > minimal_x_diff)
            {
                last_found_x = afgeleide[i][0];
                result.push_back(i);
            }
        }
        std::cout << "Found: " << result.size() << " peaks or valleys" << std::endl;
        return result;
    }

    template <std::size_t H, std::size_t W>
    bool isPeak(const Matrix<H, W, T>& afgeleide, std::size_t index) const
    {
        assert(index + 2 < H);
        return afgeleide.at(index, 1) > afgeleide.at(index + 1, 1) &&
               afgeleide.at(index + 1, 1) > afgeleide.at(index + 2, 1);
    }

    template <std::size_t H, std::size_t W>
    std::vector<std::pair<std::size_t, std::size_t>>
    classifyWave(const Matrix<H, W, T>& afgeleide, const std::vector<std::size_t>& peaks_and_valleys) const
    {
        std::vector<std::pair<std::size_t, std::size_t>> result;
        for (std::size_t i = 0; i < peaks_and_valleys.size() - 2; ++i)
        {
            // Find a valley -> peak -> valley order.
            if (!isPeak(afgeleide, peaks_and_valleys[i]) && isPeak(afgeleide, peaks_and_valleys[i + 1]) &&
                !isPeak(afgeleide, peaks_and_valleys[i + 2]))
            {
                result.push_back(std::make_pair(peaks_and_valleys[i], peaks_and_valleys[i + 2]));
                std::cout << "Found a wave from:" << peaks_and_valleys[i] << " to: " << peaks_and_valleys[i + 2]
                          << std::endl;
            }
        }
        return result;
    }

    /**
     * Function that the GaussNewton tries to calculate the parameters for.
     */
    template <std::size_t H, std::size_t aH>
    T baseFunction(const Matrix<H, 1, T>& input, const Matrix<aH, 1, T>& params) const
    {
        T x = input.at(0, 0);

        return params.at(0, 0) * std::pow(x, 3) + params.at(1, 0) * std::pow(x, 2) + params.at(2, 0) * x +
               params.at(3, 0);
    }

    template <std::size_t H>
    T baseFunction(T x, const Matrix<H, 1, T>& params) const
    {
        return params.at(0, 0) * std::pow(x, 3) + params.at(1, 0) * std::pow(x, 2) + params.at(2, 0) * x +
               params.at(3, 0);
    }

    template <std::size_t H>
    T baseDerivtive(T x, const Matrix<H,1,T>& params) const
    {
    	  return ((1.0 / 4) * params.at(0, 0) * std::pow(x, 4)) +
    	                   ((1.0 / 3) * params.at(1, 0) * std::pow(x, 3)) +
    	                   ((1.0 / 2) * params.at(2, 0) * std::pow(x, 2)) + params.at(3, 0) * x;
    }

    /**
     * Calculated derivative using: (y1 - y0)/(x1-x0).
     * But when doing so, it will change one parameter with a slight amount (DERIV_STEP).
     */
    template <std::size_t H, std::size_t aH>
    T calculateDerivative(const Matrix<H, 1, T>& input, const Matrix<aH, 1, T>& params, std::size_t param_number) const
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

    /**
     * Solve the non-linear least square problem using the Gauss-Newton algorithme.
     * https://en.wikipedia.org/wiki/Gauss%E2%80%93Newton_algorithm
     */
    template <std::size_t H, std::size_t W, std::size_t aH>
    void gaussNewton(const Matrix<H, W, T>& inputs,
                     Matrix<aH, 1, T>& params,
                     std::size_t iterations = H,
                     const float precision  = 1e-8) const
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

            std::cout << "Iteration: " << i << " " << total_deviation << std::endl;
        }
    }

  private:
    FileHandler file_instance_;
    std::vector<BottomProfile<N, 3, T>> depth_data_;
};

} /* namespace Controller */

#endif /* SRC_DEPTHPROFILER_HPP_ */
