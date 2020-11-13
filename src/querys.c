/**
 * @brief Implementa las funciones que realizan las querys a la base de datos
 *
 * @file querys.c
 * @author Erik Yuste & Lucia Martinez-Valero
 * @version 1.0
 * @date 29-10-2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "querys.h"



/*Privadas*/

/**
* stop Función que detiene la ejecución del programa hasta que el usuario pulsa una tecla
*
* @date 02-11-2020
* @author: Erik Yuste
*/
static void stop();


/**
* query_productStockInterface imprime el resultado de la query 'Stock'
*
* @date 29-10-2020
* @author: Erik Yuste
*
* @param stmt Puntero a SQLHSTMT
* @param result Puntero a SQLINTEGER donde se recibirá el resultado de la query
* @param productcode Puntero a SQLCHAR donde se encuentra el codigo de producto introducido por el usuario
*/
static void query_productStockInterface(SQLHSTMT *stmt, SQLINTEGER *result, SQLCHAR *productcode);
/**
* query_productFindInterface imprime el resultado de la query 'Find'
*
* @date 01-11-2020
* @author: Lucia Martinez-Valero
*
* @param stmt Puntero a SQLHSTMT
* @param pcode Puntero a SQLCHAR donde se recibirá el nombre de los productos resultado de la query
* @param pname Puntero a SQLCHAR donde se recibirá el codigo de los productos resultado de la query
* @param string Puntero a char donde se almacena la cadena de caracters introducida por el usuario
*/
static void query_productFindInterface(SQLHSTMT *stmt, SQLCHAR *pcode, SQLCHAR *pname, char *string);












/*
 *   PRODUCT  STOCK
 */

int query_productStock(SQLHSTMT *stmt, FILE *out){
  SQLRETURN ret; /* ODBC API return status */
  SQLCHAR productcode[MY_CHAR_LEN];
  SQLINTEGER y=(SQLINTEGER) 0;

  if(!stmt || !out) return 1;


  fprintf(out," > ");
  if(fflush(out)!=0) printf("ERROR FFLUSH");
  if(scanf("%s", (char*)productcode)==EOF) printf("ERROR SCANF");

  ret=SQLPrepare((*stmt), (SQLCHAR*) "select p.quantityinstock from products as p where p.productcode = ?", SQL_NTS);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLPREPARE %d\n", ret);


  ret=SQLBindParameter((*stmt), 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, productcode, 0, NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDPARAMETER\n");

  /* Ejecuta la query */
  ret=SQLExecute(*stmt);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLEXECUTE\n");

  /* Asigna la columna resultado a la variable y */
  ret=SQLBindCol(*stmt, 1, SQL_INTEGER, &y, (SQLLEN) sizeof(y), NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDCOL\n");

  /* Interfaz */
  query_productStockInterface(stmt, &y, productcode);

  ret=SQLCloseCursor(*stmt);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLCLOSECURSOR\n");

  if(fflush(out)!=0) printf("ERROR FFLUSH");

  return 0;
}





void query_productStockInterface(SQLHSTMT *stmt, SQLINTEGER *result, SQLCHAR *productcode){
  SQLRETURN ret;

  ret=SQLFetch(*stmt);
  if(SQL_SUCCEEDED(ret)) printf("\n\n < There are %d products with the code \'%s\' >\n\n", *result, (char*) productcode);

  else printf("\n\n < No products with the code \'%s\' found >\n\n",(char*) productcode);


  /*stop();*/

  return;
}





/*
 *  PRODUCT FIND
 */

/*select p.productcode, p.productname from products p where p.productname like '%cadena%';*/
int query_productFind(SQLHSTMT *stmt, FILE *out){
  SQLRETURN ret; /* ODBC API return status */
  SQLCHAR productcode[MY_CHAR_LEN], productname[MY_CHAR_LEN];
  char string[MY_CHAR_LEN], query[MY_CHAR_LEN]="select p.productcode, p.productname from products p where UPPER(p.productname) like UPPER('%";





  if(!stmt || !out) return 1;


  if(fflush(out)!=0) printf("ERROR FFLUSH");
  if(scanf("%s", string)==EOF) printf("ERROR SCANF");

  strcat(query, string);
  strcat(query, "%') order by p.productcode");

  ret=SQLPrepare((*stmt), (SQLCHAR*) query, SQL_NTS);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLPREPARE %d\n", ret);


  /*ret=SQLBindParameter((*stmt), 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, string, 0, NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDPARAMETER\n");*/

  /* Ejecuta la query */
  ret=SQLExecute(*stmt);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLEXECUTE\n");

  /* Asigna la columna resultado a la variable y */
  ret=SQLBindCol(*stmt, 1, SQL_CHAR, productcode, (SQLLEN) sizeof(productcode), NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDCOL\n");
  /* Asigna la columna resultado a la variable y */
  ret=SQLBindCol(*stmt, 2, SQL_CHAR, productname, (SQLLEN) sizeof(productname), NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDCOL\n");

  /* Interfaz */
  query_productFindInterface(stmt, productcode, productname, string);

  ret=SQLCloseCursor(*stmt);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLCLOSECURSOR\n");

  if(fflush(out)!=0) printf("ERROR FFLUSH");

  return 0;
}


void query_productFindInterface(SQLHSTMT *stmt, SQLCHAR *pcode, SQLCHAR *pname, char *string){
  SQLRETURN ret;
  int a=1;


  while(SQL_SUCCEEDED(ret = SQLFetch(*stmt))) {
    if(a==1){
      printf("\n\t| Product code\t| Product name\n");
      printf(  "--------+-----------------+--------------\n");
    }
      printf("   %d\t| %s\t| %s\t\n", a, (char*) pcode, (char*) pname);
      a++;
      if((a%10)==0){

        stop();
        printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\t| Product code\t| Product name\n");
        printf(  "--------+-----------------+--------------\n");
      }
  }
  printf("\n");
  if(a==1) printf("\n < No product named \'%s\' >\n\n",string);




  /*stop();*/

  return;
}


















/**/



void stop(){
  int a;
  while(getchar()!= (int)'\n');
  a = getchar();

  return;
}
