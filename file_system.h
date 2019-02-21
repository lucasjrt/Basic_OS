#ifndef FILESYSTEM_H
#define FILESYSTEM_H


typedef struct folder_info Folder;
typedef struct file_info File;
typedef struct partition_info Partition;

struct folder_info{
    char name[32];
    char path[100];
};

struct file_info{
    char name[32];
    unsigned int size;
    char path[100];
};

struct partition_info{
    unsigned int id;
    char fs_type[32];
    unsigned int max_size;
    unsigned int used;
};

/*
Examplo de path:
Correto:  /home/usuario     ;   /home/usuario/arquivo1
Incorreto: /home/usuario/   ;   /home/usuario/arquivo1/
*/

/*
Obs: Em todas as funcoes get, a estrutura passada ja tem de ter sido devidamente alocada (de maneira estatica ou dinamica).
*/

int create_partition(unsigned int st_dev_id, const char *fs_type, unsigned int partition_size);//Retorna -1 em caso de erro, caso contrario retorna o id da particao criada
Partition *get_first_partition(Partition *partition);
Partition *get_next_partition(Partition *partition);

int create_dir(const char *path, const char *folder_name, unsigned int partition_id);
int rm_dir(const char *dir_path, unsigned int partition_id);
Folder *get_folder(Folder *dir, const unsigned int partition_id, const char *path);
Folder *get_first_subdirectory(Folder *dir, const char *path, unsigned int partition_id);
Folder *get_next_folder(Folder *dir, unsigned int partition_id);

int create_file(const char *folder_path, unsigned int partition_id, const char *file_name, const char *data, unsigned int data_size);
int rm_file(const char *file_path, unsigned int partition_id);
File *get_file(File *f, const unsigned int partition_id, const char *path);
File *get_first_file(File *file, const char *path, unsigned int partition_id);
File *get_next_file(File *file, unsigned int partition_id);
void *get_file_data(File *file, void *data, unsigned int partition_id);

void show(unsigned int st_dev_id);

#endif
