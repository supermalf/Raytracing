/*
 *	Computação Gráfica - Trabalho de Raytracing
 *	Professor Marcelo Gattass
 *
 *	@file main.c 
 *
 *	@author 
 *			- Maira Noronha
 *			- Thiago Bastos
 *
 *	@date
 *			Última Modificação:		10 de Fevereiro de 2003
 *
 *	@version 2.0
 *
 *	Este arquivo não precisa ser modificado.
 *	Veja o módulo raytracing.c
 */

#include <stdio.h>
#include <time.h>

/*- Inclusao das bibliotecas IUP e CD: ------------------------------------*/
#include "iup.h"                    /* interface das funcoes IUP */
#include "iupgl.h"                  /* interface da funcao do canvas do OpenGL no IUP */
#include <windows.h>                /* inclui as definicoes do windows para o OpenGL */
#include <gl/gl.h>                  /* prototypes do OpenGL */
#include <gl/glu.h>                 /* prototypes do OpenGL */
#include <gl/glut.h>                 /* prototypes do OpenGL */


#include "image.h"                  /* TAD para imagem */
#include "color.h"
#include "algebra.h"
#include "raytracing.h"
#include "zbuffer.h"

/* -- implemented in "iconlib.c" to load standard icon images into IUP */
void IconLibOpen(void);

/*- Contexto do Programa: -------------------------------------------------*/
Scene* scene;         /* cena corrente */
int yc=0;            /* y corrente para Ray Tracing incremetnal */
int width,height=-1;
Image *image;        /* imagem que armazena o resultado até agora do algoritmo */

Ihandle* canvas;      /* ponteiro IUP dos canvas */
Ihandle* label;       /* ponteiro IUP do label para colocar mensagens para usuario */
Ihandle* toggle_ray_trace;  /* ponteiro para o elemento de toggle que ativa o rastreamento de raios */

/*- Funcoes auxiliares ------------*/


/* calcula o ray_race quando o usuario tenta salvar a imagem */
void ray_trace (void)
{
   IupGLMakeCurrent(canvas);
   
   if (scene!=NULL) {
      Camera* camera=sceGetCamera(scene);
      int w = camGetScreenWidth(camera);
      int h = camGetScreenHeight(camera);
      Vector ray;
      Vector eye;
      int x,y;

      /* transformacao de instanciacao dos objetos no sistema de coordenadas da camera */
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();           /* identidade,  ou seja nada */

      /* transformacao de projecao */
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluOrtho2D (0.0, (GLsizei)(w), 0.0, (GLsizei)(h));  /* ortografica no plano xy de [0,w]x[0,h] */

      glDisable     (GL_DEPTH_TEST);  /* desabilita o teste de profundidade do z-buffer */
      glDisable     (GL_LIGHTING);  /* desabilita a luz */

      eye = camGetEye( camera );

      for (y=0;y<h;y++) {
         glBegin(GL_POINTS);
   		for( x = 0; x < w; ++x ) {
     		Color pixel = { 0.0, 0.0, 0.0 };
			Color color;

			ray = camGetRay( camera, x, y );

			/* Obtém a amostra. */
			color = rayTrace( scene, eye, ray, 0 );

			/* Adiciona a contribuição da amostra à cor final */
			pixel = colorAddition( pixel, color );

			imageSetPixel( image, x, y, pixel );
            glColor3f((float)pixel.red,(float)pixel.green,(float)pixel.blue);
            glVertex2i(x,y);
         }
		   glEnd();
         glFlush();
      }
   }
   IupSetFocus(canvas);
}

/* Dialogo de selecao de arquivo  */
static char * get_file_name( void )
{
  Ihandle* getfile = IupFileDlg();
  char* filename = NULL;

  IupSetAttribute(getfile, IUP_TITLE, "Abertura de arquivo"  );
  IupSetAttribute(getfile, IUP_DIALOGTYPE, IUP_OPEN);
  IupSetAttribute(getfile, IUP_FILTER, "*.rt4");
  IupSetAttribute(getfile, IUP_FILTERINFO, "Arquivo de cena (*.rt4)");
  IupPopup(getfile, IUP_CENTER, IUP_CENTER);  /* o posicionamento nao esta sendo respeitado no Windows */

  filename = IupGetAttribute(getfile, IUP_VALUE);
  return filename;
}


/*------------------------------------------*/
/* Callbacks do IUP.                        */
/*------------------------------------------*/


