/**
*      @file eeprom_memory_map.h
*      @author Prithvi Bhat
*      @brief Maps various EEPROM regions
*      @date 2022-11-18
**/
#ifndef EEPROM_MAP_H
#define EEPROM_MAP_H

#define BLOCK_SIZE  32

// Coordinates
// Sensor A
#define CRD_AX      0   // 0x00
#define CRD_AY      1   // 0x32

// Sensor B
#define CRD_BX      2   // 0x64
#define CRD_BY      3   // 0x96

// Sensor C
#define CRD_CX      4   // 0x128
#define CRD_CY      5   // 0x160

// Fix Values
// Sensor A
#define FIX_X       6   // 0x192
#define FIX_Y       7   // 0x224

// Average /  Max samples
#define TC_AVG      21  // 0x672

// Beep Values
// Sensor A
#define LOAD_A      22  // 0x704
#define PER1_A      23  // 0x736
#define PER2_A      24  // 0x768
#define CONT_A      25  // 0x800

// Sensor B
#define LOAD_B      26  // 0x832
#define PER1_B      27  // 0x864
#define PER2_B      28  // 0x896
#define CONT_B      29  // 0x928

// Sensor C
#define LOAD_C      30  // 0x960
#define PER1_C      31  // 0x992
#define PER2_C      32  // 0x1024
#define CONT_C      33  // 0x1056

// Watchdog timeout / Error
#define LOAD_ERR    34  // 0x1088
#define PER1_ERR    35  // 0x1120
#define PER2_ERR    36  // 0x1152
#define CONT_ERR    37  // 0x1184

// Reset
#define LOAD_RST    38  // 0x1216
#define PER1_RST    39  // 0x1248
#define PER2_RST    40  // 0x1280
#define CONT_RST    41  // 0x1312

// IR interrupt
#define LOAD_IR     42  // 0x1344
#define PER1_IR     43  // 0x1376
#define PER2_IR     44  // 0x1408
#define CONT_IR     45  // 0x1440

#endif
