# Notes de dev

## Réflexions
- construire un VFS (Virtual File System) en espace utilisateur 
- notre système de fichiers n'est qu'une illusion. Il ne stocke aucune donnée physique
- FUSE fonctionne comme un pont : il transforme les appels système *ls*, *stat* en appels de fonctions C de notre programme *nano_getattr*, *nano_readdir*

---

## Techinique
- *nano_getattr* : Intercepte les demandes de métadonnées. Renvoie *S_IFDIR* pour la racine / et *S_IFREG* pour notre fichier virtuel

- *nano_readdir* : Remplit le buffer du terminal avec les noms des fichiers virtuels.

---

## Contraintes
Le développement nécessite d'autoriser les extensions système tierces ("Benjamin Fleischer")
### Limites
- L'arborescence est figée, on ne peut ni créer de nouveaux fichiers *touch* échoue ni en supprimer *rm* échoue

- Le fichier *bonjour.txt* déclare une taille de 25 octets, mais il n'a actuellement aucun contenu réel associable

- Si on démonte le système *umount*, tout disparaît, car rien n'est écrit sur le vrai disque dur.

---

## Idées

---

## TODO
### Bugs
- Vérifier le comportement des permissions (`0444` vs `0777`) depuis le Finder macOS par rapport au terminal.

### Suites
la commande `cat`