/**
 *	@file object.h Object: definição e operações com primitivas.
 *		As primitivas suportadas atualmente são: esferas, triângulos e paralelepípedos.
 *
 *	@author
 *			- Maira Noronha
 *			- Thiago Bastos
 *			- Mauricio Carneiro
 *
 *	@date
 *			Criado em:			01 de Dezembro de 2002
 *			Última Modificação:	22 de Janeiro de 2003
 *
 *	@version 2.0
 */

#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "color.h"
#include "algebra.h"
#include "material.h"


/************************************************************************/
/* Tipos Exportados                                                     */
/************************************************************************/
/**
 *   Tipo objeto
 */
struct _Object
{
    /**
     *  Tipo do objeto.
     */
	int type;
	/**
     *  Material do objeto.
     */
	int material;
	/**
     *  Dados do objeto.
     */
	void *data;
};

/**
 *   Objeto esfera.
 */
typedef struct
{
	
    /**
	 *  Posição do centro da esfera na cena.
	 */
	Vector center;
	  
	/**
	 *  Raio da esfera.
	 */
	double radius;
}
Sphere;

/**
 *   Objeto triângulo.
 */
typedef struct
{	
	/**
	 *  Primeiro vértice do triângulo.
	 */
	Vector v0;
	/**
	 *  Segundo vértice do triângulo.
	 */
	Vector v1;
	/**
	 *  Terceiro vértice do triângulo.
	 */
	Vector v2;
	
	Vector tex0;  /* coordenada de textura do verive 0 */
	Vector tex1;  /* coordenada de textura do verive 1 */
	Vector tex2;  /* coordenada de textura do verive 2 */
}
Triangle;

/**
 *   Objeto caixa.
 */
typedef struct
{	
	/**
	 *  Vértice de baixo e à esquerda do paralelepípedo.
	 */
	Vector bottomLeft;	
	/**
	 *  Vértice de cima e à direita do paralelepípedo.
	 */
	Vector topRight;
}
Box;

typedef struct _Object * Object;


/************************************************************************/
/* Funções Exportadas                                                   */
/************************************************************************/
/**
 *	Cria uma esfera.
 *
 *	@param material Id do material da esfera.
 *	@param center Posição do centro da esfera na cena.
 *	@param radius Raio da esfera.
 *
 *	@return Handle para o objeto criado.
 */
Object objCreateSphere( int material, const Vector center, double radius );

/**
 *	Cria um triângulo.
 *
 *	@param material Id do material do triângulo.
 *	@param v0 Primeiro vértice do triângulo.
 *	@param v1 Segundo vértice do triângulo.
 *	@param v2 Terceiro vértice do triângulo.
 *	@param tex0 Coordenadas de textura do primeiro vértice do triângulo.
 *	@param tex1 Coordenadas de textura do segundo vértice do triângulo.
 *	@param tex2 Coordenadas de textura do terceiro vértice do triângulo.
 *
 *	@return Handle para o objeto criado.
 */
Object objCreateTriangle( int material, const Vector v0, const Vector v1, const Vector v2, 
						                const Vector tex0, const Vector tex1, const Vector tex2 );

/**
 *	Cria um paralelepípedo.
 *
 *	@param material Id do material do paralelepípedo.
 *	@param bottomLeft Vértice de baixo e à esquerda do paralelepípedo.
 *	@param topRight Vértice de cima e à direita do paralelepípedo.
 *
 *	@return Handle para o objeto criado.
 */
Object objCreateBox( int material, const Vector bottomLeft, const Vector topRight );

/**
 *	Calcula a que distância um raio intercepta um objeto.
 *
 *	@param object Handle para um objeto.
 *	@param eye Origem do raio.
 *	@param ray Direção do raio.
 *
 *	@return Distância de eye até a superfície do objeto no ponto onde ocorreu a
 *				interseção. Menor ou igual a zero se não houver interseção.
 */
double objIntercept( Object object, Vector eye, Vector ray );

/**
 *	Calcula o vetor normal a um objeto em um ponto.
 *
 *	@param object Handle para um objeto.
 *	@param point Ponto na superfície do objeto onde a normal deve ser calculada.
 *
 *	@return Vetor unitário, normal ao objeto, com origem em point.
 */
Vector objNormalAt( Object object, Vector point );

/**
 *	Calcula a coordenada de textura para um objeto em um ponto.
 *
 *	@param object Handle para um objeto.
 *	@param point Ponto na superfície do objeto para onde uma coordenada de textura
 *					será calculada.
 *
 *	@return Coordenada de textura para o objeto no ponto especificado.
 */
Vector objTextureCoordinateAt( Object object, Vector point );

/**
 *	Obtém o Material de um objeto.
 */
int objGetMaterial( Object object );

/**
 *	Destrói um objeto criado com as funções objCreate*().
 */
void objDestroy( Object object );

Vector objExit( Object object, Vector point, Vector d, Vector eye);

#endif
