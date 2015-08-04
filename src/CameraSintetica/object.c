/**
 *	@file object.c Object*: definição e operações com primitivas.
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

#include "object.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <windows.h>                /* inclui as definicoes do windows para o OpenGL */
#include <gl/gl.h>                  /* prototypes do OpenGL */
#include <gl/glu.h>                 /* prototypes do OpenGL */
#include <gl/glut.h>                 /* prototypes do OpenGL */


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
     *  Material* do objeto.
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
struct _Sphere
{
	
    /**
	 *  Posição do centro da esfera na cena.
	 */
	Vector center;
	  
	/**
	 *  Raio da esfera.
	 */
	double radius;
};


/**
 *   Objeto caixa.
 */
struct _Box
{	
	/**
	 *  Vértice de baixo e à esquerda do paralelepípedo.
	 */
	Vector bottomLeft;	
	/**
	 *  Vértice de cima e à direita do paralelepípedo.
	 */
	Vector topRight;
};

/**
 *   Objeto triângulo.
 */
struct _Triangle
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
};

/************************************************************************/
/* Constantes Privadas                                                  */
/************************************************************************/
#define MIN( a, b ) ( ( a < b ) ? a : b )
#define MAX( a, b ) ( ( a > b ) ? a : b )

#ifndef EPSILON
#define EPSILON	1.0e-3
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


enum
{
	TYPE_UNKNOWN,
	TYPE_SPHERE,
	TYPE_TRIANGLE,
	TYPE_BOX
};


/************************************************************************/
/* Definição das Funções Exportadas                                     */
/************************************************************************/
Object* objCreateSphere( int material, const Vector center, double radius )
{
	Object* object;
	Sphere *sphere;

	object = (Object *)malloc( sizeof(Object) );
	sphere = (Sphere *)malloc( sizeof(Sphere) );

	sphere->center = center;
	sphere->radius = radius;

	object->type = TYPE_SPHERE;
	object->material = material;
	object->data = sphere;

	return object;
}


Object* objCreateTriangle( int material, const Vector v0, const Vector v1, const Vector v2, 
						                const Vector tex0, const Vector tex1, const Vector tex2 )
{
	Object* object;
	Triangle *triangle;

	object = (Object *)malloc( sizeof(Object) );
	triangle = (Triangle *)malloc( sizeof(Triangle) );

	triangle->v0 = v0;
	triangle->v1 = v1;
	triangle->v2 = v2;

	triangle->tex0 = tex0;
	triangle->tex1 = tex1;
	triangle->tex2 = tex2;

	object->type = TYPE_TRIANGLE;
	object->material = material;
	object->data = triangle;

	return object;
}


Object* objCreateBox( int material, const Vector bottomLeft, const Vector topRight )
{
	Object* object;
	Box *box;

	object = (Object *)malloc( sizeof(Object) );
	box = (Box *)malloc( sizeof(Box) );

	box->bottomLeft = bottomLeft;
	box->topRight = topRight;

	object->type = TYPE_BOX;
	object->material = material;
	object->data = box;

	return object;
}


