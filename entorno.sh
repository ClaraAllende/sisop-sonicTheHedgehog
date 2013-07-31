#!/bin/bash
basePath=`pwd`"/Librerias/"
koopa=`pwd`"/Koopa/"
echo "Seteando variables de entorno..."
export LD_LIBRARY_PATH=$basePath:$koopa
