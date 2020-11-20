/**
 * @brief It implements the functions for the menus management
 *
 * @file menus.c
 * @author Erik Yuste & Lucía Martinez-Valero
 * @version 1.0
 * @date 28-10-2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "querys.h"
#include "odbc.h"
#include "menus.h"


#define MAX_CHAR 255


/*Private functions*/

/**
* menus_changeScreen Imprime una cantidad de saltos de linea suficiente para que el anterior menu desaparezca de la pantalla
*
* @date 28-10-2020
* @author: Erik Yuste
*
* @param out Puntero a FILE donde se imprimirá
*/
static void menus_changeScreen(FILE *out);
/**
* menus_input Recoge la entrada del usuario sin que tenga que pulsar 'Enter'
*
* @date 28-10-2020
* @author: Erik Yuste
*
* @param c Puntero a char donde se guardará la entrada del usuario
*/
static void menus_input(char *c);
/**
* menus_inputError Imprime un mensaje de error
*
* @date 28-10-2020
* @author: Erik Yuste
*
* @param out Puntero a FILE donde se imprimirá el mensaje
*/
static void menus_inputError(FILE *out);







/*Submenus*/

/**
* menus_generalPrint Imprime la interfaz que ve el usuario en el menu general
*
* @date 28-10-2020
* @author: Erik Yuste
*
* @param out Puntero a FILE donde se imprimirá el menu
*/
static void menus_generalPrint(FILE *out);
/**
* menus_productsPrint Imprime la interfaz que ve el usuario en el menu de productos
*
* @date 28-10-2020
* @author: Erik Yuste
*
* @param out Puntero a FILE donde se imprimirá el menu
*/
static void menus_productsPrint(FILE *out);
/**
* menus_ordersPrint Imprime la interfaz que ve el usuario en el menu de pedidos
*
* @date 14-11-2020
* @author: Lucia Martinez-Valero
*
* @param out Puntero a FILE donde se imprimirá el menu
*/
static void menus_ordersPrint(FILE *out);
/**
* menus_clientesPrint Imprime la interfaz que ve el usuario en el menu de clientes
*
* @date 15-11-2020
* @author: Erik Yuste
*
* @param out Puntero a FILE donde se imprimirá el menu
*/
static void menus_customersPrint(FILE *out);

/**
* menus_products Implementa la lógica del menu de productos
*
* @date 28-10-2020
* @author: Erik Yuste
*
* @param Puntero a SQLHSTMT
* @param out Puntero a FILE donde se imprimirá el menu
* @return Devuelve 0 si no ha habido ningún error y otro entero si lo ha habido
*/
static int menus_products(SQLHSTMT *stmt, FILE *out);
/**
* menus_orders Implementa la lógica del menu de pedidos
*
* @date 14-11-2020
* @author: Lucia Martinez-Valero
*
* @param Puntero a SQLHSTMT
* @param out Puntero a FILE donde se imprimirá el menu
* @return Devuelve 0 si no ha habido ningún error y otro entero si lo ha habido
*/
static int menus_orders(SQLHSTMT *stmt, FILE *out);
/**
* menus_customers Implementa la lógica del menu de clientes
*
* @date 18-11-2020
* @author: Erik Yuste
*
* @param Puntero a SQLHSTMT
* @param out Puntero a FILE donde se imprimirá el menu
* @return Devuelve 0 si no ha habido ningún error y otro entero si lo ha habido
*/
static int menus_customers(SQLHSTMT *stmt, FILE *out);
/**
* menus_exit Función de control de salida de los menus
*
* @date 28-10-2020
* @author: Erik Yuste
*
* @param Puntero a SQLHSTMT
* @param out Puntero a FILE donde se imprimirá el menu
*/
static void menus_exit(FILE *out);


/*Products*/

/**
* menus_productsStock Llama a la función que hace la query 'Stock'
*
* @date 29-10-2020
* @author: Erik Yuste
*
* @param Puntero a SQLHSTMT
* @param out Puntero a FILE donde se imprimirá el resultado
* @return Devuelve 0 si no ha habido ningún error y otro entero si lo ha habido
*/
static int menus_productsStock(SQLHSTMT *stmt, FILE *out);
/**
* menus_productsFind Llama a la función que hace la query 'Find'
*
* @date 01-11-2020
* @author: Lucía Martinez-Valero
*
* @param Puntero a SQLHSTMT
* @param out Puntero a FILE donde se imprimirá el resultado
* @return Devuelve 0 si no ha habido ningún error y otro entero si lo ha habido
*/
static int menus_productsFind(SQLHSTMT *stmt, FILE *out);




