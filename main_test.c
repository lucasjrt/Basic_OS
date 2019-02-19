#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FileSystem.h"
#include "Device.h"
#include "st_device.h"



void show_file(File f){
    printf("<fle> %s\n", f.name);
}

void show_folder(Folder f){
    printf("<dir> %s\n", f.name);
}

void show_files(const char *path, unsigned int partition){
    File file;
    Folder folder;
    if(get_first_file(&file, path, partition)!=NULL){
        show_file(file);
        while(get_next_file(&file, partition)!=NULL){
            show_file(file);
        }
    }
    if(get_first_subdirectory(&folder, path, partition)!=NULL){
        show_folder(folder);
        while(get_next_folder(&folder, partition)!=NULL){
            show_folder(folder);
        }
    }

}

int main()
{
    char command_line[100];
    char command[20];
    char cur_path[100];
    char path[100], aux_path[100];
    int i=0, aux_size;
    unsigned int st_dev_id, cur_partition;
    st_dev_id = ins_st_dev("HD","Samsung",0,200000);
    cur_partition = create_partition(st_dev_id, "Fat32", 40000);
    if(cur_partition==-1)
        printf("Erro ao criar particao\n");

    cur_partition = create_partition(st_dev_id, "NTFS", 80000);
    if(cur_partition==-1)
        printf("Erro ao criar particao\n");

    cur_partition = create_partition(st_dev_id, "EXT4", 25000);

    if(cur_partition==-1)
        printf("Erro ao criar particao\n");
    strcpy(cur_path, "/");

    while(1){
        i=0;
        printf("%d:%s>", cur_partition, cur_path);
        scanf("%[^\n]", command_line);
        getchar();
        while(command_line[i] != ' ' && command_line[i] != '\0'){
            command[i] = command_line[i];
            i++;
        }
        command[i] = '\0';
        strcpy(path, &command_line[i+1]);

        if(strcmp(command_line, "show_device")==0)
            show(st_dev_id);
        if(strcmp(command_line, "pwd")==0){
            printf("%s\n",cur_path);
        }
        if(strcmp(command_line, "ls")==0){
            show_files(cur_path, cur_partition);
        }
        if(strcmp(command, "cd")==0){
            Folder folder;
            if(strcmp(path, "..")==0){
                strcpy(path, cur_path);
                aux_size = strlen(path);
                while(path[aux_size]!='/')
                    aux_size--;
                if(aux_size ==0)
                    path[aux_size+1] = '\0';
                else
                    path[aux_size] = '\0';

                if(get_folder(&folder, cur_partition, path)== NULL)
                        printf("Error\n");
                    else{
                        strcpy(cur_path, folder.path);
                    }
            }
            else{
                if(path[strlen(path)-1]=='/' && strlen(path) != 1){
                    path[strlen(path)-1] = '\0';
                }
                if(path[0]!='/'){
                    strcpy(aux_path, cur_path);
                    aux_size = strlen(aux_path);
                    if(aux_size > 1){
                        aux_path[aux_size] = '/';
                        aux_path[aux_size+1] = '\0';
                    }
                    strcat(aux_path, path);
                    if(get_folder(&folder, cur_partition, aux_path)== NULL)
                        printf("Error\n");
                    else{
                        strcpy(cur_path, folder.path);
                    }
                }
                else{
                    if(get_folder(&folder, cur_partition, path)== NULL)
                        printf("Error\n");
                    else{
                        strcpy(cur_path, folder.path);
                    }
                }
            }
        }
        if(strcmp(command, "mkdir")==0){
            if(path[strlen(path)-1]=='/' && strlen(path) != 1){
                path[strlen(path)-1] = '\0';
            }
            if(path[0] != '/'){
                strcpy(aux_path, cur_path);
                aux_size = strlen(aux_path);
                if(aux_size > 1){
                    aux_path[aux_size] = '/';
                    aux_path[aux_size+1] = '\0';
                }
                strcat(aux_path, path);
                i = strlen(aux_path);
                while(aux_path[i] != '/')
                    i--;
                aux_path[i] = '\0';
                if(create_dir(aux_path, &aux_path[i+1], cur_partition)==0)
                    printf("Error\n");
            }
            else{
                i = strlen(path);
                while(path[i]!='/')
                    i--;
                path[i] = '\0';
                if(strlen(path)==0){
                    if(create_dir("/", &path[i+1], cur_partition)==0)
                        printf("Error\n");

                }
                else{
                    if(create_dir(path, &path[i+1], cur_partition)==0)
                        printf("Error\n");
                }
            }
        }
        command_line[0] = '\0';
    }
    return 0;
}


/*
Exemplos de uso de algumas funcoes:

    unsigned int st_dev_id, cur_partition;
    st_dev_id = ins_st_dev("HD","Samsung",0,200000);
    cur_partition = create_partition(st_dev_id, "Fat32", 40000);
    if(cur_partition==-1)
        printf("Erro ao criar particao\n");

    cur_partition = create_partition(st_dev_id, "NTFS", 80000);
    if(cur_partition==-1)
        printf("Erro ao criar particao\n");

    cur_partition = create_partition(st_dev_id, "EXT4", 25000);

    if(cur_partition==-1)
        printf("Erro ao criar particao\n");
    strcpy(cur_path, "/");


    create_dir("/","home",cur_partition);
    create_dir("/","boot",cur_partition);
    create_dir("/","root",cur_partition);
    create_file("/",cur_partition,".history","clear ls cd ls", 14);
    create_file("/",cur_partition,".bash","Hello world",11);
    create_file("/",cur_partition,".bashh","Hello world",11);
    create_dir("/home","usuario",cur_partition);
    create_dir("/home","convidado",cur_partition);
    create_file("/home",cur_partition, "arquivo1","Este eh um arquivo de teste, se voce nao esta autorizado a le-lo feche-o imediatamente", 86);
    create_file("/home",cur_partition, "arquivo2","Outro arquivo", 13);
    create_file("/home",cur_partition, "arquivo3","Mais um arquivo", 15);
    create_dir("/home/convidado","temp_music",cur_partition);
    create_file("/home/convidado",cur_partition, "music_file", "Este eh um arquivo de musica", 28);
    create_file("/home/convidado",cur_partition, "video_file", "Este eh um arquivo de video", 27);
    create_dir("/home/convidado","temp_videos",cur_partition);
    create_dir("/home/usuario","Downloads",cur_partition);
    create_file("/home/usuario/Downloads",cur_partition,"file364","Este eh um arquivo baixado", 26);


    if(rm_dir("/home/usuario/Downloads",cur_partition))
        printf("Arquivo removido com sucesso\n");
    else
        printf("Erro ao remover o arquivo\n");


    if(rm_file("/home/convidado/video_file",cur_partition))
        printf("Arquivo removido com sucesso\n");
    else
        printf("Erro ao remover o arquivo\n");

    Partition part;
    if(get_first_partition(&part)!=NULL){
        printf("ID: %u\nMAX size: %u\nUsed: %u\nType: %s\n\n",part.id,part.max_size,part.used, part.fs_type);
        while(get_next_partition(&part)!=NULL){
            printf("ID: %u\nMAX size: %u\nUsed: %u\nType: %s\n\n",part.id,part.max_size,part.used, part.fs_type);
        }
    }

*/
