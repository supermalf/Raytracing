/**
 *	@file image.h Image: operacoes imagens em formato TGA.
 *
 *	@author 
 *          - Maira Noronha
 *          - Marcelo Gattass
 *
 *	@date
 *			Ultima Modificacao:		23 de Abril de 2003
 *
 *	@version 2.0
 */

#ifndef IMAGE_H
#define IMAGE_H

#include "color.h"

/************************************************************************/
/* Tipos Exportados                                                     */
/************************************************************************/
/**
 *   Imagem com um buffer rgb.
 */
struct Image_imp 
{
/** 
 * largura (width) em pixels
 */
  unsigned short width;
/**
 * altura (height) em pixels   
 */
  unsigned short height; 
/**
 * buffer RGB                  
 */
  unsigned char  *buf;              
};  

typedef struct Image_imp * Image;

/************************************************************************/
/* Funcoes Exportadas                                                   */
/************************************************************************/

/**
 *	Cria uma nova imagem com as dimensoes especificadas.
 *
 *	@param w Largura da imagem.
 *	@param h Altura da imagem.
 *
 *	@return Handle da imagem criada.
 */
Image   imageCreate (int w, int h);

/**
 *	Destroi a imagem.
 *
 *	@param image imagem a ser destruida.
 */
void    imageDestroy (Image image);

/**
 *	Obtem as dimensoes de uma imagem.
 *
 *	@param image Handle para uma imagem.
 *	@param w [out]Retorna a largura da imagem.
 *	@param h [out]Retorna a altura da imagem.
 */
void imageGetDimensions(Image image, int *w, int *h);

/**
 *	Ajusta o pixel de uma imagem com a cor especificada.
 *
 *	@param image Handle para uma imagem.
 *	@param x Posicao x na imagem.
 *	@param y Posicao y na imagem.
 *	@param color Cor do pixel.
 */
void imageSetPixel(Image image, int x, int y, Color color);

/**
 *	Obtem o pixel de uma imagem na posicao especificada.
 *
 *	@param image Handle para uma imagem.
 *	@param x Posicao x na imagem.
 *	@param y Posicao y na imagem.
 *
 *	@return Pixel da posicao especificada.
 */
Color imageGetPixel(Image image, int x, int y);

/**
 *	Redimensiona a imagem especificada.
 *
 *	@param image Handle para uma imagem.
 *	@param w1 Nova largura da imagem.
 *	@param h1 Nova altura da imagem.
 *  @return imagem criada.
 */
Image imageResize(Image img0, int w1, int h1);

/**
 *	Le a imagem a partir do arquivo especificado.
 *
 *	@param filename Nome do arquivo de imagem.
 *
 *	@return imagem criada.
 */
Image imageLoad(char *filename);

/**
 *	Salva a imagem no arquivo especificado em formato TGA.
 *
 *	@param filename Nome do arquivo de imagem.
 *	@param image Handle para uma imagem.
 *
 *	@return retorna 1 caso nao haja erros.
 */
int imageWriteTGA(char *filename, Image image);

#endif
