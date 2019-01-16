/*
 * _globals.h
 *
 *  Created on: 17 дек. 2018 г.
 *      Author: Alexander
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include "F28x_Project.h"
#include <stdint.h>
//#include <filter.h>

#define SIZE_CURR_BURST         0x19U   //0x19U == 25U
#define CURR_TRFER_SZ           0x2U    //0x2U == 2U
#define NUM_CURR_CH             0x2U    //0x2U == 2U
#define SIZE_FORCE_BURST        0x19U   //0x19U == 25U
#define FORCE_TRFER_SZ          0x2U    //0x2U == 2U
#define NUM_FORCE_CH            0x2U    //0x2U == 2U
#define SIZE_VOLTAGE_BURST      0x32U   //0x19U == 50U
#define VOLT_BUF_SZ             0x2U    //0x2U == 2U
#define NUM_VOLT_CH             0x1U    //0x1U == 1U

//struct VECTOR2F {
//    float32 drive1;
//    float32 drive2;
//};

typedef struct {
    float32 drive1;
    float32 drive2;
} VECTOR2F;
/*
struct DRIVE_ANG_SENS {
    uint16_t sr_load;
    uint16_t sr_rotor;
};
struct ANGLES {
    struct DRIVE_ANG_SENS drive1;
    struct DRIVE_ANG_SENS drive2;
};
struct DRIVE_ANG_SENS_FILT {
    float32 sr_load;
    float32 sr_rotor;
};
struct ANGLES_FILT {
    struct DRIVE_ANG_SENS_FILT drive1;
    struct DRIVE_ANG_SENS_FILT drive2;
};

struct ANGLES get_angles(void);
void set_angles(const volatile struct ANGLES *angl);
volatile struct ANGLES *angles_ptr(void);
*/
#endif /* GLOBALS_H_ */
