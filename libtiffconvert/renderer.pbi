XIncludeFile "defs.pbi"

Prototype.i renderer_filter(x.i, y.i, source.i, target.i)

Declare.i renderer_filter_or(filter.renderer_filter, mode)

DeclareCDLL.i renderer_begin(*handle.tiff_image, page.l)
DeclareCDLL.i renderer_stop()

DeclareCDLL.i renderer_custom(proc.renderer_filter)

DeclareCDLL.i renderer_line(*points.POINT, count.l, size.l, color.l, filter.renderer_filter = #Null)
DeclareCDLL.i renderer_single_line(*points.POINT, count.l, color.l, filter.renderer_filter = #Null)
DeclareCDLL.i renderer_rect(*bounds.RECT, fillColor.l, strokeColor.l, fill.l, stroke.l, radius.l, strokeThickness.l = 0, filter.renderer_filter = #Null)

Declare.i     renderer_text(x.l, y.l, text.s, font.i, color.l, filter.renderer_filter = #Null)
DeclareCDLL.i renderer_text_a(*point.RECT, *text, *font.font_handle, color.l, filter.renderer_filter = #Null)
DeclareCDLL.i renderer_text_w(*point.RECT, *text, *font.font_handle, color.l, filter.renderer_filter = #Null)

DeclareCDLL.i renderer_image(*bounds.RECT, *image.image_handle, filter.renderer_filter = #Null)
DeclareCDLL.i renderer_image_alpha(*bounds.RECT, *image.image_handle, alpha.a = 255, filter.renderer_filter = #Null)

; applies a custom filter if filter != #Null together with the provided drawing mode, 
; otherwise it will only apply the provided drawing mode.
Procedure.i renderer_filter_or(filter.renderer_filter, mode)
  If (filter)
    DrawingMode(#PB_2DDrawing_CustomFilter | mode)
    CustomFilterCallback(filter)
  Else 
    DrawingMode(mode)
  EndIf 
EndProcedure

; Start rendering onto the output of a single tiff page
ProcedureCDLL.i renderer_begin(*handle.tiff_image, page.l)
  If (page < 0 Or page >= *handle\PageCount)
    ProcedureReturn #False 
  EndIf 
  
  ProcedureReturn StartDrawing(ImageOutput(*handle\PageHandles(page)))
EndProcedure

; Stop rendering
ProcedureCDLL.i renderer_stop()
  StopDrawing()
EndProcedure

; Render a line starting at *points[0] and ending at *points[count - 1]
ProcedureCDLL.i renderer_line(*points.POINT, count.l, size.l, color.l, filter.renderer_filter = #Null)
  If (count <= 1)
    ProcedureReturn #False 
  EndIf 
  
  renderer_filter_or(filter, #PB_2DDrawing_AlphaBlend)
  
  Protected *a.POINT
  Protected *b.POINT
  
  For i = 0 To count - 2
    *a = *points + SizeOf(POINT) * i
    *b = *points + SizeOf(POINT) * (i + 1)
    thickLineXY(*a\x, *a\y, *b\x, *b\y, size / 2, color)
  Next 
  
  ProcedureReturn #True 
EndProcedure

; Render a line starting at *points[0] and ending at *points[count - 1], 1px
ProcedureCDLL.i renderer_single_line(*points.POINT, count.l, color.l, filter.renderer_filter = #Null)
  If (count <= 1)
    ProcedureReturn #False 
  EndIf 
  
  renderer_filter_or(filter, #PB_2DDrawing_AlphaBlend)
  
  Protected *a.POINT
  Protected *b.POINT
  
  For i = 0 To count - 2
    *a = *points + SizeOf(POINT) * i
    *b = *points + SizeOf(POINT) * (i + 1)
    LineXY(*a\x, *a\y, *b\x, *b\y, color)
  Next 
  
  ProcedureReturn #True 
EndProcedure

; Render a rectangle or rounded rectangle, and fill and/or stroke the shape.
ProcedureCDLL.i renderer_rect(*bounds.RECT, fillColor.l, strokeColor.l, fill.l, stroke.l, radius.l, strokeThickness.l = 0, filter.renderer_filter = #Null)
  Protected x, y, w, h 
  
  If (radius = 0)
    If (fill)
      renderer_filter_or(filter, #PB_2DDrawing_AlphaBlend)
      Box(*bounds\left, *bounds\top, *bounds\right - *bounds\left, *bounds\bottom - *bounds\top, fillColor)
    EndIf 
    
    If (stroke And strokeThickness > 0)
      renderer_filter_or(filter, #PB_2DDrawing_AlphaBlend | #PB_2DDrawing_Outlined)
      
      x = *bounds\left
      y = *bounds\top 
      w = *bounds\right - x
      h = *bounds\bottom - y
      
      For i = 1 To strokeThickness
        Box(x, y, w, h, strokeColor)
        x + 1 : y + 1 : w - 2 : h - 2
      Next 
    EndIf 
  Else 
    If (fill)
      renderer_filter_or(filter, #PB_2DDrawing_AlphaBlend)
      RoundBox(*bounds\left, *bounds\top, *bounds\right - *bounds\left, *bounds\bottom - *bounds\top, radius, radius, fillColor)
    EndIf 
    
    If (stroke And strokeThickness > 0)
      renderer_filter_or(filter, #PB_2DDrawing_AlphaBlend | #PB_2DDrawing_Outlined)
      
      x = *bounds\left
      y = *bounds\top 
      w = *bounds\right - x
      h = *bounds\bottom - y
      
      For i = 1 To strokeThickness
        RoundBox(x, y, w, h, radius, radius, strokeColor)
        x + 1 : y + 1 : w - 2 : h - 2
      Next 
    EndIf 
  EndIf 
  
  ProcedureReturn #True 
EndProcedure

; render (multiline) text 
; FIXME: implement word wrapping, this is not implemented right now and should be 
Procedure.i renderer_text(x.l, y.l, text.s, font.i, color.l, filter.renderer_filter = #Null)
  renderer_filter_or(filter, #PB_2DDrawing_AlphaBlend | #PB_2DDrawing_Transparent) 
  DrawingFont(FontID(font))
  
  text = ReplaceString(text, #CRLF$, #LF$)
  
  Protected lines  = CountString(text, #LF$) + 1
  Protected ycoord = y 
  Protected line.s
  
  For i = 1 To lines 
    line = StringField(text, i, #LF$)
    DrawText(x, y, StringField(text, i, #LF$), color)
    If (Trim(line) = "")
      line = "abcdefABCDEF"
    EndIf 
    y + TextHeight(line)
  Next 
  
  
  ProcedureReturn #True 
EndProcedure 

; render (multiline) ascii-text
ProcedureCDLL.i renderer_text_a(*point.RECT, *text, *font.font_handle, color.l, filter.renderer_filter = #Null)
  ProcedureReturn renderer_text(*point\left, *point\top, util_ansi_to_unicode(*text), *font\hFont, color, filter)
EndProcedure

; render (multiline) unicode-text
ProcedureCDLL.i renderer_text_w(*point.RECT, *text, *font.font_handle, color.l, filter.renderer_filter = #Null)
  ProcedureReturn renderer_text(*point\left, *point\top, PeekS(*text), *font\hFont, color, filter)
EndProcedure

; render an image, this has to be an image that is loaded through the image module
ProcedureCDLL.i renderer_image(*bounds.RECT, *image.image_handle, filter.renderer_filter = #Null)
  If (Not IsImage(*image\hImage))
    ProcedureReturn #False 
  EndIf 
  
  renderer_filter_or(filter, #PB_2DDrawing_AlphaBlend)
  DrawImage(ImageID(*image\hImage), *bounds\left, *bounds\top, *bounds\right - *bounds\left, *bounds\bottom - *bounds\top)
  
  ProcedureReturn #True 
EndProcedure

; render an image by alpha-blending it into the output, this has to be an image that is loaded through the image module
ProcedureCDLL.i renderer_image_alpha(*bounds.RECT, *image.image_handle, alpha.a = 255, filter.renderer_filter = #Null)
  If (Not IsImage(*image\hImage))
    ProcedureReturn #False 
  EndIf 
  
  renderer_filter_or(filter, #PB_2DDrawing_AlphaBlend)
  DrawAlphaImage(ImageID(*image\hImage), *bounds\left, *bounds\top, alpha)
  
  ProcedureReturn #True 
EndProcedure
; IDE Options = PureBasic 5.72 (Windows - x64)
; CursorPosition = 181
; FirstLine = 142
; Folding = --
; EnableXP