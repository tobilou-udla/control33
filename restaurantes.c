#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "restaurante.h"

// Variables globales
struct ColaPedidos cola;
struct ItemMenu menu[] = {
    {1, "Hamburguesa Clasica", 8.50, 8},
    {2, "Hamburguesa Deluxe", 12.00, 12},
    {3, "Pizza Margherita", 10.00, 15},
    {4, "Pizza Pepperoni", 12.50, 15},
    {5, "Papas Fritas", 3.50, 5},
    {6, "Nuggets de Pollo", 6.00, 7},
    {7, "Ensalada Caesar", 7.50, 5},
    {8, "Refresco", 2.00, 1},
    {9, "Agua", 1.50, 1},
    {10, "Café", 2.50, 3}
};
int numItemsMenu = 10;

void inicializarSistema() {
    cola.numPedidos = 0;
    cola.siguienteId = 1;
    
    // Intentar cargar pedidos existentes
    if (!cargarPedidos()) {
        printf("Iniciando sistema nuevo - no se encontraron pedidos previos.\n");
    } else {
        printf("Sistema cargado - %d pedidos activos encontrados.\n", cola.numPedidos);
    }
}

int menuPrincipal() {
    int opcion;
    printf("\n=== SISTEMA DE GESTIÓN DE PEDIDOS ===\n");
    printf("1. Crear Pedido Presencial\n");
    printf("2. Crear Pedido Online\n");
    printf("3. Ver Pedidos Activos\n");
    printf("4. Procesar Siguiente Pedido\n");
    printf("5. Actualizar Estado de Pedido\n");
    printf("6. Cancelar Pedido\n");
    printf("7. Ver Estadísticas\n");
    printf("8. Ver Menú\n");
    printf("9. Salir\n");
    printf("Seleccione una opción: ");
    scanf("%d", &opcion);
    limpiarBuffer();
    return opcion;
}

void mostrarMenu() {
    printf("\n=== MENÚ DEL RESTAURANTE ===\n");
    printf("ID\tNombre\t\t\tPrecio\tTiempo (min)\n");
    printf("---------------------------------------------------\n");
    for (int i = 0; i < numItemsMenu; i++) {
        printf("%d\t%-20s\t$%.2f\t%d\n", 
               menu[i].id, menu[i].nombre, 
               menu[i].precio, menu[i].tiempoPreparacion);
    }
    printf("---------------------------------------------------\n");
}

void crearPedidoPresencial() {
    if (cola.numPedidos >= 100) {
        printf("Error: Cola de pedidos llena.\n");
        return;
    }
    
    struct Pedido nuevoPedido;
    nuevoPedido.id = cola.siguienteId++;
    nuevoPedido.tipo = PRESENCIAL;
    nuevoPedido.estado = RECIBIDO;
    nuevoPedido.horaCreacion = time(NULL);
    nuevoPedido.numItems = 0;
    nuevoPedido.total = 0.0;
    
    printf("\n=== NUEVO PEDIDO PRESENCIAL ===\n");
    printf("ID del pedido: %d\n", nuevoPedido.id);
    
    printf("Nombre del cliente: ");
    leerCadena(nuevoPedido.clienteNombre, sizeof(nuevoPedido.clienteNombre));
    
    strcpy(nuevoPedido.clienteContacto, "Mesa-" );
    char mesa[10];
    printf("Número de mesa: ");
    leerCadena(mesa, sizeof(mesa));
    strcat(nuevoPedido.clienteContacto, mesa);
    
    mostrarMenu();
    
    int continuar = 1;
    while (continuar && nuevoPedido.numItems < 10) {
        int idItem, cantidad;
        printf("\nIngrese ID del item (0 para terminar): ");
        scanf("%d", &idItem);
        limpiarBuffer();
        
        if (idItem == 0) {
            continuar = 0;
            continue;
        }
        
        // Buscar item en el menú
        int encontrado = 0;
        for (int i = 0; i < numItemsMenu; i++) {
            if (menu[i].id == idItem) {
                printf("Cantidad de %s: ", menu[i].nombre);
                scanf("%d", &cantidad);
                limpiarBuffer();
                
                if (cantidad > 0) {
                    // Agregar item al pedido
                    int idx = nuevoPedido.numItems;
                    nuevoPedido.items[idx].idItem = idItem;
                    strcpy(nuevoPedido.items[idx].nombreItem, menu[i].nombre);
                    nuevoPedido.items[idx].cantidad = cantidad;
                    nuevoPedido.items[idx].precioUnitario = menu[i].precio;
                    nuevoPedido.items[idx].tiempoPreparacion = menu[i].tiempoPreparacion;
                    
                    nuevoPedido.total += menu[i].precio * cantidad;
                    nuevoPedido.numItems++;
                    
                    printf("Agregado: %d x %s ($%.2f c/u)\n", 
                           cantidad, menu[i].nombre, menu[i].precio);
                }
                encontrado = 1;
                break;
            }
        }
        
        if (!encontrado) {
            printf("Item no encontrado en el menú.\n");
        }
    }
    
    if (nuevoPedido.numItems == 0) {
        printf("No se puede crear un pedido sin items.\n");
        return;
    }
    
    // Calcular tiempo estimado y prioridad
    nuevoPedido.tiempoEstimado = calcularTiempoEstimado(nuevoPedido.items, nuevoPedido.numItems);
    calcularPrioridad(&nuevoPedido);
    nuevoPedido.horaEstimadaEntrega = nuevoPedido.horaCreacion + (nuevoPedido.tiempoEstimado * 60);
    
    // Agregar a la cola
    cola.pedidos[cola.numPedidos] = nuevoPedido;
    cola.numPedidos++;
    
    // Ordenar por prioridad
    ordenarPedidosPorPrioridad();
    
    // Guardar en archivo
    guardarPedidos();
    
    printf("\n*** PEDIDO CREADO EXITOSAMENTE ***\n");
    printf("ID: %d\n", nuevoPedido.id);
    printf("Total: $%.2f\n", nuevoPedido.total);
    printf("Tiempo estimado: %d minutos\n", nuevoPedido.tiempoEstimado);
    printf("Mesa: %s\n", nuevoPedido.clienteContacto);
}

