# NanoFS
## Description du projet : Système de fichiers (C)

NanoFS est un projet d'implémentation d'un système de fichiers en espace utilisateur (Filesystem in Userspace - FUSE) écrit en C. 
Ce projet a pour but d'explorer la programmation système bas niveau, la gestion des appels systèmes (VFS) et l'interaction avec le noyau du système d'exploitation.

---

## Fonctionnalités 
- **Gestion d'un disque binaire persistant**
- **Lecture du Superblock et de la table d'Inodes**
- **Résolution de chemins dynamique sur disque**
- **Création et écriture de fichiers (En cours)**

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
gcc -Wall mkfs_nano.c -o mkfs_nano
gcc -Wall nanofs.c -D_FILE_OFFSET_BITS=64 -I/usr/local/include/osxfuse -L/usr/local/lib -lfuse -o nanofs
```

### Utilisation
```bash
./mkfs_nano
mkdir point_de_montage
./nanofs point_de_montage
```

---
## Documentation
Voir [DEV_NOTES.md](./DEV_NOTES.md) pour les choix techniques et notes de développement.