/* - Callback de mudanca de tamanho no canvas (mesma para ambos os canvas) */
int resize_cb(Ihandle *self, int width, int height)
{
 IupGLMakeCurrent(self);  /* torna o foco do OpenGL para este canvas */

 /* define a area do canvas que deve receber as primitivas do OpenGL */
 glViewport(0,0,width,height);

 /* transformacao de instanciacao dos objetos no sistema de coordenadas da camera */
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();           /* identidade,  ou seja nada */

 /* transformacao de projecao */
 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 gluOrtho2D (0.0, (GLsizei)(width), 0.0, (GLsizei)(height));  /* ortografica no plano xy de [0,w]x[0,h] */
  
 glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
 glClear(GL_COLOR_BUFFER_BIT);

 return IUP_DEFAULT; /* retorna o controle para o gerenciador de eventos */
}


int repaint_idle_cb(Ihandle *self) 
{
   IupGLMakeCurrent(self);
   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
   glClear(GL_COLOR_BUFFER_BIT);
   return IUP_DEFAULT;
}

int repaint_ogl_cb(Ihandle *self )
{
   IupGLMakeCurrent(self);

   if (scene!=NULL) {
      Camera* cam=sceGetCamera( scene );
      Vector eye = camGetEye(cam);
      Vector ray = camGetRay(cam,0,0);
      Color back=sceGetBackgroundColor(scene,eye,ray);
      glClearColor(back.red, back.green, back.blue, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);
      ZBufferScene(scene, camGetScreenHeight(cam), camGetScreenHeight(cam) );
      glFlush();
      yc=0;
   }
   return IUP_DEFAULT;
}



/* captura um nome para um arquivo */
static char * get_new_file_name( void )
{
  Ihandle* getfile = IupFileDlg();
  char* filename = NULL;

  IupSetAttribute(getfile, IUP_TITLE, "Salva arquivo"  );
  IupSetAttribute(getfile, IUP_DIALOGTYPE, IUP_SAVE);
  IupSetAttribute(getfile, IUP_FILTER, "*.bmp");
  IupSetAttribute(getfile, IUP_FILTERINFO, "Arquivo de imagem (*.bmp)");
  IupPopup(getfile, IUP_CENTER, IUP_CENTER);  /* o posicionamento nao esta sendo respeitado no Windows */

  filename = IupGetAttribute(getfile, IUP_VALUE);
  return filename;
}

/* salva a imagem gerada */
int save_cb(Ihandle *self)
{
  char* filename = get_new_file_name( );  /* chama o dialogo de abertura de arquivo */
  char buffer[80];

  if (filename!=NULL) {
     strcpy(buffer,filename);
     if (strstr(filename,".bmp")!=NULL || strstr(filename,".BMP")!=NULL) {
        imgWriteBMP(filename,image);
     } else {
        FILE* fp_dat = fopen(strcat(buffer,".txt"),"wt");
        Camera* camera = sceGetCamera(scene);
        Matrix LookAt= camGetOGLLookAtMatrix(camera);
        Matrix P = camGetOGLPerspectiveMatrix(camera);
        Matrix V = camGetOGLViewportMatrix(camera);
        Vector  eye = camGetEye(camera);
        Vector  xAxis = camGetAxisX(camera);
        Vector  yAxis = camGetAxisY(camera);
        Vector  zAxis = camGetAxisZ(camera);
        double  focalDist = camGetFocalDistance(camera);
        Vector  marker = sceGetMarker(scene);
        Matrix  VPMview=algMult(V,algMult(P,LookAt));
        Vector  marker_prj =algCartesian(algTransf(VPMview,marker));

        if (yc==0) ray_trace( );
        imgWriteBMP(strcat(filename,".bmp"),image);

        algMatrixWrite(fp_dat,"LOOK_AT",LookAt);
        algMatrixWrite(fp_dat,"PROJECTION",P);
        algMatrixWrite(fp_dat,"VIEWPORT",V);
        algVectorWrite(fp_dat,"EYE",eye);
        fprintf(fp_dat,"FOCAL_DIST\n%.4f\n",focalDist);
        algVectorWrite(fp_dat,"AXISX",xAxis);
        algVectorWrite(fp_dat,"AXISY",yAxis);
        algVectorWrite(fp_dat,"AXISZ",zAxis);
        algVectorWrite(fp_dat,"MARKER",marker);
        algMatrixWrite(fp_dat,"VPMVIEW",VPMview);
        algVectorWrite(fp_dat,"MARKER_PRJ",marker_prj);

        fclose(fp_dat);
     }       
  }
  return IUP_DEFAULT;
}


