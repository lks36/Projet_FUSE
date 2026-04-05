#ifndef PARAMS_H
#define PARAMS_H

#include <sys/types.h>
#include <sys/stat.h>

#define MAX_FILES 100
#define MAX_FILENAME 256
#define BLOCK_SIZE 1024       //chaque bloc de données fera 1 Ko
#define NUM_BLOCKS 1024       // On aura 1024 blocs (donc un disque d'environ 1 Mo)
#define MAGIC_NUMBER 0x12345678 //signature pour vérifier que c'est bien notre disque

//Superblock (qui sera tout au début du fichier .img)
typedef struct {
    unsigned int magic;       //valoir 0x12345678
    int max_inodes;
    int num_blocks;
    int block_size;
} superblock_t;

//Notre nouvel Inode (sans pointeur !!!!)
typedef struct {
    int is_used;
    char name[MAX_FILENAME];
    mode_t mode;
    size_t size;
    uid_t uid;
    gid_t gid;
    int first_block;          //index du premier bloc de données
} nano_inode;

#endif