static int menus_ordersDetail(SQLHSTMT *stmt, FILE *out);
static int menus_ordersRange(SQLHSTMT *stmt, FILE *out);
static int menus_ordersOpen(SQLHSTMT *stmt, FILE *out);


/*Customers*/
/**
* menus_customersFind Llama a la función que hace la query 'Find'
*
* @date 13-11-2020
* @author: Erik Yuste
*
* @param Puntero a SQLHSTMT
* @param out Puntero a FILE donde se imprimirá el resultado
* @return Devuelve 0 si no ha habido ningún error y otro entero si lo ha habido
*/
static int menus_customersFind(SQLHSTMT *stmt, FILE *out);
/**
* menus_customersListProducts Llama a la función que hace la query 'ListProducts'
*
* @date 18-11-2020
* @author: Erik Yuste
*
* @param Puntero a SQLHSTMT
* @param out Puntero a FILE donde se imprimirá el resultado
* @return Devuelve 0 si no ha habido ningún error y otro entero si lo ha habido
*/
static int menus_customersListProducts(SQLHSTMT *stmt, FILE *out);
/**
* menus_customersBalance Llama a la función que hace la query 'Balance'
*
* @date 18-11-2020
* @author: Erik Yuste
*
* @param Puntero a SQLHSTMT
* @param out Puntero a FILE donde se imprimirá el resultado
* @return Devuelve 0 si no ha habido ningún error y otro entero si lo ha habido
*/
static int menus_customersBalance(SQLHSTMT *stmt, FILE *out);







/*************************************************************************/
/************************** IMPLEMENTACIONES *****************************/
/*************************************************************************/


void menus_input(char *c){
  int er=0;
  if(!c) return;

  (void) fflush(stdout);

  er=fseek(stdin,0,SEEK_END);
  er+=system("stty -echo");
  er+=system("/bin/stty raw");
  (*c) = (char) getchar();
  er+=system("/bin/stty cooked");
  er+=system("stty echo");

  return;
}





int menus_general(SQLHSTMT *stmt, FILE *out){
  char c=' ';
  int end=0;


  if(!out || !stmt) return 1;

  while(end==0){
    if(c!='\n') menus_generalPrint(out);

    menus_input(&c);

    menus_changeScreen(out);

    if(c=='1'){
      if(menus_products(stmt, out)!=0){
         printf("\n > An error occurred while loading the products menu. < \n");
      }
    }else if(c=='2'){
      if(menus_orders(stmt, out)!=0){
         printf("\n > An error occurred while loading the orders menu. < \n");
      }
    }else if(c=='3'){
      if(menus_customers(stmt, out)!=0){
         printf("\n > An error occurred while loading the customers menu. < \n");
      }
    }else if(c=='4'){
      end=1;
      menus_exit(out);
    }else if(c!='\n'){
      menus_inputError(out);
    }

  }

  return 0;
}






/* General submenus */

int menus_products(SQLHSTMT *stmt, FILE *out){
  char c=' ';
  int end=0;

  if(!out || !stmt) return 1;


  while(end==0){
    if(c!='\n') menus_productsPrint(out);

    menus_input(&c);

    if(c=='1'){
      if(menus_productsStock(stmt, out)!=0){
         printf("\n > An error occurred while loading the products stock. < \n");
      }
    }else if(c=='2'){
      if(menus_productsFind(stmt, out)!=0){
         printf("\n > An error occurred while loading the products menu. < \n");
      }
    }else if(c=='3'){
      end=1;
      menus_exit(out);
    }else if(c!='\n'){
      menus_inputError(out);
    }

  }

  return 0;
}




int menus_orders(SQLHSTMT *stmt, FILE *out){
  char c=' ';
  int end=0;

  if(!out||!stmt) return 1;
  while(end==0){
    if(c!='\n') menus_ordersPrint(out);

    menus_input(&c);


    if(c=='1'){
      if(menus_ordersOpen(stmt, out)!=0){
         printf("\n > An error occurred while loading the orders. < \n");
      }
    }else if(c=='2'){
      if(menus_ordersRange(stmt, out)!=0){
         printf("\n > An error occurred while loading the orders. < \n");
      }
    }else if(c=='3'){
      if(menus_ordersDetail(stmt, out)!=0){
         printf("\n > An error occurred while loading the orders. < \n");
      }
    }else if(c=='4'){
      end=1;
      menus_exit(out);
    }else if(c!='\n'){
      menus_inputError(out);
    }

  }

  return 0;
}

