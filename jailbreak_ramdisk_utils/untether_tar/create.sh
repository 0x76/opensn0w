#!/bin/sh
export COPYFILE_DISABLE=true
export COPY_EXTENDED_ATTRIBUTES_DISABLE=true
pushd ../untether_binaries
tar cfv untether.tar *
gzip -9 untether.tar
popd
mv ../untether_binaries/untether.tar.gz .
