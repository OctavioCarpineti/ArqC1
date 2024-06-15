//
// Created by Octavio Carpineti on 06/06/2024.
//

/*
 * 1. BIBLIOTECAS
 */

#include <stdio.h>          // Entrada/salida
#include <stdlib.h>         // Para funciones de utilidad general
#include <termios.h>        // Para la manipulación de la terminal
#include <unistd.h>         // Para operaciones (empleada para la funcion sleep, por ejemplo)
#include <stdbool.h>        // Para usar booleanos
#include <fcntl.h>          // Control de archivos
#include <ncurses.h>        // Para la manipulación de la terminal (ncurses)
#include "EasyPIO.h"        // Para manipulación de GPIO (proporcionada por el profesor)
#include "assembly_functions.h"  // Funciones en lenguaje ensamblador (depende de la implementación)

/*
 * 2. DECLARACIONES DE FUNCIONES A EMPLEAR
 */

// Declaracion de funciones QUE SE MUESTRAN
void disp_binary(unsigned char patron);
void obtenerClave(char *password);
void menuInicial();
void autoFantastico();
void choque();
void pendulo(void);
void mensajeTeclaDeDelay (void);
void corroborarModificacionDelay(void);

// Declaración de funciones relacionadas con la CONFIGURACION DE LA TERMINAL
struct termios modificarConfiguracionDeTerminal(void);
void restaurarConfiguracionDeTerminal(struct termios);
bool presionDeTeclaAsignada(int indiceDeRetardo);


// Declaración de funciones relacionadas con la CONFIGURACION DE PINES GPIO
void configurarPin(void);
void mostrarLed(unsigned char);
int retardo(int indiceDeRetardo);
void limpiarEntradaDelBuffer();
void apagarLed();

/*
 * 3. DEFINICIONES
 */

// Definicion de casillas a emplear en la contrasena
#define PASSWORD_LENGTH 5

// Definicion de pines GPIO empeados, y tiempos de retardo
const unsigned char led[] = {14, 15, 18, 23, 24, 25, 8, 7};
int tiempoDeRetardo[] = {10000, 10000, 10000, 10000};

/*
 * 4. MAIN
 */

int main(void)
{
    configurarPin(); // Configura los pines GPIO según la definición del usuario (emplea pioInit(), para inicializar EasyPIO para la manipulación de GPIO)
    char clavePredefinida[5] = {0, 9, 8, 7, 6}; // Predefinicion de clave
    char claveIngresadaPorUsuario[5];

    // Recepción y validación de la contraseña mediante BANDERAS
    // Si se ingresan incorrectamente tres veces, se aborta el programa
    for (int i = 0; i < 3; i++) {
        bool passwordFlag = true;
        obtenerClave(claveIngresadaPorUsuario);

        for (int j = 0; j < 5; j++) {
            if (clavePredefinida[j] != claveIngresadaPorUsuario[j]) {
                passwordFlag = false;
                break;
            }
        }

        if (passwordFlag) {
            printf("Bienvenido al sistema !\n\n");
            menuInicial();
            printf("Hasta luego !\n");
            break;
        } else {
            printf("Clave incorrecta\n\n");
        }

    }
}

//Función para mostrar el patrón de LEDs en formato binario
void disp_binary(unsigned char patron)
{
    // Bucle para recorrer cada bit del patrón
    for (int t = 128; t > 0; t >>= 1) {

        /*
        * Verificar si el bit actual está encendido (bit != 0: verdadero) o apagado (0: falso)
        * La variable t comienza en: 128(10) = 10000000(2). La misma se desplaza un bit a la derecha (divide su posicion en funcion de 2) mediante el operador '>>=', hasta que t = 0.
        * t = 0, cuando todos los bits han sido desplazados hacia la derecha.
        * En la primera iteración, t representa el bit más significativo '(1 << 7)', y en la última iteración, t representa el bit menos significativo (1 << 0).
        * Esto asegura que cada bit del patrón de LEDs sea probado en su respectiva posición,
        */

        if (patron & t)
        {
            /*
            * (patron & t): Operador de bits AND a nivel de bit
            * t se desplaza hacia la derecha en cada iteración del bucle for. De 128 (10000000) hasta 0.
            * Si un bit específico de patron está activado y el bit correspondiente de t también está activado (es decir, ambos son 1)
            * entonces el resultado de la operación AND será un número diferente de cero, lo que se evalúa como verdadero .
            * En este caso, cuando el resultado de (patron & t) es diferente de cero (verdadero), significa que el bit correspondiente en patron está activado
            *  lo que indica que el LED asociado está encendido.
            */

            printf("*"); // Imprimir un asterisco si el bit está encendido
        } else
        {
            printf("-"); // Imprimir un guion si el bit está apagado
        }
    }

    printf("\n"); // Imprimir una nueva línea al final del patrón
    printf("\r"); // Se presiona enter para continuar
}

