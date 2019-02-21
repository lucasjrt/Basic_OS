#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "file_system.h"
#include "device.h"
#include "st_device.h"

static unsigned int n_partition=0;

static struct partition *first_partition = NULL;

struct file{
    char name[32];
    unsigned int size;
    void *data;
    struct file *next; //File node in the same level
};

struct content_folder{
    struct file *file; //Node of files
    struct folder *next; //Node of directories
};

struct folder{
    char name[32];
    struct content_folder *content; //Pointer to content of folder
    struct folder *next;//Directories node in the same level
};

struct file_system{
    struct folder *entry_point;
};

struct partition{
    unsigned int id;
    unsigned int st_dev_id;
    char fs_type[32];
    unsigned int max_size;
    unsigned int used;
    struct file_system *fs;
    struct partition *next;
};

static struct file_system* create_fs_();
static struct partition* create_partition_(unsigned int st_dev_id, const char *fs_type, unsigned int size);
static struct folder* create_dir_(struct content_folder *cur_dir, char const *name);
static void rm_dir_rec_(struct folder *folder, unsigned int partition);
static int rm_dir_(struct content_folder *cur_dir, const char *folder_name, unsigned int partition);
static int rm_file_(struct content_folder *cur_dir, const char *file_name, unsigned int partition);
static struct file *create_file_(struct content_folder *cur_dir, const char *name, const char *data, unsigned int size);
static struct partition *get_first_partition_();
static struct partition *get_part_by_id_(unsigned int n);
static struct folder *get_folder_by_direct_path_(const unsigned int partition_id, const char *path, unsigned int size_path);
static struct folder *find_folder_(struct content_folder *dir, const char *folder_name);
static struct file *get_file_(const char *path, unsigned int partition_id);

static void show_files(struct content_folder *dir, int spaces);



static struct file_system* create_fs_(){
    struct file_system *fs;
    fs = malloc(sizeof(struct file_system));
    struct folder *entry;
    entry = malloc(sizeof(struct folder));
    struct content_folder *content_entry;
    content_entry = malloc(sizeof(struct content_folder));
    strcpy(entry->name,"");
    entry->next = NULL;
    entry->content = content_entry;
    fs->entry_point = entry;
    entry->content->file = NULL;
    entry->content->next = NULL;

    return fs;
}

int create_partition(unsigned int st_dev_id, const char *fs_type, unsigned int partition_size){
    struct partition *part;
    part = create_partition_(st_dev_id, fs_type, partition_size);
    if(part== NULL)
        return -1;
    return part->id;
}

static struct partition* create_partition_(unsigned int st_dev_id, const char *fs_type, unsigned int size){
    Storage_device st_dev;
    if(get_st_dev(&st_dev, st_dev_id)==NULL)
        return NULL;
    struct partition *part, *aux;
    part = malloc(sizeof(struct partition));

    aux = first_partition;
    if(n_partition == 0)
        first_partition = part;
    else{
        while(aux->next != NULL){
            aux = aux->next;
        }
        aux->next = part;
    }

    part->st_dev_id = st_dev.st_dev_id;
    part->next = NULL;
    strcpy(part->fs_type,fs_type);
    part->max_size = size;
    part->used = 0;
    part->id = n_partition;
    if(get_id_first_partition(st_dev.st_dev_id)==-1)
        set_first_partition(st_dev_id, part->id);
    n_partition++;
    part->fs = create_fs_();
    return part;
};

int create_dir(const char *path, const char *folder_name, unsigned int partition){
    struct folder *folder;
    folder = get_folder_by_direct_path_(partition,path,strlen(path));
    if(folder == NULL)
        return 0;
    if(create_dir_(folder->content,folder_name) == NULL)
        return 0;
    return 1;
}

static struct folder* create_dir_(struct content_folder *cur_dir, char const *name){
    if(strlen(name)==0)
        return NULL;
    struct folder *new_dir, *aux;
    struct content_folder *new_content;
    new_dir = malloc(sizeof(struct folder));
    new_content = malloc(sizeof(struct content_folder));
    new_content->file = NULL;
    new_content->next = NULL;

