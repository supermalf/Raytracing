/**
 *	@file zbuffer.h ZBuffer: desenha a cena com o OpenGL.
 *
 *	@author 
 *			- Marcelo Gattass
 *
 *	@date
 *			Criado em:			7 de novembro de 2003
 *
 *	@version 1.0
 */

#ifndef _ZBUFFER_H_
#define _ZBUFFER_H_

#include <gl/glut.h>
#include "scene.h"

/**
 *	Desenha com o OpenGL (ZBuffer) uma cena.
 *
 *	@param scene tipo abstrato a cena.
 *	@param winW  largura da janela em pixels.
 *	@param winH  altura da janela em pixels.
 *
 */
void ZBufferScene(Scene* scene, int winW, int winH );

#endif
