/**
 *	@file raytracing.c RayTracing: renderiza��o de cenas por raytracing.
 *
 *	@author 
 *			- Maira Noronha
 *			- Thiago Bastos
 *
 *	@date
 *			Criado em:			02 de Dezembro de 2002
 *			�ltima Modifica��o:		4 de Junho de 2003
 *
 *	@version 2.0
 */

#include <math.h>
#include <float.h>
#include <string.h>
#include <stdlib.h>

#include "raytracing.h"
#include "color.h"
#include "algebra.h"


/************************************************************************/
/* Constantes Privadas                                                  */
/************************************************************************/
#define MAX( a, b ) ( ( a > b ) ? a : b )

/** N�mero m�ximo de recurs�es permitidas */
#define MAX_DEPTH	6


/************************************************************************/
/* Fun��es Privadas                                                     */
/************************************************************************/
/**
 *	Obt�m uma cor atrav�s do tra�ado de um raio dentro de uma cena.
 *
 *	@param scene Handle para a cena sendo renderizada.
 *	@param eye Posi��o do observador, origem do raio.
 *	@param ray Dire��o do raio.
 *	@param depth Para controle do n�mero m�ximo de recurs�es. Fun��es clientes devem
 *					passar 0 (zero). A cada recurs�o depth � incrementado at�, no m�ximo,
 *					MAX_DEPTH. Quando MAX_DEPTH � atingido, recurs�es s�o ignoradas.
 *
 *	@return Cor resultante do tra�ado do raio.
 */
static Color shade( Scene* scene, Vector eye, Vector ray, Object* object, Vector point,
					Vector normal, int depth );

/**
 *	Encontra o primeiro objeto interceptado pelo raio originado na posi��o especificada.
 *
 *	@param scene Cena.
 *	@param eye Posi��o do Observador (origem).
 *	@param ray Raio sendo tra�ado (dire��o).
 *	@param object Onde � retornado o objeto resultante. N�o pode ser NULL.
 *	@return Dist�ncia entre 'eye' e a superf�cie do objeto interceptado pelo raio.
 *			DBL_MAX se nenhum objeto � interceptado pelo raio, neste caso
 *				'object' n�o � modificado.
 */
static double getNearestObject( Scene* scene, Vector eye, Vector ray, Object* *object );

/**
 *	Checa se objetos em uma cena impedem a luz de alcan�ar um ponto.
 *
 *	@param scene Cena.
 *	@param point Ponto sendo testado.
 *	@param rayToLight Um raio (dire��o) indo de 'point' at� 'lightLocation'.
 *	@param lightLocation Localiza��o da fonte de luz.
 *	@return Zero se nenhum objeto bloqueia a luz e n�o-zero caso contr�rio.
 */
static int isInShadow( Scene* scene, Vector point, Vector rayToLight, Vector lightLocation );


/************************************************************************/
/* Defini��o das Fun��es Exportadas                                     */
/************************************************************************/

Color rayTrace( Scene* scene, Vector eye, Vector ray, int depth )
{
	Object* object;
	double distance;

	Vector point;
	Vector normal;

	/* Calcula o primeiro objeto a ser atingido pelo raio */
	distance = getNearestObject( scene, eye, ray, &object );

	/* Se o raio n�o interceptou nenhum objeto... */
	if( distance == DBL_MAX )
	{
		return sceGetBackgroundColor( scene, eye, ray );
	}

	/* Calcula o ponto de interse��o do raio com o objeto */
	point = algAdd( eye, algScale( distance, ray ) );

	/* Obt�m o vetor normal ao objeto no ponto de interse��o */
	normal =  objNormalAt( object, point );

	return shade( scene, eye, ray, object, point, normal, depth );
}

/************************************************************************/
/* Defini��o das Fun��es Privadas                                       */
/************************************************************************/