void crearPedidoOnline() {
    if (cola.numPedidos >= 100) {
        printf("Error: Cola de pedidos llena.\n");
        return;
    }
    
    struct Pedido nuevoPedido;
    nuevoPedido.id = cola.siguienteId++;
    nuevoPedido.tipo = ONLINE;
    nuevoPedido.estado = RECIBIDO;
    nuevoPedido.horaCreacion = time(NULL);
    nuevoPedido.numItems = 0;
    nuevoPedido.total = 0.0;
    
    printf("\n=== NUEVO PEDIDO ONLINE ===\n");
    printf("ID del pedido: %d\n", nuevoPedido.id);
    
    printf("Nombre del cliente: ");
    leerCadena(nuevoPedido.clienteNombre, sizeof(nuevoPedido.clienteNombre));
    
    printf("Teléfono/Email: ");
    leerCadena(nuevoPedido.clienteContacto, sizeof(nuevoPedido.clienteContacto));
    
    mostrarMenu();
    
    int continuar = 1;
    while (continuar && nuevoPedido.numItems < 10) {
        int idItem, cantidad;
        printf("\nIngrese ID del item (0 para terminar): ");
        scanf("%d", &idItem);
        limpiarBuffer();
        
        if (idItem == 0) {
            continuar = 0;
            continue;
        }
        
        // Buscar item en el menú
        int encontrado = 0;
        for (int i = 0; i < numItemsMenu; i++) {
            if (menu[i].id == idItem) {
                printf("Cantidad de %s: ", menu[i].nombre);
                scanf("%d", &cantidad);
                limpiarBuffer();
                
                if (cantidad > 0) {
                    // Agregar item al pedido
                    int idx = nuevoPedido.numItems;
                    nuevoPedido.items[idx].idItem = idItem;
                    strcpy(nuevoPedido.items[idx].nombreItem, menu[i].nombre);
                    nuevoPedido.items[idx].cantidad = cantidad;
                    nuevoPedido.items[idx].precioUnitario = menu[i].precio;
                    nuevoPedido.items[idx].tiempoPreparacion = menu[i].tiempoPreparacion;
                    
                    nuevoPedido.total += menu[i].precio * cantidad;
                    nuevoPedido.numItems++;
                    
                    printf("Agregado: %d x %s ($%.2f c/u)\n", 
                           cantidad, menu[i].nombre, menu[i].precio);
                }
                encontrado = 1;
                break;
            }
        }
        
        if (!encontrado) {
            printf("Item no encontrado en el menú.\n");
        }
    }
    
    if (nuevoPedido.numItems == 0) {
        printf("No se puede crear un pedido sin items.\n");
        return;
    }
    
    // Calcular tiempo estimado y prioridad (online tiene menos prioridad base)
    nuevoPedido.tiempoEstimado = calcularTiempoEstimado(nuevoPedido.items, nuevoPedido.numItems);
    calcularPrioridad(&nuevoPedido);
    // Pedidos online tienen 5 minutos adicionales por delivery
    nuevoPedido.horaEstimadaEntrega = nuevoPedido.horaCreacion + ((nuevoPedido.tiempoEstimado + 5) * 60);
    
    // Agregar a la cola
    cola.pedidos[cola.numPedidos] = nuevoPedido;
    cola.numPedidos++;
    
    // Ordenar por prioridad
    ordenarPedidosPorPrioridad();
    
    // Guardar en archivo
    guardarPedidos();
    
    printf("\n*** PEDIDO ONLINE CREADO EXITOSAMENTE ***\n");
    printf("ID: %d\n", nuevoPedido.id);
    printf("Total: $%.2f\n", nuevoPedido.total);
    printf("Tiempo estimado: %d minutos (incluye delivery)\n", nuevoPedido.tiempoEstimado + 5);
    printf("Contacto: %s\n", nuevoPedido.clienteContacto);
}

