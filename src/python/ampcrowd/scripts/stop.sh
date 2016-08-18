#!/bin/bash

# Kill remaining crowd_server python processes
ps auxww | grep 'ampcrowd' | grep 'python' | grep -v 'grep' | grep -v 'SCREEN' | awk '{print $2}' | xargs kill
