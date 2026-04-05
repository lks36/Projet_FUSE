#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "params.h"

int main() {
    const char *disk_name = "disque.img";
    
    //crée le fichier binaire (wb = write binary)
    FILE *disk = fopen(disk_name, "wb");
    if (!disk) {
        perror("Erreur de creation du disque");
        return 1;
    }

    printf("1. Formatage du disque virtuel '%s'...\n", disk_name);

    //écrit le Superblock
    superblock_t sb;
    sb.magic = MAGIC_NUMBER;
    sb.max_inodes = MAX_FILES;
    sb.num_blocks = NUM_BLOCKS;
    sb.block_size = BLOCK_SIZE;
    fwrite(&sb, sizeof(superblock_t), 1, disk);

    //sdprépare la table des Inodes
    nano_inode inodes[MAX_FILES];
    memset(inodes, 0, sizeof(inodes)); // On met tout à zéro

    //on configure l'Inode 0 (la racine "/")
    inodes[0].is_used = 1;
    strcpy(inodes[0].name, "/");
    inodes[0].mode = S_IFDIR | 0755;
    inodes[0].uid = getuid();
    inodes[0].gid = getgid();
    inodes[0].size = 0;
    inodes[0].first_block = -1; // les dossiers n'ont pas de blocs de données pour l'instant

    // On écrit le tableau des inodes sur le disque (juste après le superblock)
    fwrite(inodes, sizeof(nano_inode), MAX_FILES, disk);

    //On remplit le reste du disque (les blocs de données) avec des zéros
    char empty_block[BLOCK_SIZE];
    memset(empty_block, 0, BLOCK_SIZE);
    
    printf("2. Allocation de l'espace de donnees (1 Mo)...\n");
    for (int i = 0; i < NUM_BLOCKS; i++) {
        fwrite(empty_block, BLOCK_SIZE, 1, disk);
    }

    fclose(disk);
    printf("Succes ! Le disque virtuel '%s' est pret a etre utilise.\n", disk_name);
    return 0;
}