void mostrarPedidosActivos() {
    printf("\n=== PEDIDOS ACTIVOS (Ordenados por prioridad) ===\n");
    
    if (cola.numPedidos == 0) {
        printf("No hay pedidos activos.\n");
        return;
    }
    
    printf("Pri\tID\tTipo\t\tCliente\t\tEstado\t\tTotal\tItems\tTiempo Est.\n");
    printf("---------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < cola.numPedidos; i++) {
        struct Pedido p = cola.pedidos[i];
        
        if (p.estado != ENTREGADO && p.estado != CANCELADO) {
            char prioridadStr[10];
            switch(p.prioridad) {
                case ALTA: strcpy(prioridadStr, "ALTA"); break;
                case MEDIA: strcpy(prioridadStr, "MEDIA"); break;
                case BAJA: strcpy(prioridadStr, "BAJA"); break;
            }
            
            printf("%s\t%d\t%-12s\t%-15s\t%-12s\t$%.2f\t%d\t%d min\n",
                   prioridadStr, p.id, tipoToString(p.tipo), p.clienteNombre,
                   estadoToString(p.estado), p.total, p.numItems, p.tiempoEstimado);
        }
    }
    printf("---------------------------------------------------------------------------------\n");
}

void procesarSiguientePedido() {
    printf("\n=== PROCESAR SIGUIENTE PEDIDO ===\n");
    
    // Buscar el primer pedido con estado RECIBIDO
    int indice = -1;
    for (int i = 0; i < cola.numPedidos; i++) {
        if (cola.pedidos[i].estado == RECIBIDO) {
            indice = i;
            break;
        }
    }
    
    if (indice == -1) {
        printf("No hay pedidos pendientes por procesar.\n");
        return;
    }
    
    // Cambiar estado a EN_PREPARACION
    cola.pedidos[indice].estado = EN_PREPARACION;
    
    printf("*** INICIANDO PREPARACIÓN ***\n");
    mostrarPedido(cola.pedidos[indice]);
    
    guardarPedidos();
    printf("\nPedido ID %d ahora está EN PREPARACIÓN.\n", cola.pedidos[indice].id);
}

void actualizarEstadoPedido() {
    printf("\n=== ACTUALIZAR ESTADO DE PEDIDO ===\n");
    
    int idPedido;
    printf("Ingrese ID del pedido: ");
    scanf("%d", &idPedido);
    limpiarBuffer();
    
    // Buscar pedido
    int indice = -1;
    for (int i = 0; i < cola.numPedidos; i++) {
        if (cola.pedidos[i].id == idPedido) {
            indice = i;
            break;
        }
    }
    
    if (indice == -1) {
        printf("Pedido no encontrado.\n");
        return;
    }
    
    struct Pedido *p = &cola.pedidos[indice];
    
    printf("Pedido actual:\n");
    mostrarPedido(*p);
    
    printf("\nNuevo estado:\n");
    printf("1. Recibido\n");
    printf("2. En Preparación\n");
    printf("3. Listo\n");
    printf("4. Entregado\n");
    printf("5. Cancelado\n");
    printf("Seleccione: ");
    
    int nuevoEstado;
    scanf("%d", &nuevoEstado);
    limpiarBuffer();
    
    if (nuevoEstado >= 1 && nuevoEstado <= 5) {
        EstadoPedido estadoAnterior = p->estado;
        p->estado = (EstadoPedido)nuevoEstado;
        
        // Si se marca como entregado, registrar hora de entrega
        if (p->estado == ENTREGADO) {
            p->horaEntrega = time(NULL);
        }
        
        guardarPedidos();
        printf("Estado actualizado de '%s' a '%s'.\n", 
               estadoToString(estadoAnterior), estadoToString(p->estado));
    } else {
        printf("Estado no válido.\n");
    }
}

