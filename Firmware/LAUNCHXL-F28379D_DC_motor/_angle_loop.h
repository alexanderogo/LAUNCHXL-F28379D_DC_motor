/*
 * _angle_loop.h
 *
 *  Created on: 12 џэт. 2019 у.
 *      Author: Alexander
 */

#ifndef ANGLE_LOOP_H_
#define ANGLE_LOOP_H_

#include <stdint.h>

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

void init_ang_loop(void);
//void angle_loop_calc(float32 ang_target, uint32_t divider);
void angle_loop_calc(VECTOR2F ang_target, uint32_t divider);

struct ANGLES get_angles(void);
void set_angles(const volatile struct ANGLES *angl);
volatile struct ANGLES *angles_ptr(void);

#endif /* ANGLE_LOOP_H_ */
