# StoneEng -- Heavily WIP
StoneEng is a 2D engine designed primarily to create top-down games using OpenGL rendering.
The intent for StoneEng is to be lightweight in overhead as well as dependencies.

The engine will be completely coded in C with the end goal to have an exposed API that cap be used in other languages, such as python through C bindings.
The main reason for its creation is primarily to improve C knowledge, however there are several final game ideas in mind and as such design decisions are centered around that.

### Third-Party libs
*I intend to keep this list at an absolute minimum and use libraries that can be directly included
to avoid dependency hell.*

[**stb_image.h**](https://github.com/let-def/stb_image) -- A ubiquitous header-only library for loading images.  
[**zf_log.h**](https://github.com/wonder-mice/zf_log) -- An easy to use, lightweight, header-only logging library.   
[**glad**](https://glad.dav1d.de/) -- OpenGL extension loader.  
[**glfw**](https://github.com/glfw/glfw) -- For cross platform window handling.  