void cancelarPedido() {
    printf("\n=== CANCELAR PEDIDO ===\n");
    
    int idPedido;
    printf("Ingrese ID del pedido a cancelar: ");
    scanf("%d", &idPedido);
    limpiarBuffer();
    
    // Buscar pedido
    int indice = -1;
    for (int i = 0; i < cola.numPedidos; i++) {
        if (cola.pedidos[i].id == idPedido) {
            indice = i;
            break;
        }
    }
    
    if (indice == -1) {
        printf("Pedido no encontrado.\n");
        return;
    }
    
    struct Pedido *p = &cola.pedidos[indice];
    
    if (p->estado == ENTREGADO) {
        printf("No se puede cancelar un pedido ya entregado.\n");
        return;
    }
    
    printf("Pedido a cancelar:\n");
    mostrarPedido(*p);
    
    printf("\n¿Está seguro que desea cancelar este pedido? (s/n): ");
    char confirmacion;
    scanf("%c", &confirmacion);
    limpiarBuffer();
    
    if (confirmacion == 's' || confirmacion == 'S') {
        p->estado = CANCELADO;
        guardarPedidos();
        printf("Pedido ID %d ha sido cancelado.\n", idPedido);
    } else {
        printf("Cancelación abortada.\n");
    }
}

void mostrarEstadisticas() {
    printf("\n=== ESTADÍSTICAS DEL SISTEMA ===\n");
    
    int totalPedidos = 0;
    int pedidosActivos = 0;
    int pedidosEntregados = 0;
    int pedidosCancelados = 0;
    int pedidosPresenciales = 0;
    int pedidosOnline = 0;
    float ventasTotal = 0.0;
    
    for (int i = 0; i < cola.numPedidos; i++) {
        struct Pedido p = cola.pedidos[i];
        totalPedidos++;
        
        switch(p.estado) {
            case RECIBIDO:
            case EN_PREPARACION:
            case LISTO:
                pedidosActivos++;
                break;
            case ENTREGADO:
                pedidosEntregados++;
                ventasTotal += p.total;
                break;
            case CANCELADO:
                pedidosCancelados++;
                break;
        }
        
        if (p.tipo == PRESENCIAL) {
            pedidosPresenciales++;
        } else {
            pedidosOnline++;
        }
    }
    
    printf("Total de pedidos: %d\n", totalPedidos);
    printf("Pedidos activos: %d\n", pedidosActivos);
    printf("Pedidos entregados: %d\n", pedidosEntregados);
    printf("Pedidos cancelados: %d\n", pedidosCancelados);
    printf("Pedidos presenciales: %d\n", pedidosPresenciales);
    printf("Pedidos online: %d\n", pedidosOnline);
    printf("Ventas totales: $%.2f\n", ventasTotal);
    
    if (pedidosEntregados > 0) {
        printf("Venta promedio por pedido: $%.2f\n", ventasTotal / pedidosEntregados);
    }
}

// Funciones auxiliares
void leerCadena(char *cadena, int tam) {
    if (fgets(cadena, tam, stdin) != NULL) {
        // Eliminar el salto de línea si existe
        cadena[strcspn(cadena, "\n")] = 0;
    }
}

void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int calcularTiempoEstimado(struct ItemPedido items[], int numItems) {
    int tiempoMax = 0;
    int tiempoTotal = 0;
    
    for (int i = 0; i < numItems; i++) {
        int tiempoItem = items[i].tiempoPreparacion * items[i].cantidad;
        tiempoTotal += tiempoItem;
        if (items[i].tiempoPreparacion > tiempoMax) {
            tiempoMax = items[i].tiempoPreparacion;
        }
    }
    
    // El tiempo estimado es el mayor entre el item más lento y el 70% del tiempo total
    return (tiempoMax > (tiempoTotal * 0.7)) ? tiempoMax : (int)(tiempoTotal * 0.7);
}

