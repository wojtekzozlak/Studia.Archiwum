#!/bin/bash

sudo mkdir /home/public
sudo useradd admin
sudo groupadd G1
sudo groupadd G2

sudo setfacl -m u:admin:rwx /home/public
sudo setfacl -m -d u:admin:rwx /home/public
sudo setfacl -m -d g:G1:r-x /home/public
sudo setfacl -m g:G1:r-x /home/public
sudo setfacl -m -d g:G2:rwx /home/public
sudo setfacl -m g:G2:r-x /home/public
sudo setfacl -m o::--- /home/public
