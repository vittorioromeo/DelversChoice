#!/bin/bash

mkdir -p ./db
sudo chown 89:89 -R ./db

docker run -v ./db:/var/lib/mysql superv1234/dcserver /opt/create-mysql-structure.sh