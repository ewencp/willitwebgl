// willitwebgl.cpp
// Copyright (c) 2010, Ewen Cheslack-Postava
// All rights reserved.
//
// Originally based on visualinfo.c from glew. See http://glew.sourceforge.net/
// Copyright (C) Nate Robins, 1997
//               Michael Wimmer, 1999
//               Milan Ikits, 2002-2008
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright notice,
//      this list of conditions and the following disclaimer in the documentation
//      and/or other materials provided with the distribution.
//    * Neither the name of willitwebgl nor the names of its contributors
//      may be used to endorse or promote products derived from this software
//      without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#if defined(_WIN32)
#include <GL/wglew.h>
#elif defined(__APPLE__) && !defined(GLEW_APPLE_GLX)
#include <AGL/agl.h>
#else
#include <GL/glxew.h>
#endif

#include <string>

#ifdef GLEW_MX
GLEWContext _glewctx;
#  define glewGetContext() (&_glewctx)
#  ifdef _WIN32
WGLEWContext _wglewctx;
#    define wglewGetContext() (&_wglewctx)
#  elif !defined(__APPLE__) || defined(GLEW_APPLE_GLX)
GLXEWContext _glxewctx;
#    define glxewGetContext() (&_glxewctx)
#  endif
#endif /* GLEW_MX */

typedef struct GLContextStruct
{
#ifdef _WIN32
  HWND wnd;
  HDC dc;
  HGLRC rc;
#elif defined(__APPLE__) && !defined(GLEW_APPLE_GLX)
  AGLContext ctx, octx;
#else
  Display* dpy;
  XVisualInfo* vi;
  GLXContext ctx;
  Window wnd;
  Colormap cmap;
#endif
} GLContext;

void InitContext (GLContext* ctx);
GLboolean CreateContext (GLContext* ctx);
void DestroyContext (GLContext* ctx);

void ReportInfo(const std::string& msg);

GLContext ctx;

int main(int argc, char** argv) {
    GLenum err;

    /* ---------------------------------------------------------------------- */
    /* create OpenGL rendering context */
    InitContext(&ctx);
    if (GL_TRUE == CreateContext(&ctx))
    {
        fprintf(stderr, "Error: CreateContext failed\n");
        DestroyContext(&ctx);
        return 1;
    }

    /* ---------------------------------------------------------------------- */
    /* output header information */
    /* OpenGL extensions */
    fprintf(stdout, "OpenGL vendor string: %s\n", glGetString(GL_VENDOR));
    fprintf(stdout, "OpenGL renderer string: %s\n", glGetString(GL_RENDERER));
    fprintf(stdout, "OpenGL version string: %s\n", glGetString(GL_VERSION));

    /* ---------------------------------------------------------------------- */
    /* release resources */
    DestroyContext(&ctx);

    return 0;
}

// Report information to the user.
void ReportInfo(const std::string& msg) {
    // FIXME This should use GUI output if possible since the user likely isn't
    // running from the command line.
    fprintf(stdout, "%s\n", msg.c_str());
}


#if defined(_WIN32)

void InitContext (GLContext* ctx)
{
  ctx->wnd = NULL;
  ctx->dc = NULL;
  ctx->rc = NULL;
}

GLboolean CreateContext (GLContext* ctx)
{
  WNDCLASS wc;
  PIXELFORMATDESCRIPTOR pfd;
  /* check for input */
  if (NULL == ctx) return GL_TRUE;
  /* register window class */
  ZeroMemory(&wc, sizeof(WNDCLASS));
  wc.hInstance = GetModuleHandle(NULL);
  wc.lpfnWndProc = DefWindowProc;
  wc.lpszClassName = "GLEW";
  if (0 == RegisterClass(&wc)) return GL_TRUE;
  /* create window */
  ctx->wnd = CreateWindow("GLEW", "GLEW", 0, CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL,
                          GetModuleHandle(NULL), NULL);
  if (NULL == ctx->wnd) return GL_TRUE;
  /* get the device context */
  ctx->dc = GetDC(ctx->wnd);
  if (NULL == ctx->dc) return GL_TRUE;
  /* find pixel format */
  ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
  if (visual == -1) /* find default */
  {
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
    visual = ChoosePixelFormat(ctx->dc, &pfd);
    if (0 == visual) return GL_TRUE;
  }
  /* set the pixel format for the dc */
  if (FALSE == SetPixelFormat(ctx->dc, visual, &pfd)) return GL_TRUE;
  /* create rendering context */
  ctx->rc = wglCreateContext(ctx->dc);
  if (NULL == ctx->rc) return GL_TRUE;
  if (FALSE == wglMakeCurrent(ctx->dc, ctx->rc)) return GL_TRUE;
  return GL_FALSE;
}

