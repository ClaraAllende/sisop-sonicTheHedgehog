#!/bin/bash
echo "Comenzando..."

# Path a la libreria
basePath=`pwd`"/Librerias/"
pathApi=$basePath"api/"
lib=$basePath"commons/"
libColl=$lib"collections/"
koopa=`pwd`"/Koopa/"
personaje=`pwd`"/Personaje/src/"
plataforma=`pwd`"/Plataforma/src/"
nivel=`pwd`"/Nivel/src/"

# Compilar la shared library:
echo "##############################################"
echo "############### SHARED LIBRARY ###############"
echo "##############################################"
echo "Compilando..."
gcc -c -Wall -Werror -fpic "$lib"*.c "$libColl"*.c

echo "Linkeando..."
gcc -shared -o "$basePath"libcommons.so *.o

echo "Borrando archivos .o..."
rm *.o


echo "##############################################"
echo "########## SHARED LIBRARY (KOOPA) ############"
echo "##############################################"
echo "Compilando..."
gcc -c -Wall -Werror -fpic "$koopa"commons/*.c "$koopa"commons/collections/*.c

echo "Linkeando..."
gcc -shared -o "$koopa"libso-commons-library.so *.o

echo "Borrando archivos .o..."
rm *.o


# GUI
echo "##############################################"
echo "#################### GUI #####################"
echo "##############################################"
echo "Compilando..."
gcc -c -Wall -Werror -fpic "$lib"../gui/gui.c

echo "Linkeando..."
gcc -shared -o "$basePath"libnivel-gui.so gui.o

echo "Borrando archivos .o..."
rm *.o

# Compilar libmemoria (Koopa):
echo "##############################################"
echo "############# LIBMEMORIA (KOOPA) #############"
echo "##############################################"
echo "Compilando..."
gcc -c -Wall -Werror -fpic "$koopa"koopa.c

echo "Linkeando..."
gcc -shared -o "$koopa"libmemoria.so koopa.o -L "$koopa" -lso-commons-library

echo "Borrando archivos .o..."
rm *.o




# Compilar el personaje
echo "##############################################"
echo "################## PERSONAJE #################"
echo "##############################################"
echo "Creando links para compilar..."
ln -s "$lib" "$personaje"commons
ln -s "$pathApi" "$personaje"api
ln -s "$lib" "$personaje"../includes/commons
ln -s "$pathApi" "$personaje"../includes/api

echo "Compilando..."
gcc -c -Wall -Werror "$personaje"personaje.c "$personaje"crearPersonaje.c "$personaje"entrarEnNivel.c "$personaje"jugar.c "$personaje"terminarNivel.c "$personaje"signalListeners.c "$personaje"morir.c "$personaje"handleConnectionError.c

echo "Linkeando..."
gcc personaje.o crearPersonaje.o entrarEnNivel.o jugar.o terminarNivel.o signalListeners.o morir.o handleConnectionError.o -o "$personaje"personaje -L "$basePath" -lcommons -pthread

echo "Borrando archivos .o..."
rm *.o

echo "Borrando links para compilar..."
rm "$personaje"commons
rm "$personaje"api
rm "$personaje"../includes/commons
rm "$personaje"../includes/api


# Compilar Plataforna
echo "##############################################"
echo "################# PLATAFORMA #################"
echo "##############################################"
echo "Creando links para compilar..."
ln -s "$lib" "$plataforma"commons
ln -s "$pathApi" "$plataforma"api
ln -s "$lib" "$plataforma"hilos/commons
ln -s "$pathApi" "$plataforma"hilos/api

echo "Compilando..."
gcc -c -Wall -Werror "$plataforma"plataforma.c "$plataforma"hilos/orquestador.c "$plataforma"hilos/planificador.c

echo "Linkeando..."
gcc plataforma.o planificador.o orquestador.o -o "$plataforma"plataforma -L "$basePath" -lcommons -pthread

echo "Borrando archivos .o..."
rm *.o

echo "Borrando links para compilar..."
rm "$plataforma"commons
rm "$plataforma"api
rm "$plataforma"hilos/commons
rm "$plataforma"hilos/api


# Compilar el nivel
echo "##############################################"
echo "#################### NIVEL ###################"
echo "##############################################"
echo "Creando links para compilar..."
ln -s "$lib" "$nivel"commons
ln -s "$pathApi" "$nivel"api
ln -s "$lib" "$nivel"../includes/commons
ln -s "$pathApi" "$nivel"../includes/api
ln -s "$basePath"gui "$nivel"gui
ln -s "$basePath"gui "$nivel"../includes/gui

echo "Compilando..."
gcc -c -Wall -Werror "$nivel"Nivel.c "$nivel"Configuration.c "$nivel"pantalla.c "$nivel"Deadlock.c

echo "Linkeando..."
gcc Nivel.o Configuration.o pantalla.o Deadlock.o -o "$nivel"nivel -L "$basePath" -lcommons -lnivel-gui -lncurses -pthread

echo "Borrando archivos .o..."
rm *.o

echo "Borrando links para compilar..."
rm "$nivel"commons
rm "$nivel"api
rm "$nivel"gui
rm "$nivel"../includes/commons
rm "$nivel"../includes/api
rm "$nivel"../includes/gui


echo "Seteando variables de entorno..."
export LD_LIBRARY_PATH=$basePath:$koopa

echo "Finalizado."
