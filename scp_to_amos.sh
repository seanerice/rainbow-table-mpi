#!/bin/bash

IS_VPN=$(ping -c 1 lp01.ccni.rpi.edu | grep icmp* | wc -l)

# SSH Credentials
USERNAME='PCP8rcsn'
SSH_ADDR='lp01.ccni.rpi.edu'

# Sync directories
REMOTE_DIR="$USERNAME@$SSH_ADDR:scratch/MPGoL/"
LOCAL_DIR="/home/seanrice/repos/MPGoL/"

# Do we have VPN connection?
if [ $IS_VPN = '1' ]
then
    echo "Connected to mastiff over VPN"

    # Sync files
    echo "Syncing files"
    scp -i /home/seanrice/.ssh/id_rsa $LOCAL_DIR/assignment4-5.c $REMOTE_DIR
    scp -i /home/seanrice/.ssh/id_rsa $LOCAL_DIR/run_n64.sh $REMOTE_DIR
    echo
fi
