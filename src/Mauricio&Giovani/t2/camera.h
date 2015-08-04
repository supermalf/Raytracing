/**
 *	@file camera.h Camera: definição de câmeras e cálculos relacionados.
 *
 *	@author
 *			- Maira Noronha
 *			- Thiago Bastos
 *			- Mauricio Carneiro
 *
 *	@date
 *			Criado em:			30 de Novembro de 2002
 *			Última Modificação:	22 de Janeiro de 2003
 *
 *	@version 2.0
 */

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "algebra.h"


/************************************************************************/
/* Tipos Exportados                                                     */
/************************************************************************/
/**
 *   Camera no mesmo formato do OpenGL.
 */
struct _Camera
{
	/* Definição da câmera */

    /**
     * Posição da câmera (do observador).
     */
	Vector eye;
	/**
	 * Direção para onde o observador está olhando.
	 */
	Vector at;
	/**
	 * Direção perpendicular a at que indica a orientação vertical da câmera.
	 */
	Vector up;
	/**
	 * Abertura da câmera (ângulo de visão), de 0 a 180.
	 */
	double fovy;
	/**
	 * Distância de eye até nearp (onde a cena será projetada).
	 */
	double nearp;
	/**
	 * Distância de eye até farp (background).
	 */
	double farp;
	/**
	 * Largura da tela em pixels.
	 */
	double screenWidth;
	/**
	 * Altura da tela em pixels
	 */
	double screenHeight;

	/* Estado interno */

	/**
	 * Direção x
	 */
	Vector xAxis;
	/**
	 * Direção y
	 */
	Vector yAxis;
	/**
	 * Direção z
	 */
	Vector zAxis;
	/**
	 * Origem do near plane.
	 */
	Vector nearOrigin;
	/**
	 * Vetor u do near plane.
	 */
	Vector nearU;
	/**
	 * Vetor v do near plane.
	 */
	Vector nearV;
	/**
	 * Origem do far plane.
	 */
	Vector farOrigin;
	/**
	 * Vetor normal ao far plane.
	 */
	Vector farNormal;
	/**
	 * Vetor u do far plane.
	 */
	Vector farU;
	/**
	 * Vetor v do far plane.
	 */
	Vector farV;
};

typedef struct _Camera * Camera;


/************************************************************************/
/* Funções Exportadas                                                   */
/************************************************************************/
/**
 *	Cria uma nova câmera usando as propriedades especificadas.
 *
 *	@param eye Posição da câmera (do observador).
 *	@param at Direção para onde o observador está olhando.
 *	@param up Direção perpendicular a at que indica a orientação vertical da câmera.
 *	@param fovy Abertura da câmera (ângulo de visão), de 0 a 180.
 *	@param nearp Distância de eye até nearp.
 *	@param farp Distância de eye até farp.
 *	@param screenWidth Largura da tela em pixels.
 *	@param screenHeight Altura da tela em pixels.
 *
 *	@return Handle da câmera criada.
 */
Camera camCreate( Vector eye, Vector at, Vector up, double fovy, double nearp, double farp,
					int screenWidth, int screenHeight );

/**
 *	Obtém informações sobre o far plane definido para uma câmera.
 *
 *	@param origin [out]Retorna a origem do far plane.
 *	@param normal [out]Retorna um vetor normal ao far plane.
 *	@param u [out]Retorna o vetor u do far plane.
 *	@param v [out]Retorna o vetor v do far plane.
 */
void camGetFarPlane( Camera camera, Vector *origin, Vector *normal, Vector *u, Vector *v );

/**
 *	Obtém a posição de uma câmera.
 *
 *	@return Posição do 'eye' da câmera.
 */
Vector camGetEye( Camera camera );

/**
 *	Obtém um raio saindo do eye de uma câmera e passando por um pixel especificado.
 *
 *	@param x Posição X do pixel por onde o raio deve passar.
 *	@param y Posição Y do pixel por onde o raio deve passar.
 *
 *	@return Um raio saindo de eye e passando pelo pixel (x,y) no near plane.
 */
Vector camGetRay( Camera camera, double x, double y );

/**
 *	Obtém a largura da tela de uma câmera, em pixels.
 */
int camGetScreenWidth( Camera camera );

/**
 *	Obtém a altura da tela de uma câmera, em pixels.
 */
int camGetScreenHeight( Camera camera );

/**
 *	Destrói uma câmera criada com camCreate().
 */
void camDestroy( Camera camera );

#endif