void obtenerClave(char *password)
{
    struct termios atributosPreviosDeTerminal;
    atributosPreviosDeTerminal = modificarConfiguracionDeTerminal();

    printf("Ingrese clave para poder acceder al sistema: ");

    // Read the password
    for (int i = 0; i < PASSWORD_LENGTH; i++)
    {
        password[i] = getchar();
        printf("*"); // Print asterisk
        fflush(stdout);
    }

    restaurarConfiguracionDeTerminal(atributosPreviosDeTerminal);

    printf("\n");
}

void menuInicial()
{

    int opcion;
    bool encendido = true;

    do
    {
        limpiarEntradaDelBuffer();
        printf("...MENU DE SECUENCIAS DE LEDS...\n");
        printf("Opciones de secuencias. Seleccione que secuencia representar :\n");
        printf("\t* 1. Choque\n");
        printf("\t* 2. Auto fantastico\n");
        printf("\t* 3. Carrera\n");
        printf("\t* 4. Pendulo\n");
        printf("\t* 5. Barra de carga\n");
        printf("\t* 0. (salir)\n");
        scanf("%d", &opcion);

        switch (opcion) {
            printf("---> ");
            scanf("%d", &opcion);
            switch (opcion) {
                case 1:
                    choque();
                    break;
                case 2:
                    autoFantastico();
                    break;
                case 3:
                    //chargeBar();
                    break;
                case 4:
                    pendulo();
                    break;
                case 0:
                    encendido = false;
                    break;
                default:
                    printf("Opcion no valida!\n");
                    printf("Por favor, ingrese una opcion valida\n");
                    continue;
            }
        }
        while (opcion != 0);
    }
}

void autoFantastico()
{

    mensajeTeclaDeDelay();
    printf("Auto Fantastico:\n");

    unsigned char output;

    while (true)
    {
        output = 0x80;
        for (int i = 0; i < 8; i++)
        {
            mostrarLed(output);
            disp_binary(output);
            output = output >> 1;

            if (retardo(0) == 0)
            {
                apagarLed();
                return;
            }
        }

        output = 0x2;

        for (int i = 0; i < 6; i++)
        {
            mostrarLed(output);
            disp_binary(output);
            output = output << 1;

            if (retardo(0) == 0)
            {
                apagarLed();
                return;
            }
        }

    }
}


void choque()
{
    mensajeTeclaDeDelay();
    printf("Choque:\n");

    // Definición de patrones iniciales para los LEDs
    unsigned char ledIzquierdo;
    unsigned char ledDerecho;
    unsigned char patron;

    while (true)
    {
        ledIzquierdo = 0x80; // LED izquierdo, encendido en la posición más a la izquierda (10000000 en binario)
        ledDerecho = 0x1; // LED derecho, encendido en la posición más a la derecha (00000001 en binario)
        for (int i = 0; i < 7; i++)
        {
            // Combina los patrones de ambos LEDs para obtener el patrón completo
            patron = ledIzquierdo | ledDerecho;

            mostrarLed(patron);
            disp_binary(patron);

            ledIzquierdo = ledIzquierdo >> 1; // Desplazar el LED izquierdo una posición a la izquierda
            ledDerecho = ledDerecho << 1; // Desplazar el LED derecho una posición a la derecha
            // Se terminan de desplazar, simulando la colision


            /*
            * El simbolo, en C : '<<' --> Indica desplazamiento a la derecha ('>>' <-- Desplazamiento a la izquierda)
            * En este caso, como la variable es 'UNSIGNED', el desplazamiento es LOGICO.
            * De no ser 'unsigned', seria un desplazamiento aritmetico, el cual CONSERVA EL BIT DE SIGNO.
            * En este caso se modifica al usar 'unsigned', debido a que la secuencia es dada logicamente.
            * De ser realizada mediante tabla, no variaria en caso de usar 'unsigned' o no
            * Ya que la instruccion estaria dada en al memoria. No realiza un desplazamiento como tal
            */

            sleep(1); // Pausa final de 1 segundo antes de terminar

            if (retardo(1) == 0)
            {
                apagarLed();
                return;
            }
        }
    }
}

void pendulo(void)
{
    mensajeTeclaDeDelay();
    printf("Mostrando pendulo\n");

    unsigned char patrones[14] =
            {
                    0x88, // *---*---
                    0x48, // -*--*---
                    0x28, // --*-*---
                    0x18, // ---**---
                    0x14, // ---*-*--
                    0x12, // ---*--*-
                    0x11, // ---*---*
                    0x11, // ---*---*
                    0x12, // ---*--*-
                    0x14, // ---*-*--
                    0x18, // ---**---
                    0x28, // --*-*---
                    0x48, // -*--*---
                    0x88  // *---*---
            };

    //int indiceDeRetardo = 2; // Índice para controlar el retardo inicial

    while (true)
    {
        // Mostrar la secuencia del péndulo y permitir ajuste de retardo
        for (int i = 0; i < 14; i++)
        {
            disp_binary(patrones[i]); // Mostrar el patrón actual

            // Esperar el tiempo definido en tiempoDeRetardo[indiceDeRetardo]
            //usleep(tiempoDeRetardo[indiceDeRetardo]);

            // Verificar si se presionó una tecla para ajustar el retardo
            //if (presionDeTeclaAsignada(indiceDeRetardo)) {
            //return; // Salir si se presiona la tecla de escape (ESC)

            if (retardo(2) == 0)
            {
                apagarLed();
                return;
            }
        }
    }
}
}


