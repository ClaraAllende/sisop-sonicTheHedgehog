#!/bin/bash

echo $1
echo $2

if [[ ! $1 =~ ^selfIp\=[[:digit:]]{1,3}\.[[:digit:]]{1,3}\.[[:digit:]]{1,3}\.[[:digit:]]{1,3}$ ]] || [[ ! $2 =~ ^orquestador\=[[:digit:]]{1,3}\.[[:digit:]]{1,3}\.[[:digit:]]{1,3}\.[[:digit:]]{1,3}\:[[:digit:]]{1,5}$ ]]; then
	echo "Uso: $0 selfIp=192.168.1.100 orquestador=192.168.1.101:9999"
	exit 1
fi

# Chequear si existe el archivo configs
if [ ! -f "configs.tar" ]; then
	echo "Error: no existe el archivo con las configuraciones (\"configs.tar\")."
	exit 1
fi

# Borrar la carpeta configs si existe.
if [ -d "configs" ]; then
	echo "Borrando la carpeta \"configs\"..."
	rm -rf configs
fi

# Extraer el contenido de configs.tar
echo "Extrayendo los archivos de configuración..."
tar -C . -xf configs.tar

for file in `find configs/ -name *.conf`; do
	echo "Procesando $file..."
	echo -e "\n$1\n$2" >> $file
done

echo "Terminado :)"