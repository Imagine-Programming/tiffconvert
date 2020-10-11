; This is the main file, as PureBasic requires one source file to be compiled to 
; one compiler output binary. This file merely includes the exported modules so 
; that they will be compiled as well.

EnableExplicit ; force all symbols to be declared, have PB make no assumptions.

; link with all the decoders, as tiff might contain embedded
; images in marks we need to render onto the burned output image.
UseTIFFImageDecoder()
UseTGAImageDecoder()
UseGIFImageDecoder()
UsePNGImageDecoder()
UsePNGImageEncoder()
UseJPEGImageDecoder()
UseJPEGImageEncoder()
UseJPEG2000ImageDecoder()
UseJPEG2000ImageEncoder()

; utility exports 
XIncludeFile "utilities.pbi"

; tiff loading exports 
XIncludeFile "tiff-image.pbi"

; renderer exports 
XIncludeFile "renderer.pbi"

; font exports 
XIncludeFile "font.pbi"

; image exports 
XIncludeFile "image.pbi"
; IDE Options = PureBasic 5.71 LTS (Windows - x64)
; ExecutableFormat = Shared dll
; CursorPosition = 4
; EnableThread
; EnableXP
; Executable = libtiffconvert.dll