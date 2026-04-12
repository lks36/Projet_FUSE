# Notes de dev - Phase : Persistance

## Réflexions
- construire un VFS (Virtual File System) en espace utilisateur 
- transition réussie vers une **architecture dynamique basée sur des Inodes**.
- système de fichiers est désormais capable d'allouer de la mémoire dynamiquement pour stocker le contenu des fichiers créés par l'utilisateur. Nous avons un véritable **RAM Disk** fonctionnel
- on a maintenant le cycle complet d'un fichier (Création -> Écriture -> Lecture -> Suppression)

---

## Technique (Ce qui fonctionne)
- **Structure nano_inode :** Création de la "carte d'identité" des fichiers (état d'occupation, nom, permissions, taille, pointeur vers le contenu).

- **nano_getattr** : Intercepte les demandes de métadonnées. Renvoie *S_IFDIR* pour la racine / et *S_IFREG* pour notre fichier virtuel

- **nano_readdir** : Remplit le buffer du terminal avec les noms des fichiers virtuels.

- **nano_read** : Implémentation réussie. Permet à la commande **cat** de lire le contenu du fichier **bonjour.txt** en gérant correctement les offsets et la taille du buffer.

- **truncate** : Ajuste dynamiquement la taille allouée en mémoire via realloc

- **write** : Écrit les données de l'espace utilisateur vers la mémoire RAM allouée à l'Inode

- **unlink** : Suppression logique (Inode) et physique (RAM). Libération du pointeur *content* et remise à zéro de **is_used**.
---

## Contraintes
Le développement nécessite d'autoriser les extensions système tierces ("Benjamin Fleischer")

### Limites
- Le développement nécessite d'autoriser les extensions système tierces ("Benjamin Fleischer") sur macOS ARM64

- Si on démonte le système *umount*, tout disparaît

- Si on quitte le programme brutalement, la RAM allouée par **strdup** et **realloc** n'est pas libérée proprement

- Les fichiers affichent toujours la date du 1er janvier 1970 car les champs `st_atime` et `st_mtime` ne sont pas encore mis à jour lors des écritures

- On ne peut créer que des fichiers à la racine, pas de mkdir

- **Formatage requis** : Toute modification des structures dans `params.h` nécessite de relancer `./mkfs_nano`

- **Accès séquentiel** : Le parcours de la table d'inodes est en O(N). Pour 100 fichiers c'est invisible, mais ce serait une limite pour un système plus vaste

---

## TODO
### Bugs
- Vérifier le comportement des permissions **0444** vs **0777** depuis le Finder macOS par rapport au terminal
- Mémoire : il faut assurer qu'il n'y a pas de fuites mémoire lors de la création/destruction des inodes à l'avenir

### Suites
- **Réécrire `mknod`** : cherche un Inode libre sur le disque et y écrive les nouvelles métadonnées
- **Réécrire `write`** : calculer l'offset du bloc de données (`BLOCK_SIZE * index`) pour y écrire les octets
- **Gestion du temps** : Ajouter l'horodatage réel.