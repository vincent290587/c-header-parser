//
// Created by vgol on 29/06/2021.
//

#ifndef CHP_EMBEDDEDH_H
#define CHP_EMBEDDEDH_H

#include <stdint.h>

typedef unsigned char Boolean8bit;

//typedef enum {
//    false8bit = 0x00,
//    true8bit = 0xff,
//} Boolean8bit;

//typedef struct {
//    unsigned char val;
//} Boolean8bit;

typedef enum {
    eMySampleEnumVal1 = 1,
    eMySampleEnumVal2 = 3,
} eMySampleEnum;

typedef struct __attribute__ ((__packed__)) {
    unsigned char id;///< test1
    unsigned char software_id;
    unsigned int  obc_uptime;
    unsigned int  obc_epoch;
    unsigned int  current_size;
    unsigned int  epoch_next;
    unsigned int  last_id;
    unsigned char scheduler_state;
    char          prevFile[13];
} ttc_cmdscheduler_replypacket_t;

typedef struct __attribute__((__packed__)) {
    char filepath[33]; //Enough space for /12345678/12345678/12345678.ext + \0
    Boolean8bit relative;
    uint8_t epoch1;
    uint16_t epoch2;
    uint32_t epoch3;
    Boolean8bit performClear;
} ttc_subcmd_executefile_t;

typedef struct {
    uint8_t part1 : 3;
    uint8_t part2 : 7;
} sBitfields;

typedef struct _TestStruct {
    int16_t manager[3];
    float testf;///< test1
    double testd; ///< test2
    eMySampleEnum teste; ///< eMySampleEnum
    ttc_subcmd_executefile_t s1;
    ttc_cmdscheduler_replypacket_t s2;
} TestStruct;

void test_function(TestStruct *p_test);

#endif //CHP_EMBEDDEDH_H
