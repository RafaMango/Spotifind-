#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct 
{
  char id[100];
  List *artists; //artista
  char album_name[200]; //album nombre
  char track_name[200];
  float tempo; //tempo
  char track_genre[100]; // genero
}Song; //cancion

// Nueva estructura para listas de tempo
typedef struct 
{
    List *lentas;
    List *moderadas;
    List *rapidas;
} TempoLists;

// Menú principal
void mostrarMenuPrincipal() {
  limpiarPantalla();
  puts("========================================");
  puts("     Base de Datos de Spotifind");
  puts("========================================");

  puts("1) Cargar cancion");
  puts("2) Buscar por genero");
  puts("3) Buscar por artista");
  puts("4) Buscar por id");
  puts("5) Buscar por tempo");
  puts("6) Crear PlayList");
  puts("7) Agregar cancion");
  puts("8) Mostrar PlayList");
  puts("9) Salir");
}

void limpiarBuffer()
{
  int c;
  while ((c = getchar()) != '\n' && c != EOF); 
}

/*Función general para imprimir los datos de cada canción*/

void print_song(Song *song)
{
  printf("ID: %s\n", song->id);
  printf("Track: %s\n", song->track_name);
  printf("Album: %s\n", song->album_name);
  printf("Artistas: ");
  for(char *artist = list_first(song->artists); artist != NULL; artist = list_next(song->artists))
  {
    printf("%s",artist);
    if(list_next(song->artists) != NULL)printf(", ");
  }
  printf("\n");
  printf("Genero: %s\n", song->track_genre);
  printf("Tempo: %2.f BPM\n", song->tempo);
  printf("---------------------------------------\n");
}

/**
 * Compara dos claves de tipo string para determinar si son iguales.
 * Esta función se utiliza para inicializar mapas con claves de tipo string.
 *
 * @param key1 Primer puntero a la clave string.
 * @param key2 Segundo puntero a la clave string.
 * @return Retorna 1 si las claves son iguales, 0 de lo contrario.
 */
int is_equal_str(void *key1, void *key2) {
  return strcmp((char *)key1, (char *)key2) == 0;
}

/**
 * Compara dos claves de tipo entero para determinar si son iguales.
 * Esta función se utiliza para inicializar mapas con claves de tipo entero.
 *
 * @param key1 Primer puntero a la clave entera.
 * @param key2 Segundo puntero a la clave entera.
 * @return Retorna 1 si las claves son iguales, 0 de lo contrario.
 */
int is_equal_int(void *key1, void *key2) {
  return *(int *)key1 == *(int *)key2; // Compara valores enteros directamente
}

/**
  Carga canciones desde un archivo CSV y las almacena en los mapas y listas correspondientes.
  Almacena id, genero, artistas en mapas y las el tempo de las canciones en listas. 
 */