void calcularPrioridad(struct Pedido *pedido) {
    // Lógica de priorización:
    // - Pedidos presenciales tienen mayor prioridad base
    // - Pedidos pequeños (pocos items) tienen mayor prioridad
    // - Tiempo de preparación corto = mayor prioridad
    
    int puntajePrioridad = 0;
    
    // Tipo de pedido
    if (pedido->tipo == PRESENCIAL) {
        puntajePrioridad += 3;
    } else {
        puntajePrioridad += 1;
    }
    
    // Número de items (menos items = más prioridad)
    if (pedido->numItems <= 2) {
        puntajePrioridad += 3;
    } else if (pedido->numItems <= 5) {
        puntajePrioridad += 2;
    } else {
        puntajePrioridad += 1;
    }
    
    // Tiempo de preparación
    if (pedido->tiempoEstimado <= 5) {
        puntajePrioridad += 3;
    } else if (pedido->tiempoEstimado <= 10) {
        puntajePrioridad += 2;
    } else {
        puntajePrioridad += 1;
    }
    
    // Asignar prioridad final
    if (puntajePrioridad >= 7) {
        pedido->prioridad = ALTA;
    } else if (puntajePrioridad >= 4) {
        pedido->prioridad = MEDIA;
    } else {
        pedido->prioridad = BAJA;
    }
}

void ordenarPedidosPorPrioridad() {
    // Ordenamiento burbuja por prioridad y tiempo de creación
    for (int i = 0; i < cola.numPedidos - 1; i++) {
        for (int j = 0; j < cola.numPedidos - i - 1; j++) {
            struct Pedido *p1 = &cola.pedidos[j];
            struct Pedido *p2 = &cola.pedidos[j + 1];
            
            // Solo ordenar pedidos activos
            if ((p1->estado == ENTREGADO || p1->estado == CANCELADO) &&
                (p2->estado != ENTREGADO && p2->estado != CANCELADO)) {
                // Intercambiar: mover activos hacia adelante
                struct Pedido temp = *p1;
                *p1 = *p2;
                *p2 = temp;
            } else if ((p1->estado != ENTREGADO && p1->estado != CANCELADO) &&
                       (p2->estado != ENTREGADO && p2->estado != CANCELADO)) {
                // Ambos activos: ordenar por prioridad y luego por tiempo
                if (p1->prioridad < p2->prioridad || 
                    (p1->prioridad == p2->prioridad && p1->horaCreacion > p2->horaCreacion)) {
                    struct Pedido temp = *p1;
                    *p1 = *p2;
                    *p2 = temp;
                }
            }
        }
    }
}

void guardarPedidos() {
    FILE *archivo = fopen("pedidos.dat", "wb");
    if (archivo == NULL) {
        printf("Error al guardar pedidos.\n");
        return;
    }
    
    fwrite(&cola, sizeof(struct ColaPedidos), 1, archivo);
    fclose(archivo);
}

int cargarPedidos() {
    FILE *archivo = fopen("pedidos.dat", "rb");
    if (archivo == NULL) {
        return 0; // No existe archivo
    }
    
    if (fread(&cola, sizeof(struct ColaPedidos), 1, archivo) != 1) {
        fclose(archivo);
        return 0;
    }
    
    fclose(archivo);
    return 1;
}

void mostrarPedido(struct Pedido pedido) {
    printf("\n--- PEDIDO ID: %d ---\n", pedido.id);
    printf("Tipo: %s\n", tipoToString(pedido.tipo));
    printf("Estado: %s\n", estadoToString(pedido.estado));
    printf("Cliente: %s\n", pedido.clienteNombre);
    printf("Contacto: %s\n", pedido.clienteContacto);
    printf("Items:\n");
    
    for (int i = 0; i < pedido.numItems; i++) {
        printf("  %d x %s - $%.2f c/u\n", 
               pedido.items[i].cantidad,
               pedido.items[i].nombreItem,
               pedido.items[i].precioUnitario);
    }
    
    printf("Total: $%.2f\n", pedido.total);
    printf("Tiempo estimado: %d minutos\n", pedido.tiempoEstimado);
    
    char buffer[26];
    struct tm* tm_info = localtime(&pedido.horaCreacion);
    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    printf("Hora de creación: %s\n", buffer);
    
    if (pedido.estado == ENTREGADO) {
        tm_info = localtime(&pedido.horaEntrega);
        strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
        printf("Hora de entrega: %s\n", buffer);
    }
}

char* estadoToString(EstadoPedido estado) {
    switch(estado) {
        case RECIBIDO: return "Recibido";
        case EN_PREPARACION: return "En Preparación";
        case LISTO: return "Listo";
        case ENTREGADO: return "Entregado";
        case CANCELADO: return "Cancelado";
        default: return "Desconocido";
    }
}

char* tipoToString(TipoPedido tipo) {
    switch(tipo) {
        case PRESENCIAL: return "Presencial";
        case ONLINE: return "Online";
        default: return "Desconocido";
    }
}
