# NanoFS
## Description du projet : Système de fichiers (C)

NanoFS est un projet d'implémentation d'un système de fichiers en espace utilisateur (Filesystem in Userspace - FUSE) écrit en C. 
Ce projet a pour but d'explorer la programmation système bas niveau, la gestion des appels systèmes (VFS) et l'interaction avec le noyau du système d'exploitation.

---

## Fonctionnalités 
- **Point de montage virtuel** géré par FUSE
- **Architecture dynamique :** Table d'Inodes en RAM gére plusieurs fichiers simultanément
- **Métadonnées dynamiques :** Interception de getattr avec attribution de l'UID/GID de l'utilisateur courant pour respecter les permissions **POSIX**
- **Listing :** Implémentation de readdir pour explorer l'arborescence
- **Création de fichiers** Support de la commande **touch** avec recherche du premier Inode libre
- **Écriture et Allocation dynamique (write et truncate) :** Support de la redirection de flux
- Allocation et redimensionnement dynamiques de la mémoire via realloc et memcpy
- **Lecture (read) :** Support de la commande cat avec gestion précise des offsets.

---
## Installation et execution
### Prérequis (macOS Apple Silicon - M1/M2/M3/M4)
- Un système d'exploitation basé sur macOS
- GCC (GNU Compiler Collection)
- macFuse

**MacFuse :**
```bash
brew install --cask macfuse
```

### Compilation
```bash
gcc -Wall nanofs.c -D_FILE_OFFSET_BITS=64 -I/usr/local/include/osxfuse -L/usr/local/lib -lfuse -o nanofs
```

### Utilisation
```bash
mkdir point_de_montage
./nanofs point_de_montage
touch point_de_montage/secret.txt
echo "Ceci est un test d'ecriture dynamique en RAM !" > point_de_montage/secret.txt
ls -l point_de_montage
cat point_de_montage/secret.txt
# devrait afficher la chaîne de caractères gérée par le programme C
umount point_de_montage
#démonter le système de fichiers proprement
```

---
## Documentation
Voir [DEV_NOTES.md](./DEV_NOTES.md) pour les choix techniques et notes de développement.
