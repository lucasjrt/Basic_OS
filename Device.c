#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "Device.h"


static unsigned int n_dev;
static struct device *first_device;
struct device *ins_device_(const char *name,const char *vendor,unsigned int port);

struct device{
    unsigned int id;
    char name[32];
    char vendor[32];
    unsigned int port;
    struct device *next;
};


Device *get_device(Device *dev, unsigned int dev_id){
    struct device *aux_dev;
    aux_dev = first_device;
    while(aux_dev != NULL){
        if(aux_dev->id == dev_id){
            strcpy(dev->name, aux_dev->name);
            strcpy(dev->vendor, aux_dev->vendor);
            dev->port = aux_dev->port;
            return dev;
        }
        aux_dev = aux_dev->next;
    }
    return NULL;
}

int ins_device(const char *name, const char *vendor, unsigned int port){
    struct device *dev;
    dev = ins_device_(name, vendor, port);
    if(dev != NULL){
        return dev->id;
    }
    return -1;
}

struct device *ins_device_(const char *name,const char *vendor,unsigned int port){
    struct device *dev, *aux_dev;
    dev = malloc(sizeof(struct device));
    if(dev == NULL)
        return NULL;
    strcpy(dev->name,name);
    dev->port = port;
    strcpy(dev->vendor,vendor);
    dev->id = n_dev;
    dev->next = NULL;

    if(first_device == NULL){
        first_device = dev;
    }
    else{
        aux_dev = first_device;
        while(aux_dev->next == NULL){
            aux_dev = aux_dev->next;
        }
        aux_dev->next = dev;
    }
    n_dev++;

    return dev;
}
