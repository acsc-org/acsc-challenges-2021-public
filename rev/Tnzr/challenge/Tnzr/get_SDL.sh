#!/bin/bash

wget https://www.libsdl.org/release/SDL2-devel-2.0.16-VC.zip
wget https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-devel-2.0.15-VC.zip
wget https://www.libsdl.org/projects/SDL_image/release/SDL2_image-devel-2.0.5-VC.zip

for file in *.zip; do
    unzip $file 
done
