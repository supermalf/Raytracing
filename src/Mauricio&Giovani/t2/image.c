/**
 *	@file image.c Image: operações imagens em formato TGA.
 *
 *	@author 
 *          - Maira Noronha
 *          - Marcelo Gattass
 *
 *	@date
 *			Última Modificação:		23 de Abril de 2003
 *
 *	@version 2.0
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "image.h"

/************************************************************************/
/* Definicao das Funcoes Privadas                                       */
/************************************************************************/

/*  getuint e putuint:
 * Funcoes auxiliares para ler e escrever inteiros na ordem (lo-hi)
 * Note que no Windows as variaveis tipo "unsigned short int" sao armazenadas 
 * no disco em dois bytes na ordem inversa.  Ou seja, o numero 400, 
 * por exemplo, que pode ser escrito como 0x190, fica armazenado em 
 * dois bytes consecutivos 0x90 e 0x01. Nos sistemas UNIX e Mac 
 * este mesmo inteiro seria armazenado na ordem 0x01 e 0x90.  
 *  O armazenamento do Windows e'  chamado de "little endian"  
 * (i.e., lowest-order byte stored first), e no sitemas Unix sao
 * "big-endian" (i.e., highest-order byte stored first). 
 */  
static int getuint(unsigned short *uint, FILE *input)
{
  int got;
  unsigned char temp[2];
  unsigned short tempuint;

  got = (int) fread(&temp, 1, 2, input);
  if (got != 2) return 0;

  tempuint = ((unsigned short)(temp[1])<<8) | ((unsigned short)(temp[0]));

  *uint = tempuint;

  return 1;
}

static int putuint(unsigned short uint, FILE *output)
{
  int put;
  unsigned char temp[2];

  temp[0] = uint & 0xff;
  temp[1] = (uint >> 8) & 0xff;

  put = (int) fwrite(&temp, 1, 2, output);
  if (put != 2) return 0;
 
  return 1;
}

/************************************************************************/
/* Definicao das Funcoes Exportadas                                     */
/************************************************************************/

Image imageCreate (int w, int h)
{
   Image image;
   image = (Image) malloc (sizeof(struct Image_imp));
   assert(image);
   image->width  =(unsigned int) w;
   image->height =(unsigned int) h;
   image->buf = (unsigned char *) malloc (w * h * 3);
   assert(image->buf);
   return image;
}

void imageDestroy (Image image)
{
   if (image)
   {
      if (image->buf) free (image->buf);
      free(image);
   }
}

#define ROUND(_) (int)floor( (_) + 0.5 )

Image imageResize(Image img0, int w1, int h1) 
{
	Image img1 = imageCreate(w1,h1);
	int w0, h0;
	int x0,y0,x1,y1;
	Color color;

	imageGetDimensions(img0, &w0, &h0);

	for (y1=0;y1<h1;y1++)
		for (x1=0;x1<w1;x1++)
		{
			x0=ROUND((w0-1)*x1/(w1-1));
			y0=ROUND((h0-1)*y1/(h1-1));
			color = imageGetPixel(img0,x0,y0);
			imageSetPixel(img1,x1,y1,color);
		}
		return img1;
}

void imageGetDimensions(Image image, int *w, int *h)
{
   if (w) *w = image->width;
   if (h) *h = image->height;
}

void imageSetPixel(Image image, int x, int y, Color color)
{
   int pos = (y*image->width*3) + (x*3);

   image->buf[pos  ] = (unsigned char)(color.red * 255);
   image->buf[pos+1] = (unsigned char)(color.green * 255);
   image->buf[pos+2] = (unsigned char)(color.blue * 255);
}

Color imageGetPixel(Image image, int x, int y)
{
   Color color;
   int pos = (y*image->width*3) + (x*3);

   color.red   = (double)(image->buf[pos]) / 255.;
   color.green = (double)(image->buf[pos+1]) / 255.;
   color.blue  = (double)(image->buf[pos+2]) / 255.;

   return color;
}


