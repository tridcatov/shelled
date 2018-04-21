function SwitchSensor(canvas, onstatechange){
    var self = {}

    self.canvas = canvas
    self.state = false

    self.on = function(){
        self.state = true
        self.element.setAttribute('state', 'on')
    }

    self.off = function(){
        self.state = false
        self.element.setAttribute('state', 'off')
    }

    var element = document.createElement('switch_sensor')
    element.setAttribute('state', 'off')

    element.onclick = function(event){
        if(self.state)
            self.off()
        else
            self.on()
    }

    canvas.appendChild(element)
    self.element = element

    return self
}