    strcpy(new_dir->name,name);
    new_dir->next = NULL;
    new_dir->content = new_content;
    if(cur_dir->next==NULL){
        cur_dir->next = new_dir;
    }
    else{
        if(strcmp(cur_dir->next->name,name)==0){
            free(new_dir);
            free(new_content);
            return NULL;
        }
        aux = cur_dir->next;
        while(aux->next!=NULL){
            if(strcmp(aux->next->name,name)==0){
                free(new_dir);
                free(new_content);
                return NULL;
            }
            aux = aux->next;
        }
        aux->next = new_dir;
    }
    return new_dir;
}

int rm_dir(const char *dir_path, unsigned int partition){
    struct folder *folder;
    if(dir_path == NULL)
        return 0;
    if(dir_path[0] == '\0')
        return 0;
    if(dir_path[0] == '/' && dir_path[1] == '\0'){
        File file;
        Folder dir;

        folder = get_folder_by_direct_path_(partition, dir_path, 1);
        if(folder->content->file == NULL && folder->content->next == NULL)
            return 0;

        while(get_first_file(&file,dir_path,partition) != NULL){
            rm_file(file.path, partition);
        }

        while(get_first_subdirectory(&dir, dir_path, partition) != NULL){
            rm_dir(dir.path, partition);
        }

        return 1;
    }

    unsigned int i = strlen(dir_path);
    while(dir_path[i] != '/'){
        i--;
    }
    folder = get_folder_by_direct_path_(partition, dir_path, i);
    if(folder == NULL)
        return 0;
    if(rm_dir_(folder->content, &(dir_path[i+1]), partition)==1)
        return 1;
    return 0;
}

static void rm_dir_rec_(struct folder *folder, unsigned int partition){
    struct folder *aux_d;
    struct folder *aux2_d;
    struct file *aux_f, *aux2_f;
    aux_d = folder->content->next;
    aux_f = folder->content->file;

    while(aux_f != NULL){
        aux2_f = aux_f->next;
        rm_file_(folder->content, aux_f->name, partition);
        aux_f = aux2_f;
    }

    if(aux_d != NULL){
        aux2_d = aux_d->next;
        rm_dir_rec_(aux_d, partition);

        while(aux2_d != NULL){
            rm_dir_rec_(aux2_d, partition);
            aux2_d = aux2_d->next;
        }
    }
    free(folder->content);
    folder->content = NULL;
    free(folder);
}

static int rm_dir_(struct content_folder *cur_dir, const char *folder_name, unsigned int partition){
    if(cur_dir == NULL)
        return 0;
    struct folder *aux, *aux2;
    if(cur_dir->next == NULL)
        return 0;
    if(strcmp(cur_dir->next->name, folder_name)==0){
        aux = cur_dir->next->next;
        rm_dir_rec_(cur_dir->next, partition);
        cur_dir->next = aux;
        return 1;
    }
    aux = cur_dir->next;
    while(aux->next != NULL){
        if(strcmp(aux->next->name,folder_name) == 0)
            break;
        aux = aux->next;
    }
    if(aux->next == NULL)
        return 0;
    aux2 = aux->next->next;
    rm_dir_rec_(aux->next, partition);
    aux->next = aux2;
    return 1;

};

int rm_file(const char *file_path, unsigned int partition){
    struct folder *folder;
    struct file *file;
    unsigned int i = strlen(file_path);

    while(file_path[i] != '/'){
        i--;
    }
    folder = get_folder_by_direct_path_(partition, file_path, i);
    if(folder == NULL)
        return 0;

    file = get_file_(file_path, partition);
    if(file == NULL)
        return 0;
    return rm_file_(folder->content, &file_path[i+1], partition);
}