/* calcula uma linha da imagem pelo Ray Tracing a cada camada de idle */
int idle_cb(void)
{
   IupGLMakeCurrent(canvas);
   
   if (scene!=NULL&&yc<height) {
      Camera* camera=sceGetCamera(scene);
      int w = camGetScreenWidth(camera);
      int h = camGetScreenHeight(camera);
      Vector ray;
      Vector eye;
      int x;

      /* transformacao de instanciacao dos objetos no sistema de coordenadas da camera */
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();           /* identidade,  ou seja nada */

      /* transformacao de projecao */
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluOrtho2D (0.0, (GLsizei)(w), 0.0, (GLsizei)(h));  /* ortografica no plano xy de [0,w]x[0,h] */

      glDisable     (GL_DEPTH_TEST);  /* desabilita o teste de profundidade do z-buffer */
      glDisable     (GL_LIGHTING);  /* desabilita a luz */

      eye = camGetEye( camera );

      glBegin(GL_POINTS);
   		for( x = 0; x < width; ++x ) {
     		Color pixel = { 0.0, 0.0, 0.0 };
			Color color;

			ray = camGetRay( camera, x, yc );

			/* Obtém a amostra. */
			color = rayTrace( scene, eye, ray, 0 );

			/* Adiciona a contribuição da amostra à cor final */
			pixel = colorAddition( pixel, color );

			imageSetPixel( image, x, yc, pixel );
            glColor3f((float)pixel.red,(float)pixel.green,(float)pixel.blue);
            glVertex2i(x,yc);
         }
		glEnd();
		yc++;
	}
   else {
	  IupSetFunction (IUP_IDLE_ACTION, (Icallback) NULL); /* a imagem ja' esta' completa ou nao tem cena */
     IupSetAttribute(toggle_ray_trace,"VALUE","OFF");
   }

   IupSetFocus(canvas);
	return IUP_DEFAULT;
}

int ray_trace_cb(Ihandle *self, int v)
{
   if (v==1)
      IupSetFunction (IUP_IDLE_ACTION, (Icallback) idle_cb); 
   else
      IupSetFunction (IUP_IDLE_ACTION, NULL); 

  	return IUP_DEFAULT;
}


/* carrega uma nova cena */
int load_cb(void) {
  char* filename = get_file_name();  /* chama o dialogo de abertura de arquivo */
  char buffer[80];
  Camera* camera;

  if (filename==NULL) return 0;

  /* Le a cena especificada */
  scene = sceLoad( filename );
  if( scene == NULL ) return IUP_DEFAULT;
  camera = sceGetCamera(scene);
  width = camGetScreenWidth( camera );
  height = camGetScreenHeight( camera );

  if (image) imgDestroy(image);
  image = imgCreate( width, height );
  IupSetfAttribute(label, "TITLE", "%s (%3dx%3d)", strrchr(filename,'\\')+1, width, height);
  IupSetFunction("repaint_cb", (Icallback) repaint_ogl_cb);
  sprintf(buffer,"%3dx%3d", width, height);
  IupSetAttribute(canvas,IUP_RASTERSIZE,buffer);
  repaint_ogl_cb(canvas);
  IupSetFocus(canvas);
  return IUP_DEFAULT;
}

