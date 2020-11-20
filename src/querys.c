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

/**
* query_ordersOpenInterface imprime el resultado de la query 'Stock'
*
* @date 06-11-2020
* @author: Lucia Martinez-Valero
*
* @param stmt Puntero a SQLHSTMT
* @param result Puntero a SQLCHAR donde se recibirá el resultado de la query
*/
static void query_ordersOpenInterface(SQLHSTMT *stmt, SQLINTEGER *onum);
static void  query_ordersRangeInterface(SQLHSTMT *stmt, SQLINTEGER *ordernumber, SQLCHAR *orderdate, SQLCHAR *shippeddate);
static void query_ordersDetailsInterface(SQLHSTMT *stmt, SQLINTEGER *odnum, SQLDATE *oddate, SQLCHAR *st, SQLCHAR *pc, SQLINTEGER *q, SQLDOUBLE *price, SQLDOUBLE *subtotal);


/**
* query_customersFindInterface imprime el resultado de la query 'Find'
*
* @date 13-11-2020
* @author: Erik Yuste
*
* @param stmt Puntero a SQLHSTMT
* @param cnum Puntero a SQLCHAR donde se recibirá el numero de los clientes resultado de la query
* @param cname Puntero a SQLCHAR donde se recibirá el nombre de los clientes resultado de la query
* @param cfn Puntero a SQLCHAR donde se recibirá el primer nombre de los clientes resultado de la query
* @param csn Puntero a SQLCHAR donde se recibirá el segundo nombre de los clientes resultado de la query
* @param string Puntero a char donde se almacena la cadena de caracters introducida por el usuario
*/
static void query_customersFindInterface(SQLHSTMT *stmt, SQLCHAR *cnum, SQLCHAR *cname, SQLCHAR *cfn, SQLCHAR *csn,char *string);

/**
* query_customersListProductsInterface imprime el resultado de la query 'ListProducts'
*
* @date 18-11-2020
* @author: Erik Yuste
*
* @param stmt Puntero a SQLHSTMT
* @param cnum SQLINTEGER que es el numero de cliente introducido por el usuario
* @param pname Puntero a SQLCHAR donde se recibirá el nombre de los productos resultado de la query
* @param qordered Puntero a SQLINTEGER donde se recibirá el numero de productos resultado de la query
*/
static void query_customersListProductsInterface(SQLHSTMT *stmt, SQLINTEGER cnum, SQLCHAR *pname, SQLINTEGER *qordered);

/**
* query_customersListProductsInterface imprime el resultado de la query 'ListProducts'
*
* @date 18-11-2020
* @author: Erik Yuste
*
* @param stmt Puntero a SQLHSTMT
* @param cnum SQLINTEGER que es el numero de cliente introducido por el usuario
* @param pname Puntero a SQLCHAR donde se recibirá el nombre de los productos resultado de la query
* @param qordered Puntero a SQLINTEGER donde se recibirá el numero de productos resultado de la query
*/
static void query_customersBalanceInterface(SQLHSTMT *stmt, SQLINTEGER cnum, SQLDOUBLE *saldo);


/**
* querys_input Obtiene un caracter del usuario
*
* @date 19-11-2020
* @author: Erik Yuste
*
* @param c Puntero a char
*/
static void querys_input(char *c);

/**
* querys_printPage imprime la pagina (10 elementos) del fichero
*
* @date 19-11-2020
* @author: Erik Yuste
*
* @param fname string del nombre del fichero
* @param pag el numero de la pagina
* @param titulo string del titulo de la pagina
*/
static void querys_printPage(char *fname, int pag, char *titulo);

/**
* querys_deleteFile elimina un fichero
*
* @date 19-11-2020
* @author: Erik Yuste
*
* @param fname string del nombre del fichero
*/
static void querys_deleteFile(char *fname);







/* IMPLEMENTACIONES */

