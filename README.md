# SOA_TheShooter
En este programa codificado en C, he diseñado y implementado una aplicación capaz de diferenciar y manipular volúmenes formateados en FAT16 o EXT2.

La primera de las funcionalidades es mostrar toda la información relacionada con nuestro sistema de archivos. Según si es un tipo de sistema mostraremos unos datos u otros.

La segunda funcionalidad es de búsqueda de archivos o directorios. Dado un sistema de ficheros y un nombre de archivo a buscar, hay que informar al usuario si éste existe o no en nuestro sistema.

La tercera y última funcionalidad es la de eliminar un elemento del sistema de archivos deseado. Se recibirá tanto el sistema como el archivo a eliminar y lo eliminaremos si lo encontramos.

Hay que decir que nuestro programa siempre será capaz de identificar el tipo de sistema de ficheros sin que lo tenga que indicar el usuario.

Además, para pasar los parámetros al programa seguiremos el siguiente formato:

./shooter <operacion> <dir_volumen> [<archivo>]

Como se puede observar existe un campo operación que variará entre /info, /find y /delete según la operación que se quiera realizar.

Por otra parte en el campo de "dir_volum" introduciremos la dirección al volumen donde se quiere buscar la información.

El campo fichero sólo se tendrá que introducir en caso de que se quiera hacer una búsqueda o eliminar un elemento de nuestro sistema de archivos.
