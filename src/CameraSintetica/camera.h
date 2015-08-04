/**
 *	@file camera.h Camera*: definição de câmeras e cálculos relacionados.
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

typedef struct _Camera Camera;


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
Camera* camCreate( Vector eye, Vector at, Vector up, double fovy, double nearp, double farp,
					int screenWidth, int screenHeight );

/**
 *	Obtém informações sobre o far plane definido para uma câmera.
 *
 *	@param origin [out]Retorna a origem do far plane.
 *	@param normal [out]Retorna um vetor normal ao far plane.
 *	@param u [out]Retorna o vetor u do far plane.
 *	@param v [out]Retorna o vetor v do far plane.
 */
void camGetFarPlane( Camera* camera, Vector *origin, Vector *normal, Vector *u, Vector *v );

/**
 *	Obtém a posição de uma câmera.
 *
 *	@return Posição do 'eye' da câmera.
 */
Vector camGetEye( Camera* camera );

/**
 *	Obtém a distancia focal tomada como a distancia do plano near.
 *
 *	@return distancia do plano near.
 */
double camGetFocalDistance( Camera* camera );


/**
 *	Obtém o eixo X de uma câmera.
 *
 *	@return eixo X da câmera.
 */
Vector camGetAxisX( Camera* camera );

/**
 *	Obtém o eixo Y de uma câmera.
 *
 *	@return eixo Y da câmera.
 */
Vector camGetAxisY( Camera* camera );

/**
 *	Obtém o eixo Z de uma câmera.
 *
 *	@return eixo Z da câmera.
 */
Vector camGetAxisZ( Camera* camera );

/**
 *	Obtém um raio saindo do eye de uma câmera e passando por um pixel especificado.
 *
 *	@param x Posição X do pixel por onde o raio deve passar.
 *	@param y Posição Y do pixel por onde o raio deve passar.
 *
 *	@return Um raio saindo de eye e passando pelo pixel (x,y) no near plane.
 */
Vector camGetRay( Camera* camera, double x, double y );

/**
 *	Obtém a largura da tela de uma câmera, em pixels.
 */
int camGetScreenWidth( Camera* camera );

/**
 *	Obtém a altura da tela de uma câmera, em pixels.
 */
int camGetScreenHeight( Camera* camera );

/**
 *	Destrói uma câmera criada com camCreate().
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

