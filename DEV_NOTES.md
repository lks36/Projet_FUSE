# Notes de dev

## Réflexions
- construire un VFS (Virtual File System) en espace utilisateur 
- notre système de fichiers n'est qu'une illusion. Il ne stocke aucune donnée physique
- FUSE fonctionne comme un pont : il transforme les appels système (`ls`, `stat`, `cat`) en appels de fonctions C de notre programme (`nano_getattr`, `nano_readdir`, `nano_read`).
---

## Technique (Ce qui fonctionne)
- *nano_getattr* : Intercepte les demandes de métadonnées. Renvoie *S_IFDIR* pour la racine / et *S_IFREG* pour notre fichier virtuel

- *nano_readdir* : Remplit le buffer du terminal avec les noms des fichiers virtuels.

- **nano_read** : Implémentation réussie. Permet à la commande **cat** de lire le contenu du fichier **bonjour.txt** en gérant correctement les offsets et la taille du buffer.
---

## Contraintes
Le développement nécessite d'autoriser les extensions système tierces ("Benjamin Fleischer")
### Limites
- Le développement nécessite d'autoriser les extensions système tierces ("Benjamin Fleischer") sur macOS ARM64.

- L'arborescence est figée, on ne peut ni créer de nouveaux fichiers *touch* échoue ni en supprimer *rm* échoue

- Si on démonte le système *umount*, tout disparaît, car rien n'est écrit sur le vrai disque dur.

- L'arborescence est figée (Hardcodée) : on ne peut ni créer de nouveaux fichiers **touch** échoue ni en supprimer **rm** échoue.

- Pas de persistance : Si on démonte le système **umount**, tout disparaît, car rien n'est écrit sur le vrai disque dur.

---

## Idées

---

## TODO
### Bugs
- Vérifier le comportement des permissions **0444** vs **0777** depuis le Finder macOS par rapport au terminal.

### Suites
- on va essayer de passer d'une architecture statique à dynamique
- Créer une structure **struct inode** en C pour représenter la fiche d'identité générique de n'importe quel fichier, au lieu d'écrire son nom en dur dans le code.