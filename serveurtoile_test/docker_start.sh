#!/bin/bash

set -e # exit on error

HOSTNAME=$(hostname)

echo "Pruning docker images"

# Remove serveurtoile_nginx image
docker stop serveurtoile_nginx 2> /dev/null || true
docker rmi -f $(docker images -q serveurtoile_nginx) 2> /dev/null || true
docker system prune -af 2> /dev/null || true

# check if hostname contains '42paris.fr'
if [[ $HOSTNAME == *"42paris.fr"* ]]; then
    echo "Running on 42 machine"

    echo "Removing old docker image"
    rm -rf ~/goinfre/serveurtoile_docker
    mkdir -p ~/goinfre/serveurtoile_docker

    echo "Copying files to goinfre"
    cp -f Dockerfile ~/goinfre/serveurtoile_docker
    cp -r front ~/goinfre/serveurtoile_docker/front
    cp -f nginx.conf ~/goinfre/serveurtoile_docker/nginx.conf
    cp -f entrypoint.sh ~/goinfre/serveurtoile_docker/entrypoint.sh

    cd ~/goinfre/serveurtoile_docker/
fi

echo "Building docker image"
docker build -t serveurtoile_nginx .

echo "Starting docker container"

docker run -it --rm -d -p 8081:80 --name serveurtoile_nginx serveurtoile_nginx
echo "Container started, nginx instance accessible at : http://127.0.0.1:8081"
echo "Press enter to stop the container"
read
docker stop serveurtoile_nginx 2> /dev/null || true
