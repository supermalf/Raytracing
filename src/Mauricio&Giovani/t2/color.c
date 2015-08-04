/**
 *	@file color.c Color: manipulação de cores.
 *
 *	@author 
 *			- Maira Noronha
 *			- Thiago Bastos
 *
 *	@date
 *			Criado em:			1 de Dezembro de 2002
 *			Última Modificação:	22 de Janeiro de 2003
 *
 *	@version 2.0
 */

#include "color.h"

#include <stdio.h>


/************************************************************************/
/* Constantes Privadas                                                  */
/************************************************************************/
#define MAX( a, b ) ( ( a > b ) ? a : b )
#define MIN( a, b ) ( ( a < b ) ? a : b )
#define CLAMP( intensity ) MAX( 0.0, MIN( 1.0, intensity ) )


/************************************************************************/
/* Definições das Funções Exportadas                                    */
/************************************************************************/
Color colorCreate3b( unsigned char red, unsigned char green, unsigned char blue )
{
	Color color= { ( red / 255.0 ), ( green / 255.0 ), ( blue / 255.0 ) };

	return color;
}

Color colorAddition( Color c1, Color c2 )
{
	Color color = { CLAMP( c1.red + c2.red ), CLAMP( c1.green + c2.green ), CLAMP( c1.blue + c2.blue ) };

	return color;
}

Color colorScale( double s, Color c )
{
	Color color = { CLAMP( s * c.red ), CLAMP( s * c.green ), CLAMP( s * c.blue ) };

	return color;
}

Color colorMultiplication( Color c1, Color c2 )
{
	Color color = { ( c1.red * c2.red ), ( c1.green * c2.green ), ( c1.blue * c2.blue ) };

	return color;
}

Color colorReflection( double s, Color l, Color m )
{
	Color color = { CLAMP( s * ( l.red * m.red ) ), CLAMP( s * ( l.green * m.green ) ), CLAMP( s * ( l.blue * m.blue ) ) };
	
	return color;
}

Color colorNormalize( Color c1 )
{
	Color color= { c1.red/255.0, c1.green/255.0, c1.blue/255.0 };

	return color;
}

