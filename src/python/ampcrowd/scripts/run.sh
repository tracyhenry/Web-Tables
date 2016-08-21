#!/bin/bash

# Kill old processes
./scripts/stop.sh

# Print errors directly to console for easy debugging with -d
if [ "$1" == "-d" ] || [ "$2" == "-d" ]
then
    export DEVELOP=1
else
    export DEVELOP=0
fi

# Enable SSL
if [ "$1" == "-s" ] || [ "$2" == "-s" ]
then
    export SSL=1
else
    export SSL=0
fi

# Run the application
pushd ampcrowd > /dev/null
gunicorn -c ../deploy/gunicorn_config.py crowd_server.wsgi:application
popd > /dev/null
