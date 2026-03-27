# Notes de dev

## Réflexions
- construire un VFS (Virtual File System) en espace utilisateur 
- transition réussie vers une **architecture dynamique basée sur des Inodes**.
- système de fichiers est désormais capable d'allouer de la mémoire dynamiquement pour stocker le contenu des fichiers créés par l'utilisateur. Nous avons un véritable **RAM Disk** fonctionnel

---

## Technique (Ce qui fonctionne)
- **Structure nano_inode :** Création de la "carte d'identité" des fichiers (état d'occupation, nom, permissions, taille, pointeur vers le contenu).

- *nano_getattr* : Intercepte les demandes de métadonnées. Renvoie *S_IFDIR* pour la racine / et *S_IFREG* pour notre fichier virtuel

- *nano_readdir* : Remplit le buffer du terminal avec les noms des fichiers virtuels.

- **nano_read** : Implémentation réussie. Permet à la commande **cat** de lire le contenu du fichier **bonjour.txt** en gérant correctement les offsets et la taille du buffer.

- **truncate** :** Ajuste dynamiquement la taille allouée en mémoire via realloc

- *write* :** Écrit les données de l'espace utilisateur vers la mémoire RAM allouée à l'Inode
---

## Contraintes
Le développement nécessite d'autoriser les extensions système tierces ("Benjamin Fleischer")

### Limites
- Le développement nécessite d'autoriser les extensions système tierces ("Benjamin Fleischer") sur macOS ARM64

- Si on démonte le système *umount*, tout disparaît

- Si on quitte le programme brutalement, la RAM allouée par **strdup** et **realloc** n'est pas libérée proprement

---

## Idées

---

## TODO
### Bugs
- Vérifier le comportement des permissions **0444** vs **0777** depuis le Finder macOS par rapport au terminal
- Mémoire : il faut assurer qu'il n'y a pas de fuites mémoire lors de la création/destruction des inodes à l'avenir

### Suites
- README doit mettre à jour !!!!!!!
- l'écriture de données (commande echo "texte" > fichier.txt)