static int rm_file_(struct content_folder *cur_dir, const char *file_name, unsigned int partition){
    struct file *aux_f;
    struct file *aux2_f;
    struct partition *part;
    part = get_part_by_id_(partition);
    if(cur_dir->file == NULL)
        return 0;
    if(strcmp(cur_dir->file->name, file_name)==0){
        part->used -= cur_dir->file->size;
        free(cur_dir->file->data);
        aux_f = cur_dir->file->next;
        free(cur_dir->file);
        cur_dir->file = aux_f;
        return 1;
    }
    aux_f = cur_dir->file;
    while(strcmp(aux_f->next->name, file_name) != 0){
        if(aux_f->next->next == NULL)
            return 0;
        aux_f = aux_f->next;
    }
    part->used -= aux_f->next->size;
    free(aux_f->next->data);
    aux2_f = aux_f->next->next;
    free(aux_f->next);
    aux_f->next = aux2_f;
    return 1;
};

int create_file(const char *folder_path, unsigned int partition, const char *file_name, const char *data, unsigned int data_size){
    struct file *file;
    struct folder *folder;
    struct partition *part;
    folder = get_folder_by_direct_path_(partition, folder_path, strlen(folder_path));
    if(folder == NULL)
        return 0;
    file = create_file_(folder->content,file_name,data,data_size);
    if(file == NULL)
        return 0;
    part = get_part_by_id_(partition);
    part->used+=data_size;
    return 1;
}

static struct file *create_file_(struct content_folder *cur_dir, const char *name, const char *data, unsigned int size){
    if(strlen(name)==0)
        return NULL;
    struct file *new_file, *aux;
    new_file = malloc(sizeof(struct file));
    unsigned char *new_data;
    new_data = malloc(size);
    for(unsigned i=0; i<size;i++){
        new_data[i] = ((unsigned char*)data)[i];
    }
    strcpy(new_file->name, name);
    new_file->data = new_data;
    new_file->size = size;
    new_file->next = NULL;


    if(cur_dir->file==NULL){
        cur_dir->file = new_file;
    }
    else{
        if(strcmp(cur_dir->file->name,name)==0){
            free(new_data);
            free(new_file);
            return NULL;
        }
        aux = cur_dir->file;
        while(aux->next!=NULL){
            if(strcmp(aux->next->name,name)==0){
                free(new_data);
                free(new_file);
                return NULL;
            }
            aux = aux->next;
        }
        aux->next = new_file;
    }
    return new_file;
};

Partition *get_first_partition(Partition *partition){
    if(partition == NULL)
        return NULL;
    struct partition *part;
    part = get_first_partition_();
    if(part == NULL)
        return NULL;
    partition->id = part->id;
    partition->max_size = part->max_size;
    partition->used = part->used;
    strcpy(partition->fs_type, part->fs_type);
    return partition;
}

static struct partition *get_first_partition_(){
    return first_partition;
}

Partition *get_next_partition(Partition *partition){
    struct partition *part;
    part = get_part_by_id_(partition->id);
    if(part == NULL)
        return NULL;
    if(part->next == NULL)
        return NULL;
    strcpy(partition->fs_type, part->next->fs_type);
    partition->id = part->next->id;
    partition->max_size = part->next->max_size;
    partition->used = part->next->used;
    return partition;
}

static struct partition *get_part_by_id_(unsigned int n){
    struct partition *aux;
    aux = first_partition;
    while(aux != NULL && aux->id != n){
        aux = aux->next;
    }
    if(aux == NULL)
        return NULL;
    return aux;
};

