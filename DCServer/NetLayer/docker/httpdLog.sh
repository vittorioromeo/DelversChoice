#!/bin/bash

sudo docker exec $(sudo docker ps -a -q) cat /var/log/httpd/error_log
