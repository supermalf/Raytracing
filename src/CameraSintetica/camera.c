/**
 *	@file camera.c Camera*: definição de câmeras e cálculos relacionados.
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

#include "camera.h"
#include "algebra.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>


/************************************************************************/
/* Constantes Privadas                                                  */
/************************************************************************/
#define M_PI	3.14159265358979323846

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



Vector eye0,at0,up0;
double fovy0,nearp0,farp0;
double screenWidth0,screenHeight0;

/************************************************************************/
/* Funções Privadas                                                     */
/************************************************************************/
static void computeInternalParameters( Camera* camera );


/************************************************************************/
/* Definição das Funções Exportadas                                     */
/************************************************************************/
Camera* camCreate( Vector eye, Vector at, Vector up, double fovy, double nearp, double farp,
					int screenWidth, int screenHeight )
{
	Camera* camera = (struct _Camera *)malloc( sizeof(struct _Camera) );

	/* define os parametros primariios */
	camera->eye = eye;
	camera->at = at;
	camera->up = up;
	camera->fovy = fovy;
	camera->nearp = nearp;
	camera->farp = farp;
	camera->screenWidth = screenWidth;
	camera->screenHeight = screenHeight;

	/* define os valores derivados */
	computeInternalParameters( camera );

   /* salva estes parametros para funcao reset */
	eye0 = eye;
	at0 = at;
	up0 = up;
	fovy0 = fovy;
	nearp0 = nearp;
	farp0 = farp;
	screenWidth0 = screenWidth;
	screenHeight0 = screenHeight;


	return camera;
}

void camGetFarPlane( Camera* camera, Vector *origin, Vector *normal, Vector *u, Vector *v )
{
	*origin = camera->farOrigin;
	*normal = camera->farNormal;
	*u = camera->farU;
	*v = camera->farV;	
}

Vector camGetEye( Camera* camera )
{
	return camera->eye;
}

double camGetFocalDistance( Camera* camera )
{
   return camera->screenHeight/(2*tan( ( M_PI * camera->fovy ) / ( 2.0 * 180.0 ) ));
}

Vector camGetAxisX( Camera* camera )
{
   return camera->xAxis;
}

Vector camGetAxisY( Camera* camera )
{
   return camera->yAxis;
}

Vector camGetAxisZ( Camera* camera )
{
   return camera->zAxis;
}


Vector camGetRay( Camera* camera, double x, double y )
{
	Vector u = algScale( ( x / camera->screenWidth ), camera->nearU );
	Vector v = algScale( ( y / camera->screenHeight ), camera->nearV );
	Vector point = algAdd( algAdd( camera->nearOrigin, u ), v );

	return algUnit( algSub( point, camera->eye ) );
}

int camGetScreenWidth( Camera* camera )
{
	return (int)camera->screenWidth;
}

int camGetScreenHeight( Camera* camera )
{
	return (int)camera->screenHeight;
}


void camDestroy( Camera* camera )
{
	free( camera );
}


/************************************************************************/
/* Definição das Funções Privadas                                       */
/************************************************************************/
static void computeInternalParameters( Camera* camera )
{
	double sx, sy, sz;
  
   /* Calcula sistema de coordenadas da câmera */
	camera->zAxis = algUnit( algSub( camera->eye, camera->at ) );
	camera->xAxis = algUnit( algCross( camera->up, camera->zAxis ) );
	camera->yAxis = algCross( camera->zAxis, camera->xAxis );


	/* Calcula a origem do near plane */
	sz = camera->nearp;
	sy = ( sz * tan( ( M_PI * camera->fovy ) / ( 2.0 * 180.0 ) ) );
	sx = ( ( sy * camera->screenWidth ) / camera->screenHeight );
	camera->nearOrigin = algLinComb( 4,
								1.0, camera->eye,
								-sz, camera->zAxis,
								-sy, camera->yAxis,
								-sx, camera->xAxis );

	/* Calcula os eixos (u,v) do near plane */
	camera->nearU = algScale( ( 2 * sx ), camera->xAxis );
	camera->nearV = algScale( ( 2 * sy ), camera->yAxis );

	/* Calcula a origem do far plane */
	sz *= ( camera->farp / camera->nearp );
	sy *= ( camera->farp / camera->nearp );
	sx *= ( camera->farp / camera->nearp );
	camera->farOrigin = algLinComb( 4,
								1.0, camera->eye,
								-sz, camera->zAxis,
								-sy, camera->yAxis,
								-sx, camera->xAxis );

	/* Calcula os eixos (u,v) do far plane */
	camera->farU = algScale( ( 2 * sx ), camera->xAxis );
	camera->farV = algScale( ( 2 * sy ), camera->yAxis );
	camera->farNormal = algUnit( algCross( camera->farU, camera->farV ) );
}

