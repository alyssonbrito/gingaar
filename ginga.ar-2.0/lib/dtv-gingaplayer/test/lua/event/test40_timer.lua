-- Test calling two timers with the same callback function
require 'testSystem.testSystem'

local cant = 0

function testResult()
  assertEquals(cant, 2)
  event.post( {class = 'user', action = 'signal'} )
end

function callback()
  cant = cant + 1
end

function handler( evt )
  if evt.class == 'key' then return end

  if evt.class == 'ncl' and evt.action == 'start' then
    event.post( {class = 'user', action = 'wait'} )
    event.timer(10, callback)
    event.timer(10, callback)
    event.timer(30, testResult)
  end
end

event.register(handler)