void DestroyContext (GLContext* ctx)
{
  if (NULL == ctx) return;
  if (NULL != ctx->rc) wglMakeCurrent(NULL, NULL);
  if (NULL != ctx->rc) wglDeleteContext(wglGetCurrentContext());
  if (NULL != ctx->wnd && NULL != ctx->dc) ReleaseDC(ctx->wnd, ctx->dc);
  if (NULL != ctx->wnd) DestroyWindow(ctx->wnd);
  UnregisterClass("GLEW", GetModuleHandle(NULL));
}

/* ------------------------------------------------------------------------ */

#elif defined(__APPLE__) && !defined(GLEW_APPLE_GLX)

void InitContext (GLContext* ctx)
{
  ctx->ctx = NULL;
  ctx->octx = NULL;
}

GLboolean CreateContext (GLContext* ctx)
{
  int attrib[] = { AGL_RGBA, AGL_NONE };
  AGLPixelFormat pf;
  /* check input */
  if (NULL == ctx) return GL_TRUE;
  /*int major, minor;
  SetPortWindowPort(wnd);
  aglGetVersion(&major, &minor);
  fprintf(stderr, "GL %d.%d\n", major, minor);*/
  pf = aglChoosePixelFormat(NULL, 0, attrib);
  if (NULL == pf) return GL_TRUE;
  ctx->ctx = aglCreateContext(pf, NULL);
  if (NULL == ctx->ctx || AGL_NO_ERROR != aglGetError()) return GL_TRUE;
  aglDestroyPixelFormat(pf);
  /*aglSetDrawable(ctx, GetWindowPort(wnd));*/
  ctx->octx = aglGetCurrentContext();
  if (GL_FALSE == aglSetCurrentContext(ctx->ctx)) return GL_TRUE;
  return GL_FALSE;
}

void DestroyContext (GLContext* ctx)
{
  if (NULL == ctx) return;
  aglSetCurrentContext(ctx->octx);
  if (NULL != ctx->ctx) aglDestroyContext(ctx->ctx);
}

/* ------------------------------------------------------------------------ */

#else /* __UNIX || (__APPLE__ && GLEW_APPLE_GLX) */

char* display = NULL;

void InitContext (GLContext* ctx)
{
  ctx->dpy = NULL;
  ctx->vi = NULL;
  ctx->ctx = NULL;
  ctx->wnd = 0;
  ctx->cmap = 0;
}

GLboolean CreateContext (GLContext* ctx)
{
  int attrib[] = { GLX_RGBA, GLX_DOUBLEBUFFER, None };
  int erb, evb;
  XSetWindowAttributes swa;
  /* check input */
  if (NULL == ctx) return GL_TRUE;
  /* open display */
  ctx->dpy = XOpenDisplay(display);
  if (NULL == ctx->dpy) return GL_TRUE;
  /* query for glx */
  if (!glXQueryExtension(ctx->dpy, &erb, &evb)) return GL_TRUE;
  /* choose visual */
  ctx->vi = glXChooseVisual(ctx->dpy, DefaultScreen(ctx->dpy), attrib);
  if (NULL == ctx->vi) return GL_TRUE;
  /* create context */
  ctx->ctx = glXCreateContext(ctx->dpy, ctx->vi, None, True);
  if (NULL == ctx->ctx) return GL_TRUE;
  /* create window */
  /*wnd = XCreateSimpleWindow(dpy, RootWindow(dpy, vi->screen), 0, 0, 1, 1, 1, 0, 0);*/
  ctx->cmap = XCreateColormap(ctx->dpy, RootWindow(ctx->dpy, ctx->vi->screen),
                              ctx->vi->visual, AllocNone);
  swa.border_pixel = 0;
  swa.colormap = ctx->cmap;
  ctx->wnd = XCreateWindow(ctx->dpy, RootWindow(ctx->dpy, ctx->vi->screen),
                           0, 0, 1, 1, 0, ctx->vi->depth, InputOutput, ctx->vi->visual,
                           CWBorderPixel | CWColormap, &swa);
  /* make context current */
  if (!glXMakeCurrent(ctx->dpy, ctx->wnd, ctx->ctx)) return GL_TRUE;
  return GL_FALSE;
}

void DestroyContext (GLContext* ctx)
{
  if (NULL != ctx->dpy && NULL != ctx->ctx) glXDestroyContext(ctx->dpy, ctx->ctx);
  if (NULL != ctx->dpy && 0 != ctx->wnd) XDestroyWindow(ctx->dpy, ctx->wnd);
  if (NULL != ctx->dpy && 0 != ctx->cmap) XFreeColormap(ctx->dpy, ctx->cmap);
  if (NULL != ctx->vi) XFree(ctx->vi);
  if (NULL != ctx->dpy) XCloseDisplay(ctx->dpy);
}

#endif /* __UNIX || (__APPLE__ && GLEW_APPLE_GLX) */
