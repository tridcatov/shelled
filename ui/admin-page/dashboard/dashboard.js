function Dashboard(canvas){
    var dashboard = {}

    dashboard.canvas = canvas
    dashboard.svg = createSVG(canvas)
    dashboard.arrow = createArrow(dashboard.svg)
    dashboard.arrows = []
    dashboard.selectedDevices = []

    dashboard.setMovable = function(element){

        element.onmousedown = function(event){
            if(dashboard.mousedownEvent && dashboard.mouseMoveOwner && element != dashboard.mouseMoveOwner)
                return

            dashboard.mouseMoveOwner = element

            dashboard.mousedownEvent = {
                pageX: event.pageX - element.offsetLeft,
                pageY: event.pageY - element.offsetTop
            }
        }

        element.onmouseup = function(event){
            dashboard.mousedownEvent = undefined
            dashboard.mouseMoveOwner = undefined
        }
    }

    canvas.onmousemove = function(event){
        if(dashboard.mouseMoveOwner && dashboard.mousedownEvent){
            dashboard.mouseMoveOwner.style.left = event.pageX - dashboard.mousedownEvent.pageX + 'px'
            dashboard.mouseMoveOwner.style.top = event.pageY - dashboard.mousedownEvent.pageY + 'px'

            dashboard.updateArrows()
        }
        else if(dashboard.selectedNode){
            var sourcePorts = dashboard.selectedNode.element.element.querySelectorAll('port_number')

            if(!sourcePorts.length)
                sourcePorts = dashboard.selectedNode.element.element.querySelectorAll('port')

            var sourcePort = sourcePorts[dashboard.selectedNode.targetPort]

            updateArrow(
                dashboard.arrow,

                sourcePort.offsetLeft + sourcePort.offsetWidth / 2 + dashboard.selectedNode.element.element.offsetLeft,
                sourcePort.offsetTop + sourcePort.offsetHeight / 2 + dashboard.selectedNode.element.element.offsetTop,

                event.pageX - dashboard.canvas.offsetLeft,
                event.pageY - dashboard.canvas.offsetTop
            )
        }
    }

    canvas.onmouseup = function(){
        dashboard.mousedownEvent = undefined
        dashboard.selectedNode = undefined
        updateArrow(dashboard.arrow, 0, 0, 0, 0)
    }

    dashboard.updateArrows = function(){
        for(var i in dashboard.arrows){
            var currentArrow = dashboard.arrows[i]

            var sourcePorts = currentArrow.source.element.element.querySelectorAll('port_number')
            
            if(!sourcePorts.length)
                sourcePorts = currentArrow.source.element.element.querySelectorAll('port')
            
            var sourcePort = sourcePorts[currentArrow.source.targetPort]

            var destinationPorts = currentArrow.destination.element.element.querySelectorAll('port_number')

            if(!destinationPorts.length)
                destinationPorts = currentArrow.destination.element.element.querySelectorAll('port')

            var destinationPort = destinationPorts[currentArrow.destination.targetPort]

            updateArrow(
                currentArrow.arrow,

                sourcePort.offsetLeft + sourcePort.offsetWidth / 2 + currentArrow.source.element.element.offsetLeft,
                sourcePort.offsetTop + sourcePort.offsetHeight / 2 + currentArrow.source.element.element.offsetTop,

                destinationPort.offsetLeft + destinationPort.offsetWidth / 2 + currentArrow.destination.element.element.offsetLeft,
                destinationPort.offsetTop + destinationPort.offsetHeight / 2 + currentArrow.destination.element.element.offsetTop
            )
        }
    }

    return dashboard
}