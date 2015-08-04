/**
 *	@file zbuffer.c RayTracing: renderização de cenas pelo OpenGL.
 *
 *	@author 
 *			- Marcelo Gattass
 *			- Thiago Bastos
 *
 *	@date
 *			Criado em:             10 de novembro de 2003
 *
 *	@version 1.0
 */


#include <stdlib.h>
#include <gl/glut.h>

#include "zbuffer.h"

#define SIMULATE 1

void ZBufferScene(Scene* scene, int winW, int winH )
{
 	Camera* camera = sceGetCamera(scene);
	/* parametros internos da camera (correspondentes a gluPerspective) */
	double fovy, aspect, zNear, zFar; 

	/* parametros externos da camera (correspondentes a glLookAt) */
    double eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz;

	/* Luz, objetos e acao... */
	int numberOfLights  = sceGetLightCount(scene);
	int numberOfObjects = sceGetObjectCount(scene);
	int i;


    /* inicializa o OpenGL */
    glEnable     (GL_DEPTH_TEST);  /* habilita o teste de profundidade do z-buffer */
    glDepthFunc  (GL_LESS);        /* menor z significa mais proximo */
    glEnable     (GL_LIGHTING);    /* ativa os calculos de iluminacao */
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);  /* reflexao especular com relacao ao olho */
    glShadeModel (GL_SMOOTH);      /* suavisa as diferencas de cores entre os objetos */
    glEnable     (GL_NORMALIZE);   /* normaliza os vetores normais antes de utilizar */    
	 glClear      (GL_DEPTH_BUFFER_BIT); /* limpa o buffer de profundidade */

    /* Define a matrix de projecao */
    glMatrixMode(GL_PROJECTION);
   if (SIMULATE) {
      Matrix P = camGetOGLPerspectiveMatrix(camera); 
      glLoadMatrixd(algGetOGLMatrix(&P));
   }
   else {
    glLoadIdentity();
    glViewport(0,0,winW,winH);
	 camGetOGLPerspectiveParameters(camera, &fovy, &aspect, &zNear, &zFar);
    gluPerspective(fovy, aspect, zNear, zFar);
   }

	/* define a matriz do modelo e da transformacao para a camera */
   glMatrixMode (GL_MODELVIEW);
   if (SIMULATE) {
      Matrix LookAt = camGetOGLLookAtMatrix(camera); 
      glLoadMatrixd(algGetOGLMatrix(&LookAt));
   }
   else {
//      float m[16];
      glLoadIdentity();
      camGetOGLLookAtParameters(camera, &eyex,&eyey,&eyez, &centerx,&centery,&centerz, &upx,&upy,&upz);
	   gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, upx, upy, upz);
//      glGetFloatv(GL_MODELVIEW_MATRIX,m);
   }

	/* define as luzes no sistema de eixos dos modelos globais */
	for( i = 0; i <numberOfLights; ++i ) 
	{
 	 Light*  light          = sceGetLight( scene, i );
	 Vector lightPosition  = lightGetPosition(light);
	 Color  lightColor     = lightGetColor(light);
     Color  ambientLight   = sceGetAmbientLight(scene);

	 float tmp[4];
	 
	 colorGetOGLVector(ambientLight,tmp);      
	     glLightfv(GL_LIGHT0+i, GL_AMBIENT, tmp);
     colorGetOGLVector(lightColor,tmp);        
	     glLightfv(GL_LIGHT0+i, GL_DIFFUSE, tmp);
         glLightfv(GL_LIGHT0+i, GL_SPECULAR,tmp);
     algGetOGLVector(lightPosition,tmp); 
	     glLightfv(GL_LIGHT0+i, GL_POSITION,tmp);
	 glEnable(GL_LIGHT0+i);
	}


	/* desenha os objetos */
	for (i=0;i < numberOfObjects; i++ )
	{
 	 Object* object          = sceGetObject(scene,i);
	 Material* material      = sceGetMaterial(scene,objGetMaterial(object));
	 Color diffuseColor     = matGetDiffuse(material, algVector(0,0,0,1));
	 Color specularColor    = matGetSpecular(material);
	 double specularExpoent = matGetSpecularExponent(material);

	 float tmp[4],zero[]={0.0,0.0,0.0,1.0};
	 colorGetOGLVector(diffuseColor,tmp);
	   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, tmp);
	 colorGetOGLVector(specularColor,tmp);
       glMaterialfv(GL_FRONT, GL_SPECULAR, tmp);
     glMaterialf(GL_FRONT, GL_SHININESS, specularExpoent );
     glMaterialfv(GL_FRONT, GL_EMISSION, zero);

	 /* cada objeto deve se instaciar */
	 glPushMatrix();
	   objDraw(object);
     glPopMatrix();
	}

  glFlush();
}