Image imageLoad (char *filename) 
{
   FILE        *filePtr;

   Image          image;            /* imagem a ser criada */

   unsigned char imageType;        /* 2 para imagens RGB */ 
   unsigned short int imageWidth;  /* largura da imagem */
   unsigned short int imageHeight; /* altura da imagem */
   unsigned char bitCount;         /* numero de bits por pixel */
 
   long        imageIdx;         /* contadore de laco */
   unsigned char colorSwap;      /* variavel para traca */
   
   unsigned char ucharSkip;      /* dado lixo unsigned char */
   short int     sintSkip;       /* dado lixo short int */

   /* abre o arquivo com a imagem TGA */
   filePtr = fopen(filename, "rb");
   assert(filePtr);

   /* pula os primeiros dois bytes que devem ter valor zero */
   ucharSkip = getc(filePtr); /* tamanho do descritor da imagem (0) */
   if (ucharSkip != 0) printf("erro na leitura de %s: imagem com descritor\n", filename);

   ucharSkip = getc(filePtr); 
   if (ucharSkip != 0) printf("erro na leitura de %s: imagem com tabela de cores\n", filename);
   
   /* le o tipo de imagem (que deve ser obrigatoriamente 2).  
      NÃ£o estamos tratando dos outros tipos */
   imageType=getc(filePtr);
   assert(imageType == 2);

   /* pula 9 bytes relacionados com a tabela de cores 
     (que nao existe quando a imagem e' RGB, imageType=2) */
   getuint((short unsigned int *)&sintSkip,filePtr);
   getuint((short unsigned int *)&sintSkip,filePtr);
   ucharSkip = getc(filePtr); 

   /* especificacao da imagem */
   getuint((short unsigned int *)&sintSkip,filePtr);      /* origem em x (por default = 0) */
   getuint((short unsigned int *)&sintSkip,filePtr);      /* origem em y (por default = 0) */ 
   getuint(&imageWidth,filePtr);   /* largura */
   getuint(&imageHeight,filePtr);   /* altura */

   /* read image bit depth */
   bitCount=getc(filePtr);
   assert(bitCount == 24);  /* colorMode -> 3 = BGR (24 bits) */

   /* read 1 byte of garbage data */
   ucharSkip = getc(filePtr); 

   /* cria uma instancia do tipo Imagem */
   image = imageCreate(imageWidth,imageHeight);
   assert(image);

   /* read in image data */
   fread(image->buf, sizeof(unsigned char), 3*imageWidth*imageHeight, filePtr);
   
   /* change BGR to RGB so OpenGL can read the image data */
   for (imageIdx = 0; imageIdx < 3*imageWidth*imageHeight; imageIdx += 3)
   {
      colorSwap = image->buf[imageIdx];
      image->buf[imageIdx] = image->buf[imageIdx + 2];
      image->buf[imageIdx + 2] = colorSwap;
   }

   fclose(filePtr);
   return image;
}

int imageWriteTGA(char *filename, Image image)
{
   unsigned char imageType=2;      /* RGB(A) sem compressÃ£o */
   unsigned char bitDepth=24;      /* 24 bits por pixel */

   FILE         *filePtr;         /* ponteiro do arquivo */
   long        imageIdx;         /* indice para varrer os pixels */
   unsigned char colorSwap;      /* variavel temporaria para trocar de RGBA para BGRA */ 

   unsigned char byteZero=0;      /* usado para escrever um byte zero no arquivo */
   short int     shortZero=0;     /* usado para escrever um short int zero no arquivo */


   /* cria um arquivo binario novo */
   filePtr = fopen(filename, "wb");
   assert(filePtr);

   /* escreve o cabecalho */
   putc(byteZero,filePtr);     /* 0, no. de caracteres no campo de id da imagem */
   putc(byteZero,filePtr);     /* = 0, imagem nao tem palheta de cores */
   putc(imageType,filePtr);    /* = 2 -> imagem "true color" (RGB) */
   putuint(shortZero,filePtr); /* info sobre a tabela de cores (inexistente) */
   putuint(shortZero,filePtr);              /* idem */
   putc(byteZero,filePtr);                  /* idem */
   putuint(shortZero,filePtr);    /* =0 origem em x */
   putuint(shortZero,filePtr);    /* =0 origem em y */
   putuint(image->width,filePtr);   /* largura da imagem em pixels */
   putuint(image->height,filePtr);  /* altura da imagem em pixels */
   putc(bitDepth,filePtr);      /* numero de bits de um pixel */
   putc(byteZero, filePtr);   /* =0 origem no canto inf esquedo sem entrelacamento */

   /* muda os pixels de RGB para BGR */ 
   for (imageIdx = 0; imageIdx < 3*image->width*image->height ; imageIdx += 3) 
   {
      colorSwap = image->buf[imageIdx];
      image->buf[imageIdx] = image->buf[imageIdx + 2];
      image->buf[imageIdx + 2] = colorSwap;
   }

   /* escreve o buf de cores da imagem */
   fwrite(image->buf, sizeof(unsigned char), 3*image->width*image->height, filePtr);

   /* muda os pixels de BGR para RGB novamente */ 
   for (imageIdx = 0; imageIdx < 3*image->width*image->height ; imageIdx += 3) 
   {
      colorSwap = image->buf[imageIdx];
      image->buf[imageIdx] = image->buf[imageIdx + 2];
      image->buf[imageIdx + 2] = colorSwap;
   }

   fclose(filePtr);
   return 1;
}

