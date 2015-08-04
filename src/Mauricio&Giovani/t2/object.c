/**
 *	@file object.c Object: definição e operações com primitivas.
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

#include "scene.h"
#include "camera.h"
#include "object.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/************************************************************************/
/* Constantes Privadas                                                  */
/************************************************************************/
#define MIN( a, b ) ( ( a < b ) ? a : b )

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
Object objCreateSphere( int material, const Vector center, double radius )
{
	Object object;
	Sphere *sphere;

	object = (struct _Object *)malloc( sizeof(struct _Object) );
	sphere = (Sphere *)malloc( sizeof(Sphere) );

	sphere->center = center;
	sphere->radius = radius;

	object->type = TYPE_SPHERE;
	object->material = material;
	object->data = sphere;

	return object;
}


Object objCreateTriangle( int material, const Vector v0, const Vector v1, const Vector v2, 
						                const Vector tex0, const Vector tex1, const Vector tex2 )
{
	Object object;
	Triangle *triangle;

	object = (struct _Object *)malloc( sizeof(struct _Object) );
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


Object objCreateBox( int material, const Vector bottomLeft, const Vector topRight )
{
	Object object;
	Box *box;

	object = (struct _Object *)malloc( sizeof(struct _Object) );
	box = (Box *)malloc( sizeof(Box) );

	box->bottomLeft = bottomLeft;
	box->topRight = topRight;

	object->type = TYPE_BOX;
	object->material = material;
	object->data = box;

	return object;
}


double objIntercept( Object object, Vector eye, Vector ray )
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

			if( divisor <= -EPSILON )
			{
				distance = ( dividend / divisor );
			}

			if( distance >= 1.0 )
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
				else
					return -1.0;
			}

			return distance;
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


Vector objNormalAt( Object object, Vector point )
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

Vector objTextureCoordinateAt( Object object, Vector point )
{
	if( object->type == TYPE_SPHERE )
	{
		Vector normal = objNormalAt( object, point );
		double phi = atan2( normal.y, normal.x );
		double theta = atan2( sqrt( ( normal.x * normal.x ) + ( normal.y * normal.y ) ), normal.z );

		return algVector( ( 3 * ( 1 + phi / M_PI ) ), ( 3 * ( 1 - theta / M_PI ) ), 0, 1 );
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



/* Triângulos - texture mapping */


	else if( object->type == TYPE_TRIANGLE )
	{
      Vector n = objNormalAt( object, point );  //unitario?
      double A1, A2, A3, Ar;
      double L1, L2, L3;

      Triangle *triangle = (Triangle *)object->data;
      double u1,u2,u3, v1,v2,v3, u,v;
      Vector p1, p2, p3, pi;
      Vector v23, v31, v12;

      u1 = triangle->tex0.x ;
      v1 = triangle->tex0.y ;
      u2 = triangle->tex1.x ;
      v2 = triangle->tex1.y ;
      u3 = triangle->tex2.x ;
      v3 = triangle->tex2.y ;

      p1 = triangle->v0;
      p2 = triangle->v1;
      p3 = triangle->v2;
      pi = point;

      v23 = algSub(p2,p3);
      v31 = algSub(p3,p1);
      v12 = algSub(p1,p2);
         
      A1 = algDot( n , algCross(v23, algSub(pi,p2)) );
      //A1 = A1/2.0;
      A2 = algDot( n , algCross(v31, algSub(pi,p3)) );
      //A2 = A2/2.0;
      A3 = algDot( n , algCross(v12, algSub(pi,p1)) );
      //A3 = A3/2.0;

      Ar = A1 + A2 + A3;

      L1 = A1 / Ar;
      L2 = A2 / Ar;
      //L3 = A3 / Ar;
      L3 = 1 - (L1 + L2);

      u= (L1*u1) + (L2*u2) + (L3*u3);
      v= (L1*v1) + (L2*v2) + (L3*v3);
		
		return algVector( u, v, 0, 1 );
	} 



	/* Tipo de Objeto Inválido: nunca deve acontecer */
	return algVector( 0, 0, 0, 1 );	
}



int objGetMaterial( Object object )
{
	return object->material;
}

void objDestroy( Object object )
{
	free( object );
}




Vector objExit( Object object, Vector point, Vector d, Vector eye)
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
            
            if( (( -b + root ) / ( 2 * a ) ) > ( ( -b - root ) / ( 2.0 * a ))  )
               distance = (( -b + root ) / ( 2 * a ) );
            else
               distance = ( ( -b - root ) / ( 2.0 * a ) );

			
               //distance = MAX( ( ( -b + root ) / ( 2 * a ) ), ( ( -b - root ) / ( 2.0 * a ) )  );
			}

			return algAdd(point, algScale(distance, d)) ;
		}
   }	
   //case TYPE_TRIANGLE:
	//case TYPE_BOX:
	return point ;
}

