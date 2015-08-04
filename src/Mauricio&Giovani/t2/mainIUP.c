/*============================================================================
 * 2o trabalho                                                               *
 * Curso de Computacao Grafica 2004.2, prof. Marcelo Gattass.                *
 * Aluno(s): Mauricio Ferreira e Giovani Tadei                               *
 *===========================================================================*/



#include <stdio.h>
#include <time.h>

/*- Inclusao das bibliotecas IUP e CD: ------------------------------------*/
#include <iup.h>                    /* interface das funcoes IUP */
#include <iupgl.h>                  /* interface da funcao do canvas do OpenGL no IUP */
#include <windows.h>                /* inclui as definicoes do windows para o OpenGL */
#include <gl/gl.h>                  /* prototypes do OpenGL */
#include <gl/glu.h>                 /* prototypes do OpenGL */


#include "image.h"                  /* TAD para imagem */
#include "color.h"
#include "algebra.h"
#include "raytracing.h"


/************************************************************************/
/* Funcoes Exportadas                                                   */
/************************************************************************/

void paint(int x1,int x2, int y1, int y2);
void setImg (int xRay, int x2, int yRay, int y2);
void iniFilaQuad (int x1, int x2, int y1, int y2);
void insFilaQuad (int x1, int x2, int y1, int y2);
void delFilaQuad (int *x1, int *x2, int *y1, int *y2);
void IconLibOpen(void);   /* implemented in "iconlib.c" */

int RefInc(void);
int RefProg(void);


/************************************************************************/
/* Variaveis Globais                                                    */
/************************************************************************/


   Scene scene;         /* cena corrente */
   int ref=0;           /* Refinamento: 0-Incremental  1-Progressivo */
   int yc=0;            /* y corrente para Ray Tracing incremetnal */
   int width,height=-1; /* alrgura e altura corrente */
   Image image;         /* imagem que armazena o resultado até agora do algoritmo */
   Vector eye;
   Camera camera;

   Ihandle *canvas;      /* ponteiro IUP dos canvas */
   Ihandle *label;       /* ponteiro IUP do label para colocar mensagens para usuario */





/*********************************
  Struct Fila de Quadrantes
*********************************/


   typedef struct tgFilaQuad
	{
      int x1;
      int x2;
      int y1;
      int y2;

      struct tgFilaQuad * pProx;
		
	} tpFilaQuad ;  


/*********************************
  Struct Cabeca da Fila de Quadrantes
*********************************/


   typedef struct tgCabecaFila
	{

      int elem;
      struct tgFilaQuad * pIni;
      struct tgFilaQuad * pFim;
		
	} tpCabecaFila ;  


   static tpFilaQuad * pFilaQuad = NULL;
   static tpCabecaFila * pCabecaFila = NULL;


/************************************************************************/
/* Definicao das Funcoes Privadas                                       */
/************************************************************************/


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


/************************************************************************/
/* Callbacks do IUP.                                                    */
/************************************************************************/


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


/* captura um nome para um arquivo */
static char * get_new_file_name( void )
{
  Ihandle* getfile = IupFileDlg();
  char* filename = NULL;

  IupSetAttribute(getfile, IUP_TITLE, "Salva arquivo"  );
  IupSetAttribute(getfile, IUP_DIALOGTYPE, IUP_SAVE);
  IupSetAttribute(getfile, IUP_FILTER, "*.tga");
  IupSetAttribute(getfile, IUP_FILTERINFO, "Arquivo de imagem (*.tga)");
  IupPopup(getfile, IUP_CENTER, IUP_CENTER);  /* o posicionamento nao esta sendo respeitado no Windows */

  filename = IupGetAttribute(getfile, IUP_VALUE);
  return filename;
}

/* salva a imagem gerada */
int save_cb(Ihandle *self)
{
  char* filename = get_new_file_name( );  /* chama o dialogo de abertura de arquivo */
  if (filename==NULL) return 0;
  imageWriteTGA(filename,image);
  return IUP_DEFAULT;
}



