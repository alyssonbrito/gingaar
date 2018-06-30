-- Test posting an event of class ncl, type presentation and action resume
require 'testSystem.testSystem'

function handlerOne(evt)
  if evt.class == 'key' then return end
  
  if evt.class == 'ncl' and evt.action == "start" then
    newEvent = {
      class='ncl',
      type='presentation',
      action='resume',
      label='label'
    }
    assertTrue(event.post( 'in', newEvent ), 'No se pudo enviar el evento')
    return true
  end
end

function nclHandler(evt)
  if evt.class == 'key' then return end
  if evt.class == 'ncl' and evt.action == 'stop' then return end
  
  if evt.class == 'ncl' then
    assertEquals(evt.type, 'presentation', 'Se esperaba recibir el evento de type presentation' )
    assertEquals(evt.action, 'resume', 'Se esperaba recibir el evento con action resume')
  end
end

event.register(1, handlerOne )
event.register(2, nclHandler )
