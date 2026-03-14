# NanoFS
## Description du projet : Système de fichiers (C)

NanoFS est un projet d'implémentation d'un système de fichiers en espace utilisateur (Filesystem in Userspace - FUSE) écrit en C. 
Ce projet a pour but d'explorer la programmation système bas niveau, la gestion des appels systèmes (VFS) et l'interaction avec le noyau du système d'exploitation.

---

## Fonctionnalités 
- Point de montage virtuel
- Interception des appels système `getattr` (métadonnées) et `readdir` (listing)
- Simulation de la présence d'un fichier en lecture seule (`bonjour.txt`).

---
## Installation et execution
### Prérequis (macOS Apple Silicon - M1/M2/M3/M4)
- Un système d'exploitation basé sur macOS
- GCC (GNU Compiler Collection)

*puis*
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
ls -l point_de_montage
#devrait afficher le fichier virtuel bonjour.txt
umount point_de_montage
#démonter le système de fichiers proprement
```

---
## Documentation
Voir [DEV_NOTES.md](./DEV_NOTES.md) pour les choix techniques et notes de développement.
