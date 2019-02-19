#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "st_device.h"
#include "Device.h"

static unsigned int n_storage_dev=0;
static struct storage_device *first_st_device=NULL;
struct storage_device* ins_st_dev_(const char *name, const char *vendor, unsigned int port, unsigned int MAX_size);

struct storage_device{
    unsigned int st_dev_id;
    unsigned int dev_id;
    unsigned int size;
    struct storage_device *next;
    int id_of_first_partition;
};

int get_id_first_partition(unsigned int st_dev_id){
    struct storage_device *aux_st_dev;
    aux_st_dev = first_st_device;
    while(aux_st_dev != NULL){
        if(aux_st_dev->st_dev_id == st_dev_id){
            return aux_st_dev->id_of_first_partition;
        }
        aux_st_dev = aux_st_dev->next;
    }
    return -1;
}

void set_first_partition(unsigned int st_dev_id, unsigned int partition_id){
    struct storage_device *aux_st_dev;
    while(aux_st_dev != NULL){
        if(aux_st_dev->st_dev_id == st_dev_id){
            aux_st_dev->id_of_first_partition = partition_id;
        }
        aux_st_dev = aux_st_dev->next;
    }
}

Storage_device *get_st_dev(Storage_device *st_dev, unsigned int st_dev_id){
    struct storage_device *aux_st_dev;
    aux_st_dev = first_st_device;
    while(aux_st_dev != NULL){
        if(aux_st_dev->st_dev_id == st_dev_id){
            st_dev->dev_id = aux_st_dev->dev_id;
            st_dev->size = aux_st_dev->size;
            st_dev->st_dev_id = st_dev_id;
            return st_dev;
        }
        aux_st_dev = aux_st_dev->next;
    }
    return NULL;
}

int ins_st_dev(const char *name, const char *vendor, unsigned int port, unsigned int MAX_size){
    struct storage_device *st_dev;
    st_dev = ins_st_dev_(name,vendor,port,MAX_size);
    if(st_dev == NULL)
        return -1;
    return st_dev->st_dev_id;
}

struct storage_device* ins_st_dev_(const char *name, const char *vendor, unsigned int port, unsigned int MAX_size){
    int dev_id = ins_device(name, vendor, port);
    if(dev_id ==-1)
        return NULL;

    struct storage_device *st_dev, *aux_st_dev;

    st_dev = malloc(sizeof(struct storage_device));
    if(st_dev == NULL)
        return NULL;

    if(first_st_device == NULL){
        first_st_device = st_dev;
    }
    else{
        aux_st_dev = first_st_device;
        while(aux_st_dev->next == NULL){
            aux_st_dev = aux_st_dev->next;
        }
        aux_st_dev->next = st_dev;
    }
    st_dev->dev_id = dev_id;
    st_dev->st_dev_id = n_storage_dev;
    st_dev->id_of_first_partition = -1;
    st_dev->next = NULL;
    st_dev->size = MAX_size;
    n_storage_dev++;
    return st_dev;
};
