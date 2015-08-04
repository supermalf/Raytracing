/**
 *	@file camera.h Camera*: defini��o de c�meras e c�lculos relacionados.
 *
 *	@author
 *			- Maira Noronha
 *			- Thiago Bastos
 *			- Mauricio Carneiro
 *
 *	@date
 *			Criado em:			30 de Novembro de 2002
 *			�ltima Modifica��o:	22 de Janeiro de 2003
 *
 *	@version 2.0
 */

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "algebra.h"


/************************************************************************/
/* Tipos Exportados                                                     */
/************************************************************************/

typedef struct _Camera Camera;


/************************************************************************/
/* Fun��es Exportadas                                                   */
/************************************************************************/
/**
 *	Cria uma nova c�mera usando as propriedades especificadas.
 *
 *	@param eye Posi��o da c�mera (do observador).
 *	@param at Dire��o para onde o observador est� olhando.
 *	@param up Dire��o perpendicular a at que indica a orienta��o vertical da c�mera.
 *	@param fovy Abertura da c�mera (�ngulo de vis�o), de 0 a 180.
 *	@param nearp Dist�ncia de eye at� nearp.
 *	@param farp Dist�ncia de eye at� farp.
 *	@param screenWidth Largura da tela em pixels.
 *	@param screenHeight Altura da tela em pixels.
 *
 *	@return Handle da c�mera criada.
 */
Camera* camCreate( Vector eye, Vector at, Vector up, double fovy, double nearp, double farp,
					int screenWidth, int screenHeight );

/**
 *	Obt�m informa��es sobre o far plane definido para uma c�mera.
 *
 *	@param origin [out]Retorna a origem do far plane.
 *	@param normal [out]Retorna um vetor normal ao far plane.
 *	@param u [out]Retorna o vetor u do far plane.
 *	@param v [out]Retorna o vetor v do far plane.
 */
void camGetFarPlane( Camera* camera, Vector *origin, Vector *normal, Vector *u, Vector *v );

/**
 *	Obt�m a posi��o de uma c�mera.
 *
 *	@return Posi��o do 'eye' da c�mera.
 */
Vector camGetEye( Camera* camera );

/**
 *	Obt�m a distancia focal tomada como a distancia do plano near.
 *
 *	@return distancia do plano near.
 */
double camGetFocalDistance( Camera* camera );


/**
 *	Obt�m o eixo X de uma c�mera.
 *
 *	@return eixo X da c�mera.
 */
Vector camGetAxisX( Camera* camera );

/**
 *	Obt�m o eixo Y de uma c�mera.
 *
 *	@return eixo Y da c�mera.
 */
Vector camGetAxisY( Camera* camera );

/**
 *	Obt�m o eixo Z de uma c�mera.
 *
 *	@return eixo Z da c�mera.
 */
Vector camGetAxisZ( Camera* camera );

/**
 *	Obt�m um raio saindo do eye de uma c�mera e passando por um pixel especificado.
 *
 *	@param x Posi��o X do pixel por onde o raio deve passar.
 *	@param y Posi��o Y do pixel por onde o raio deve passar.
 *
 *	@return Um raio saindo de eye e passando pelo pixel (x,y) no near plane.
 */
Vector camGetRay( Camera* camera, double x, double y );

/**
 *	Obt�m a largura da tela de uma c�mera, em pixels.
 */
int camGetScreenWidth( Camera* camera );

/**
 *	Obt�m a altura da tela de uma c�mera, em pixels.
 */
int camGetScreenHeight( Camera* camera );

/**
 *	Destr�i uma c�mera criada com camCreate().
 */
void camDestroy( Camera* camera );

/*
 * Obetem os parametros necessarios para a funcao glPerspective.
 */
void camGetOGLPerspectiveParameters(Camera* camera,double* fovy, double* aspect, double* zNear, double* zFar);


/*
 * Obtem a matriz Perspective da camera.
 */
Matrix camGetOGLPerspectiveMatrix(Camera* camera); 


/*
 * Obtem os parametros necessarios para a funcao glLookAt.
 */
void camGetOGLLookAtParameters(Camera* camera, double* eyex, double* eyey, double* eyez, 
				double* centerx, double* centery, double* centerz, 
            double* upx, double* upy, double* upz);

/*
 * Obtem a matriz de LookAt.
 */
Matrix camGetOGLLookAtMatrix(Camera* camera); 


/*
 * Obtem a matriz de Viewport.
 */
Matrix camGetOGLViewportMatrix(Camera* camera); 


/*
 * Move a camera.
 */ 
void camMoveFoward(Camera* camera);
void camMoveBackward(Camera* camera);
void camMoveUp(Camera* camera);
void camMoveDown(Camera* camera);
void camMoveRight(Camera* camera);
void camMoveLeft(Camera* camera);
void camPitchUp(Camera* camera);
void camPitchDown(Camera* camera);
void camYawRight(Camera* camera);
void camYawLeft(Camera* camera);
void camRolRight(Camera* camera);
void camRolLeft(Camera* camera);
void camReset(Camera* camera);


#endif

