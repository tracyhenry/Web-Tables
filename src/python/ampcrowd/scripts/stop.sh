#!/bin/bash

# Kill remaining crowd_server python processes
ps auxww | grep crowd_server | grep -v grep | awk '{print $2}' | xargs kill
