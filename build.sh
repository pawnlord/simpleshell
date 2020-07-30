#!/bin/bash

gcc src/* -o simpleshell -Iinc -Llib -lconfigreader

cp -i config/default.cfg $HOME/.config/simpleshell.cfg

