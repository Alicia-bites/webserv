# 🔋 Tests complets

Cette branche contient l'entièreté des tests de serveurtoile.

# 📦 Préparation des tests

## 🐋 Lancer l'instance nginx

```bash
./docker_start.sh
```

## 🕸️ Lancer l'instance serveurtoile
```
./webserv
```

## Installer les dépendances

### Ubuntu

```bash
sudo apt install python3 python3-pip
pip3 install -r requirements.txt
```

# 🧪 Lancer les tests

## Préparation

```bash
pytest -m prereq # Vérifie que les deux instances sont bien lancées
```

## Exécution

```bash
pytest # Lance tout les tests
```


# 🐛 Débugger

## `test_nginx.py` échoue

Il faut vérifier que le conteneur nginx est bien lancé.

```bash
docker ps
```

## `test_webserv.py` échoue

Il faut vérifier que le serveurtoile est bien lancé.

```bash
ps aux | grep webserv
```