Folder *get_folder(Folder *dir, const unsigned int partition_id, const char *path){
    int i=0,j;
    struct folder *aux_dir;
    struct folder *temp_delete;
    char aux_name[32];
    struct partition *aux;
    aux_dir = malloc(sizeof(struct folder));
    temp_delete = aux_dir;
    aux = get_part_by_id_(partition_id);
    if(aux == NULL)
        return NULL;

    if(path[0]=='/' && path[1]=='\0'){
        strcpy(dir->name,"");
        strcpy(dir->path,"/");
        return dir;
    }
    strcpy(aux_dir->name,"");

    aux_dir->content = aux->fs->entry_point->content;
    while(path[i] != '\0'){
        j=0;
        if(path[i]=='/')
            i++;
        while(path[i] != '/' && path[i]!='\0'){
            aux_name[j] = path[i];
            j++;
            i++;
        }
        if(path[i] != '\0')
            i++;
        aux_name[j] = '\0';
        aux_dir = find_folder_(aux_dir->content,aux_name);
        if(aux_dir == NULL)
            return NULL;
    }
    strcpy(dir->name,aux_dir->name);
    strcpy(dir->path,path);
    char char_aux = path[0];
    if(path[0]!='/'){
        i=1;
        dir->path[0] = '/';
        while(path[i]!='\0'){
            dir->path[i] = char_aux;
            char_aux = path[i];
            i++;
        }
        dir->path[i] = char_aux;
    }
    if(dir->path[strlen(dir->path)-1] == '/'){
        dir->path[strlen(dir->path)-1] = '\0';
    }
    strcpy(dir->path,dir->path);
    free(temp_delete);
    return dir;
}

static struct folder *get_folder_by_direct_path_(const unsigned int partition_id, const char *path, unsigned int size_path){
    int i=0,j;
    struct folder *aux_dir;
    struct folder *temp_delete;
    char aux_name[32];
    struct partition *aux;
    aux_dir = malloc(sizeof(struct folder));
    temp_delete = aux_dir;
    aux = get_part_by_id_(partition_id);
    if(aux == NULL)
        return NULL;

    if(path[0]=='/' && path[1]=='\0'){
        return aux->fs->entry_point;
    }
    strcpy(aux_dir->name,"");

    aux_dir->content = aux->fs->entry_point->content;

    while(path[i] != '\0' && i<size_path){
        j=0;
        if(path[i]=='/')
            i++;
        while(path[i] != '/' && path[i]!='\0'){
            aux_name[j] = path[i];
            j++;
            i++;
        }
        if(path[i] != '\0')
            i++;
        aux_name[j] = '\0';
        aux_dir = find_folder_(aux_dir->content,aux_name);
        if(aux_dir == NULL)
            return NULL;
        //show_dir(aux_dir);
    }
    free(temp_delete);
    return aux_dir;
}

static struct folder *find_folder_(struct content_folder *dir, const char *folder_name){
    if(dir == NULL)
        return NULL;
    if(dir->next == NULL)
        return NULL;
    struct folder *aux;
    aux = dir->next;
    while(aux->next != NULL && strcmp(aux->name,folder_name)!=0){
        aux = aux->next;
    }

    if(strcmp(aux->name,folder_name)!=0)
        return NULL;
    return aux;
}

Folder *get_first_subdirectory(Folder *dir, const char *path, unsigned int partition_id){
    if(dir == NULL)
        return NULL;
    struct folder *f;
    f = get_folder_by_direct_path_(partition_id, path,strlen(path));
    if(f == NULL)
        return NULL;
    if(f->content->next == NULL)
        return NULL;
    strcpy(dir->path, path);
    strcpy(dir->name, f->content->next->name);

    if(dir->path[strlen(dir->path)-1]=='/')
        strcat(dir->path,dir->name);
    else
        strcat(strcat(dir->path, "/"), dir->name);
    return dir;
}

Folder *get_next_folder(Folder *dir, unsigned int partition_id){
    if(dir == NULL)
        return NULL;
    struct folder *f;
    f = get_folder_by_direct_path_(partition_id,dir->path, strlen(dir->path));
    if(f == NULL)
        return NULL;
    if(f->next == NULL)
        return NULL;
    strcpy(dir->name, f->next->name);
    unsigned int i = strlen(dir->path);
    while(dir->path[i] != '/'){
        i--;
    }
    i++;
    dir->path[i] = '\0';
    int j=0;
    while(i<strlen(f->next->name)+strlen(dir->path)){
        dir->path[i] = f->next->name[j];
        i++;j++;
    }
    return dir;
}

static struct file *get_file_(const char *path, unsigned int partition_id){
    struct folder *folder;
    struct file *file;
    unsigned int i;
    i = strlen(path);
    while(path[i] != '/'){
        i--;
    }
    folder = get_folder_by_direct_path_(partition_id, path, i);
    if(folder == NULL)
        return NULL;