double objIntercept( Object* object, Vector eye, Vector ray )
{
	switch( object->type )
	{
	case TYPE_SPHERE:
		{
			Sphere *s = (Sphere *)object->data;

			double a, b, c, delta;
			double distance = -1.0;

			Vector fromSphereToEye;

			fromSphereToEye = algSub( eye, s->center );

			a = algDot( ray, ray );
			b = ( 2.0 * algDot( ray, fromSphereToEye ) );
			c = ( algDot( fromSphereToEye, fromSphereToEye ) - ( s->radius * s->radius ) );

			delta = ( ( b * b ) - ( 4 * a * c ) );

			if( fabs( delta ) <= EPSILON )
			{
				distance = ( -b / (2 * a ) );
			}
			else if( delta > EPSILON )
			{
				double root = sqrt( delta );
				distance = MIN( ( ( -b + root ) / ( 2 * a ) ), ( ( -b - root ) / ( 2.0 * a ) )  );
			}

			return distance;
		}

	case TYPE_TRIANGLE:
		{
			Triangle *t = (Triangle *)object->data;


			double dividend, divisor;
			double distance = -1.0;

			Vector v0ToV1 = algSub( t->v1, t->v0 );
			Vector v1ToV2 = algSub( t->v2, t->v1 );
			Vector normal = algCross( v0ToV1, v1ToV2 );
			Vector eyeToV0 = algSub( t->v0, eye );

			dividend = algDot( eyeToV0, normal );
			divisor = algDot( ray, normal );

			if( divisor <= -EPSILON )  /* garante que o raio ve o lado positivo */
			{
				distance = ( dividend / divisor );
			}

			if( distance >= 0.0001 )
			{
				double a0, a1, a2;

				Vector v2ToV0 = algSub( t->v0, t->v2 );
				Vector p = algAdd( eye, algScale( distance, ray ) );
				Vector n0 = algCross( v0ToV1, algSub( p, t->v0 ) );
				Vector n1 = algCross( v1ToV2, algSub( p, t->v1 ) );
				Vector n2 = algCross( v2ToV0, algSub( p, t->v2 ) );

				normal = algUnit(normal);
				a0 = ( 0.5 * algDot( normal, n0 ) );
				a1 = ( 0.5 * algDot( normal, n1 ) );
				a2 = ( 0.5 * algDot( normal, n2 ) );

				if ( (a0>0) && (a1>0) && (a2>0) )  
					return distance;
			}

			return -1.0;
		}

	case TYPE_BOX:
		{
			Box *box = (Box *)object->data;

			double xmin = box->bottomLeft.x;
		    double ymin = box->bottomLeft.y;
		    double zmin = box->bottomLeft.z;
		    double xmax = box->topRight.x;
		    double ymax = box->topRight.y;
		    double zmax = box->topRight.z;
			
			double x, y, z;
			double distance = -1.0;

			if( ray.x > EPSILON || -ray.x > EPSILON )
			{
				if( ray.x > 0 )
				{
					x = xmin;
					distance = ( ( xmin - eye.x ) / ray.x );
				}
				else
				{
					x = xmax;
					distance = ( ( xmax - eye.x ) / ray.x );
				}

				if( distance > EPSILON )
				{
					y = ( eye.y + ( distance * ray.y ) ); 
					z = ( eye.z + ( distance * ray.z ) ); 
                    if( ( y >= ymin ) && ( y <= ymax ) && ( z >= zmin ) && ( z <= zmax ) )
                        return distance;
				}
			}

			if( ray.y > EPSILON || -ray.y > EPSILON )
			{
				if( ray.y > 0 )
				{
					y = ymin;
					distance = ( ( ymin - eye.y ) / ray.y );
				}
				else
				{
					y = ymax;
					distance = ( ( ymax - eye.y ) / ray.y );
				}
				
				if( distance > EPSILON )
				{
					x = ( eye.x + ( distance * ray.x ) ); 
					z = ( eye.z + ( distance * ray.z ) ); 
				    if( ( x >= xmin ) && ( x <= xmax ) && ( z >= zmin ) && ( z <= zmax ) )
					    return distance;
				}

			}

			if( ray.z > EPSILON || -ray.z > EPSILON )
			{
				if( ray.z > 0 )
				{
					z = zmin;
					distance = ( (zmin - eye.z ) / ray.z );
				}
				else
				{
					z = zmax;
					distance = ( ( zmax - eye.z ) / ray.z );
				}

				if( distance > EPSILON )
				{
					x = ( eye.x + ( distance * ray.x ) ); 
					y = ( eye.y + ( distance * ray.y ) ); 
				    if( ( x >= xmin ) && ( x <= xmax ) && ( y >= ymin ) && ( y <= ymax ) )	
						return distance;
				}
			}

			return -1.0;
		}
	
	default:
		/* Tipo de Objeto Inválido: nunca deve acontecer */
		return -1.0;
	}
}

