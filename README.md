# Proyecto 1 - Los Quessoo

### Autores

- Cristóbal Chauriye, 16635116
- Paolo Fabia, 16635698
- Fernanda Mansilla, 14631652
- Antonio Mora, 16635469

### Decisiones de Diseño

El programa se diseñó utilizando el lenguaje C que permite un manejo de bajo nivel de la información y memoria. Para el trabajo y lectura del disco, se crearon 5 structs diferentes:

1. Bloque directorio
2. Entrada directorio
3. Bloque índice
4. Bloque de datos
5. Bloque de direccionamiento indirecto
6. Bitmap

Cada uno de estos contiene atributos que facilitan el trabajo con el acceso a la memoria del disco, como arreglos de caracteres un un determinado número de bites ```` unsigned int entries[64]; ```` o bien atributos para identiicar el tipo de archivo ```` int valid;````, ````char name[29]````. Estos se pueden encontrar en el archivo ````dir_block.h````.

El programa se encuentra modularizado en los siguientes archivos:

- /API
  - `os_API.c `: contiene las funciones requeridas en el enunciado.
  - `os_API.h` : contiene la declaración de funciones.
  
- /blocks
  - `dir_block.c` : contiene las funciones que inicializan los structs y otras que apoyan el manejo de bloques de directorio.
  - `dir_block.h`: contiene la declaración de funciones de dir_block.c

- /functions
  - `functions.c`: son funciones que facilitan y simplifican el código, modularizando ciertas acciones reiterativas.
  - `functions.h`: contiene la declaración de funciones.
  
Durante el desarrollo del proyecto, se utilizaron variables decalrativas, todas en inglés y buscamos empaquetar funcionalidaes específicas que facilitan su reutilización es distintas funciones.

### Supuestos

- Para el uso de `os_ls`, suponemos que el enunciado se refiere a mostrar todos los archivos dentro de un directorio, no así otros directorios. (esto es modificable de manera sencilla, pero preferimos ser explñicitos con el enunciado y dejarlo como supuesto).
- Con respecto los bloques de bitmap, los bloques libres y ocupados fueron calculados con respecto al disco en su totalidad.
