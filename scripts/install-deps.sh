#!/usr/bin/env sh
# Install this project's Linux build dependencies on this machine (Debian/Ubuntu).
set -eu
here=$(dirname "$0")
sudo apt-get update
sudo apt-get install -y $(cat "$here/packages.txt")