int menus_customers(SQLHSTMT *stmt, FILE *out){
  char c=' ';
  int end=0;

  if(!out || !stmt) return 1;


  while(end==0){
    if(c!='\n') menus_customersPrint(out);

    menus_input(&c);


    if(c=='1'){
      if(menus_customersFind(stmt, out)!=0){
         printf("\n > An error occurred while loading the customers data. < \n");
      }
    }else if(c=='2'){
      if(menus_customersListProducts(stmt, out)!=0){
         printf("\n > An error occurred while loading the customers data. < \n");
      }
    }else if(c=='3'){
      if(menus_customersBalance(stmt, out)!=0){
         printf("\n > An error occurred while loading the customers data. < \n");
      }
    }else if(c=='4'){
      end=1;
      menus_exit(out);
    }else if(c!='\n'){
      menus_inputError(out);
    }

  }

  return 0;
}

void menus_exit(FILE *out){
  if(!out) return;
  fprintf(out, "\nMENU Exit\n");
  return;
}









/* Products options */
int menus_productsStock(SQLHSTMT *stmt, FILE *out){

  printf("\nEnter productcode > ");
  if(query_productStock(stmt, out)!=0) return 1;

  return 0;
}
int menus_productsFind(SQLHSTMT *stmt, FILE *out){

  printf("\nEnter productname > ");
  if(query_productFind(stmt, out)!=0) return 1;

  return 0;
}


/* Orders options */
int menus_ordersOpen(SQLHSTMT *stmt, FILE *out){

  if(!stmt||!out) return 1;
  printf("\n");
  if(query_ordersOpen(stmt, out)!=0) return 1;

  return 0;
}

int menus_ordersRange(SQLHSTMT *stmt, FILE *out){

  if(!stmt||!out) return 1;
  printf("\n");
  if(query_ordersRange(stmt, out)!=0) return 1;

  return 0;
}

int menus_ordersDetail(SQLHSTMT *stmt, FILE *out){

  if(!stmt||!out) return 1;
  printf("\nEnter ordernumber > ");
  if(query_ordersDetails(stmt, out)!=0) return 1;

  return 0;
}

/* Customers options */
int menus_customersFind(SQLHSTMT *stmt, FILE *out){

  if(!stmt||!out) return 1;
  printf("\nEnter customer name > ");
  if(query_customersFind(stmt, out)!=0) return 1;

  return 0;
}

int menus_customersListProducts(SQLHSTMT *stmt, FILE *out){

  if(!stmt||!out) return 1;
  printf("\nEnter customer number > ");
  if(query_customersListProducts(stmt, out)!=0) return 1;

  return 0;
}

int menus_customersBalance(SQLHSTMT *stmt, FILE *out){

  if(!stmt||!out) return 1;
  printf("\nEnter customer number > ");
  if(query_customersBalance(stmt, out)!=0) return 1;

  return 0;
}









/***********************
 *                     *
 *  Menus' interfaces  *
 *                     *
 ***********************/

void menus_generalPrint(FILE *out){
  menus_changeScreen(out);

  fprintf(out, "\n\n ################################\n");
  fprintf(out, " # Choose what you want to show #\n");
  fprintf(out, " ################################\n");
  fprintf(out, "\n\t1. Products.\n\t2. Orders.\n\t3. Customers.\n\t4. Exit.\n\n");
  fprintf(out, "Enter a number that corresponds to your choice > ");

  return;
}

void menus_productsPrint(FILE *out){

  fprintf(out, "\n\n ##########################\n");
  fprintf(out, " # Choose products option #\n");
  fprintf(out, " ##########################\n");
  fprintf(out, "\n\t1. Stock.\n\t2. Find.\n\t3. Back.\n\n");
  fprintf(out, "Enter a number that corresponds to your choice > ");

  return;
}

void menus_ordersPrint(FILE *out){

  fprintf(out, "\n\n ########################\n");
  fprintf(out, " # Choose orders option #\n");
  fprintf(out, " ########################\n");
  fprintf(out, "\n\t1. Open.\n\t2. Range.\n\t3. Detail.\n\t4. Back.\n\n");
  fprintf(out, "Enter a number that corresponds to your choice > ");

  return;
}

void menus_customersPrint(FILE *out){

  fprintf(out, "\n\n ##########################\n");
  fprintf(out, " # Choose customer option #\n");
  fprintf(out, " ##########################\n");
  fprintf(out, "\n\t1. Find.\n\t2. List Products.\n\t3. Balance.\n\t4. Back\n\n");
  fprintf(out, "Enter a number that corresponds to your choice > ");

  return;
}



/* General Purpose */

void menus_inputError(FILE *out){
  fprintf(out, "\n\n > Please enter a valid option < \n\n");
  return;
}

void menus_changeScreen(FILE *out){
  fprintf(out, "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  return;
}
