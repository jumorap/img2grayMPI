- Platform supported Windows, Linux, Mac with nvidia GPU

# ¿Qué es?
El presente proyecto permite aplicar el filtro de blanco y negro por consola a imágenes con diferentes extensiones de archivo. El programa se encuentra paralelizado con MPI y permite al usuario seleccionar el número de nodos sobre los cuales trabajará el algoritmo

# ¿Cómo instalar?
**1.** Abra su consola y clone el repositorio
```
git clone https://github.com/jumorap/img2grayMPI
```

**2.** Cambie el directorio de trabajo actual a la carpeta donde se encuentra el repositorio
```
cd img2grayMPI
```

# ¿Cómo usarlo?
**1.** Compile el proyecto con GNU Compiler Collection (GCC) instalado en su sistema
- Opción 1:
```
make
```
- Opción 2:
```
mpicc main.c -o gray -lm
```

**2.** Ejecute el programa
```
./gray <input_image> <output_image>
```

**3.** (opcional) Puede correr los tests para verificar que el programa funciona correctamente

Se recomienda hacer uso de Linux o Git Bash en Windows para correr los tests

- Opción 1:
```
make test
```

- Opción 2:
```
sh test.sh
```
