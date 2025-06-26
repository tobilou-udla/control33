#include <stdio.h>
#include "restaurante.h"

int main() {
    printf("=== SISTEMA DE GESTIÓN DE PEDIDOS PARA RESTAURANTES ===\n");
    printf("Iniciando sistema...\n");
    
    // Inicializar el sistema
    inicializarSistema();
    
    int opcion;
    do {
        opcion = menuPrincipal();
        
        switch(opcion) {
            case 1:
                crearPedidoPresencial();
                break;
            case 2:
                crearPedidoOnline();
                break;
            case 3:
                mostrarPedidosActivos();
                break;
            case 4:
                procesarSiguientePedido();
                break;
            case 5:
                actualizarEstadoPedido();
                break;
            case 6:
                cancelarPedido();
                break;
            case 7:
                mostrarEstadisticas();
                break;
            case 8:
                mostrarMenu();
                break;
            case 9:
                printf("Guardando datos y cerrando sistema...\n");
                guardarPedidos();
                printf("¡Hasta luego!\n");
                break;
            default:
                printf("Opción no válida. Intente nuevamente.\n");
                break;
        }
        
        if (opcion != 9) {
            printf("\nPresione Enter para continuar...");
            getchar();
        }
        
    } while(opcion != 9);
    
    return 0;
}