Vector objInterceptExit( Object* object, Vector point, Vector d )
{
	switch( object->type )
	{
	case TYPE_SPHERE:
		{
			Sphere *s = (Sphere *)object->data;

			double a, b, c, delta, distance;

			Vector fromSphereToEye;

			fromSphereToEye = algSub( point, s->center );

			a = algDot( d, d );
			b = ( 2.0 * algDot( d, fromSphereToEye ) );
			c = ( algDot( fromSphereToEye, fromSphereToEye ) - ( s->radius * s->radius ) );

			delta = ( ( b * b ) - ( 4 * a * c ) );

			if( fabs( delta ) <= EPSILON )
			{
				distance = ( -b / (2 * a ) );
			}
			else if( delta > EPSILON )
			{
				double root = sqrt( delta );
				distance = MAX( ( ( -b + root ) / ( 2 * a ) ), ( ( -b - root ) / ( 2.0 * a ) )  );
			}

			return algAdd(point, algScale(distance, d));
		}

	case TYPE_TRIANGLE:
	case TYPE_BOX:
		break;
	}
	return point;
}


Vector objNormalAt( Object* object, Vector point )
{
	if( object->type == TYPE_SPHERE )
	{
		Sphere *sphere = (Sphere *)object->data;

		return algScale( ( 1.0 / sphere->radius ),
					algSub( point, sphere->center ) );
	}
	else if ( object->type == TYPE_TRIANGLE )
	{
		Triangle *triangle = (Triangle *)object->data;

		return algCross( algSub( triangle->v1, triangle->v0 ),
					algSub( triangle->v2, triangle->v0 ) );
	}
	else if ( object->type == TYPE_BOX )
	{
		Box *box = (Box *)object->data;
		/* Seleciona a face mais próxima de point */
		if( fabs( point.x - box->bottomLeft.x ) < EPSILON )
		{
			return algVector( -1, 0, 0, 1  );
		}
		else if( fabs( point.x - box->topRight.x ) < EPSILON )
		{
			return algVector( 1, 0, 0, 1 );
		}
		else if( fabs( point.y - box->bottomLeft.y ) < EPSILON )
		{
			return algVector( 0, -1, 0, 1 );
		}
		else if( fabs( point.y - box->topRight.y ) < EPSILON )
		{
			return algVector( 0, 1, 0, 1 );
		}
		else if( fabs( point.z - box->bottomLeft.z ) < EPSILON )
		{
			return algVector( 0, 0, -1, 1 );
		}
		else if( fabs( point.z - box->topRight.z ) < EPSILON )
		{
			return algVector( 0, 0, 1, 1 );
		}
		else
		{
			return algVector( 0, 0, 0, 1 );
		}
	} 
	else
	{
		/* Tipo de Objeto Inválido: nunca deve acontecer */
		return algVector( 0, 0, 0, 1 );
	}
}

