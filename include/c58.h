
#define mode_fm  1
#define mode_ssb 0
//******* First Column [R2]
#define up 			1			//K1
#define down		2			//K2
#define mic_up		4			//K4
#define mic_down	8			//K8

#define mic_pressed	12			//K4 & K8
#define freq_tuning	15

//#define k_5_25		16
#define k_5_25		32			//L4
//#define busy		32
#define busy		64			//L8
//******* second column [R3]
#define mhz			1			//K1
#define scan_memo	2			//K2
#define scan_all	4			//K4
#define ccl			8			//K8
//#define simplex		16		// Sw closed = simplex
#define simplex		32			//L4 Sw closed = simplex
//#define duplex		32		// Sw closed = modo R1
#define duplex		64			//L8 Sw closed = modo R2
//******* third column [R4]
#define step 		1			//K1
#define rcl 		2			//K2
#define ent			4			//K4
#define nb			8			//K8
//******* fourth column [R5]

//#define fm			16		// Sw closed = FM
#define fm			32			//L4 Sw closed = FM


// ************* 

#define tx_sense 	8
#define ssb_call	43500
#define fm_call		52000	
#define no 			0
#define yes			1

#define timer_on TIMSK|=(1<<OCIE1A)
#define timer_off TIMSK&=(~(1<<OCIE1A))

#define plus	1
#define minus	2
#define A	0
#define B	1