#define FUSE_USE_VERSION 26 //on utilise une version stable et classique de FUSE
#define _FILE_OFFSET_BITS 64 //pour supporter les fichiers de grande taille
#define MAX_FILES 100 //notre système pourra contenir 100 fichiers max
#define MAX_FILENAME 256 

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

//voici la carte d'identité de nos fichiers
typedef struct {
    int is_used;                 //0 si l'emplacement est libre, 1 s'il est occupé
    char name[MAX_FILENAME];     // Le chemin du fichier (ex: "/bonjour.txt")
    mode_t mode;                 //est-ce un dossier (S_IFDIR) ou un fichier (S_IFREG) ? + les permissions
    size_t size;                 //taille en octets
    char *content;               //pointeur vers le vrai contenu du fichier
} nano_inode;

//crée notre "disque dur" : un simple tableau de 100 inodes en mémoire (RAM)
static nano_inode inode_table[MAX_FILES];

//parcourt le tableau et renvoie l'index du fichier s'il existe, sinon -1
static int find_inode(const char *path) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (inode_table[i].is_used && strcmp(inode_table[i].name, path) == 0) {
            return i; // Fichier trouvé à la case 'i' !
        }
    }
    return -1; // Fichier introuvable
}

// essaye on défini notre texte globalement, contenu de fichier "bonjour.txt" que nous allons afficher quand on le lira
static const char *contenu_bonjour = "Bonjour Kun, futur etudiant en Master a la Sorbonne!\n";


//fonction getattr : elle dit à machine les caractéristiques d'un fichier ou d'un dossier quand on lui demande
static int nano_getattr(const char *path, struct stat *stbuf) {
    //on commence par tout mettre à zéro pour éviter les valeurs indésirables
    memset(stbuf, 0, sizeof(struct stat));

    stbuf->st_uid = getuid(); //propriétaire est l'utilisateur qui lance le programme (---> getuid())
    stbuf->st_gid = getgid(); //le groupe est le groupe de l'utilisateur
    //on cherche d'abord le fichier dans notre table d'inodes pour voir s'il existe
    int idx = find_inode(path); 
    if (idx == -1) {
        return -ENOENT; //le fichier n'existe pas, on retourne une erreur "No such file or directory"
    }
    //si on le trouve, on copie simplement les infos de notre Inode vers FUSE
    stbuf->st_mode = inode_table[idx].mode;
    stbuf->st_size = inode_table[idx].size;
    
    //petite subtilité pour les dossiers (nlink = 2) vs fichiers (nlink = 1)
    if (S_ISDIR(inode_table[idx].mode)) {
        stbuf->st_nlink = 2;
    } else {
        stbuf->st_nlink = 1;
    }
    
    return 0;
}

//liste TOUS les fichiers de notre table
static int nano_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                        off_t offset, struct fuse_file_info *fi) {
    // On n'accepte de lister que la racine "/"
    if (strcmp(path, "/") != 0)
        return -ENOENT;

    // Un dossier doit toujours contenir "." et ".."
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

//parcourt tout notre disque dur
    for (int i = 1; i < MAX_FILES; i++) { //commence à 1 pour ignorer "/"
        if (inode_table[i].is_used) {
            //inode_table[i].name vaut "/bonjour.txt"
            //le +1 permet d'ignorer le premier slash et d'afficher juste "bonjour.txt"
            filler(buf, inode_table[i].name + 1, NULL, 0); 
        }
    }

    return 0;
}

//gérer la lecture (cat), lit le contenu depuis l'inode
static int nano_read(const char *path, char *buf, size_t size, off_t offset,
                     struct fuse_file_info *fi) {
    
    int idx = find_inode(path);
    if (idx == -1) return -ENOENT;

    size_t len = inode_table[idx].size;

    if (offset >= len) return 0;
    if (offset + size > len) size = len - offset;

    // On lit les données depuis le pointeur 'content' de notre inode
    memcpy(buf, inode_table[idx].content + offset, size);
    
    return size;
}

//gérer la création de fichier (touch), crée une nouvelle entrée dans notre table d'inodes
static int nano_mknod(const char *path, mode_t mode, dev_t rdev) {
    //on vérifie d'abord que le fichier n'existe pas déjà
    if (find_inode(path) != -1) {
        return -EEXIST; // fichier existe déjà
    }

    //on cherche une place libre dans notre table d'inodes
    int free_idx = -1;
    //on parcourt la table pour trouver la première case libre
    for (int i = 0; i < MAX_FILES; i++) {
        if (inode_table[i].is_used == 0) {
            free_idx = i;
            break; // on trouve la première case libre et on s'arrête
        }
    }

    //si on n'a pas trouvé de place libre, on retourne une erreur d'espace insuffisant
    if (free_idx == -1) {
        return -ENOSPC;
    }

    //on remplit la nouvelle entrée d'inode avec les infos du fichier à créer, et on marque cette case comme utilisée
    inode_table[free_idx].is_used = 1;//on marque cette case comme utilisée
    strcpy(inode_table[free_idx].name, path); //on copie le chemin du fichier dans notre table
    
    inode_table[free_idx].mode = S_IFREG | mode; //on indique que c'est un fichier régulier et on ajoute les permissions demandées
    
    inode_table[free_idx].size = 0;
    inode_table[free_idx].content = NULL; //le contenu est vide au départ, on allouera de la mémoire quand on écrira dedans

    return 0; // 0 signifie "Succès" pour le noyau macOS
}

//préparer notre système de fichiers au démarrage
static void init_fs() {
    //vide tout le disque dur (on marque tout comme "libre")
    for (int i = 0; i < MAX_FILES; i++) {
        inode_table[i].is_used = 0;
    }

    //crée le dossier racine "/" dans l'Inode 0
    inode_table[0].is_used = 1;
    strcpy(inode_table[0].name, "/");
    inode_table[0].mode = S_IFDIR | 0755;
    inode_table[0].size = 0;
    inode_table[0].content = NULL;

    //crée notre fichier "/bonjour.txt" dans l'Inode 1
    inode_table[1].is_used = 1;
    strcpy(inode_table[1].name, "/bonjour.txt");
    inode_table[1].mode = S_IFREG | 0444;
    
    //alloue de la mémoire pour le texte (comme un vrai OS alloue des blocs)
    const char *texte = "Bonjour Kun, futur etudiant en Master a la Sorbonne!\n";
    inode_table[1].size = strlen(texte);
    inode_table[1].content = strdup(texte); // strdup copie la chaîne en l'allouant en mémoire
}

//reli nos fonctions à fuse pour qu'il puisse les appeler quand il en a besoin
static struct fuse_operations nano_oper = {
    .getattr    = nano_getattr, // fonction de récupération des attributs d'un fichier ou dossier
    .readdir    = nano_readdir, // fonction de lecture de dossier
    .read       = nano_read, // fonction de lecture
    .mknod      = nano_mknod, // fonction de création de fichier
};

//lance FUSE et lui donne le contrôle
int main(int argc, char *argv[]) {
    init_fs(); //prépare notre système de fichiers avant de le lancer
    return fuse_main(argc, argv, &nano_oper, NULL);
}