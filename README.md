<div align="center">

# 🎵 EchoStream V2.0

### Plataforma de streaming musical desarrollada en C++

[![C++](https://img.shields.io/badge/C%2B%2B-17-blue?style=for-the-badge&logo=c%2B%2B)](https://isocpp.org/)
[![Visual Studio](https://img.shields.io/badge/Visual%20Studio-2022-purple?style=for-the-badge&logo=visualstudio)](https://visualstudio.microsoft.com/)
[![Estado](https://img.shields.io/badge/Estado-Finalizado-brightgreen?style=for-the-badge)]()
[![Curso](https://img.shields.io/badge/Curso-Estructuras%20de%20Datos%20Avanzadas-orange?style=for-the-badge)]()
[![UPC](https://img.shields.io/badge/Universidad-UPC-red?style=for-the-badge)]()

</div>

---

## 📌 Descripción general

**EchoStream V2.0** es una aplicación de consola desarrollada en **C++** para el curso de **Estructuras de Datos Avanzadas**.  
El proyecto simula una plataforma de streaming musical inspirada en Spotify, permitiendo gestionar:

- 👤 Usuarios
- 🎵 Canciones
- 📂 Playlists
- ❤️ Favoritos
- 🕘 Historial de reproducción
- ▶️ Cola de canciones
- 🤖 Recomendaciones musicales
- 🤝 Relaciones de amistad entre usuarios

> El objetivo principal del proyecto es aplicar estructuras de datos avanzadas y algoritmos de ordenamiento en un caso práctico y funcional.

---

## 🎓 Información académica

- **Curso:** Estructuras de Datos Avanzadas - `1ACC0264`
- **Universidad:** Universidad Peruana de Ciencias Aplicadas (UPC)
- **Tipo de trabajo:** Trabajo Final

---

## 👥 Integrantes

- **Gerardo Morales**
- **Ilder Ojeda**

---

## ✨ Funcionalidades principales

✅ Registro e inicio de sesión de usuarios  
✅ Carga de canciones desde `biblioteca.txt`  
✅ Visualización del catálogo musical  
✅ Creación y gestión de playlists  
✅ Agregado de canciones a favoritos  
✅ Manejo de cola de reproducción  
✅ Registro de historial de canciones escuchadas  
✅ Reproducción anterior mediante pila  
✅ Modo aleatorio usando Fisher-Yates Shuffle  
✅ Recomendaciones musicales según historial  
✅ Búsqueda de canciones por ID usando árbol AVL  
✅ Búsqueda de playlists mediante tabla hash  
✅ Relaciones de amistad entre usuarios usando grafos  
✅ Cálculo de afinidad musical mediante distancia euclidiana  

---

## 🧠 Tecnologías y conceptos aplicados

<div align="center">

| Programación | Estructuras | Algoritmos | Persistencia |
|---|---|---|---|
| Programación Orientada a Objetos | Listas enlazadas | Insertion Sort | Archivos `.txt` |
| Templates | Pila | Shell Sort | Carga de dataset |
| C++ | Cola | Merge Sort | Guardado de historial |
| Visual Studio | Tabla Hash | Quick Sort | Usuarios y playlists |
|  | Árbol AVL | Heap Sort |  |
|  | Grafos | Fisher-Yates Shuffle |  |

</div>

---

## 🗂️ Estructuras de datos utilizadas

| Estructura | Uso en el proyecto |
|---|---|
| `ListaDoble<T>` | Almacena usuarios, favoritos, playlists y colecciones generales. |
| `ListaCircularDoble<T>` | Guarda canciones dentro de una playlist, permitiendo recorrido continuo. |
| `Pila<T>` | Maneja el historial de reproducción del usuario. |
| `Cola<T>` | Maneja la cola de canciones por reproducir. |
| `TablaHashPlaylist` | Permite buscar playlists por nombre de forma eficiente. |
| `AVL<T>` | Organiza el catálogo musical para buscar canciones por ID. |
| `CGrafo<T>` | Representa amistades entre usuarios y afinidad musical. |
| `Heap` | Se usa para ordenar recomendaciones según menor distancia euclidiana. |

---

## ⚙️ Algoritmos implementados

| Algoritmo | Aplicación |
|---|---|
| **Insertion Sort** | Ordena canciones alfabéticamente por nombre. |
| **Shell Sort** | Ordena canciones por número de reproducciones. |
| **Merge Sort** | Ordena canciones por duración. |
| **Quick Sort** | Ordena favoritos por nombre. |
| **Heap Sort** | Ordena recomendaciones musicales. |
| **Fisher-Yates Shuffle** | Mezcla canciones en modo aleatorio. |

> Para aplicar algunos algoritmos, las estructuras como listas o colas se convierten temporalmente a `vector`.  
> Esto permite ordenar o mezclar los datos sin modificar directamente la estructura principal.

---

## 🏗️ Arquitectura del proyecto

```text
UI.h
└── Interfaz de consola, menús, login, registro y navegación.

Gestores.h
└── Lógica principal del sistema. Coordina usuarios, biblioteca,
    reproducción, recomendaciones, historial, amistades y persistencia.

Algoritmos.h
└── Algoritmos de ordenamiento y mezcla reutilizables.

Entidades.h
└── Clases principales del dominio: Cancion, Usuario, Playlist, Podcast, etc.

EstructurasDatos.h
└── Implementación de listas, pila, cola, AVL, grafo y tabla hash.

Utilidades.h
└── Funciones auxiliares para nombres de artistas, álbumes, géneros y manejo de datos.
🧾 Archivos de datos

## 🧾 Archivos de datos

El sistema utiliza archivos de texto para conservar información entre ejecuciones.

| Archivo | Descripción |
|---|---|
| `biblioteca.txt` | Contiene el catálogo musical del sistema. |
| `usuarios.txt` | Contiene usuarios, suscripciones, playlists, favoritos y amistades. |
| `historial.txt` | Contiene el historial de reproducción de los usuarios. |

### Formato de `biblioteca.txt`

```text
CANCION,id,nombre,artistaId,albumId,duracion,generoId,reproducciones
```

**Ejemplo:**

```text
CANCION,1,Master of Puppets,14,141,515,10,25000
```

### Formato de `historial.txt`

```text
HIST,idUsuario,idContenido,nombre,tipo,artista,fecha
```

**Ejemplo:**

```text
HIST,99,999,Smells Like Teen Spirit,Cancion,Nirvana,2026-07-08 20:10:01
```

---

## 🤖 Sistema de recomendaciones

El sistema de recomendaciones se basa en el historial de reproducción del usuario.

### Proceso:

1. Se calcula un **perfil musical** del usuario.
2. Se consideran características como:
   - duración promedio
   - género
   - artista
   - álbum
3. Cada canción del catálogo se compara con ese perfil mediante **distancia euclidiana**.
4. Finalmente, las recomendaciones se ordenan usando **Heap Sort**.

> Mientras menor sea la distancia, mayor será la similitud con los gustos del usuario.

---

## 🤝 Grafo de usuarios

EchoStream también incluye una funcionalidad social basada en grafos.

- Cada usuario representa un **vértice**.
- Cada amistad representa una **arista**.
- El peso de la arista representa la **afinidad musical**.

La afinidad se calcula comparando los vectores de preferencias musicales mediante **distancia euclidiana**.

---

## 🚀 Cómo ejecutar el proyecto

1. Abrir el proyecto en **Visual Studio**.
2. Verificar que los archivos `.txt` estén dentro de la carpeta del proyecto:
   - `biblioteca.txt`
   - `usuarios.txt`
   - `historial.txt`
3. Compilar en modo `Debug` o `Release`.
4. Ejecutar el archivo principal:

```text
EchoStreamV2.cpp
```

5. Usar el menú de consola para registrarse, iniciar sesión y probar las funcionalidades.

---

## 📋 Requisitos

- Visual Studio 2022 o superior.
- Compilador compatible con C++.
- Sistema operativo Windows.
- Archivos `.txt` cargados correctamente en la carpeta del proyecto.

---

## 📈 Complejidades principales

| Función | Complejidad |
|---|---|
| Insertar al final en ListaDoble | `O(1)` |
| Encolar canción | `O(1)` |
| Push en historial | `O(1)` |
| Buscar playlist en Hash | `O(1)` promedio / `O(n)` peor caso |
| Buscar canción en AVL | `O(log n)` |
| Fisher-Yates Shuffle | `O(n)` |
| Insertion Sort | `O(n²)` |
| Shell Sort | `O(n²)` peor caso |
| Merge Sort | `O(n log n)` |
| Heap Sort | `O(n log n)` |

---

## 📌 Estado del proyecto

<div align="center">

### ✅ Proyecto finalizado  
### 📚 Entrega del curso de Estructuras de Datos Avanzadas

</div>

---

## 📄 Licencia

Este proyecto fue desarrollado con **fines académicos**.

---

<div align="center">

✨ *Gracias por visitar este repositorio* ✨

</div>
