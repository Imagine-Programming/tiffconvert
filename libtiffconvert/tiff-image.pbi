XIncludeFile "pbPDFModule.pbi"
XIncludeFile "multipagetiff.pbi"
XIncludeFile "utilities.pbi"
XIncludeFile "defs.pbi"

Declare.i       tiff_image_copy_page(*handle.tiff_header, page.l)
Declare.i       tiff_verify_header(*buffer.tiff_header, size.l)
Declare.i       tiff_image_open(szFilepath.s)
DeclareCDLL.i   tiff_image_open_p(*buffer.tiff_header, size.l, release_raw.l = #False)
DeclareCDLL.i   tiff_image_open_a(*filepath)
DeclareCDLL.i   tiff_image_open_w(*filepath)
DeclareCDLL.i   tiff_image_page_count(*handle.tiff_image)
DeclareCDLL.i   tiff_image_close(*handle.tiff_image)

DeclareCDLL.i   tiff_image_page_width(*handle.tiff_image, page.l)
DeclareCDLL.i   tiff_image_page_height(*handle.tiff_image, page.l)
DeclareCDLL.i   tiff_image_page_scale(*handle.tiff_image, page.l, width.l, height.l, smooth.l)

Declare.i       tiff_image_export_page(*handle.tiff_image, page.l, szFilepath.s, codec.l, options.l)
DeclareCDLL.i   tiff_image_export_page_a(*handle.tiff_image, page.l, *filepath, codec.l, options.l)
DeclareCDLL.i   tiff_image_export_page_w(*handle.tiff_image, page.l, *filepath, codec.l, options.l)
DeclareCDLL.i   tiff_image_export_page_p(*handle.tiff_image, page.l, *lpdwSize, codec.l, options.l)
DeclareCDLL.i   tiff_image_export_page_p24(*handle.tiff_image, page.l, *lpdwSize, codec.l, options.l)

Declare.i       tiff_get_image_format(codec.l)
Declare.i       tiff_image_cleanup_pdf(pdf.l, List images.i())
Declare.i       tiff_image_scale(*lpdwWidth, *lpdwHeight, maxWidth, maxHeight)
Declare.i       tiff_image_export_pdf(*handle.tiff_image, szFilepath.s, codec.l, options.l)
DeclareCDLL.i   tiff_image_export_pdf_a(*handle.tiff_image, *filepath, codec.l, options.l)
DeclareCDLL.i   tiff_image_export_pdf_w(*handle.tiff_image, *filepath, codec.l, options.l)

; Copy a tiff image page to 32-bit RGBA format
Procedure.i tiff_image_copy_page(*handle.tiff_image, page.l)
  Protected original = CatchTIFFPage(#PB_Any, *handle\RawData, page)
  If (Not original)
    ProcedureReturn #Null 
  EndIf 
  
  Protected copy = CreateImage(#PB_Any, ImageWidth(original), ImageHeight(original), 32, #PB_Image_Transparent)
  If (Not copy)
    FreeImage(original)
    ProcedureReturn #Null 
  EndIf 
  
  If (StartDrawing(ImageOutput(copy)))
    DrawingMode(#PB_2DDrawing_AlphaBlend)
    DrawAlphaImage(ImageID(original), 0, 0, 255)
    StopDrawing()
  Else 
    FreeImage(original)
    FreeImage(copy)
    ProcedureReturn #Null 
  EndIf 
  
  FreeImage(original)
  ProcedureReturn copy
EndProcedure

; Verify that the @*buffer points to data that contains a valid 
; tiff image header, which also contains a valid IFD0 offset.
Procedure.i tiff_verify_header(*buffer.tiff_header, size.l)
  ; not a tiff
  If (*buffer\ByteOrder <> $4949 And *buffer\ByteOrder <> $4d4d)
    ProcedureReturn #False 
  EndIf 
  
  If (*buffer\ByteOrder = $4d4d)
    *buffer\Magic = SwapBytesW(*buffer\Magic)
    *buffer\IFD0  = SwapBytesL(*buffer\IFD0)
    
    ; magic number invalid, not a tiff
    If (*buffer\Magic <> 42)
      ProcedureReturn #False 
    EndIf 
  EndIf 
  
  ; offset to IFD0 exceeds size
  If (*buffer\IFD0 >= size)
    ProcedureReturn #False 
  EndIf 
  
  ProcedureReturn #True 
EndProcedure

; Open a file as tiff image and return a handle to a struct 
; containing a pointer to the raw data, and an array of decoded
; images representing each page.
Procedure.i tiff_image_open(szFilepath.s)
  Protected *buffer.tiff_header = ReadFileToMemory(szFilepath)
  If (Not *buffer)
    ProcedureReturn #Null
  EndIf 
  
  Protected *handle.tiff_image = tiff_image_open_p(*buffer, MemorySize(*buffer), #True)
  If (Not *handle)
    ProcedureReturn #Null 
  EndIf 
  
  *handle\Filepath = szFilepath
  ProcedureReturn *handle
EndProcedure

; Decode a buffer as tiff image and reutrn a handle to a struct
; containing a pointer to the raw data, and an array of decoded
; images representing each page. Always specify param 3 as #false
; when used externally. Only PureBasic buffers from this DLL can
; be freed, the rest is up to you.
ProcedureCDLL.i tiff_image_open_p(*buffer.tiff_header, size.l, release_raw.l = #False)
  If (Not tiff_verify_header(*buffer, size))
    If (release_raw)
      FreeMemory(*buffer)
    EndIf 
    ProcedureReturn #Null
  EndIf 
  
  Protected pages = CatchTIFFPage(#PB_Any, *buffer, -1)
  If (Not pages)
    If (release_raw)
      FreeMemory(*buffer)
    EndIf 
    ProcedureReturn #Null
  EndIf 
  
  Protected *handle.tiff_image = AllocateMemory(SizeOf(tiff_image))
  If (Not *handle)
    If (release_raw)
      FreeMemory(*buffer)
    EndIf 
    ProcedureReturn #Null
  EndIf 
  
  With *handle
    \Filepath   = ""
    \RawData    = *buffer 
    \PageCount  = pages
    \ReleaseRaw = release_raw
    
    Dim \PageHandles(\PageCount)
  EndWith

  Protected i  
  For i = 0 To *handle\PageCount - 1 
    *handle\PageHandles(i) = tiff_image_copy_page(*handle, i)
    If (Not *handle\PageHandles(i))
      tiff_image_close(*handle)
      ProcedureReturn #Null 
    EndIf 
  Next 
  
  ProcedureReturn *handle 
EndProcedure

; Open a file as tiff image and return a handle to a struct 
; containing a pointer to the raw data, and an array of decoded
; images representing each page.
ProcedureCDLL.i tiff_image_open_a(*filepath)
  ProcedureReturn tiff_image_open(util_ansi_to_unicode(*filepath))
EndProcedure

; Open a file as tiff image and return a handle to a struct 
; containing a pointer to the raw data, and an array of decoded
; images representing each page.
ProcedureCDLL.i tiff_image_open_w(*filepath)
  ProcedureReturn tiff_image_open(PeekS(*filepath))
EndProcedure

; Determine the number of pages in an opened tiff image
ProcedureCDLL.i tiff_image_page_count(*handle.tiff_image)
  ProcedureReturn *handle\PageCount
EndProcedure

; Close a previously opened tiff image and release all the resources
; used by the handle. This closes all the page images and frees the 
; raw data for the image data, but also for the handle.
ProcedureCDLL.i tiff_image_close(*handle.tiff_image)
  Protected i
  For i = 0 To ArraySize(*handle\PageHandles()) - 1 
    If (IsImage(*handle\PageHandles(i)))
      FreeImage(*handle\PageHandles(i))
    EndIf 
  Next 
  
  If (*handle\ReleaseRaw)
    FreeMemory(*handle\RawData)
  EndIf 
  
  FreeMemory(*handle)
EndProcedure

; Determine the width in pixels of a specific page
ProcedureCDLL.i tiff_image_page_width(*handle.tiff_image, page.l)
  If (page < 0 Or page >= *handle\PageCount)
    ProcedureReturn 0
  EndIf 
  
  If (Not IsImage(*handle\PageHandles(page)))
    ProcedureReturn 0
  EndIf 
  
  ProcedureReturn ImageWidth(*handle\PageHandles(page))
EndProcedure

; Determine the height in pixels of a specific page
ProcedureCDLL.i tiff_image_page_height(*handle.tiff_image, page.l)
  If (page < 0 Or page >= *handle\PageCount)
    ProcedureReturn 0
  EndIf 
  
  If (Not IsImage(*handle\PageHandles(page)))
    ProcedureReturn 0
  EndIf 
  
  ProcedureReturn ImageHeight(*handle\PageHandles(page))
EndProcedure

ProcedureCDLL.i tiff_image_page_scale(*handle.tiff_image, page.l, maxwidth.l, maxheight.l, smooth.l)
  If (page < 0 Or page >= *handle\PageCount)
    ProcedureReturn #False 
  EndIf 
  
  Protected width.l   = tiff_image_page_width(*handle, page)
  Protected height.l  = tiff_image_page_height(*handle, page)
  Protected options.l = #PB_Image_Raw
  
  If (maxwidth = 0)  : maxwidth  = width  : EndIf 
  If (maxheight = 0) : maxheight = height : EndIf 
  If (smooth)        : options   = #PB_Image_Smooth : EndIf 
  
  ; no scaling required, image has dimensions within limits
  If (width <= maxwidth And height <= maxheight)
    ProcedureReturn #True 
  EndIf 
  
  tiff_image_scale(@width, @height, maxwidth, maxheight)
  ProcedureReturn ResizeImage(*handle\PageHandles(page), width, height, options)
EndProcedure

; Export a single page from the tiff image to image file.
Procedure.i tiff_image_export_page(*handle.tiff_image, page.l, szFilepath.s, codec.l, options.l)
  If (page < 0 Or page >= *handle\PageCount)
    ProcedureReturn #False 
  EndIf 
  
  Select codec 
    Case #TIFF_EXPORT_PNG
      ProcedureReturn SaveImage(*handle\PageHandles(page), szFilepath, #PB_ImagePlugin_PNG)
    Case #TIFF_EXPORT_JPEG
      ProcedureReturn SaveImage(*handle\PageHandles(page), szFilepath, #PB_ImagePlugin_JPEG, options)
    Case #TIFF_EXPORT_JPEG2000
      ProcedureReturn SaveImage(*handle\PageHandles(page), szFilepath, #PB_ImagePlugin_JPEG2000, options)
    Case #TIFF_EXPORT_BITMAP
      ProcedureReturn SaveImage(*handle\PageHandles(page), szFilepath, #PB_ImagePlugin_BMP)
    Default
      ProcedureReturn #False 
  EndSelect
EndProcedure

; Export a single page from the tiff image to image file.
ProcedureCDLL.i tiff_image_export_page_a(*handle.tiff_image, page.l, *filepath, codec.l, options.l)
  ProcedureReturn tiff_image_export_page(*handle, page, util_ansi_to_unicode(*filepath), codec, options)
EndProcedure

; Export a single page from the tiff image to image file.
ProcedureCDLL.i tiff_image_export_page_w(*handle.tiff_image, page.l, *filepath, codec.l, options.l)
  ProcedureReturn tiff_image_export_page(*handle, page, PeekS(*filepath), codec, options)
EndProcedure

; Export a single page from the tiff image to encoded buffer.
ProcedureCDLL.i tiff_image_export_page_p(*handle.tiff_image, page.l, *lpdwSize, codec.l, options.l)
  If (page < 0 Or page >= *handle\PageCount)
    ProcedureReturn #Null 
  EndIf 
  
  Protected *buffer = #Null 
  
  Select codec 
    Case #TIFF_EXPORT_PNG
      *buffer = EncodeImage(*handle\PageHandles(page), #PB_ImagePlugin_PNG)
    Case #TIFF_EXPORT_JPEG
      *buffer = EncodeImage(*handle\PageHandles(page), #PB_ImagePlugin_JPEG, options)
    Case #TIFF_EXPORT_JPEG2000
      *buffer = EncodeImage(*handle\PageHandles(page), #PB_ImagePlugin_JPEG2000, options)
    Case #TIFF_EXPORT_BITMAP
      *buffer = EncodeImage(*handle\PageHandles(page), #PB_ImagePlugin_BMP)
    Default
      ProcedureReturn #Null 
  EndSelect
  
  If (*buffer And *lpdwSize)
    PokeL(*lpdwSize, MemorySize(*buffer))
  EndIf 
  
  ProcedureReturn *buffer
EndProcedure

ProcedureCDLL.i tiff_image_export_page_p24(*handle.tiff_image, page.l, *lpdwSize, codec.l, options.l)
  If (page < 0 Or page >= *handle\PageCount)
    ProcedureReturn #Null 
  EndIf 
  
  Protected *buffer = #Null 
  
  Select codec 
    Case #TIFF_EXPORT_PNG
      *buffer = EncodeImage(*handle\PageHandles(page), #PB_ImagePlugin_PNG, 0, 24)
    Case #TIFF_EXPORT_JPEG
      *buffer = EncodeImage(*handle\PageHandles(page), #PB_ImagePlugin_JPEG, options, 24)
    Case #TIFF_EXPORT_JPEG2000
      *buffer = EncodeImage(*handle\PageHandles(page), #PB_ImagePlugin_JPEG2000, options, 24)
    Case #TIFF_EXPORT_BITMAP
      *buffer = EncodeImage(*handle\PageHandles(page), #PB_ImagePlugin_BMP, 0, 24)
    Default
      ProcedureReturn #Null 
  EndSelect
  
  If (*buffer And *lpdwSize)
    PokeL(*lpdwSize, MemorySize(*buffer))
  EndIf 
  
  ProcedureReturn *buffer
EndProcedure

Procedure.i tiff_pdf_get_image_format(codec.l)
  Select codec 
    Case #TIFF_EXPORT_PNG
      ProcedureReturn PDF::#Image_PNG
    Case #TIFF_EXPORT_JPEG
      ProcedureReturn PDF::#Image_JPEG
    Case #TIFF_EXPORT_JPEG2000
      ProcedureReturn PDF::#Image_JPEG2000
    Default
      ProcedureReturn -1 
  EndSelect
EndProcedure

Procedure.i tiff_image_cleanup_pdf(pdf.l, List images.i())
  If (pdf <> 0)
    PDF::Close(pdf)
  EndIf 
  
  ForEach (images())
    If (IsImage(images()))
      FreeMemory(images())
    EndIf 
  Next 
EndProcedure

Procedure.i tiff_image_scale(*lpdwWidth, *lpdwHeight, maxWidth, maxHeight)
  Protected ow.d = PeekL(*lpdwWidth)
  Protected oh.d = PeekL(*lpdwHeight)
  Protected mw.d = maxWidth
  Protected mh.d = maxHeight
  
  If (mw * oh < mh * ow)
    PokeL(*lpdwWidth, mw)
    PokeL(*lpdwHeight, oh * mw / ow)
  Else 
    PokeL(*lpdwWidth, ow * mh / oh)
    PokeL(*lpdwHeight, mh)
  EndIf 
EndProcedure

Procedure.i tiff_image_export_pdf(*handle.tiff_image, szFilepath.s, codec.l, options.l)
  If (*handle\PageCount = 0)
    ProcedureReturn #False 
  EndIf 
  
  If (Not IsImage(*handle\PageHandles(0)))
    ProcedureReturn #False 
  EndIf 
  
  If (codec < 0 Or codec > #TIFF_EXPORT_JPEG2000)
    ProcedureReturn #False 
  EndIf 
  
  Protected landscape  = Bool(ImageWidth(*handle\PageHandles(0)) > ImageHeight(*handle\PageHandles(0)))
  Protected pdf_mode.s = "P"
  If (landscape) : pdf_mode = "L" : EndIf 
  
  Protected pdf = PDF::Create(#PB_Any, pdf_mode, "mm", PDF::#Format_A4)
  If (Not pdf)
    ProcedureReturn #False 
  EndIf 
  
  Protected NewList images.i()
  
  PDF::SetMargin(pdf, PDF::#LeftMargin, 0)
  PDF::SetMargin(pdf, PDF::#RightMargin, 0)
  PDF::SetMargin(pdf, PDF::#TopMargin, 0)
  
  Protected i
  For i = 0 To *handle\PageCount - 1 
    Protected size.l
    Protected *image = tiff_image_export_page_p24(*handle, i, @size, codec, options)
    
    landscape = Bool(ImageWidth(*handle\PageHandles(i)) > ImageHeight(*handle\PageHandles(i)))
    pdf_mode.s = "P"
    If (landscape) : pdf_mode = "L" : EndIf 
    
    If (Not *image)
      tiff_image_cleanup_pdf(pdf, images())
      ProcedureReturn #False 
    EndIf 
    
    AddElement(images())
    images() = *image
    
    Protected width.l     = ImageWidth(*handle\PageHandles(i))
    Protected height.l    = ImageHeight(*handle\PageHandles(i))
    Protected maxWidth.l  = PDF::GetPageWidth(pdf) - PDF::GetMargin(pdf, PDF::#LeftMargin) - PDF::GetMargin(pdf, PDF::#RightMargin)
    Protected maxHeight.l = PDF::GetPageHeight(pdf) - PDF::GetMargin(pdf, PDF::#TopMargin)
    
    tiff_image_scale(@width, @height, maxWidth, maxHeight)
    
    PDF::AddPage(pdf, pdf_mode, PDF::#Format_A4)
    PDF::ImageMemory(pdf, "tiff_page_" + Str(i), *image, size, tiff_pdf_get_image_format(codec), #PB_Default, #PB_Default, width, height)
  Next 
  
  PDF::Close(pdf, szFilepath)
  
  If (PDF::GetErrorCode() <> 0)
    tiff_image_cleanup_pdf(0, images())
    ProcedureReturn #False 
  EndIf 
  
  tiff_image_cleanup_pdf(0, images())
  ProcedureReturn #True 
EndProcedure

; Export all the pages in the tiff to a PDF.
ProcedureCDLL.i tiff_image_export_pdf_a(*handle.tiff_image, *filepath, codec.l, options.l)
  ProcedureReturn tiff_image_export_pdf(*handle, util_ansi_to_unicode(*filepath), codec, options)
EndProcedure

; Export all the pages in the tiff to a PDF.
ProcedureCDLL.i tiff_image_export_pdf_w(*handle.tiff_image, *filepath, codec.l, options.l)
  ProcedureReturn tiff_image_export_pdf(*handle, PeekS(*filepath), codec, options)
EndProcedure

; IDE Options = PureBasic 5.71 LTS (Windows - x64)
; CursorPosition = 390
; FirstLine = 386
; Folding = ----
; EnableXP