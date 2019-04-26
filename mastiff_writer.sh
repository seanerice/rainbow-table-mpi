#!/bin/bash

IS_VPN=$(ping -c 1 mastiff.cs.rpi.edu | grep icmp* | wc -l)

# SSH Credentials
USERNAME='rices'
SSH_ADDR='mastiff.cs.rpi.edu'

# Sync directories
REMOTE_DIR="$USERNAME@$SSH_ADDR:final/"
LOCAL_DIR="/home/seanrice/repos/rainbow-table-mpi/"

# Do we have VPN connection?
if [ $IS_VPN = '1' ]
then
    echo "Connected to mastiff over VPN"

    # Sync files
    echo "Syncing files"
    scp -i /home/seanrice/.ssh/id_rsa -r $LOCAL_DIR/source/ $REMOTE_DIR
    scp -i /home/seanrice/.ssh/id_rsa $LOCAL_DIR/Makefile $REMOTE_DIR
    echo

    # Compile
    echo "Compiling"
    ssh $USERNAME@$SSH_ADDR "make all -C /home/parallel/spring-2019/rices/final"
    echo

    # Run
    echo "Run"
    ssh $USERNAME@$SSH_ADDR "/usr/local/mpich-3.2/bin/mpirun -np 1 /home/parallel/spring-2019/rices/final/writer"
    echo

    # Sync files
    echo "Syncing files"
    # scp -i /home/seanrice/.ssh/id_rsa $REMOTE_DIR/../heatmap_out.ibin $LOCAL_DIR/output_mastiff
    # scp -i /home/seanrice/.ssh/id_rsa $REMOTE_DIR/../uni_out.usbin $LOCAL_DIR/output_mastiff
    echo
fi