    file = folder->content->file;

    while(file != NULL){
        if(strcmp(file->name,&path[i+1])==0){
            return file;
        }
        file = file->next;
    }
    return NULL;
};

File *get_file(File *f, const unsigned int partition_id, const char *path){
    if(f == NULL)
        return NULL;
    struct file *file;
    file = get_file_(path,partition_id);
    if(file == NULL)
        return NULL;
    strcpy(f->name, file->name);
    strcpy(f->path, path);
    f->size = file->size;
    return f;
}

File *get_first_file(File *file, const char *path, unsigned int partition_id){
    if(strlen(path)<1)
        return NULL;
    struct folder *dir;
    dir = get_folder_by_direct_path_(partition_id,path, strlen(path));
    if(dir == NULL)
        return NULL;
    if(dir->content->file == NULL)
        return NULL;
    strcpy(file->name, dir->content->file->name);
    file->size = dir->content->file->size;
    strcpy(file->path,path);
    if(path[strlen(path)-1]=='/')
        strcat(file->path, dir->content->file->name);
    else
        strcat(strcat(file->path, "/"), file->name);

    return file;
}

File *get_next_file(File *file, unsigned int partition_id){
    if(file == NULL)
        return NULL;
    struct file *new_f;
    new_f = get_file_(file->path, partition_id);
    if(new_f == NULL)
        return NULL;
    new_f = new_f->next;
    if(new_f == NULL)
        return NULL;
    strcpy(file->name, new_f->name);
    file->size = new_f->size;
    unsigned int i = strlen(file->path);
    while(file->path[i] != '/'){
        i--;
    }
    i++;
    file->path[i] = '\0';
    unsigned int j=0;
    while(i<strlen(file->name)+strlen(file->path)){
        file->path[i] = file->name[j];
        i++;j++;
    }
    return file;
}

void *get_file_data(File *file, void *data, unsigned int partition_id){
    struct file *true_file;
    if(file == NULL)
        return NULL;
    true_file = get_file_(file->path, partition_id);
    if(true_file == NULL)
        return NULL;
    for(unsigned int i=0; i<file->size;i++){
        ((char *)data)[i] = ((char *)true_file->data)[i];
    }
    return data;
}

static void show_files(struct content_folder *dir, int spaces){
    int i;
    struct file *aux_f = dir->file;
    struct folder *aux_d = dir->next;
    while(aux_f != NULL){
        for(i=0;i<spaces;i++)printf(" ");
        printf("<fle> %s  ",aux_f->name); for(i=0;i<aux_f->size;i++)printf("%c",((char*)aux_f->data)[i]); printf("\n");
        aux_f = aux_f->next;
    }
    while(aux_d != NULL){
        for(i=0;i<spaces;i++)printf(" ");
        printf("<dir> %s\n",aux_d->name);
        show_files(aux_d->content,spaces+5);
        aux_d = aux_d->next;
    }
}

void show(unsigned int st_dev_id){
    Storage_device st_dev;
    Device dev;
    if(get_st_dev(&st_dev, st_dev_id)==NULL)
        return;
    if(get_device(&dev, st_dev.dev_id)==NULL)
        return;

    printf("\nDevice information:\n");
    printf("Name: %s\nVendor: %s\nPort: %u\n",dev.name, dev.vendor, dev.port);

    printf("Storage device information:\n");
    printf("Size: %u\n", st_dev.size);

    printf("\nPartitions:\n\n");
    struct partition *aux;
    int i=0;
    aux = first_partition;
    while(aux!=NULL){
        printf("Partition %i\n",i);
        printf("Partition_ID: %d\nStorage Device ID: %d\nType: %s\nMax Size: %u\nUsed: %u\n",aux->id, aux->st_dev_id,aux->fs_type, aux->max_size, aux->used);
        show_files(aux->fs->entry_point->content, aux->id);
        aux = aux->next;
        i++;
        printf("\n");
    }
}