Vector objTextureCoordinateAt( Object* object, Vector point )
{
	if( object->type == TYPE_SPHERE )
	{
		Vector normal = objNormalAt( object, point );
		double phi = atan2( normal.y, normal.x );
		double theta = atan2( sqrt( ( normal.x * normal.x ) + ( normal.y * normal.y ) ), normal.z );

		return algVector(  ( 3 * ( 1 - theta / M_PI ) ), ( 3 * ( 1 + phi / M_PI ) ), 0, 1 );
	} 
	else if( object->type == TYPE_TRIANGLE )
	{
		Triangle *t = (Triangle *)object->data;
		Vector v0ToV1 = algSub( t->v1, t->v0 );
		Vector v1ToV2 = algSub( t->v2, t->v1 );
		Vector normal = algCross( v0ToV1, v1ToV2 );
		Vector p = point ;


		double a0, a1, a2 ,at;

		Vector v2ToV0 = algSub( t->v0, t->v2 );

		Vector n0 = algCross( v1ToV2, algSub( p, t->v1 ) );
		Vector n1 = algCross( v2ToV0, algSub( p, t->v2 ) );
		Vector n2 = algCross( v0ToV1, algSub( p, t->v0 ) );

		normal = algUnit(normal);
		a0 = ( 0.5 * algDot( normal, n0 ) );
		a1 = ( 0.5 * algDot( normal, n1 ) );
		a2 = ( 0.5 * algDot( normal, n2 ) );
		at = a0 + a1 + a2 ;
		a0 /= at;
		a1 /= at;
		a2 /= at;


		p= algLinComb(3,a0,t->tex0,a1,t->tex1,a2,t->tex2);

		return p;

	} 
	else if( object->type == TYPE_BOX )
	{
		Box *box = (Box *)object->data;

		double xmin = box->bottomLeft.x;
		double ymin = box->bottomLeft.y;
		double zmin = box->bottomLeft.z;
		double xmax = box->topRight.x;
		double ymax = box->topRight.y;
		double zmax = box->topRight.z;

		if( ( fabs( point.x - xmin ) < EPSILON ) || ( fabs( point.x - xmax ) < EPSILON ) )
		{
			return algVector( ( ( point.y - ymin ) / ( ymax - ymin ) ), ( ( point.z - zmin ) / ( zmax - zmin ) ), 0, 1 );
		}
		else if( ( fabs( point.y - ymin ) < EPSILON ) || ( fabs( point.y - ymax ) < EPSILON ) )
		{
			return algVector( ( ( point.z - zmin ) / ( zmax - zmin ) ), ( ( point.x - xmin ) / ( xmax - xmin ) ), 0, 1 );
		}
		else if( ( fabs( point.z - zmin ) < EPSILON ) || ( fabs( point.z - zmax ) < EPSILON ) )
		{
			return algVector( ( ( point.x - xmin ) / ( xmax - xmin ) ), ( ( point.y - ymin ) / ( ymax - ymin ) ), 0, 1 );
		}
	} 

	/* Tipo de Objeto Inválido: nunca deve acontecer */
	return algVector( 0, 0, 0, 1 );	
}

int objGetMaterial( Object* object )
{
	return object->material;
}

void objDestroy( Object* object )
{
	free( object );
}

void objDraw( Object* object )
{
	switch( object->type )
	{
 	case TYPE_SPHERE:
		{
			Sphere *s = (Sphere *)object->data;
			glTranslated(s->center.x,s->center.y,s->center.z);
            glutSolidSphere(s->radius,32,32);
			return;
		}
	case TYPE_TRIANGLE:
		{
			Triangle *t = (Triangle *)object->data;
			Vector n=algUnit(algCross(algSub(t->v1,t->v0),algSub(t->v2,t->v1)));
			float tmp[4];
			algGetOGLVector(n,tmp);
			glBegin(GL_TRIANGLES);
			  glNormal3fv(tmp);
			  glVertex4d(algGetX(t->v0),algGetY(t->v0),algGetZ(t->v0),1.0);
			  glVertex4d(algGetX(t->v1),algGetY(t->v1),algGetZ(t->v1),1.0);
			  glVertex4d(algGetX(t->v2),algGetY(t->v2),algGetZ(t->v2),1.0);
		    glEnd();
			return;
		}

	case TYPE_BOX:
		{
			Box *box = (Box *)object->data;
			double size = (box->topRight.x - box->bottomLeft.x);
		    double sy   = (box->topRight.y - box->bottomLeft.y)/size;
		    double sz   = (box->topRight.z - box->bottomLeft.z)/size;
		    double xc   = (box->topRight.x + box->bottomLeft.x)/2.0;
		    double yc   = (box->topRight.y + box->bottomLeft.y)/2.0;
		    double zc   = (box->topRight.z + box->bottomLeft.z)/2.0;
	
   	    	glTranslated(xc,yc,zc);
			glScaled(1.0,sy,sz);
			glutSolidCube(size);
		}
	default:
		/* Tipo de Objeto Inválido: nunca deve acontecer */
		return;
	}
}