/* - Callback de repaint do canvas  */
int repaint_cb(Ihandle *self)
{
  int w,h;
  int x,y;
  Color rgb;


  if (yc!= height) return IUP_DEFAULT; /* esta callback so'desenha depois que o algoritmo termina a imagem */

  imageGetDimensions(image,&w,&h);
  IupGLMakeCurrent(self);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glBegin(GL_POINTS);
    for (y=0;y<h;y++) {
      for (x=0; x<w; x++) {
         rgb=imageGetPixel(image, x, y);
         glColor3f((float)rgb.red,(float)rgb.green,(float)rgb.blue);
         glVertex2i(x,y);
       }
    }
  glEnd();
  
  glFlush();
  return IUP_DEFAULT; /* retorna o controle para o gerenciador de eventos */
} 




/**********************************************************************

		===================================
			CALLBACK IDLE
		===================================

  Funcao:  
  Ideia: Calcula uma linha da imagem a cada camada de idle  

**********************************************************************/


   int idle_cb(void)
   {
      if (ref== 1)
         return RefProg();

      else
         return RefInc();
   }



/**********************************************************************

		===================================
			Refinamento Incremental
		===================================

  Funcao:  RefInc
  Ideia:   Calcula uma linha da imagem a cada camada de idle  

**********************************************************************/

   int RefInc(void)
   {
	   int x;
	   
	   /* Faz uma linha de pixels por vez */
       if (yc<height) {
           IupGLMakeCurrent(canvas);
           glBegin(GL_POINTS);
   		   for( x = 0; x < width; ++x ) {
     		   Color pixel = { 0.0, 0.0, 0.0 };
			   Vector ray;				
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
	   else
	     IupSetFunction (IUP_IDLE_ACTION, (Icallback) NULL); /* a imagem ja' esta' completa */

	   return IUP_DEFAULT;
   }


/**********************************************************************

		===================================
			Refinamento Progressivo
		===================================

  Funcao:  RefProg
  Ideia:   Refinamento de mosaico inverso.

**********************************************************************/


   int RefProg(void)
   {
      int w,h;
      int xm, ym, x, y;


      Color pixel = { 0.0, 0.0, 0.0 };			   

      w=width-1;
      h=height-1;   


      /* Inicializa Fila - Quadro da imagem inteira */

      iniFilaQuad (0, w, 0, h);

      setImg (0, w, 0, h);
      
      glBegin(GL_POINTS);
      paint  (0, w, 0, h);
      glFlush();
      glEnd();


      while (pCabecaFila->elem != 0)
      {
         glBegin(GL_POINTS);


         /* Retira o primeiro da fila */

         delFilaQuad (&x, &w, &y, &h);
         
         xm = (x+w)/2;
         ym = (y+h)/2;



      /* Verificar se precisa inserir na Fila*/

         if(w-xm>=1 || h-ym>=1 )
         {

         /* Quadrante 1 - Sup. Esq. */
            
            insFilaQuad (x,xm,ym+1,h);
            setImg      (x,xm,ym+1,h);

         /* Quadrante 2 - Inf. Dir. */
            
            insFilaQuad (xm+1,w,y,ym);
            setImg      (xm+1,w,y,ym);

         /* Quadrante 3 - Sup. Dir. */
            
            insFilaQuad (xm+1,w,ym+1,h);
            setImg      (xm+1,w,ym+1,h);
            
         /* Quadrante 4 - Inf. Esq. */
            
            insFilaQuad (x,xm,y,ym);

         }
         
      
      /* Pintar Pixeis calculados na tela */

         paint (0, w, 0, h);   
         glFlush();
         glEnd();
      }
   

   /* Para que o canvas possa ser redezenhado */
   
      yc = height; 

	   return IUP_DEFAULT;
   }


/**********************************************************************

		===================================
			Inicializar a Fila
		===================================

  Funcao:  iniFilaQuad
  Ideia:   Incializar a fila com o valor do Quadrante dado.

**********************************************************************/

   void iniFilaQuad (int x1, int x2, int y1, int y2)
   {
      tpFilaQuad   * pFilaQuadAux   = NULL;
      tpCabecaFila * pCabecaFilaAux = NULL;

      pFilaQuadAux   = (tpFilaQuad *) malloc (1 * (sizeof (tpFilaQuad)));
      pCabecaFilaAux = (tpCabecaFila *) malloc (1 * (sizeof (tpCabecaFila)));
 
      pFilaQuadAux->x1    = x1;
      pFilaQuadAux->x2    = x2;
      pFilaQuadAux->y1    = y1;
      pFilaQuadAux->y2    = y2;
      pFilaQuadAux->pProx = NULL;

      pFilaQuad = pFilaQuadAux;

      pCabecaFilaAux->elem = 1;
      pCabecaFilaAux->pIni = pFilaQuad;
      pCabecaFilaAux->pFim = pFilaQuad;

      pCabecaFila = pCabecaFilaAux;
         
      return;
   }

/**********************************************************************

		===================================
			Insere na Fila
		===================================

  Funcao:  insFilaQuad
  Ideia:   Insere na fila o Quadrante dado.

**********************************************************************/

   void insFilaQuad (int x1, int x2, int y1, int y2)
   {

      tpFilaQuad * pFilaQuadAux = NULL;

      if(pCabecaFila->pIni == NULL)
         iniFilaQuad (x1, x2, y1, y2);

      else
      {
         pFilaQuadAux = (tpFilaQuad *) malloc (1 * (sizeof (tpFilaQuad)));
 
         pFilaQuadAux->x1    = x1;
         pFilaQuadAux->x2    = x2;
         pFilaQuadAux->y1    = y1;
         pFilaQuadAux->y2    = y2;
         pFilaQuadAux->pProx = NULL;


         (pCabecaFila->pFim)->pProx = pFilaQuadAux;

         pCabecaFila->pFim = pFilaQuadAux;

         pCabecaFila->elem += 1;
      }

      return;

   }

/**********************************************************************

		===================================
			Remove da Fila
		===================================

  Funcao:  delFilaQuad
  Ideia:   Remove o primeiro quadrante da fila.

**********************************************************************/

   void delFilaQuad (int *x1, int *x2, int *y1, int *y2)
   {
      int xf1, xf2, yf1, yf2;

      tpFilaQuad * pFilaQuadAux = NULL;

      pFilaQuadAux = pCabecaFila->pIni;
 
      xf1 = pFilaQuadAux->x1;
      xf2 = pFilaQuadAux->x2;
      yf1 = pFilaQuadAux->y1;
      yf2 = pFilaQuadAux->y2;


      pCabecaFila->elem -= 1;

      pCabecaFila->pIni = pFilaQuadAux->pProx;

      pFilaQuad = pCabecaFila->pIni;

      pFilaQuadAux->pProx = NULL;

      free(pFilaQuadAux);


      if(pCabecaFila->elem == 1)
         pCabecaFila->pFim = pCabecaFila->pIni;
         

      if(pCabecaFila->elem == 0)
         pCabecaFila->pFim = NULL;
         

      (*x1) = xf1;
      (*x2) = xf2;
      (*y1) = yf1;
      (*y2) = yf2;

      return;
   }




/**********************************************************************

		===================================
			         Set Image
		===================================

  Funcao:  setImg
  Ideia:   Seta os pixeis da imagem da cor do RT do pixel inferior esquero
          de acordo com os limites do quadrante dado.

**********************************************************************/

   void setImg (int xRay,int x2,int yRay, int y2)
   {  
      int w,h;
      int x,y;
      int x1 = xRay, y1 = yRay;

      Vector ray;
      Color pixel = { 0.0, 0.0, 0.0 };			   
      Color color;
      
      w=width;
      h=height;
      
      ray   = camGetRay( camera, xRay, yRay );
      color = rayTrace( scene, eye, ray, 0 );
      pixel = colorAddition( pixel, color );


      for (y=y1; y<=y2; y++) {
         for( x=x1; x<=x2 ; x++ ) 
            imageSetPixel( image, x, y, pixel );

      }

      return;
   }



/**********************************************************************

		===================================
			         Paint
		===================================

  Funcao:  paint
  Ideia:   Pinta na tela o quadrante.

**********************************************************************/



   void paint(int x1,int x2, int y1, int y2)
   {
      int x, y;
      Color rgb;

      for (y=y1;y<=y2;y++) {
         for (x=x1; x<=x2; x++)
         {
            rgb=imageGetPixel(image, x, y);
            glColor3f((float)rgb.red,(float)rgb.green,(float)rgb.blue);
            glVertex2i(x,y);
         }
      }

      IupSetFunction (IUP_IDLE_ACTION, (Icallback) NULL); 
      return;
   
   }




/**********************************************************************

		===================================
			CALLBACK Refinamento
		===================================

**********************************************************************/

   int ref_cb(Ihandle *self)
   {
  
      switch (IupAlarm ("Selecionar Metodo de Refinamento", 
           "== METODOS DE REFINAMENTO ==", "Refinamento Incremental", "Refinamento Progressivo", "Cancelar"))
      {
         case 1:
            ref=0;
            break;

         case 2:
            ref=1;
            break;

         case 3:
            break ;
     }

     return IUP_DEFAULT;

   }



/* --------------- Gattass ---------------------------- */

/* carrega uma nova cena */
int load_cb(void) {

  char* filename = get_file_name();  /* chama o dialogo de abertura de arquivo */
  char buffer[30];

  if (filename==NULL) return 0;

  /* Le a cena especificada */
  scene = sceLoad( filename );
  if( scene == NULL ) return IUP_DEFAULT;

  camera = sceGetCamera( scene );
  eye = camGetEye( camera );
  width = camGetScreenWidth( camera );
  height = camGetScreenHeight( camera );
  yc=0;

  if (image) imageDestroy(image);//


  image = imageCreate( width, height );
  IupSetfAttribute(label, "TITLE", "%3dx%3d", width, height);
  IupSetFunction (IUP_IDLE_ACTION, (Icallback) idle_cb);
  sprintf(buffer,"%3dx%3d", width, height);
  IupSetAttribute(canvas,IUP_RASTERSIZE,buffer);
  return IUP_DEFAULT;
}




/*-------------------------------------------------------------------------*/
/* Incializa o programa.                                                   */
/*-------------------------------------------------------------------------*/

int init(void)
{
  Ihandle *dialog, *statusbar,  *box;

  Ihandle *toolbar, *load, *save, *ref;

  /* creates the toolbar and its buttons */
  load = IupButton("", "load_cb");
  IupSetAttribute(load,"TIP","Carrega uma imagem.");
  IupSetAttribute(load,"IMAGE","icon_lib_open");
  IupSetFunction("load_cb", (Icallback)load_cb);

  save = IupButton("", "save_cb");
  IupSetAttribute(save,"TIP","Salva no formato GIF.");
  IupSetAttribute(save,"IMAGE","icon_lib_save");

  ref = IupButton("", "ref_cb");
  IupSetAttribute(ref,"TIP","Refinamento.");
  IupSetAttribute(ref,"IMAGE","icon_lib_preview");
  
  toolbar = IupHbox(
       load, 
       save,
       ref,
	   IupFill(),
     NULL);

  IupSetAttribute(toolbar, "ALIGNMENT", "ACENTER");
 

  /* cria um canvas */
  canvas = IupGLCanvas("repaint_cb"); 
  IupSetAttribute(canvas,IUP_RASTERSIZE,"500x500");
  IupSetAttribute(canvas, "RESIZE_CB", "resize_cb");

  /* associa o evento de repaint a funccao repaint_cb */
  IupSetFunction("repaint_cb", (Icallback) repaint_cb);
  IupSetFunction("save_cb", (Icallback)save_cb);
  IupSetFunction("ref_cb", (Icallback)ref_cb);
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
  IupSetAttribute(dialog, "TITLE", "CG2004: Trab. 2");


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