#!/bin/bash

# Kill remaining crowd_server python processes
ps auxww | grep gunicorn | grep guest | grep -v grep | awk '{print $2}' | xargs kill -9