static void querys_input(char *c){
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

static void querys_printPage(char *fname, int pag, char *titulo){
  FILE *f=NULL;
  int i=0;
  char registro[MY_CHAR_LEN]=" \n";


  f=fopen(fname,"r");
  if(!f) return;

  printf("%s\n",titulo);

  while(fgets(registro, MY_CHAR_LEN, f)!=NULL){

    if(i/10==pag){


      printf("%s", registro);
    }else if(i/10>pag) break;

    i++;
  }

  (void)fclose(f);


  return;
}

static void querys_deleteFile(char *fname){
  char rem[50]="rm ./";
  int err=0;

  strcat(rem, fname);

  err+=system("stty -echo");
  err+=system(rem);
  err+=system("stty echo");

  return;
}













/*
 *   PRODUCT  STOCK
 */

int query_productStock(SQLHSTMT *stmt, FILE *out){
  SQLRETURN ret; /* ODBC API return status */
  char productcode[MY_CHAR_LEN]=" \n";
  SQLINTEGER y=(SQLINTEGER) 0;

  if(!stmt || !out) return 1;


  if(fflush(out)!=0) printf("ERROR FFLUSH");
  if(scanf("%s", (char*)productcode)==EOF) printf("ERROR SCANF");

  ret=SQLPrepare((*stmt), (SQLCHAR*) "select p.quantityinstock from products as p where p.productcode = ?", SQL_NTS);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLPREPARE %d\n", ret);


  ret=SQLBindParameter((*stmt), 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, (SQLCHAR*) productcode, 0, NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDPARAMETER\n");

  /* Ejecuta la query */
  ret=SQLExecute(*stmt);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLEXECUTE\n");

  /* Asigna la columna resultado a la variable y */
  ret=SQLBindCol(*stmt, 1, SQL_INTEGER, &y, (SQLLEN) sizeof(y), NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDCOL\n");

  /* Interfaz */
  query_productStockInterface(stmt, &y, (SQLCHAR*) productcode);

  ret=SQLCloseCursor(*stmt);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLCLOSECURSOR\n");

  if(fflush(out)!=0) printf("ERROR FFLUSH");

  return 0;
}



void query_productStockInterface(SQLHSTMT *stmt, SQLINTEGER *result, SQLCHAR *productcode){
  SQLRETURN ret;

  if(!stmt||!result||!productcode){
    printf("INTERFACE FAILURE\n");
    return;
  }

  ret=SQLFetch(*stmt);
  if(SQL_SUCCEEDED(ret)) printf("\n\n < There are %d products with the code \'%s\' >\n\n", *result, (char*) productcode);

  else printf("\n\n < No products with the code \'%s\' found >\n\n",(char*) productcode);


  /*stop();*/

  return;
}









/*
 *  PRODUCT FIND
 */
int query_productFind(SQLHSTMT *stmt, FILE *out){
  SQLRETURN ret; /* ODBC API return status */
  char productcode[MY_CHAR_LEN] = " \n", productname[MY_CHAR_LEN] = " \n";
  char string[MY_CHAR_LEN] = " \n", query[]= "select p.productcode, p.productname from products p where position(UPPER(?) in UPPER(p.productname))>0 order by p.productcode";

  if(!stmt || !out) return 1;


  (void) fflush(out);
  if(scanf("%s", (char*) string)==EOF) printf("ERROR SCANF");


  ret=SQLPrepare((*stmt), (SQLCHAR*) query, SQL_NTS);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLPREPARE %d\n", ret);


  ret=SQLBindParameter((*stmt), 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, string, 0, NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDPARAMETER\n");

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
  query_productFindInterface(stmt,(SQLCHAR*) productcode,(SQLCHAR*) productname, (char*) string);

  ret=SQLCloseCursor(*stmt);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLCLOSECURSOR\n");

  if(fflush(out)!=0) printf("ERROR FFLUSH");

  return 0;
}


void query_productFindInterface(SQLHSTMT *stmt, SQLCHAR *pcode, SQLCHAR *pname, char *string){
  SQLRETURN ret;
  int a=1;

  if(!stmt || !pcode || !pname || !string){
    printf("INTERFACE FAILURE\n");
    return;
  }

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






/*
 *  ORDER OPEN
 */

int query_ordersOpen(SQLHSTMT *stmt, FILE *out){
  SQLRETURN ret; /* ODBC API return status */
  SQLINTEGER ordernumber = (SQLINTEGER) 0;
  char query[]="select o.ordernumber from orders o where o.shippeddate isnull order by o.ordernumber";


  if(!stmt || !out) return -1;


  if(fflush(out)!=0) printf("ERROR FFLUSH");

  ret=SQLPrepare((*stmt), (SQLCHAR*) query, SQL_NTS);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLPREPARE %d\n", ret);

  /* Ejecuta la query */
  ret=SQLExecute(*stmt);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLEXECUTE\n");

  /* Asigna la columna resultado a la variable */
  ret=SQLBindCol(*stmt, 1, SQL_INTEGER, &ordernumber, (SQLLEN) sizeof(ordernumber), NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDCOL\n");


  /* Interfaz */
  query_ordersOpenInterface(stmt, &ordernumber);

  ret=SQLCloseCursor(*stmt);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLCLOSECURSOR\n");

  if(fflush(out)!=0) printf("ERROR FFLUSH");

  return 0;
}

static void query_ordersOpenInterface(SQLHSTMT *stmt, SQLINTEGER *onum){
  int a=1;
  SQLRETURN ret;

  if(!stmt || !onum){
    printf("INTERFACE FAILURE\n");
    return;
  }

  while(SQL_SUCCEEDED(ret = SQLFetch(*stmt))) {

      if(a==1){
        printf("\n\t| Order number\t|\n");
        printf(  "--------+-----------------+\n");
      }
      printf("   %d\t| %d\t|\n", a, *((int*) onum));
      a++;
      /*if((a%10)==0){

        stop();
        printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\t| Order number\t|\n");
        printf(  "--------+-----------------+\n");
      }*/
  }
  printf("\n");
  if(a==1) printf("\n < All the orders have been shipped >\n\n");

  return;
}





/*
 *  ORDER RANGE
 */

int query_ordersRange(SQLHSTMT *stmt, FILE *out){
  SQLRETURN ret; /* ODBC API return status */
  SQLINTEGER ordernumber= (SQLINTEGER) 0;
  int i=0, k=0;
  char odd[MY_CHAR_LEN]=" \n", odd2[MY_CHAR_LEN]=" \n", in[MY_CHAR_LEN]=" \n", orderdate[MY_CHAR_LEN]=" \n", shippeddate[MY_CHAR_LEN]=" \n";
  char query[]="select o.ordernumber, o.orderdate, o.shippeddate from orders o where o.orderdate >= ? and o.orderdate <= ? order by o.ordernumber";

  if(!stmt || !out) return 1;


  (void) fflush(out);
  printf("Enter dates (YYYY-MM-DD - YYYY-MM-DD) > ");

  (void)fgets(in, MY_CHAR_LEN, stdin);
  if(strlen(in)<20) (void)fgets(in, MY_CHAR_LEN, stdin);

/*premparamos las fechas para el bind parameter*/
  while(in[i]!='\0'){
    if(in[i]!=' '){
      odd[i]=in[i];
    }
    if(in[i]==' '){
      odd[i]='\0';
      i+=3;
    }
    if(i>=13){
      odd2[k]=in[i];
      k++;
    }
    i++;
  }
  odd2[k]='\0';

  ret=SQLPrepare((*stmt), (SQLCHAR*) query, SQL_NTS);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLPREPARE %d\n", ret);


  ret=SQLBindParameter((*stmt), 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, odd, 0, NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDPARAMETER\n");
  ret=SQLBindParameter((*stmt), 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, odd2, 0, NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDPARAMETER\n");

  ret=SQLExecute(*stmt);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLEXECUTE\n");

  /* Asigna la columna resultado a las variables  */
  ret=SQLBindCol(*stmt, 1, SQL_INTEGER, &ordernumber, (SQLLEN) sizeof(ordernumber), NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDCOL 1\n");
  ret=SQLBindCol(*stmt, 2, SQL_CHAR, orderdate, (SQLLEN) sizeof(orderdate), NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDCOL 2\n");
  ret=SQLBindCol(*stmt, 3, SQL_C_CHAR, shippeddate, (SQLLEN) sizeof(shippeddate), NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDCOL 3\n");


  /* Interfaz */
  query_ordersRangeInterface(stmt, &ordernumber, (SQLCHAR*) orderdate, (SQLCHAR*) shippeddate);

  ret=SQLCloseCursor(*stmt);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLCLOSECURSOR\n");

  if(fflush(out)!=0) printf("ERROR FFLUSH");

  return 0;
}

static void  query_ordersRangeInterface(SQLHSTMT *stmt, SQLINTEGER *ordernumber, SQLCHAR *orderdate, SQLCHAR *shippeddate){
  int a=1;
  SQLRETURN ret;
  char t[]="    | Order\t| Date  \t| Shipped\n";

  while(SQL_SUCCEEDED(ret = SQLFetch(*stmt))) {
    if(a==1){
      printf("\n%s", t);
      printf(  "----+-----------+---------------+-----------------------\n");
    }

    if(a<10)
      printf(" 0%d | %d\t| %s\t| %s\t\n", a, *((int*) ordernumber), (char*) orderdate, (char*)shippeddate);
    else
      printf(" %d | %d\t| %s\t| %s\t\n", a, *((int*) ordernumber), (char*) orderdate, (char*)shippeddate);

      a++;
      if((a%10)==0){

        stop();
        printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n%s", t);
        printf(  "\n----+-----------+---------------+-----------------------\n");
      }
  }
}






/*
 *  ORDER DETAILS
 */
int query_ordersDetails(SQLHSTMT *stmt, FILE *out){
  SQLRETURN ret; /* ODBC API return status */
  SQLINTEGER ordernumber= (SQLINTEGER) 0, qord= (SQLINTEGER) 0;
  SQLDOUBLE price= (SQLDOUBLE) 0.0, subtotal= (SQLDOUBLE) 0.0;
  char pcode[MY_CHAR_LEN]="\n", stat[MY_CHAR_LEN]="\n", orderdate[MY_CHAR_LEN]="\n";
  int odn=0;
  char query[]="select o.ordernumber, o.orderdate, o.status, od.productcode, od.quantityordered, od.priceeach, od.quantityordered*od.priceeach as subtotal from orders o join orderdetails od on o.ordernumber=od.ordernumber where o.ordernumber = ? group by o.ordernumber, od.productcode, od.quantityordered, od.priceeach, od.orderlinenumber order by od.orderlinenumber";

  if(!stmt || !out) return 1;


  (void) fflush(out);
  if(scanf("%d", &odn)==EOF) printf("ERROR SCANF");
  printf("%d\n", odn);
  if(odn<1){
    printf("Order number given is not valid\n");
    return 0;
  }

  ret=SQLPrepare((*stmt), (SQLCHAR*) query, SQL_NTS);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLPREPARE %d\n", ret);

  ret=SQLBindParameter((*stmt), 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &odn, 0, NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDPARAMETER\n");

  ret=SQLExecute(*stmt);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLEXECUTE\n");

  /* Asignamos a cada columna de resultados una variable */
  ret=SQLBindCol(*stmt, 1, SQL_INTEGER, &ordernumber, (SQLLEN) sizeof(ordernumber), NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDCOL 1\n");
  ret=SQLBindCol(*stmt, 2, SQL_CHAR, (SQLCHAR*) orderdate, (SQLLEN) sizeof(orderdate), NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDCOL 2\n");
  ret=SQLBindCol(*stmt, 3, SQL_CHAR, (SQLCHAR*) stat, (SQLLEN) sizeof(stat), NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDCOL 3\n");
  ret=SQLBindCol(*stmt, 4, SQL_CHAR, (SQLCHAR*) pcode, (SQLLEN) sizeof(pcode), NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDCOL 4\n");
  ret=SQLBindCol(*stmt, 5, SQL_INTEGER, &qord, (SQLLEN) sizeof(qord), NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDCOL 5\n");
  ret=SQLBindCol(*stmt, 6, SQL_DOUBLE, &price, (SQLLEN) sizeof(price), NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDCOL 6\n");
  ret=SQLBindCol(*stmt, 7, SQL_DOUBLE, &subtotal, (SQLLEN) sizeof(price), NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDCOL 7\n");


  query_ordersDetailsInterface(stmt, &ordernumber, (SQLCHAR*) orderdate, (SQLCHAR*) stat, (SQLCHAR*) pcode, &qord, &price, &subtotal);

  ret=SQLCloseCursor(*stmt);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLCLOSECURSOR\n");

  if(fflush(out)!=0) printf("ERROR FFLUSH");

  return 0;
}

static void query_ordersDetailsInterface(SQLHSTMT *stmt, SQLINTEGER *odnum, SQLCHAR *oddate, SQLCHAR *st, SQLCHAR *pc, SQLINTEGER *q, SQLDOUBLE *price, SQLDOUBLE *subtotal){
  int nreg=0, pag=0, j=0;
  double total=0.0;
  SQLRETURN ret;
  char t[]="\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nProduct Code\t| Quantity \t| Price per Unit\n", input=' ', num[50]=" \n";
  FILE *f=NULL;

  f=fopen(TEMP_FILE,"w");
  if(f==NULL) return;

  /*Primero calculamos el total, sumando la columna subtotal*/
  while(SQL_SUCCEEDED(ret = SQLFetch(*stmt))){
    (void)snprintf(num,50, "%.2lf", *((double *) price));
    for(j=0;j<(int)strlen(num);j++) 
      if(num[j]==',') 
        num[j]='.';

    fprintf(f,"%s\t| %d\t| %s\n", (char*) pc , *((int*) q), num);
    total += (((double) *subtotal)) ;
    nreg++;
  }

  (void)fclose(f);

  

  if(nreg==0){
    printf("\n");
    printf("\n < No order with number \'%d\' >\n\n", (int) *odnum);
    querys_deleteFile(TEMP_FILE);
    return;
  }

  (void)snprintf(num, 50,"%.2lf", total);
    for(j=0;j<(int)strlen(num);j++) 
      if(num[j]==',') 
        num[j]='.';

  printf("\nOrder Date: %s  --- Status: %s\n", (char *)oddate, (char *)st);
  printf("Total sum = %s\n", num);


  printf("> Press q for exit or any other to continue > ");
  querys_input(&input);


  do{
    

    querys_printPage(TEMP_FILE, pag, t);

    querys_input(&input);
    if(input=='<'&&pag>0) pag--;
    else if(input=='>'&&pag<(nreg/10)) pag++;

  }while(input!='q');

  printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

  querys_deleteFile(TEMP_FILE);

  return;
}















/*
 *  CUSTOMERS FIND
 */
/*
select c.customernumber, c.customername, c.contactfirstname, c.contactlastname from customers c where (c.contactfirstname like '%Mary%') or (c.contactlastname like '%Mary%') order by c.customernumber;*/
int query_customersFind(SQLHSTMT *stmt, FILE *out){
  SQLRETURN ret; /* ODBC API return status */
  char cnumber[MY_CHAR_LEN]= "\n", cname[MY_CHAR_LEN]= "\n", cfirstn[MY_CHAR_LEN]= "\n", csecondn[MY_CHAR_LEN]= "\n";
  char string[MY_CHAR_LEN], query[]="select c.customernumber, c.customername, c.contactfirstname, c.contactlastname from customers c where position(UPPER(?) in UPPER(c.contactfirstname))>0 or position(UPPER(?) in UPPER(c.contactlastname))>0 order by c.customernumber";



  if(!stmt || !out) return 1;


  if(fflush(out)!=0) printf("ERROR FFLUSH");
  if(scanf("%s", string)==EOF) printf("ERROR SCANF");


  ret=SQLPrepare((*stmt), (SQLCHAR*) query, SQL_NTS);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLPREPARE %d\n", ret);


  ret=SQLBindParameter((*stmt), 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, string, 0, NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDPARAMETER\n");
  ret=SQLBindParameter((*stmt), 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, string, 0, NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDPARAMETER\n");

  /* Ejecuta la query */
  ret=SQLExecute(*stmt);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLEXECUTE\n");

  /* Asigna la columna resultado a la variable y */
  ret=SQLBindCol(*stmt, 1, SQL_CHAR, cnumber, (SQLLEN) sizeof(cnumber), NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDCOL 1\n");
  /* Asigna la columna resultado a la variable y */
  ret=SQLBindCol(*stmt, 2, SQL_CHAR, cname, (SQLLEN) sizeof(cname), NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDCOL 2\n");

  ret=SQLBindCol(*stmt, 3, SQL_CHAR, cfirstn, (SQLLEN) sizeof(cfirstn), NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDCOL 3\n");

  ret=SQLBindCol(*stmt, 4, SQL_CHAR, csecondn, (SQLLEN) sizeof(csecondn), NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDCOL 4\n");

  /* Interfaz */
  query_customersFindInterface(stmt, (SQLCHAR*) cnumber, (SQLCHAR*) cname, (SQLCHAR*) cfirstn, (SQLCHAR*) csecondn, string);

  ret=SQLCloseCursor(*stmt);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLCLOSECURSOR\n");

  if(fflush(out)!=0) printf("ERROR FFLUSH");

  return 0;
}


void query_customersFindInterface(SQLHSTMT *stmt, SQLCHAR *cnum, SQLCHAR *cname, SQLCHAR *cfn, SQLCHAR *csn,char *string){
  SQLRETURN ret;
  FILE *f=NULL;
  int pag=0, nreg=0;
  char titulo[]="Number | Customer Name \t| Customer First Name\t| Customer Second Name\n", input=' ';


  if(!stmt||!cnum||!cname||!cfn||!csn||!string){
    printf("INTERFACE FAILURE\n");
    return;
  }

  f=fopen(TEMP_FILE, "w");
  if(!f) return;

  while(SQL_SUCCEEDED(ret = SQLFetch(*stmt))){
    fprintf(f,"%s    | %s\t| %s\t| %s\n", (char*) cnum, (char*) cname, (char*)cfn, (char*)csn);
    nreg++;
  }

  (void)fclose(f);

  if(nreg==0){
    printf("\n");
    printf("\n < No product named \'%s\' >\n\n",string);
    querys_deleteFile(TEMP_FILE);
    return;
  }


  do{

    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

    querys_printPage(TEMP_FILE, pag, titulo);

    querys_input(&input);
    if(input=='<'&&pag>0) pag--;
    else if(input=='>'&&pag<(nreg/10)) pag++;

  }while(input!='q');

  printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

  querys_deleteFile(TEMP_FILE);

  /*stop();*/

  return;
}







/*
 * CUSTOMERS LISTPRODUCTS
 */

/*select p.productname, sum(od.quantityordered) from customers c join orders o on c.customernumber=o.customernumber join orderdetails od on o.ordernumber=od.ordernumber join products p on od.productcode=p.productcode where c.customernumber=? group by p.productcode, p.productname order by p.productcode;*/
int query_customersListProducts(SQLHSTMT *stmt, FILE *out){
  SQLRETURN ret; /* ODBC API return status */
  char pname[MY_CHAR_LEN]= "\n";
  char query[]="select p.productname, sum(od.quantityordered) from customers c join orders o on c.customernumber=o.customernumber join orderdetails od on o.ordernumber=od.ordernumber join products p on od.productcode=p.productcode where c.customernumber=? group by p.productcode, p.productname order by p.productcode";
  SQLINTEGER cnumber= (SQLINTEGER) 0, qordered= (SQLINTEGER) 0;


  if(!stmt || !out) return 1;


  if(fflush(out)!=0) printf("ERROR FFLUSH");
  if(scanf("%d", &cnumber)==EOF) printf("ERROR SCANF");


  ret=SQLPrepare((*stmt), (SQLCHAR*) query, SQL_NTS);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLPREPARE %d\n", ret);


  ret=SQLBindParameter((*stmt), 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &cnumber, 0, NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDPARAMETER\n");

  /* Ejecuta la query */
  ret=SQLExecute(*stmt);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLEXECUTE\n");

  /* Asigna la columna resultado a la variable y */
  ret=SQLBindCol(*stmt, 1, SQL_CHAR, pname, (SQLLEN) sizeof(pname), NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDCOL 1\n");
  /* Asigna la columna resultado a la variable y */
  ret=SQLBindCol(*stmt, 2, SQL_INTEGER, &qordered, (SQLLEN) sizeof(qordered), NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDCOL 2\n");


  /* Interfaz */
  query_customersListProductsInterface(stmt, cnumber, (SQLCHAR*)pname, &qordered);

  ret=SQLCloseCursor(*stmt);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLCLOSECURSOR\n");

  if(fflush(out)!=0) printf("ERROR FFLUSH");

  return 0;
}

void query_customersListProductsInterface(SQLHSTMT *stmt, SQLINTEGER cnum, SQLCHAR *pname, SQLINTEGER *qordered){
  SQLRETURN ret;
  FILE *f=NULL;
  int pag=0, nreg=0;
  char titulo[]="Product Name \t| Quantity Ordered\n", input=' ';


  f=fopen(TEMP_FILE, "w");
  if(!f) return;

  while(SQL_SUCCEEDED(ret = SQLFetch(*stmt))){
    fprintf(f,"%s %d\n", (char*) pname, (int) *qordered);
    nreg++;
  }

  (void)fclose(f);

  if(nreg==0){
    printf("\n");
    printf("\n < No product ordered by the customer with the code \'%d\' >\n\n",(int) cnum);
    querys_deleteFile(TEMP_FILE);
    return;
  }


  do{

    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

    querys_printPage(TEMP_FILE, pag, titulo);

    querys_input(&input);
    if(input=='<'&&pag>0) pag--;
    else if(input=='>'&&pag<(nreg/10)) pag++;

  }while(input!='q');

  printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");


  querys_deleteFile(TEMP_FILE);

  /*stop();*/

  return;
}










/*
 * CUSTOMERS BALANCE
 */

/*
select pay.suma-s.saldo as balance
from (select sum(pm.amount) as suma
  from payments pm
  where pm.customernumber=141
  group by pm.customernumber) as pay,
  (select sum(od.quantityordered*od.priceeach) as saldo
  from customers c
  join orders o on c.customernumber=o.customernumber
  join orderdetails od on o.ordernumber=od.ordernumber
  where c.customernumber=141
  group by c.customernumber) as s;
  */
int query_customersBalance(SQLHSTMT *stmt, FILE *out){
  SQLRETURN ret; /* ODBC API return status */
  char query[]="select pay.suma-s.saldo as balance from (select sum(pm.amount) as suma from payments pm where pm.customernumber=? group by pm.customernumber) as pay, (select sum(od.quantityordered*od.priceeach) as saldo from customers c join orders o on c.customernumber=o.customernumber join orderdetails od on o.ordernumber=od.ordernumber where c.customernumber=? group by c.customernumber) as s";
  SQLINTEGER cnumber= (SQLINTEGER) 0;
  SQLDOUBLE saldo= (SQLDOUBLE) 0.0;


  if(!stmt || !out) return 1;


  if(fflush(out)!=0) printf("ERROR FFLUSH");
  if(scanf("%d", &cnumber)==EOF) printf("ERROR SCANF");


  ret=SQLPrepare((*stmt), (SQLCHAR*) query, SQL_NTS);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLPREPARE %d\n", ret);


  ret=SQLBindParameter((*stmt), 1, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &cnumber, 0, NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDPARAMETER\n");
  ret=SQLBindParameter((*stmt), 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &cnumber, 0, NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDPARAMETER\n");

  /* Ejecuta la query */
  ret=SQLExecute(*stmt);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLEXECUTE\n");

  /* Asigna la columna resultado a la variable y */
  ret=SQLBindCol(*stmt, 1, SQL_DOUBLE, &saldo, (SQLLEN) sizeof(saldo), NULL);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLBINDCOL 2\n");


  /* Interfaz */
  query_customersBalanceInterface(stmt, cnumber, &saldo);

  ret=SQLCloseCursor(*stmt);
  if(!SQL_SUCCEEDED(ret)) printf("ERROR SQLCLOSECURSOR\n");

  if(fflush(out)!=0) printf("ERROR FFLUSH");

  return 0;
}

void query_customersBalanceInterface(SQLHSTMT *stmt, SQLINTEGER cnum, SQLDOUBLE *saldo){
  SQLRETURN ret;
  char num[20]= " \n";
  int j=0;

  if(!stmt||!saldo){
    printf("INTERFACE FAILURE\n");
    return;
  }

  if(SQL_SUCCEEDED(ret = SQLFetch(*stmt))) {

    (void) snprintf(num,20,"%.2lf", (double) *saldo);
    for(j=0;j<(int)strlen(num);j++) 
      if(num[j]==',') 
        num[j]='.';

    printf("\n\n   Balance = %s\n\n", num);

  }else{
    printf("\n\n < No customer with code \'%d\' >\n\n",(int) cnum);
  }

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
