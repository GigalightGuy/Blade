#!/bin/bash

echo "
██████  ██       █████  ██████  ███████     ██████  ██████   ██████       ██ ███████  ██████ ████████ 
██   ██ ██      ██   ██ ██   ██ ██          ██   ██ ██   ██ ██    ██      ██ ██      ██         ██    
██████  ██      ███████ ██   ██ █████       ██████  ██████  ██    ██      ██ █████   ██         ██    
██   ██ ██      ██   ██ ██   ██ ██          ██      ██   ██ ██    ██ ██   ██ ██      ██         ██    
██████  ███████ ██   ██ ██████  ███████     ██      ██   ██  ██████   █████  ███████  ██████    ██    
                                                                                                      
                                                                                                      
 ██████  ███████ ███    ██ ███████ ██████   █████  ████████  ██████  ██████                           
██       ██      ████   ██ ██      ██   ██ ██   ██    ██    ██    ██ ██   ██                          
██   ███ █████   ██ ██  ██ █████   ██████  ███████    ██    ██    ██ ██████                           
██    ██ ██      ██  ██ ██ ██      ██   ██ ██   ██    ██    ██    ██ ██   ██                          
 ██████  ███████ ██   ████ ███████ ██   ██ ██   ██    ██     ██████  ██   ██                          "

echo '

PROJECTS MENU

(1) - Empty Game Project  
(2) - Example Game Project
(0) - EXIT                    

'

read -p "Choose an option: " option

ProjectName=""

function MakeDefaultDirectories()
{
    mkdir $ProjectName
    mkdir -p $ProjectName/assets/sprites $ProjectName/assets/shaders $ProjectName/src/Components $ProjectName/src/Systems $ProjectName/lib
}

function LoadDefaultFiles()
{
    touch $ProjectName/assets/shaders/default.vert
    touch $ProjectName/assets/shaders/default.frag

    touch $ProjectName/src/$ProjectName.cpp
    touch $ProjectName/src/$ProjectName.hpp

    touch $ProjectName/CMakeLists.txt
}

if [ $option == '0' ]; 
	then
		echo "Exiting Generator"
		exit 0
	else
        if [ $option == '1' ];
            then
                read -p "Project Name: " ProjectName
                MakeDefaultDirectories
                LoadDefaultFiles
                echo "Generating Empty Game Project"
		        exit 0
            else
                echo "Generating Example Game Project"
		        exit 0
            fi
fi