static Color shade( Scene* scene, Vector eye, Vector ray, Object* object, Vector point,
					Vector normal, int depth )
{
	int i;
	double k;
	Light* light;
	Vector L,r,v;
	Color colorDifusa, colorEspecular, rcolor;

	Material* material = sceGetMaterial(scene,objGetMaterial(object));

	double reflectionFactor = matGetReflectionFactor( material );
	double specularExponent = matGetSpecularExponent( material );
	double refractedIndex   = matGetRefractionIndex( material );
	double opacity = matGetOpacity( material );

	Color ambient = sceGetAmbientLight( scene );
	Color diffuse = matGetDiffuse( material, objTextureCoordinateAt( object, point ) );	
	Color specular = matGetSpecular( material );

	/* Come�a com a cor ambiente */
	Color color = colorMultiplication( diffuse, ambient );

   for(i = 0; i < sceGetLightCount(scene); i++)
	{
		light = sceGetLight( scene, i );

		/*vetor unit�rio na posicao de point para a posicao da luz*/
		v = algUnit( algSub(eye, point));
		L = algUnit( algSub(lightGetPosition(light), point));
		normal = algUnit(normal);
		r = algReflect( L , normal ) ;

		if(algDot(L, normal) > 0)    
		{
			if(! isInShadow( scene , point , L, lightGetPosition(light) ) )
			{
				/*componente difusa*/
				colorDifusa = colorScale( algDot( normal , L), colorMultiplication( lightGetColor(light), diffuse ) );

				/*componente especular*/
				colorEspecular = colorScale( (pow(algDot(r, v), matGetSpecularExponent( material ))), colorMultiplication( lightGetColor(light) , specular ) ) ;
				color = colorAddition(color, colorAddition( colorDifusa, colorEspecular ));
			}
		}
	}

	/*incluindo efeito de superf�cies refletoras*/
	if(depth >= MAX_DEPTH) return color ;

	k = matGetReflectionFactor( material );

	if(k != 0)
	{
		/*raio na direcao de reflexao*/
		r = algUnit(algReflect(v, normal));

		/*Calcula a cor correspondente ao raio*/
		rcolor = rayTrace( scene, point, r, depth + 1);
		color = colorAddition(color, colorScale(k, rcolor));
	}

	/*incluindo efeito de superf�cies transparentes*/
	if(opacity < 1)
	{
		Color refractedColor;
		Vector refractedRay = algSnell(ray, normal, 1., refractedIndex);
		point = objInterceptExit(object, point, refractedRay);
		normal = algMinus(objNormalAt(object, point));
		refractedRay = algSnell(refractedRay, normal, refractedIndex, 1.);
		if(algNorm(refractedRay) > 1.e-4)
		{
			refractedColor = rayTrace(scene, point, refractedRay, depth + 1);
			color = colorAddition(color, colorScale((1 - opacity), refractedColor));
		}
	}

	return color;
}


static double getNearestObject( Scene* scene, Vector eye, Vector ray, Object** object )
{
	int i;
	int objectCount = sceGetObjectCount( scene );

	double closest = DBL_MAX;

	/* Para cada objeto na cena */
	for( i = 0; i < objectCount; ++i ) {
		Object* currentObject = sceGetObject( scene, i );
		double distance = objIntercept( currentObject, eye, ray );
		
		if( distance > 0.0 && distance < closest )
		{
			closest = distance;
			*object = currentObject;
		}
	}

	return closest;
}

static int isInShadow( Scene* scene, Vector point, Vector rayToLight, Vector lightLocation )
{
	int i;
	int objectCount = sceGetObjectCount( scene );

	/* maxDistance = dist�ncia de point at� lightLocation */
	double maxDistance = algNorm( algSub( lightLocation, point ) );

	/* Para cada objeto na cena */
	for( i = 0; i < objectCount; ++i )
	{
		double distance = objIntercept( sceGetObject( scene, i ), point, rayToLight );
		
		if( distance > 0.1 && distance < maxDistance )
		{
			return 1;
		}
	}

	return 0;
}

