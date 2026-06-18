#!/usr/bin/env sh
# Download this project's Linux build dependencies as .deb files (Debian/Ubuntu)
# so an offline machine can install them. Copy the output dir to the target and:
#     sudo dpkg -i debs/*.deb
# Run this on a clean/matching box so the .deb set is complete. Output: ./debs
set -eu
here=$(dirname "$0")
out="debs"
mkdir -p "$out"
sudo apt-get install -y --download-only --reinstall $(cat "$here/packages.txt")
cp /var/cache/apt/archives/*.deb "$out"/
echo "Downloaded $(ls "$out"/*.deb | wc -l) .deb files into $out/"
