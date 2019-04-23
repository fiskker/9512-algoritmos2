#include <iostream>
#include <fstream>

#include "types.h"
#include "diccionario.h"
#include "process.h"

status_t compress(istream *iss, ostream *oss)
{	
	if(iss == NULL || oss == NULL)
		return ERROR_NULL_POINTER;	

	Diccionario dic;
	Simbolo buffer;

	size_t indice;
	unsigned char lec; 	
	char c;

	while(iss->get(c))
	{
		// Casteamos el char leído a unsigned char
		lec = static_cast<unsigned char>(c);

		// Parseamos lo leido al sufijo del buffer
		buffer.setSufijo(lec);

		// Buscamos si el símbolo se encuentra en el diccionario.
		if(!dic.buscarSimbolo(buffer, indice))
		{
			// Caso: No se encontro en el diccionario
			// Agregamos el buffer al diccionario
			
			dic.agregarSimbolo(buffer);

			// Envíamos el prefijo a la salida (std o archivo)
			*oss << buffer.getPrefijo() << TOKEN_SEPARATOR;

			// Buscamos el indice del sufijo
			indice = (size_t)buffer.getSufijo();
		}

		// Agregamos el indice del encontrado o el agregado
		buffer.setPrefijo(indice);
	}

	// Chequeamos si el bit fail esta encendido y el EOF no fue leído
	if(!iss->eof() && iss->fail())
        return ERROR_READ_FILE;

	else
		// Chequeamos que el prefijo no sea void, podria suceder que el archivo este vacio por lo que
		// nunca entra a la loop de lectura.
		if(buffer.getPrefijo() != VOID)
		{
			// Imprimimos el ultimo prefijo (al llegar a EOF)
			*oss << buffer.getPrefijo() << endl;			
		}


    return OK;		
}

status_t decompress(istream *iss, ostream *oss)
{
	if(iss == NULL || oss == NULL)
		return ERROR_NULL_POINTER;	
	
	// Creamos un diccionario y un simbolo buffer
	Diccionario dic;
	Simbolo buffer;

    status_t st;

	// Inicializamos los indices a vacío.
	size_t indice_ant = VOID;
	size_t indice_act = VOID;

	// Utilizamos este caracter para leer los separadores.
	char c;

	// Primer indice (no se agrega nada al diccionario):
	if(*iss >> indice_act)
	{
		// Buscamos el sufijo dado el indice
		unsigned char sfx;
		if( (st = dic.getSufijoByIndex(indice_act, sfx)) != OK)
		{
			return st;
		}

		// Imprimimos el sufijo
		*oss << sfx;

		// Actualizamos el indice
		indice_ant = indice_act;

		// Salteamos el separador
		*iss >> c;
	}
	else
        return ERROR_READ_FILE;

	// Comenzamos la lectura (desde el segundo)
	while(*iss >> indice_act)
	{
		// Agregamos el indice anterior al prefijo del buffer.
		buffer.setPrefijo(indice_ant);

		// Llamamos a la función reconstruir cadena para realizar la insercion del sufijo en el buffer y la impresión.
		if( (st = dic.reconstruirCadena(indice_act, buffer, oss, indice_ant)) != OK)
			return st;

		// Agregamos el simbolo al diccionario
		dic.agregarSimbolo(buffer);

		// Actualizamos el indice
		indice_ant = indice_act;

		// Salteamos la coma
		*iss >> c;
		if(c != TOKEN_SEPARATOR)
			return ERROR_FILE_FORMAT;
	}

	// Chequeamos si el bit fail esta encendido y el EOF no fue leído
	if(!iss->eof() && iss->fail())
        return ERROR_READ_FILE;

    return OK;
}