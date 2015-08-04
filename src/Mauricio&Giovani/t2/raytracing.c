/*============================================================================
 * 2o trabalho                                                               *
 * Curso de Computacao Grafica 2004.2, prof. Marcelo Gattass.                *
 * Aluno(s): Mauricio Ferreira e Giovani Tadei                               *
 *===========================================================================*/


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

   /** Número máximo de recursões permitidas */
   #define MAX_DEPTH	6


      int bump=0;
      int sShadow=0;
      int refr=0;


   /************************************************************************/
   /* Funções Privadas                                                     */
   /************************************************************************/
   /**
    *	Obtém uma cor através do traçado de um raio dentro de uma cena.
    *
    *	@param scene Handle para a cena sendo renderizada.
    *	@param eye Posição do observador, origem do raio.
    *	@param ray Direção do raio.
    *	@param depth Para controle do número máximo de recursões. Funções clientes devem
    *					passar 0 (zero). A cada recursão depth é incrementado até, no máximo,
    *					MAX_DEPTH. Quando MAX_DEPTH é atingido, recursões são ignoradas.
    *
    *	@return Cor resultante do traçado do raio.
    */
   static Color shade( Scene scene, Vector eye, Vector ray, Object object, Vector point,
					   Vector normal, int depth );

   /**
    *	Encontra o primeiro objeto interceptado pelo raio originado na posição especificada.
    *
    *	@param scene Cena.
    *	@param eye Posição do Observador (origem).
    *	@param ray Raio sendo traçado (direção).
    *	@param object Onde é retornado o objeto resultante. Não pode ser NULL.
    *	@return Distância entre 'eye' e a superfície do objeto interceptado pelo raio.
    *			DBL_MAX se nenhum objeto é interceptado pelo raio, neste caso
    *				'object' não é modificado.
    */
   static double getNearestObject( Scene scene, Vector eye, Vector ray, Object *object );

   /**
    *	Checa se objetos em uma cena impedem a luz de alcançar um ponto.
    *
    *	@param scene Cena.
    *	@param point Ponto sendo testado.
    *	@param rayToLight Um raio (direção) indo de 'point' até 'lightLocation'.
    *	@param lightLocation Localização da fonte de luz.
    *	@return Zero se nenhum objeto bloqueia a luz e não-zero caso contrário.
    */
   static int isInShadow( Scene scene, Vector point, Vector rayToLight, Vector lightLocation );


   /************************************************************************/
   /* Definição das Funções Exportadas                                     */
   /************************************************************************/

   Color rayTrace( Scene scene, Vector eye, Vector ray, int depth )
   {
	   Object object;
	   double distance;

	   Vector point;
	   Vector normal;

	   /* Calcula o primeiro objeto a ser atingido pelo raio */
	   distance = getNearestObject( scene, eye, ray, &object );

	   /* Se o raio não interceptou nenhum objeto... */
	   if( distance == DBL_MAX )
	   {
		   return sceGetBackgroundColor( scene, eye, ray );
	   }

	   /* Calcula o ponto de interseção do raio com o objeto */
	   point = algAdd( eye, algScale( distance, ray ) );

	   /* Obtém o vetor normal ao objeto no ponto de interseção */
	   normal =  objNormalAt( object, point );

	   return shade( scene, eye, ray, object, point, normal, depth );
   }

   /************************************************************************/
   /* Definição das Funções Privadas                                       */
   /************************************************************************/

   static Color shade( Scene scene, Vector eye, Vector ray, Object object, Vector point,
					   Vector normal, int depth )
   {
      int i;
      double prod,cos_alfa,cos_beta;


      Vector reverseRay = algMinus( ray );
      Vector L, N, R, V, Lnorm, Rnorm, Nnorm;
	  
	   Material material = sceGetMaterial(scene,objGetMaterial(object));
	  
	   double reflectionFactor = matGetReflectionFactor( material );
	   double specularExponent = matGetSpecularExponent( material );
      double opacityFactor    = matGetOpacity         ( material );
      double refractionFactor = matGetRefractionIndex ( material );
      
      int j;
      int fontes_aux = 8;
      double L_raio;
      double shadow_factor, light_factor;
      Vector L_aux[7];

      /*SShadow*/
      int lamps = 1;
      double lamppower = 1.0;
      Vector Lpos;
 




      /* Pegando parametros */

	   Color ambient  = sceGetAmbientLight( scene );
	   Color diffuse  = matGetDiffuse( material, objTextureCoordinateAt( object, point ) );	
	   Color specular = matGetSpecular( material );
	  

	   /* Começa com a cor ambiente */

	   Color color = colorMultiplication( diffuse, ambient );

	  
      /* Para cada LUZ */
     
      N = normal;
      V = reverseRay;

      for( i=0 ; i < sceGetLightCount( scene ); i++ )
      {
         Lpos  = lightGetPosition(sceGetLight(scene, i ));
         L     = algSub (Lpos, point) ;
         Lnorm = algUnit(L);
         Nnorm = algUnit(N);

         prod  = algDot (Lnorm, Nnorm);


      /* Soft Shadow */

         if(sShadow == 1)
         {

            L_raio = 7.5;
            L_aux[0] = L;

            /* Colocando fontes auxiliares no centro da esfera*/
            for(j=0;j<7;j++)
            {
               L_aux[j] = L;
            }

            L_aux[1].x = L.x + L_raio;
            L_aux[2].x = L.x - L_raio;
            L_aux[3].y = L.y + L_raio;
            L_aux[4].y = L.y - L_raio;
            L_aux[5].z = L.z + L_raio;
            L_aux[6].z = L.z - L_raio;

            lamps=7;
            lamppower=0.25;
    
         }

         /* Lampadas dividas pelo Soft Shadow*/
         for(j=0;j<lamps;j++) 
         {
            if(sShadow == 1)
            {
               Lnorm = algUnit(L_aux[j]);
               Lpos = L_aux[j];
            }


       /* Se o objeto estiver numa regiao obscura */

         if( prod > 0)
         {


      /* Sombra p/ objetos transp*/

      /* To generate shadows I used some simple equations that generated a softening effect. It is as follows

          opaque = 1-transparency
          shadowfactor = .25^opaque
          illumination value = shadowfactor * normal illumination 

      */
      if (isInShadow (scene, point, Lnorm, Lpos))
      {
         if(opacityFactor < 1.0)
         {

            shadow_factor = pow(2.55, opacityFactor);
            light_factor  = (2.55/fontes_aux);

            cos_alfa = prod / algNorm(Lnorm) * algNorm(N);

				color.red   += scene->lights[i]->color.red   * diffuse.red    - opacityFactor * (light_factor * shadow_factor) ;
				color.green += scene->lights[i]->color.green * diffuse.green  - opacityFactor * (light_factor * shadow_factor) ;
				color.blue  += scene->lights[i]->color.blue  * diffuse.blue   - opacityFactor * (light_factor * shadow_factor) ;
         }
         

      }

            /* Se a luz nao for bloqueada no ponto */

            if (! (isInShadow (scene, point, Lnorm, Lpos)))
            {
  
               if (j==0)
                  light_factor = lamppower;
               
               else
                  light_factor = (1.0/fontes_aux);


          /* Bump Mapping */
               
               if(bump == 1)
               {
               
                  /*confere se há textura*/

                  if ( (diffuse.blue  != material->diffuseColor.blue )&&
                       (diffuse.green != material->diffuseColor.green)&&
                       (diffuse.red != material->diffuseColor.red))
                  {
                     double lum;
                     Vector vetor_plano_1, vetor_plano_2, vetor_plano_ale ;
                     vetor_plano_1.x = N.x ;
                     vetor_plano_1.y = -(N.z) ;
                     vetor_plano_1.z = N.y ;


                     lum = (diffuse.red * 0.3) + (diffuse.green * 0.59) + (diffuse.blue * 0.11) ;                            


                     vetor_plano_2 = algCross ( N, vetor_plano_1);


                     vetor_plano_1 = algUnit(vetor_plano_1) ;
                     vetor_plano_2 = algUnit(vetor_plano_2) ;
                       

                     vetor_plano_ale = algAdd(vetor_plano_1, vetor_plano_2) ;
                     vetor_plano_ale = algUnit(vetor_plano_ale) ;
                     vetor_plano_ale = algScale (1-lum, vetor_plano_ale) ;
                     N = algAdd (Nnorm, vetor_plano_ale) ;
                     Nnorm = algUnit(N) ;
                     prod = algDot (Lnorm, Nnorm) ;
                  }
               }
   


               /* Componente Difusa */

               cos_alfa = prod / algNorm(Lnorm) * algNorm(Nnorm);


				   color.red   += scene->lights[i]->color.red   * diffuse.red   * cos_alfa * light_factor ;
				   color.green += scene->lights[i]->color.green * diffuse.green * cos_alfa * light_factor;
				   color.blue  += scene->lights[i]->color.blue  * diffuse.blue  * cos_alfa * light_factor;
            
				 
               /* Componente Especular */

               R        = algReflect(L,N) ;
			      Rnorm    = algReflect(Lnorm,N);			 	 
			      prod     = algDot (V, Rnorm);
			      cos_beta = prod / algNorm(V) * algNorm(Rnorm);
			   
				   color.red   += scene->lights[i]->color.red   * specular.red   * (pow(cos_beta,specularExponent)) * light_factor ;
				   color.green += scene->lights[i]->color.green * specular.green * (pow(cos_beta,specularExponent)) * light_factor ;
				   color.blue  += scene->lights[i]->color.blue  * specular.blue  * (pow(cos_beta,specularExponent)) * light_factor ;
         
            }


         }
         }
      }
     



     /* Reflexão */

	  if (reflectionFactor > 0 && depth < MAX_DEPTH)
	  {
		  Vector ReflectedRay;
		  Color ReflectedColor ;
		  
		  ReflectedRay = algReflect(V, N);
		  
		  
		  /* Lança um raio a partir do ponto sendo analisado com direção refletida em torno da normal) */
		  ReflectedColor = rayTrace (scene, point, ReflectedRay, depth + 1) ;
		  
		  color.red   += ReflectedColor.red   * reflectionFactor ;
		  color.blue  += ReflectedColor.blue  * reflectionFactor ;
		  color.green += ReflectedColor.green * reflectionFactor ;
	  }


     /* Transparencia (Refracao) */
     /*********** CORRIGIR ************/

if (refr==1)
{

     if(opacityFactor < 1 && depth < MAX_DEPTH)
     {
//        Vector Vt;
        Vector RefractedRay;
        //Vector t;
        Vector v,n,v_linha,n_linha;
        Color RefractedColor ;
        //double sin_teta1, sin_teta2, cos_teta1, cos_teta2, cos_teta1temp;
        
        //3
        double n1,n2,n_snell;
        double thetai, thetar;

        
/*
//METODO1

			RefractedRay = algSnell(ray, normal, 1.0, refractionFactor) ;

			point = objExit(object, point, RefractedRay,eye) ;

			normal = algMinus(objNormalAt(object, point)) ;
			RefractedRay = algSnell(RefractedRay, normal, refractionFactor, 1.) ;

			if (algNorm(RefractedRay) > 1.e-4)
			{
				RefractedColor = rayTrace(scene, point, RefractedRay, depth) ;
				color = colorAddition(color, colorScale((1-opacityFactor), RefractedColor)) ;
			}
*/

        
     

/*
//METODO2 
  
        v = V;//algUnit(V); 
        n = N;//algUnit(N); 
        v_linha = algMinus (v);
        n_linha = algMinus(n);

        
        Vt        = algSub ((algScale((algDot (v, n)), n)), v);
        sin_teta1 = algNorm (Vt);
        sin_teta2 = refractionFactor * sin_teta1; 
        cos_teta2 = sqrt(1.0 - pow(sin_teta2,2.0));
        t         = algScale( (1.0 / algNorm (Vt)), Vt);

        RefractedRay = algAdd( (algScale(sin_teta2, t)) , (algScale(cos_teta2, n_linha)) );



/*
//METODO3

        cos_teta2   = algNorm(algMinus(n));
        cos_teta1temp = (1.0 / cos_teta2) * algNorm(v) ;
        cos_teta1     = 1.0  / cos_teta1temp;

        RefractedRay   = algAdd( algScale(refractionFactor, v_linha) , algScale ((refractionFactor * cos_teta1 - cos_teta2), n));
        RefractedColor = rayTrace (scene, point, RefractedRay, depth + 1) ;


        color.red   += RefractedColor.red   * (1 - opacityFactor) ;
		  color.blue  += RefractedColor.blue  * (1 - opacityFactor) ;
		  color.green += RefractedColor.green * (1 - opacityFactor) ;
 */


        v = V;//algUnit(V); 
        n = N;//algUnit(N); 
        v_linha = algMinus (v);
        n_linha = algMinus(n);

        n1      = 1.0;
        n2      = refractionFactor;
        n_snell = 0.4;//n1 / n2;

        thetai = algDot(n,v);
        thetar = thetai*1.0/n_snell;

        RefractedRay  = algSub( algScale(n_snell, v) , algScale (thetar - n_snell * thetai, n) );
     

        /* Lança um raio */
		  RefractedColor = rayTrace (scene, point, RefractedRay, depth+1);
        //opacityFactor = 0;
		  
        color.red   += RefractedColor.red   * (1 - opacityFactor) ;
		  color.blue  += RefractedColor.blue  * (1 - opacityFactor) ;
		  color.green += RefractedColor.green * (1 - opacityFactor) ;
     }
}

     return color;
   }



   static double getNearestObject( Scene scene, Vector eye, Vector ray, Object *object )
   {
	   int i;
	   int objectCount = sceGetObjectCount( scene );

	   double closest = DBL_MAX;

	   /* Para cada objeto na cena */
	   for( i = 0; i < objectCount; ++i )
	   {
		   Object currentObject = sceGetObject( scene, i );
		   double distance = objIntercept( currentObject, eye, ray );
		   
		   if( distance > 0.0 && distance < closest )
		   {
			   closest = distance;
			   *object = currentObject;
		   }
	   }

	   return closest;
   }


   /* Sombra Comum */

   static int isInShadow( Scene scene, Vector point, Vector rayToLight, Vector lightLocation )
   {
	   int i;
	   int objectCount = sceGetObjectCount( scene );

	   /* maxDistance = distância de point até lightLocation */
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


