#!/bin/bash

myDir=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
root=$(realpath $myDir/../../www/)
sudo docker run -t -i -v $root:/srv/http --name dcserver_instance -p 81:80 -p 443:443 -d superv1234/dcserver /bin/bash -c "cd '/usr'; sudo /usr/bin/mysqld_safe --datadir='/var/lib/mysql'& sudo apachectl -DFOREGROUND"