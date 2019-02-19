#ifndef DEVICE_H
#define DEVICE_H

typedef struct device_info Device;

struct device_info{
    unsigned int id;
    char name[32];
    char vendor[32];
    unsigned int port;
};

Device *get_device(Device *dev, unsigned int dev_id);
int ins_device(const char *name, const char *vendor, unsigned int port);

#endif

