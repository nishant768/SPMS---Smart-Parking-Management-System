// parking_sensor.h – Shared header for kernel module and userspace
// Used by ioctl commands

#ifndef PARKING_SENSOR_H
#define PARKING_SENSOR_H

#include <linux/ioctl.h>

#define PARKING_MAGIC   'P'
#define MAX_SLOTS       20

// IOCTL commands
#define PARKING_GET_STATUS   _IOR(PARKING_MAGIC, 1, int)   // Read slot status
#define PARKING_SET_OCCUPIED _IOW(PARKING_MAGIC, 2, int)   // Mark slot occupied
#define PARKING_SET_FREE     _IOW(PARKING_MAGIC, 3, int)   // Mark slot free
#define PARKING_GET_COUNT    _IOR(PARKING_MAGIC, 4, int)   // Get free slot count

// Sensor data structure written to /dev/parking_sensor
struct SensorData {
    int  slotId;      // 0-indexed slot number
    int  status;      // 0 = free, 1 = occupied
    long timestamp;   // unix timestamp
};

#endif // PARKING_SENSOR_H
