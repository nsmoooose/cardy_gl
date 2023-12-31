#!/bin/bash

# Windows is wierd. The exe file is locked for some reason. So the ldd command
# doesn't work unless I have a sleep here. Can it be the antivirus command that
# locks it?
# sleep 1.5

mapfile -t files < <(ldd $1 | grep -vi WINDOWS | grep -oP "(?<=\=\> ).*(?= \()")
for f in "${files[@]}"; do
    if [[ "$f" =~ "$(pwd)/$2" ]]; then
    	# Dependency is already present in the target directory.
	    continue
    fi
    cp -v "$f" "$2"
done
