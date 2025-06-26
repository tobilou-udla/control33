#ifndef RESTAURANTE_H
#define RESTAURANTE_H

#include <time.h>

// Enumeraciones para estados y tipos
typedef enum {
    RECIBIDO = 1,
    EN_PREPARACION = 2,
    LISTO = 3,
    ENTREGADO = 4,
    CANCELADO = 5
} EstadoPedido;

typedef enum {
    PRESENCIAL = 1,
    ONLINE = 2
} TipoPedido;

typedef enum {
    BAJA = 1,
    MEDIA = 2,
    ALTA = 3
} Prioridad;

// Estructura para items del menú
struct ItemMenu {
    int id;
    char nombre[50];
    float precio;
    int tiempoPreparacion; // en minutos
};

// Estructura para items de un pedido
struct ItemPedido {
    int idItem;
    char nombreItem[50];
    int cantidad;
    float precioUnitario;
    int tiempoPreparacion;
};

// Estructura principal del pedido
struct Pedido {
    int id;
    TipoPedido tipo;
    EstadoPedido estado;
    Prioridad prioridad;
    time_t horaCreacion;
    time_t horaEstimadaEntrega;
    time_t horaEntrega;
    char clienteNombre[50];
    char clienteContacto[50];
    struct ItemPedido items[10]; // máximo 10 items por pedido
    int numItems;
    float total;
    int tiempoEstimado; // en minutos
};

// Estructura para la cola de pedidos
struct ColaPedidos {
    struct Pedido pedidos[100]; // máximo 100 pedidos activos
    int numPedidos;
    int siguienteId;
};

// Funciones principales
void inicializarSistema();
int menuPrincipal();
void crearPedidoPresencial();
void crearPedidoOnline();
void mostrarPedidosActivos();
void actualizarEstadoPedido();
void mostrarEstadisticas();
void procesarSiguientePedido();
void cancelarPedido();

// Funciones auxiliares
void leerCadena(char *cadena, int tam);
void mostrarMenu();
int calcularTiempoEstimado(struct ItemPedido items[], int numItems);
void calcularPrioridad(struct Pedido *pedido);
void ordenarPedidosPorPrioridad();
void guardarPedidos();
int cargarPedidos();
void mostrarPedido(struct Pedido pedido);
char* estadoToString(EstadoPedido estado);
char* tipoToString(TipoPedido tipo);
void limpiarBuffer();

#endif
