# 🎵 Spotifind – Sistema de Gestión de Canciones

**Spotifind** es una aplicación desarrollada en C que permite gestionar canciones y playlists de manera eficiente. Pensado como un sistema similar a un mini Spotify, permite al usuario buscar, filtrar y organizar canciones usando estructuras de datos como listas y mapas.

---

## 📚 Características de una canción

Cada canción contiene:

- Un ID único
- Nombre de la canción
- Nombre del artista
- Género musical
- Tempo (velocidad en BPM)
- Popularidad (opcional)
  
Las canciones pueden buscarse por distintos criterios y añadirse a múltiples playlists personalizadas por el usuario.

---

## 🚀 Instalación y ejecución

Para compilar el proyecto, abre una terminal en la raíz del proyecto y ejecuta:

```bash
gcc tdas/*.c spotifind.c -Wno-unused-result -o spotifind
```

Luego, para ejecutar el programa:

```bash
./spotifind
```

---

## 🧠 Funcionalidades

### 📥 Cargar canciones
- Carga una base de datos de canciones desde un archivo CSV.

### 🔍 Buscar canciones
- **Por nombre**
- **Por ID**
- **Por artista**
- **Por género**
- **Por tempo** (con rango)

### 🎶 Playlists
- Crear playlists personalizadas.
- Agregar canciones a una playlist.
- Mostrar contenido de una playlist.
- Eliminar canciones de una playlist.

### 📊 Orden y filtrado
- Las canciones pueden listarse según diferentes criterios (ID, nombre, etc.).
- Las playlists mantienen el orden de inserción de las canciones.

---

## 📦 Uso de TDAs

El sistema utiliza:

- **Listas Doblemente Enlazadas**: para las playlists, permitiendo inserciones y eliminaciones eficientes.
- **Mapas Hash Abiertos**: para búsquedas rápidas por ID, nombre, artista y género.
- **Arreglos**: para manipulación temporal y ordenamiento.

Estas estructuras permiten un manejo flexible, eficiente y escalable de la información musical.

---

## 📋 Ejemplo de uso

```
=== SPOTIFIND ===
1. Cargar canciones desde CSV
2. Buscar canción por nombre
3. Buscar canción por artista
4. Crear playlist
5. Agregar canción a playlist
6. Mostrar playlist
7. Eliminar canción de playlist
8. Salir
Opción:
```

**Opción 2:** Buscar canción por nombre  
```
Ingrese el nombre de la canción: Blinding Lights
Resultado:
- Blinding Lights - The Weeknd (ID: 10123, Género: Synthpop, Tempo: 171 BPM)
```

**Opción 5:** Agregar canción a playlist  
```
Ingrese ID de la canción: 10123
Ingrese nombre de la playlist: Favoritas
Canción agregada correctamente.
```

---

## ⚠️ Funcionalidades no implementadas o limitaciones

- No se controla la duplicación de canciones dentro de una playlist.
- Búsqueda exacta; no se admiten búsquedas parciales o insensibles a mayúsculas/minúsculas.
- Programa demora en la carga del archivo CSV.

---

## 🛠️ Contribución individual

Este proyecto fue desarrollado por Rafaela Bascuñán y Alex Reyes como parte de una tarea académica. 

Rafaela Bascuñán se encargó de:

- Programar las funciones para cargar las canciones desde el archivo CSV.
- Implementar las funciones de búsqueda por artista, género e ID.
- Diseñar y desarrollar la estructura TempoList para clasificar las canciones según su tempo (lentas, moderadas y rápidas).

Alex Reyes:

- Diseñar la lógica de gestión de canciones, playlists y liberar memoria de los mapas y listas.
- Implementar estructuras de datos de mapa para id, genero, artista y lisat de reproducciones.
- Programar las funciones de crear lista, agregar a la lista y mostrar lista.
- Programar funciones para liberar memoria.

---

## 📄 Licencia

Este proyecto es parte de una tarea universitaria. Su uso está limitado a fines educativos y no debe ser distribuido con fines comerciales.