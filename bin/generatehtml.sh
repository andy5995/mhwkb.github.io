#!/bin/sh

LANG=EN

cd $HOME/src/mhwkb && \
./mhwkb Knowledge_Base/$LANG tmp && \
cp templates/mhwkb_style.css tmp

exit 0;
