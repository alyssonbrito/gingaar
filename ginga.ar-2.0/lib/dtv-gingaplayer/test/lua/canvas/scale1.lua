-- Testea #attrScale: escalar un canvas a menor tamaño.
local subCanvas = canvas:new(500,500)
subCanvas:attrColor("green")
subCanvas:drawRect("fill",0,0,250,250)
subCanvas:attrColor("red")
subCanvas:drawRect("fill",0,250,250,250)
subCanvas:attrColor("blue")
subCanvas:drawRect("fill",250,0,250,250)
subCanvas:attrColor("yellow")
subCanvas:drawRect("fill",250,250,250,250)
subCanvas:attrScale(100,100)
canvas:compose(0,0,subCanvas)
canvas:flush()