void cargar_canciones(Map *songs_by_id, Map *songs_by_genero, Map* songs_by_artist, TempoLists *tempos)
{
  FILE *archivo = fopen("song_dataset_.csv", "r");
  if (archivo == NULL) 
  {
    perror("Error al abrir el archivo");
    presioneTeclaParaContinuar();
    return;
  }

 
  // Lee la primera linea del archivo CSV(encabezado)
  char **cabezera = leer_linea_csv(archivo, ',');
  if(cabezera == NULL)
  {
    printf("Archivo vacio o fomato incorrecto.\n");
    fclose(archivo);
    presioneTeclaParaContinuar();
    return;
  }

  char **campos;
  campos = leer_linea_csv(archivo, ','); // Lee los encabezados del CSV
  if(!campos)
  {
    printf("Archivo vacio o formato incorrecto.\n");
    fclose(archivo);
    return;
  }
  // Lee cada línea del archivo CSV hasta el final
  int contador = 0;
  while((campos = leer_linea_csv(archivo, ',')) != NULL) 
  {
    //if(contador >= 100)break;
    // Crea una nueva estructura song y almacena los datos
    Song *song = (Song *)malloc(sizeof(Song));

    if(song == NULL)
    {
      printf("Error de memoria\n");
      presioneTeclaParaContinuar();
      continue;
    }
    strcpy(song->id, campos[0]);
    song->artists = split_string(campos[2], ";");
    strcpy(song->album_name, campos[3]);
    strcpy(song->track_name, campos[4]);
    song->tempo = atof(campos[18]);
    strcpy(song->track_genre, campos[20]);
    // Inserta la cancion en el mapa usando el ID como clave
    map_insert(songs_by_id, song->id, song);

    //insertar por genero
    MapPair *genre_pair = map_search(songs_by_genero, song->track_genre);
    if(genre_pair == NULL)
    {
      List* genre_list = list_create();
      list_pushBack(genre_list, song);
      map_insert(songs_by_genero, strdup(song->track_genre), genre_list);
    }
    else
    {
      List *genre_list = (List*)genre_pair->value;
      list_pushBack(genre_list, song);
    }
    //insertar por artista
    for(char *artist = list_first(song->artists); artist != NULL; artist = list_next(song->artists))
    {
      MapPair *artist_pair = map_search(songs_by_artist, artist);
      if(!artist_pair)
      {
        List* artist_list = list_create();
        list_pushBack(artist_list, song);
        map_insert(songs_by_artist, strdup(artist), artist_list);
      }
      else
      {
        List *artist_list = (List*)artist_pair->value;
        list_pushBack(artist_list, song);
      }
    }
    // Insertar en la lista de tempo adecuada
    if (song->tempo < 80)
      list_pushBack(tempos->lentas, song);
    else if (song->tempo <= 120)
      list_pushBack(tempos->moderadas, song);
    else
      list_pushBack(tempos->rapidas, song);

    contador++;
    // Imprimir los datos leídos para verificar
    printf("Cancion #%d cargada:\n", contador + 1);
  }
  printf("Se cargaron %d canciones.\n", contador);
  fclose(archivo); // Cierra el archivo después de leer todas las líneas
}
/**
 Busca y muestra la información de una canción por género en un mapa.
 Además de imprimir la información de cada canción, también cuenta cuántas canciones se encontraron.
 **/
void buscar_por_genero(Map *songs_by_genre) 
{
  char genero[100];

  printf("Ingrese el genero de la cancion: ");
  scanf("%s", genero);

  MapPair *pair = map_search(songs_by_genre, genero);
  int contador = 0;
  if (pair != NULL) 
  {
      List* lista = (List*)pair->value;
      Song *song= list_first(lista);
      
      while (song != NULL) 
      {
        print_song(song);
        song = list_next(lista);
        contador++;
      }
      printf("Se encontraron %d canciones del genero '%s'.\n", contador, genero);
      printf("=======================================\n");
  }
  else
  {
    printf("No se encontraron canciones del genero '%s'.\n", genero);
  }
}

