#define FUSE_USE_VERSION 26 // On utilise une version stable et classique de FUSE
#define _FILE_OFFSET_BITS 64 // Pour supporter les fichiers de grande taille

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>


// on défini notre texte globalement, contenu de fichier "bonjour.txt" que nous allons afficher quand on le lira
static const char *contenu_bonjour = "Bonjour Kun, futur etudiant en Master a la Sorbonne!\n";


//fonction getattr : elle dit à machine les caractéristiques d'un fichier ou d'un dossier quand on lui demande
static int nano_getattr(const char *path, struct stat *stbuf) {
    //on commence par tout mettre à zéro pour éviter les valeurs indésirables
    memset(stbuf, 0, sizeof(struct stat));
    //si on demande la racine du dossier ("/")
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755; //c'est un dossier (Directory)
        stbuf->st_nlink = 2;
        return 0;
    }
    //si on demande notre faux fichier "/bonjour.txt"
    else if (strcmp(path, "/bonjour.txt") == 0) {
        stbuf->st_mode = S_IFREG | 0444; //c'est un fichier normal (Regular), en lecture seule
        stbuf->st_nlink = 1;
        stbuf->st_size = strlen(contenu_bonjour); //la taille de la phrase qu'il contiendra plus tard
        return 0;
    }
    return -ENOENT; //fichier non trouvé
}

//fonction readdir liste les fichiers et dossiers présents dans un dossier
static int nano_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                        off_t offset, struct fuse_file_info *fi) {
    
    //accepte de lister que la racine "/"
    if (strcmp(path, "/") != 0)
        return -ENOENT;

    //rempli le dossier avec les fichiers de base et notre faux fichier
    filler(buf, ".", NULL, 0);//dossier actuel
    filler(buf, "..", NULL, 0); //dossier parent
    filler(buf, "bonjour.txt", NULL, 0); //faux fichier !

    return 0;
}

//gérer la lecture (cat)
static int nano_read(const char *path, char *buf, size_t size, off_t offset,
                     struct fuse_file_info *fi) {
    
    // On vérifie qu'on essaie bien de lire NOTRE fichier
    if (strcmp(path, "/bonjour.txt") != 0)
        return -ENOENT;

    size_t len = strlen(contenu_bonjour);

    // Si on demande à lire au-delà de la fin du fichier, on renvoie 0 (Fin de fichier / EOF)
    if (offset >= len) {
        return 0;
    }

    // Si l'OS demande plus d'octets qu'il n'en reste, on réduit la taille demandée
    if (offset + size > len) {
        size = len - offset;
    }

    // On copie les octets depuis notre chaîne de caractères vers le buffer de FUSE
    memcpy(buf, contenu_bonjour + offset, size);
    
    return size; // FUSE a besoin de savoir combien d'octets on a réellement lus
}

//reli nos fonctions à fuse pour qu'il puisse les appeler quand il en a besoin
static struct fuse_operations nano_oper = {
    .getattr    = nano_getattr, // fonction de récupération des attributs d'un fichier ou dossier
    .readdir    = nano_readdir, // fonction de lecture de dossier
    .read       = nano_read, // fonction de lecture
};

//lance FUSE et lui donne le contrôle
int main(int argc, char *argv[]) {
    return fuse_main(argc, argv, &nano_oper, NULL);
}