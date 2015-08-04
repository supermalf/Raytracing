/*
 *	Computação Gráfica - Trabalho de Raytracing
 *	Professor Marcelo Gattass
 *
 *	@file main.c 
 *
 *	@author 
 *			- Maira Noronha
 *			- Thiago Bastos
 *
 *	@date
 *			Última Modificação:		10 de Fevereiro de 2003
 *
 *	@version 2.0
 *
 *	Este arquivo não precisa ser modificado.
 *	Veja o módulo raytracing.c
 */

#include "raytracing.h"
#include <stdio.h>
#include <time.h>


/*
 *	Imprime o tempo de renderizacao da cena.
 */
void displayRenderingTime( unsigned long begin, unsigned long end );
 
/*
 *	Reporta progresso de renderizacao.
 */
void reportProgress( int percentage );

/*
 *	Funcao principal.
 */
int main( int argc, char *argv[] )
{
	Scene scene;
	Image image;
	int result;
	unsigned long begin;
	unsigned long end;

	printf( "Computacao Grafica - Trabalho de Raytracing\n");

	/* Checa argumentos */
	if( argc != 3 )
	{
		printf( "Uso: %s <arquivo de entrada> <arquivo de saida>\n", argv[0] );
		return 1;
	}

	/* Le a cena especificada */
	scene = sceLoad( argv[1] );
	if( !scene )
	{
		printf( "ERRO: Nao foi possivel ler a cena do arquivo especificado (%s).\n", argv[1] );
		return 1;
	}

	/* Renderiza a cena */
	printf( "\nProgresso de renderizacao:   0%%" );

	begin = clock();
	
	image = rayTraceScene( scene, reportProgress );
	
	end = clock();

	sceDestroy( scene );

	/* Checa se a imagem obtida e valida */
	if( !image )
	{
		printf( "\n\nERRO: funcao rayTraceScene().\n" );
		return 1;
	}

	displayRenderingTime( begin, end );

	/* Salva imagem no arquivo especificado */
	result = imageWriteTGA( argv[2], image );	
	imageDestroy( image );

	if( !result )
	{
		printf( "\nERRO: Nao foi possivel escrever no arquivo de saida %s ", argv[2] );
		return 1;
	}

	return 0;
}

void displayRenderingTime( unsigned long begin, unsigned long end )
{
	unsigned long duration = (( end - begin )*1000)/CLOCKS_PER_SEC;

	unsigned long hours;
	unsigned long minutes;
	unsigned long seconds;
	unsigned long milliseconds;

	milliseconds = ( duration % 1000 );
	duration /= 1000;

	seconds = ( duration % 60 );
	duration /= 60;

	minutes = ( duration % 60 );
	duration /= 60;

	hours = duration;

	printf( "\nCena foi renderizada em " );

	if( hours )
	{
		printf( "%lu horas, %lu minutos, %lu segundos e %lu milisegundos.\n", hours, minutes, seconds, milliseconds );
	}
	else if( minutes )
	{
		printf( "%lu minutos, %lu segundos e %lu milisegundos.\n", minutes, seconds, milliseconds );
	}
	else
	{
		printf( "%lu segundos e %lu milisegundos.\n", seconds, milliseconds );
	}
}

void reportProgress( int percentage )
{
	printf( "\b\b\b\b%3i%%", percentage );
}

