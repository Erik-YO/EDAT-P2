CC = gcc
CFLAGS = -Wall -Wextra -pedantic -ansi -g
LDLIBS = -lodbc

OBJ=./obj
SRC=./src
INC=./include

PROG_NAME=menu




export PGDATABASE:=classicmodels
export PGUSER :=alumnodb
export PGPASSWORD :=alumnodb
export PGCLIENTENCODING:=LATIN9
export PGHOST:=localhost




DBNAME = classicmodels
PSQL = psql
CREATEDB = createdb
DROPDB = dropdb --if-exists
PG_DUMP = pg_dump
PG_RESTORE = pg_restore


all: dropdb createdb restore shell
createdb:
	@echo Creando BBDD
	@$(CREATEDB)
dropdb:
	@echo Eliminando BBDD
	@$(DROPDB) $(DBNAME)
	rm -f *.log
dump:
	@echo creando dumpfile
	@$(PG_DUMP) > $(DBNAME).sql
restore:
	@echo restore data base
	@cat $(DBNAME).sql | $(PSQL)
psql: shell
shell:
	@echo create psql shell
	@$(PSQL)



compile: $(OBJ)/menus.o $(OBJ)/querys.o $(OBJ)/odbc.o $(OBJ)/main.o
	@echo Compilando...
	gcc -o $(PROG_NAME) $(OBJ)/menus.o $(OBJ)/querys.o $(OBJ)/odbc.o $(OBJ)/main.o $(LDLIBS)
	@echo Creado ejecutable \"$(PROG_NAME)\"
	@echo


execute: compile
	@echo
	@echo Ejecutando...
	@echo
	./$(PROG_NAME)
	@echo
	@echo Ejecucion finalizada
	@echo



$(OBJ)/main.o: $(OBJ)/menus.o $(OBJ)/odbc.o $(SRC)/main.c
	gcc $(FLAGS) -I$(INC) -c $(SRC)/main.c -o $(OBJ)/main.o $(LDLIBS)

$(OBJ)/menus.o: $(SRC)/menus.c $(INC)/menus.h $(OBJ)/querys.o
	gcc $(FLAGS) -I$(INC) -c $(SRC)/menus.c -o $(OBJ)/menus.o $(LDLIBS)

$(OBJ)/querys.o: $(SRC)/querys.c $(INC)/querys.h
	gcc $(FLAGS) -I$(INC) -c $(SRC)/querys.c -o $(OBJ)/querys.o $(LDLIBS)

$(OBJ)/odbc.o: $(SRC)/odbc.c $(INC)/odbc.h
	gcc $(FLAGS) -I$(INC) -c $(SRC)/odbc.c -o $(OBJ)/odbc.o $(LDLIBS)





permisions:
	chmod +x *.sh

test: compile permisions
	./products_stock.sh
	./products_find.sh
	./orders_open.sh
	./orders_range.sh
	./orders_detail.sh
	./customers_find.sh
	./customers_list_products.sh
	./customers_balance.sh




##
## Para Generar el fichero splint.log, ejecutar las tres secuencialmente
##
extract:
	mv ./src/*.c ./
	mv ./include/*.h ./

splint:
	splint -nullpass *.c *.h > splint.log

insert:
	mv ./*.c ./src/
	mv ./*.h ./include/






clean :
	rm -f $(OBJ)/*.o ./$(PROG_NAME)