/*
* Modifica la configuración de la terminal para desactivar el modo canónico y el eco.
* Devuelve los atributos originales de la terminal para poder restaurarlos más tarde.
*/
struct termios modificarConfiguracionDeTerminal(void)
{
    struct termios atributosPreviosDeTerminal, atributosNuevosDeTerminal;

    // Obtener los atributos actuales de la terminal
    tcgetattr(STDIN_FILENO, &atributosPreviosDeTerminal);

    // Copiar los atributos actuales a los nuevos atributos
    atributosNuevosDeTerminal = atributosPreviosDeTerminal;

    // Desactivar el modo canónico y el eco
    atributosNuevosDeTerminal.c_lflag &= ~(ICANON | ECHO);

    // Aplicar los nuevos atributos a la terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &atributosNuevosDeTerminal);

    return atributosPreviosDeTerminal;
}

/*
 * Restaura la configuración original de la terminal.
 */
void restaurarConfiguracionDeTerminal(struct termios)
{
    // Restaura los atributos originales de la terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &atributosPreviosDeTerminal);
}

bool presionDeTeclaAsignada(int indiceDeRetardo)
{
    struct termios atributosPreviosDeTerminal = modificarConfiguracionDeTerminal();
    int caracter, oldf;

    // Configurar el descriptor de archivo de la entrada estándar en modo no bloqueante
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    // Lee un carácter de la entrada estándar
    caracter = getchar();

    /*
    *'Flecha hacia abajo' es una tecla asignada ---> 66 = Flecha hacia abajo
    *'Flecha hacia arriba' es una tecla asignada ---> 65 = Flecha hacia arriba
    */
    if (caracter == 65)
    {
        if (tiempoDeRetardo[indiceDeRetardo] > 1000)
        {
            tiempoDeRetardo[indiceDeRetardo] = tiempoDeRetardo[indiceDeRetardo] - 1000;
        }
    }  else if (caracter == 66)
    {
        tiempoDeRetardo[indiceDeRetardo] = tiempoDeRetardo[indiceDeRetardo] + 1000;
    }

    restaurarConfiguracionDeTerminal(oldattr);

    // Restore the file descriptor mode
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    // If esc key is hit, return 1
    if (caracter == 27)
    {
        ungetc(caracter, stdin);
        return 1;
    }

    // Esc wasn't hit, return 0
    return 0;
}

void configurarPin(void)
{
    pioInit();

    for (int i = 0; i < 8; i++)
    {
        pinMode(led[i], OUTPUT);
    }
}

void mostrarLed(unsigned char output)
{
    for (int j = 0; j < 8; j++)
    {
        digitalWrite(led[j], (output >> j) & 1);
    }

}

/*
 * Cada secuencia se asocia con un indice de retardo particular, en lo que comprende al uso de la funcion de retardo
 * Se utilizan diferentes índices en la función retardo para manejar retardos específicos de cada secuencia de luces.
 * Esto permite que cada secuencia tenga su propio tiempo de retardo ajustable, lo cual es crucial para la personalización y control independiente de cada secuencia de luces.
 */

int retardo(int indiceDeRetardo)
{
    int i;
    unsigned int j;

    // Se repite el una cantidad de n veces (n = indiceDeRetardo)
    for (i = tiempoDeRetardo[indiceDeRetardo]; i > 0; --i)

        if (presionDeTeclaAsignada(indiceDeRetardo)) {
            return 0;
        }

    return 1;
}

void mensajeTeclaDeDelay (void)
{
    printf("Presione esc para finalizar la secuencia\n");
    printf("Presione W para aumentar la velocidad\n");
    printf("Presione S para disminuir la velocidad\n");
}

void corroborarModificacionDelay(void)
{
    // Verificar si se presionó W (aumentar retardo) o S (disminuir retardo)
    int ch = getchar();
    if (ch == 'w' || ch == 'W') {
        if (delayIndex > 0) {
            delayIndex--; // Reducir el índice de retardo si es posible
        }
    } else if (ch == 's' || ch == 'S') {
        if (delayIndex < 3) {
            delayIndex++; // Aumentar el índice de retardo si es posible
        }
    }
}

void limpiarEntradaDelBuffer()
{
    printf('"ENTER" para continuar...\n');
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        // Descartar cambios
    }
}

void apagarLed()
{
    unsigned char apagar = 0x0;
    mostrarLed(apagar);
}
