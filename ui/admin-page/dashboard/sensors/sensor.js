function DigitalSensor(dashboard, numberOfContacts, x, y, sn){
    var self = {}

    self.dashboard = dashboard

    var element = document.createElement('digital_sensor')
    element.style.left = x + 'px'
    element.style.top = y + 'px'
    element.setAttribute('state', 'off')

    var label = document.createElement('label')
    label.innerHTML = sn + ': CC Sensor'
    element.appendChild(label)

    var ports = document.createElement('ports')
    element.appendChild(ports)

    for(var i = 0; i < numberOfContacts; ++i){
        var port = document.createElement('port')
        port.setAttribute('state', 'off')
        ports.appendChild(port)

        function f(i){
            port.onmousedown = function(event){
                event.stopPropagation()
            }

            port.onclick = function(event){
                dashboard.selectedNode = {
                    element: self,
                    targetPort: i
                }
                event.stopPropagation()
            }
        }

        f(i)
    }

    dashboard.setMovable(element)

    self.updateState = function(state){
        var ports = element.querySelectorAll('port')

        for(var i in state){
            if(state[i])
                ports[i].setAttribute('state', 'on')
            else
                ports[i].setAttribute('state', 'off')
        }
    }

    dashboard.canvas.appendChild(element)
    self.element = element

    return self
}

function AnalogSensor(dashboard, numberOfContacts, x, y, sn){
    var self = {}

    self.dashboard = dashboard

    var element = document.createElement('analog_sensor')
    element.style.left = x + 'px'
    element.style.top = y + 'px'
    element.setAttribute('state', 'off')

    var label = document.createElement('label')
    label.innerHTML = sn + ': ADC'
    element.appendChild(label)

    var ports = document.createElement('ports')
    element.appendChild(ports)

    for(var i = 0; i < numberOfContacts; ++i){
        var port = document.createElement('port')
        port.innerHTML = 0
        ports.appendChild(port)
    }

    var portNumbers = document.createElement('port_numbers')
    element.appendChild(portNumbers)

    for(var i = 0; i < numberOfContacts; ++i){
        var f = function(i){
            var portNumber = document.createElement('port_number')
            portNumber.innerHTML = i + 1

            portNumber.onmousedown = function(event){
                event.stopPropagation()
            }

            portNumber.onclick = function(event){
                dashboard.selectedNode = {
                    element: self,
                    targetPort: i
                }
                event.stopPropagation()
            }

            portNumbers.appendChild(portNumber)
        }

        f(i)
    }

    self.updateState = function(state){
        var ports = element.querySelectorAll('port')

        for(var i in state)
            ports[i].innerHTML = state[i]
    }

    self.dashboard.setMovable(element)
    dashboard.canvas.appendChild(element)
    self.element = element

    return self
}