-- Testea #drawRect pasándole como parámetro un número negativo en la coordenada y
local x,y = 10,-20
local width,height = 60,60
canvas:attrColor("red")
canvas:drawRect("frame",x,y,width,height)
canvas:flush()