/************************************************************************/
/* Definição das Funções para o OpenGL                                  */
/************************************************************************/
void camGetOGLPerspectiveParameters(Camera* camera,double* fovy, double* aspect, double* zNear, double* zFar)
{
	*fovy	= camera->fovy;
   *aspect = camera->screenWidth/camera->screenHeight;
	*zNear	= camera->nearp;
	*zFar	= camera->farp;
	return;
}

Matrix camGetOGLPerspectiveMatrix(Camera* camera) 
{
   double n = camera->nearp;
   double t = ( n * tan( ( M_PI * camera->fovy ) / ( 2.0 * 180.0 ) ) );
   double b = -t;
   double r = camera->screenWidth*t/camera->screenHeight;
   double l = -r;
   double f = camera->farp;

   Matrix P = algMatrix4x4(2*n/(r-l),   0.     , (r+l)/(r-l),      0.     ,
                              0.    , 2*n/(t-b), (t+b)/(t-b),      0.     ,
                              0.    ,   0.     ,-(f+n)/(f-n), -2*f*n/(f-n),
                              0.    ,   0.     ,    -1.    ,       0.      );
   return P;
}

void camGetOGLLookAtParameters(Camera* camera, double* eyex, double* eyey, double* eyez, 
				double* centerx, double* centery, double* centerz, 
            double* upx, double* upy, double* upz)
{
	*eyex		= camera->eye.x;
	*eyey		= camera->eye.y;
	*eyez		= camera->eye.z;
	*centerx	= camera->at.x;
	*centery	= camera->at.y;
	*centerz	= camera->at.z;
	*upx		= camera->up.x;
	*upy		= camera->up.y;
	*upz		= camera->up.z;
	return;
}

Matrix camGetOGLLookAtMatrix(Camera* camera) 
{
   Matrix R = algMatrix4x4(camera->xAxis.x,camera->xAxis.y,camera->xAxis.z,0.,
                           camera->yAxis.x,camera->yAxis.y,camera->yAxis.z,0.,
                           camera->zAxis.x,camera->zAxis.y,camera->zAxis.z,0.,
                                        0.,             0.,             0.,1.);
   Matrix T = algMatrixTransl(-camera->eye.x,-camera->eye.y,-camera->eye.z);

   return algMult(R,T);
}


Matrix camGetOGLViewportMatrix(Camera* camera) 
{
   double w = camera->screenWidth;
   double h = camera->screenHeight;

   Matrix V = algMatrix4x4(  w/2    ,   0.     ,  0.    ,      w/2  ,
                              0.    ,  h/2     ,  0.    ,      h/2  ,
                              0.    ,   0.     ,  0.5   ,      0.5  ,
                              0.    ,   0.     ,  0.    ,       1.  );
   return V;
}


void camMoveFoward(Camera* camera) {
   double d = algNorm(algSub(camera->at,camera->eye))/50.;
   Vector v = algScale(-d,camera->zAxis);
   camera->eye = algAdd(camera->eye,v);
   camera->at  = algAdd(camera->at,v);
   computeInternalParameters(camera);
}

void camMoveBackward(Camera* camera) {
   double d = algNorm(algSub(camera->at,camera->eye))/50.;
   Vector v = algScale(d,camera->zAxis);
   camera->eye = algAdd(camera->eye,v);
   camera->at  = algAdd(camera->at,v);
   computeInternalParameters(camera);
}

void camMoveUp(Camera* camera) {
   double d = algNorm(algSub(camera->at,camera->eye))/50.;
   Vector v = algScale(d,camera->yAxis);
   camera->eye = algAdd(camera->eye,v);
   camera->at  = algAdd(camera->at,v);
   computeInternalParameters(camera);
}

void camMoveDown(Camera* camera) {
   double d = algNorm(algSub(camera->at,camera->eye))/50.;
   Vector v = algScale(-d,camera->yAxis);
   camera->eye = algAdd(camera->eye,v);
   camera->at  = algAdd(camera->at,v);
   computeInternalParameters(camera);
}

void camMoveLeft(Camera* camera) {
   double d = algNorm(algSub(camera->at,camera->eye))/50.;
   Vector v = algScale(-d,camera->xAxis);
   camera->eye = algAdd(camera->eye,v);
   camera->at  = algAdd(camera->at,v);
   computeInternalParameters(camera);
}

void camMoveRight(Camera* camera) {
   double d = algNorm(algSub(camera->at,camera->eye))/50.;
   Vector v = algScale(d,camera->xAxis);
   camera->eye = algAdd(camera->eye,v);
   camera->at  = algAdd(camera->at,v);
   computeInternalParameters(camera);
}