/*
Esta funcion busca canciones por artista en un mapa y muestra la información de cada canción.
Además de imprimir la información de cada canción, también cuenta cuántas canciones se encontraron.
*/
void buscar_by_artista(Map *songs_by_artista) 
{
  char artista[100];

  printf("Ingrese el Artista de la cancion: ");
  if (fgets(artista, sizeof(artista), stdin) == NULL) {
    printf("Error al leer el artista.\n");
    return;
  }
  // Eliminar salto de línea al final
  size_t len = strlen(artista);
  if (len > 0 && artista[len-1] == '\n') 
  {
    artista[len-1] = '\0';
  }

  MapPair *pair = map_search(songs_by_artista, artista);
  int contador = 0;
  if (pair != NULL) 
  {
      List* lista = (List*)pair->value;
      printf("Canciones del artista '%s':\n", artista);
      printf("=======================================\n");

      Song *song= list_first(lista);
      
      while (song != NULL) 
      {
        print_song(song);
        song = list_next(lista);
        contador++;
      }
      printf("Se encontraron %d canciones del artista '%s'.\n", contador, artista);
      printf("=======================================\n");
  }
  else
  {
    printf("No se encontraron canciones del artista '%s'.\n", artista);
  }
}
/*
Esta función busca una canción por su ID en un mapa y muestra la información de la canción.
Si se encuentra la canción, se imprime su información; de lo contrario, se muestra un mensaje indicando que no se encontró.
*/
void buscar_por_id(Map *songs_byid)
 {
  char id[100];

  printf("Ingrese el ID de la cancion: ");
  scanf("%99s", id);

  MapPair *pair = map_search(songs_byid, id);
  
  if (pair != NULL) 
  {
      Song *song = (Song *)pair->value;
      print_song(song);
  }
  else
  {
      printf("No se encontro ninguna canción con el ID '%s'.\n", id);
  }
}
/*
Esta funcion busca canciones por tempo en una lista y muestra la información de cada canción.
Además de imprimir la información de cada canción, también cuenta cuántas canciones se encontraron.
*/
void buscar_por_tempo(TempoLists *tempos) {
    int opcion;
    printf("Buscar canciones por tempo:\n");
    printf("1) Lenta (<80 BPM)\n");
    printf("2) Moderada (80-120 BPM)\n");
    printf("3) Rápida (>120 BPM)\n");
    printf("Seleccione una opción: ");
    scanf("%d", &opcion);

    List *lista = NULL;
    if (opcion == 1) lista = tempos->lentas;
    else if (opcion == 2) lista = tempos->moderadas;
    else if (opcion == 3) lista = tempos->rapidas;
    else {
        printf("Opción inválida.\n");
        return;
    }

    Song *song = list_first(lista);
    if (!song) {
        printf("No hay canciones en esta categoría.\n");
        return;
    }
    int contador = 0;
    while (song) {
        print_song(song);
        song = list_next(lista);
        contador++;
    }
    printf("Se encontraron %d canciones en esta categoría.\n", contador);
    printf("=======================================\n");
    
}
/*
-Esta funcion crea una lista de reproducción y la almacena en un mapa.
-El nombre de la lista se solicita al usuario y se verifica si ya existe en el mapa.
-Si la lista ya existe, se muestra un mensaje de error. Si no existe, se crea una nueva lista y se almacena en el mapa.
-Si la lista se crea exitosamente, se muestra un mensaje de confirmación.
-Si la lista ya existe, se muestra un mensaje de error.
*/
void crear_lista_canciones(Map *listas_reproducciones)
{
  char nombre_lista[100];
  limpiarBuffer();
  printf("Ingrese el nombre de la Playlist: ");
  fgets(nombre_lista, 100, stdin);
  // Eliminar salto de línea
  nombre_lista[strcspn(nombre_lista, "\n")] = '\0';

  printf("Creando PlayList '%s'\n", nombre_lista);

  if (map_search(listas_reproducciones, nombre_lista) != NULL) {
    printf("La playlist '%s' ya existe.\n", nombre_lista);
    return;
  }

  List *nueva_lista = list_create();
  map_insert(listas_reproducciones, strdup(nombre_lista), nueva_lista);
  printf("Playlist '%s' creada exitosamente.\n", nombre_lista);
}
/*
Agregar una canción a una playlist existente. Primero solicita el ID de la canción y el nombre de la playlist, 
luego busca ambos en sus mapas correspondientes. Si encuentra la playlist y la canción, la agrega al final de la lista de reproducción
correspondiente. Si no encuentra alguno de los dos, muestra un mensaje de error.
*/
void agregar_a_lista(Map *listas_reproducciones, Map *songs_by_id) 
{
  char id[100], nombre_lista[100];

  limpiarBuffer();
  printf("Ingrese el ID de la cancion: ");
  fgets(id, 100, stdin);
  id[strcspn(id, "\n")] = '\0';

  printf("Ingrese el nombre de la playlist: ");
  fgets(nombre_lista, 100, stdin);
  // Eliminar salto de línea
  nombre_lista[strcspn(nombre_lista, "\n")] = '\0';

  printf("Buscando playlist '%s'\n", nombre_lista);

  MapPair *lista_pair = map_search(listas_reproducciones, nombre_lista);
  if (lista_pair == NULL) {
    printf("No se encontro la playlist '%s'.\n", nombre_lista);
    return;
  }

  MapPair *song_pair = map_search(songs_by_id, id);
  if (song_pair == NULL) {
    printf("No se encontro la cancion con ID '%s'.\n", id);
    return;
  }

  Song *song = (Song*)song_pair->value;
  List *lista = (List*)lista_pair->value;
  list_pushBack(lista, song);
  printf("Cancion '%s' agregada a la playlist '%s'.\n", song->track_name, nombre_lista);
}
/*
Muestra las canciones almacenadas en la listad e reproducciones la cual identifica por el nombre asignado
*/
void mostrar_lista_reproducciones(Map *listas_reproducciones)
{
  char nombre_lista[100];
  printf("Ingrese el nombre de la playlist: ");
  limpiarBuffer();
  fgets(nombre_lista, 100, stdin);
  nombre_lista[strcspn(nombre_lista, "\n")] = '\0';

  MapPair *pair = map_search(listas_reproducciones, nombre_lista);
  if (pair == NULL) {
    printf("No se encontro la playlist '%s'.\n", nombre_lista);
    return;
  }

  List *lista = (List *)pair->value;
  if (list_first(lista) == NULL) {
    printf("La playlist '%s' esta vacia.\n", nombre_lista);
    return;
  }

  printf("=== Canciones en la playlist '%s' ===\n", nombre_lista);
  Song *song = list_first(lista);
  while (song != NULL) {
    print_song(song);
    song = list_next(lista);
  }
  printf("=======================================\n");
  printf("Total de canciones en la playlist '%s': %d\n", nombre_lista, list_size(lista));
  printf("=======================================\n");
  printf("Presione cualquier tecla para continuar...\n");
  getchar();
  limpiarBuffer();
}


