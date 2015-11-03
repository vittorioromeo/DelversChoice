#!/bin/bash

> script.sql

cd ./parts && for x in *.sql; do cat $x >> ../script.sql; echo -e "\n\n\n" >> ../script.sql; done