void camPitchDown(Camera* camera) {
   Matrix T = algMatrixTransl(-camera->eye.x,-camera->eye.y,-camera->eye.z);
   Matrix R = algMatrixRotate(-5,camera->xAxis.x,camera->xAxis.y,camera->xAxis.z);
   Matrix Tinv = algMatrixTransl(+camera->eye.x,+camera->eye.y,+camera->eye.z);
   Matrix TMP = algMult(R,T);
   Matrix M   = algMult(Tinv,TMP);
   Vector new_at = algTransf(M,camera->at);
   Vector new_up = algTransf(R,camera->up);
   camera->at=new_at;
   camera->up=new_up;
   computeInternalParameters(camera);
}

void camPitchUp(Camera* camera) {
   Matrix T = algMatrixTransl(-camera->eye.x,-camera->eye.y,-camera->eye.z);
   Matrix R = algMatrixRotate(+5,camera->xAxis.x,camera->xAxis.y,camera->xAxis.z);
   Matrix Tinv = algMatrixTransl(+camera->eye.x,+camera->eye.y,+camera->eye.z);
   Matrix TMP = algMult(R,T);
   Matrix M   = algMult(Tinv,TMP);
   Vector new_at = algTransf(M,camera->at);
   Vector new_up = algTransf(R,camera->up);
   camera->at=new_at;
   camera->up=new_up;
   computeInternalParameters(camera);
}

void camYawRight(Camera* camera) {
   Matrix T = algMatrixTransl(-camera->eye.x,-camera->eye.y,-camera->eye.z);
   Matrix R = algMatrixRotate(-5,camera->yAxis.x,camera->yAxis.y,camera->yAxis.z);
   Matrix Tinv = algMatrixTransl(+camera->eye.x,+camera->eye.y,+camera->eye.z);
   Matrix TMP = algMult(R,T);
   Matrix M   = algMult(Tinv,TMP);
   Vector new_at = algTransf(M,camera->at);
   Vector new_up = algTransf(R,camera->up);
   camera->at=new_at;
   camera->up=new_up;
   computeInternalParameters(camera);
}

void camYawLeft(Camera* camera) {
   Matrix T = algMatrixTransl(-camera->eye.x,-camera->eye.y,-camera->eye.z);
   Matrix R = algMatrixRotate(+5,camera->yAxis.x,camera->yAxis.y,camera->yAxis.z);
   Matrix Tinv = algMatrixTransl(+camera->eye.x,+camera->eye.y,+camera->eye.z);
   Matrix TMP = algMult(R,T);
   Matrix M   = algMult(Tinv,TMP);
   Vector new_at = algTransf(M,camera->at);
   Vector new_up = algTransf(R,camera->up);
   camera->at=new_at;
   camera->up=new_up;
   computeInternalParameters(camera);
}

void camRolRight(Camera* camera) {
   Matrix T = algMatrixTransl(-camera->eye.x,-camera->eye.y,-camera->eye.z);
   Matrix R = algMatrixRotate(-5,camera->zAxis.x,camera->zAxis.y,camera->zAxis.z);
   Matrix Tinv = algMatrixTransl(+camera->eye.x,+camera->eye.y,+camera->eye.z);
   Matrix TMP = algMult(R,T);
   Matrix M   = algMult(Tinv,TMP);
   Vector new_at = algTransf(M,camera->at);
   Vector new_up = algTransf(R,camera->up);
   camera->at=new_at;
   camera->up=new_up;
   computeInternalParameters(camera);
}

void camRolLeft(Camera* camera) {
   Matrix T = algMatrixTransl(-camera->eye.x,-camera->eye.y,-camera->eye.z);
   Matrix R = algMatrixRotate(+5,camera->zAxis.x,camera->zAxis.y,camera->zAxis.z);
   Matrix Tinv = algMatrixTransl(+camera->eye.x,+camera->eye.y,+camera->eye.z);
   Matrix TMP = algMult(R,T);
   Matrix M   = algMult(Tinv,TMP);
   Vector new_at = algTransf(M,camera->at);
   Vector new_up = algTransf(R,camera->up);
   camera->at=new_at;
   camera->up=new_up;
   computeInternalParameters(camera);
}

void camReset(Camera* camera) {
	/* redefine os parametros primariios */
	camera->eye = eye0;
	camera->at = at0;
	camera->up = up0;
	camera->fovy = fovy0;
	camera->nearp = nearp0;
	camera->farp = farp0;
	camera->screenWidth = screenWidth0;
	camera->screenHeight = screenHeight0;

	/* define os valores derivados */
	computeInternalParameters( camera );
}
