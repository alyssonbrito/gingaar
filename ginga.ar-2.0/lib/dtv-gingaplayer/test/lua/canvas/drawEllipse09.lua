-- Testea #drawEllipse pasándole como parámetro un número negativo en la coordenada y
local xc,yc = 360,-100
local width,height = 320,320
local ang_start, ang_end = 0,360
canvas:attrColor("red")
canvas:drawEllipse("fill",xc,yc,width,height,ang_start,ang_end)
canvas:flush()