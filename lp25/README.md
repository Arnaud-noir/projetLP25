# Gestionnaire de processus multi-hôtes (style htop)

Projet en C pour Linux offrant une interface ncurses permettant d'afficher et d'interagir avec les processus locaux et distants (SSH/Telnet) via des onglets.

## Fonctionnalités
- Affichage dynamique de la liste des processus (`ps -eo pid,user,pcpu,pmem,etime,comm`).
- Onglets par machine (local + distants).
- Recherche par chaîne sur la commande.
- Actions interactives:
  - F5: Pause (SIGSTOP)
  - F6: Arrêt (SIGTERM)
  - F7: Tuer (SIGKILL)
  - F8: Redémarrer (kill + relance de la commande)
- Options de ligne de commande conformes au sujet.
- Lecture d'un fichier de configuration distant caché et en mode `600`.
- `--dry-run` pour tester l'accès sans affichage.

## Dépendances
- Linux avec `gcc` et `make`.
- Bibliothèque `ncurses` (paquet `libncurses-dev` sur Debian/Ubuntu, `ncurses-devel` sur Fedora/RHEL).
- Client `ssh` installé pour les connexions SSH.
- Client `telnet` (optionnel, implémentation basique).

## Compilation
```bash
make
```

## Utilisation
Affichage local par défaut:
```bash
./procmanager
```

Aide:
```bash
./procmanager --help
```

Tester l'accès sans afficher:
```bash
./procmanager --dry-run
```

Utiliser un fichier de configuration distant (caché et mode 600):
```bash
# Exemple de fichier .config
# nom:adresse:port:username:password:type
srv1:server1.example.com:22:alice:motdepasse:ssh
srv2:192.168.1.10:23:bob:secret:telnet

chmod 600 .config
./procmanager -c .config -a
```

Spécifier une machine distante directement:
```bash
./procmanager -s server2.example.com -u alice -t ssh
# Mot de passe demandé si non fourni
```
Ou via `--login`:
```bash
./procmanager --login bob@192.168.1.20 --connexion-type telnet
```

## Sécurité du fichier de config
Le programme alerte si le fichier n'est pas caché (préfixe `.`) ou si ses droits ne sont pas `600`. Vous pouvez forcer malgré tout l'exécution (avertissement).

## Notes sur Telnet
L'implémentation Telnet est simplifiée et dépend fortement de la configuration du serveur distant. Elle peut échouer si le serveur utilise des bannières/étapes d'authentification non standards. SSH est recommandé.

## Conformité aux conventions
- `snake_case` pour les noms.
- Espaces autour des opérateurs.
- Position des accolades conforme.
- Doxygen dans les entêtes des fonctions importantes.

## Contrôle de fuites mémoire
Le projet minimise les allocations et libère la mémoire. Vous pouvez vérifier avec `valgrind`:
```bash
valgrind --leak-check=full ./procmanager --dry-run
```

## Licence
Code fourni à des fins pédagogiques.
