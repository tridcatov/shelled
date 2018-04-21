function DigitalRelay(dashboard, numberOfContacts, onchange, x, y){
    var self = {}

    self.dashboard = dashboard

    var element = document.createElement('digital_relay')
    element.style.left = x + 'px'
    element.style.top = y + 'px'
    element.setAttribute('state', 'off')

    var ports = document.createElement('ports')
    element.appendChild(ports)

    for(var i = 0; i < numberOfContacts; ++i){
        var f = function(i){
            var port = document.createElement('port')
            port.setAttribute('state', 'off')

            port.onmouseup = function(event){
                event.stopPropagation()
            }

            port.onmousedown = function(event){
                event.stopPropagation()
            }
            
            port.onclick = function(event){
                if(dashboard.selectedNode){
                    if(dashboard.onNewArrow){
                        dashboard.onNewArrow({
                            source: dashboard.selectedNode,
                            destination: {
                                element: self,
                                targetPort: i
                            },
                            arrow: dashboard.arrow
                        })
                    }
                    else{
                        dashboard.arrows.push({
                            source: dashboard.selectedNode,
                            destination: {
                                element: self,
                                targetPort: i
                            },
                            arrow: dashboard.arrow
                        })

                        dashboard.arrow = createArrow(dashboard.svg)
                        dashboard.updateArrows()
                    }
                }
                else
                    updateArrow(dashboard.arrow, 0, 0, 0, 0)

                dashboard.selectedNode = undefined
                updateArrow(dashboard.arrow, 0, 0, 0, 0)

                event.stopPropagation()

                /*if(port.getAttribute('state') == 'on'){
                    port.setAttribute('state', 'off')
                    onchange(i, 0)
                }
                else{
                    port.setAttribute('state', 'on')
                    onchange(i, 1)
                }*/
            }
            
            ports.appendChild(port)
        }

        f(i)
    }

    var label = document.createElement('label')
    label.innerHTML = 'Relay'
    element.appendChild(label)

    dashboard.setMovable(element)
    dashboard.canvas.appendChild(element)
    self.element = element

    return self
}

function AnalogRelay(dashboard, numberOfContacts, onchange, x, y){
    var self = {}

    self.dashboard = dashboard

    var element = document.createElement('analog_relay')
    element.style.left = x + 'px'
    element.style.top = y + 'px'
    element.setAttribute('state', 'off')

    var portNumbers = document.createElement('port_numbers')
    element.appendChild(portNumbers)

    for(var i = 0; i < numberOfContacts; ++i){
        var f = function(i){
            var portNumber = document.createElement('port_number')
            portNumber.innerHTML = i + 1

            portNumber.onmouseup = function(event){
                event.stopPropagation()
            }

            portNumber.onmousedown = function(event){
                event.stopPropagation()
            }

            portNumber.onclick = function(event){
                if(dashboard.selectedNode){
                    if(dashboard.onNewArrow){
                        dashboard.onNewArrow({
                            source: dashboard.selectedNode,
                            destination: {
                                element: self,
                                targetPort: i
                            },
                            arrow: dashboard.arrow
                        })
                    }
                    else{
                        dashboard.arrows.push({
                            source: dashboard.selectedNode,
                            destination: {
                                element: self,
                                targetPort: i
                            },
                            arrow: dashboard.arrow
                        })

                        dashboard.arrow = createArrow(dashboard.svg)
                        dashboard.updateArrows()
                    }
                }
                else
                    updateArrow(dashboard.arrow, 0, 0, 0, 0)

                dashboard.selectedNode = undefined
                updateArrow(dashboard.arrow, 0, 0, 0, 0)

                event.stopPropagation()
            }

            portNumbers.appendChild(portNumber)
        }

        f(i)
    }

    var ports = document.createElement('ports')
    element.appendChild(ports)

    for(var i = 0; i < numberOfContacts; ++i){
        var f = function(i){
            var input = document.createElement('input')
            input.setAttribute('type', 'number')
            input.value = 0

            input.onchange = function(){
                onchange(i, parseInt(input.value))
            }

            ports.appendChild(input)
        }

        f(i)
    }

    var label = document.createElement('label')
    label.innerHTML = 'DAC'
    element.appendChild(label)

    dashboard.setMovable(element)
    dashboard.canvas.appendChild(element)
    self.element = element

    return self
}