/*
Esta función libera la memoria de un mapa que contiene canciones, liberando tanto las claves como los valores asociados a cada entrada,
ademas de libera la memoria de las listas de artistas asociadas a cada canción.
*/
void liberar_mapa_id(Map* songs_byid)
{
  MapPair *pair;
  while(pair)
  {
    free(pair->key);
    Song *song = (Song*)pair->value;
    if(song != NULL)
    {
      char *artist = list_first(song->artists);
      while(artist != NULL)
      {
        free(artist);
        artist = list_next(song->artists);
      }
      list_clean(song->artists);
      free(song->artists);
      free(song);
    }
    pair = map_next(songs_byid);
  }
  map_clean(songs_byid);
  free(songs_byid);
}
/*
Esta función libera la memoria de un mapa que contiene listas de canciones, liberando tanto las claves como los valores asociados a 
cada entrada y libera la memoria de las listas de canciones asociadas a cada entrada.
*/
void liberar_mapa_de_listas(Map* map)
{
  MapPair *pair = map_first(map);
  while(pair)
  {
    free(pair->key);
    List *lista = (List*)pair->value;
    if(lista != NULL)
    {
      list_clean(lista);
      free(lista);
    }
    pair = map_next(map);
  }
  map_clean(map);
  free(map);
}
/*
Esta función libera la memoria de una lista y sus elementos.
*/
void liberar_lista(List *list)
{
  list_clean(list);
  free(list);
}

int main() 
{
  char opcion;

  // Crea un mapa para cada búsqueda
  Map *songs_byid = map_create(is_equal_str);
  Map *songs_by_genero = map_create(is_equal_str);
  Map *songs_by_artista = map_create(is_equal_str);
  Map *listas_reproducciones = map_create(is_equal_str);

  //estructura de listas de tempo
  TempoLists *tempos = malloc(sizeof(TempoLists));
  tempos->lentas = list_create();
  tempos->moderadas = list_create();
  tempos->rapidas = list_create();
  
  do 
  {
    mostrarMenuPrincipal();
    printf("Ingrese su opcion: ");
    opcion = getchar();
    while (getchar() != '\n');
    switch (opcion)
    {
    case '1':
      cargar_canciones(songs_byid, songs_by_genero, songs_by_artista, tempos);
      break;
    case '2':
      buscar_por_genero(songs_by_genero);
      break;
    case '3':
      buscar_by_artista(songs_by_artista);
      break;

    case '4':
      buscar_por_id(songs_byid);
      break;

    case '5':
      buscar_por_tempo(tempos);
      break;
    case '6':
       crear_lista_canciones(listas_reproducciones);
       break;
    case '7':
      agregar_a_lista(listas_reproducciones, songs_byid);
      break;
    case '8':
      mostrar_lista_reproducciones(listas_reproducciones);
      break;
    
    case '9':
      puts("Saliendo de Spotifind");
      break;
    default:
      puts("Opcion no valida. Por favor, intente de nuevo.");
    }
    presioneTeclaParaContinuar();

  } while (opcion != '9');
  // Libera la memoria de los mapas
    liberar_mapa_id(songs_byid);
    liberar_mapa_de_listas(songs_by_genero);
    liberar_mapa_de_listas(songs_by_artista);
    liberar_mapa_de_listas(listas_reproducciones);
  // Libera la memoria de las listas de tempo
    liberar_lista(tempos->lentas);
    liberar_lista(tempos->moderadas);
    liberar_lista(tempos->rapidas);
    free(tempos);
  // Libera la memoria de las listas de reproducción 
  return 0;
}