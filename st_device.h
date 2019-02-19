#ifndef ST_DEVICE_H
#define ST_DEVICE_H

typedef struct st_info Storage_device;


struct st_info{
    unsigned int st_dev_id;
    unsigned int dev_id;
    unsigned int size;
};

int get_id_first_partition(unsigned int st_dev_id);
void set_first_partition(unsigned int st_dev_id, unsigned int partition_id);
Storage_device *get_st_dev(Storage_device *st_dev, unsigned int st_dev_id);
int ins_st_dev(const char *name, const char *vendor, unsigned int port, unsigned int MAX_size);

#endif
