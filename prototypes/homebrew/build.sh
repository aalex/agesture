#!/bin/bash
g++ -o run main.cpp nearestneighborfinder.cpp `pkg-config --libs --cflags glib-2.0`