/* Callback que trata de eventos de teclado */
int keypress_cb( Ihandle *self, int c, int press ) 
{
   Camera* camera=sceGetCamera(scene);

	/* se está recebendo evento de soltar a tecla, retorna */
	if( press == 0 )
		return IUP_DEFAULT;

	switch( c )
	{
		/* move a câmera para a frente */
		case K_UP:
         camMoveFoward(camera);
			break;

		/* move a câmera para trás */
		case K_DOWN:
         camMoveBackward(camera);
			break;

		/* move a câmera para a esquerda */
		case K_LEFT:
         camMoveLeft(camera);
			break;

		/* move a câmera para a direita */
		case K_RIGHT:
         camMoveRight(camera);
			break;

		/* move a câmera para cima */
		case K_sUP:
         camMoveUp(camera);
			break;

		/* move a câmera para baixo */
		case K_sDOWN:
         camMoveDown(camera);
			break;

		/* aponta a câmera para cima */
		case K_cUP:
         camPitchUp(camera);
			break;

		/* aponta a câmera para baixo */
		case K_cDOWN:
         camPitchDown(camera);
			break;

		/* aponta a câmera para direita */
		case K_cRIGHT:
         camYawRight(camera);
			break;

		/* aponta a câmera para esquerda */
		case K_cLEFT:
         camYawLeft(camera);
			break;

		/* rola a câmera para direita */
		case K_END:
         camRolRight(camera);
			break;

		/* rola a câmera para esquerda */
		case K_HOME:
         camRolLeft(camera);
			break;

      /* reseta a câmera para a posicao original */
		case K_SP:
         camReset(camera);
			break;

		case K_R:
		case K_r:
  	      IupSetFunction (IUP_IDLE_ACTION, (Icallback) idle_cb); /* a imagem ja' esta' completa */
  			break;



		case K_ESC:
         return IUP_CLOSE;
	}

   repaint_ogl_cb(canvas);
	/* retorna o controle para o gerenciador de eventos */
	return IUP_DEFAULT;
}


/*-------------------------------------------------------------------------*/
/* Incializa o programa.                                                   */
/*-------------------------------------------------------------------------*/

int init(void)
{
  Ihandle *dialog, *statusbar,  *box;

  Ihandle *toolbar, *load, *save;

  /* creates the toolbar and its buttons */
  load = IupButton("", "load_cb");
  IupSetAttribute(load,"TIP","Carrega uma imagem.");
  IupSetAttribute(load,"IMAGE","icon_lib_open");
  IupSetFunction("load_cb", (Icallback)load_cb);

  save = IupButton("", "save_cb");
  IupSetAttribute(save,"TIP","Salva no formato GIF.");
  IupSetAttribute(save,"IMAGE","icon_lib_save");

  toggle_ray_trace = IupToggle("","ray_trace");
  IupSetAttribute(toggle_ray_trace,"TIP","aplica o algoritmo de tracado de raios.");
  IupSetAttribute(toggle_ray_trace,"IMAGE","icon_lib_preview");
  IupSetFunction("ray_trace", (Icallback)ray_trace_cb);
  
  toolbar = IupHbox(
       load, 
       save,
       toggle_ray_trace,
	    IupFill(),
     NULL);

  IupSetAttribute(toolbar, "ALIGNMENT", "ACENTER");
 

  /* cria um canvas */
  canvas = IupGLCanvas("repaint_cb"); 
  IupSetAttribute(canvas,IUP_RASTERSIZE,"400x300");
  IupSetAttribute(canvas, "RESIZE_CB", "resize_cb");
  IupSetAttribute( canvas, IUP_KEYPRESS_CB, "keyPress" ); 
  IupSetFunction( "keyPress", (Icallback) keypress_cb );


  /* associa eventos a callbacks */
  IupSetFunction("repaint_cb", (Icallback) repaint_idle_cb);
  IupSetFunction("save_cb", (Icallback)save_cb);
  IupSetFunction("resize_cb", (Icallback) resize_cb);
  IupSetFunction (IUP_IDLE_ACTION, (Icallback) NULL);


  /* the status bar is just a label to put some usefull information in run time */
  label = IupLabel("status");
  IupSetAttribute(label, "EXPAND", "HORIZONTAL");
  IupSetAttribute(label, "FONT", "COURIER_NORMAL_10");
  statusbar = IupSetAttributes(IupHbox(
                IupFrame(IupHbox(label, NULL)), 
                NULL), "MARGIN=5x5");

  /* this is the most external box that puts together
     the toolbar, the two canvas and the status bar */
  box = IupVbox(
          toolbar,
          canvas, 
          statusbar, 
          NULL);

  /* create the dialog and set its attributes */
  dialog = IupDialog(box);
  IupSetAttribute(dialog, "CLOSE_CB", "app_exit_cb");
  IupSetAttribute(dialog, "TITLE", "CG2005: Trab. 3");


  IupShowXY(dialog, IUP_CENTER, IUP_CENTER);

  return 1;
}

/*-------------------------------------------------------------------------*/
/* Rotina principal.                                                       */
/*-------------------------------------------------------------------------*/
void main(void) {
    IupOpen();
    IupGLCanvasOpen();
    IconLibOpen();
    if ( init() )
		IupMainLoop();
    IupClose();
}