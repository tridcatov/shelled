var websocketAddress = 'ws://10.200.200.27:5000'
var host = 'http://10.200.200.27/'

function get(url, onsuccess){
    var xhr = new XMLHttpRequest()

    xhr.open('GET', url, true)
    xhr.setRequestHeader('Content-type', 'application/json')

    xhr.onreadystatechange = function() {
        if(xhr.readyState == XMLHttpRequest.DONE && xhr.status == 200) {
            if(onsuccess)
                onsuccess(xhr.responseText)
        }
    }

    xhr.send(null)
}

function post(url, data, onsuccess){
    var xhr = new XMLHttpRequest()

    xhr.open('POST', url, true)
    xhr.setRequestHeader('Content-type', 'application/json')

    xhr.onreadystatechange = function() {
        if(xhr.readyState == XMLHttpRequest.DONE && xhr.status == 200) {
            if(onsuccess)
                onsuccess(xhr.responseText)
        }
    }

    xhr.send(JSON.stringify(data))
}


var popup = document.querySelector('popup')

function showPopup(){
    popup.querySelector('textarea').value = ''
    popup.style.display = 'table'
}

function hidePopup(){
    popup.style.display = 'none'
}

function saveArrow(){
    window.newArrow.text = popup.querySelector('textarea').value

    var link = {
        source:{
            device: {
                id:           window.newArrow.source.element.device.id,
                serialNumber: window.newArrow.source.element.device.serialNumber
            },
            port: window.newArrow.source.targetPort
        },

        destination:{
            device: {
                id:           window.newArrow.destination.element.device.id,
                serialNumber: window.newArrow.destination.element.device.serialNumber
            },
            port: window.newArrow.destination.targetPort
        },

        script: window.newArrow.text
    }

    post(host + 'rest/v1/connect', link, function(){

    })

    dashboard.arrows.push(window.newArrow)
    dashboard.arrow = createArrow(dashboard.svg)
    dashboard.updateArrows()
    hidePopup()

    //serialize(dashboard.selectedDevices, dashboard.arrows)
}

hidePopup()
var dashboard = Dashboard(document.querySelector('admin-page right-panel .wrapper'))

dashboard.onNewArrow = function(newArrow){
    window.newArrow = newArrow
    showPopup()
}

function addDigitalSensor(){
    var digitalSensor = DigitalSensor(dashboard, 5)
    /*
    setTimeout(function(){
        digitalSensor.updateState([0, 1, 0, 0, 0])
    }, 1000)*/
}

function addAnalogSensor(){
    var analogSensor = AnalogSensor(dashboard, 5)
/*
    setTimeout(function(){
        analogSensor.updateState([0, 1, 20, 30, 50])
    }, 1000)*/
}

function addDigitalRelay(){
    var digitalRelay = DigitalRelay(dashboard, 5, function(portNumber, state){
        console.log('change ' + portNumber + ' port state to ' + state)
    })
}

function addAnalogRelay(){
    var analogRelay = AnalogRelay(dashboard, 5, function(portNumber, state){
        console.log('change ' + portNumber + ' port state to ' + state)
    })
}


var digitalSensorsParent = document.querySelector('admin-page left-panel digital-sensors')
var analogSensorsParent = document.querySelector('admin-page left-panel analog-sensors')
var digitalRelaysParent = document.querySelector('admin-page left-panel digital-relays')
var analogRelaysParent = document.querySelector('admin-page left-panel analog-relays')

function safeText(text){
    var safeText = ''

    for(var i = 0; i < text.length; ++i){
        if(text[i] == '<')
            safeText += '&lt;'
        else if(text[i] == '>')
            safeText += '&gt;'
        else
            safeText += text[i]
    }

    return safeText
}

function setDeviceParameters(device, number, label){
    device.setAttribute('draggable', 'true')
    device.innerHTML = '<number>' + safeText(number) + '</number><label>' + safeText(label) + '</label>'
}

var allDevices = []

function disallowUsedDevices(){
    for(var i in dashboard.selectedDevices){
        var currentDevice = dashboard.selectedDevices[i]

        for(var j in allDevices){
            var currentDeviceFromAll = allDevices[j]

            if(currentDevice.id == currentDeviceFromAll.device.id){
                //currentDeviceFromAll.viewElement.setAttribute('draggable', 'false')
                //currentDeviceFromAll.viewElement.style.cursor = 'not-allowed'
                currentDeviceFromAll.viewElement.style.display = 'none'
            }
        }
    }
}

function onDigitalSensor(numberOfPins, serialNumber, label, id){
    var digitalSensor = document.createElement('sensor')
    setDeviceParameters(digitalSensor, serialNumber, label)
    digitalSensorsParent.appendChild(digitalSensor)

    digitalSensor.ondragstart = function(event){
        event.dataTransfer.setData("Text", JSON.stringify({
            id:           id,
            numberOfPins: numberOfPins,
            serialNumber: serialNumber,
            label:        label,
            type:         'digital sensor'
        }));
    }

    digitalSensor.ondragend = disallowUsedDevices

    allDevices.push({
        viewElement: digitalSensor,
        device: {
            id:           id,
            numberOfPins: numberOfPins,
            serialNumber: serialNumber,
            label:        label,
            type:         'digital sensor'
        }
    })
}

function onAnalogSensor(numberOfPins, serialNumber, label, id){
    var analogSensor = document.createElement('sensor')
    setDeviceParameters(analogSensor, serialNumber, label)
    analogSensorsParent.appendChild(analogSensor)

    analogSensor.ondragstart = function(event){
        event.dataTransfer.setData("Text", JSON.stringify({
            id:           id,
            numberOfPins: numberOfPins,
            serialNumber: serialNumber,
            label:        label,
            type:         'analog sensor'
        }));
    }

    analogSensor.ondragend = disallowUsedDevices

    allDevices.push({
        viewElement: analogSensor,
        device: {
            id:           id,
            numberOfPins: numberOfPins,
            serialNumber: serialNumber,
            label:        label,
            type:         'analog sensor'
        }
    })
}

function onDigitalRelay(numberOfPins, serialNumber, label, id){
    var digitalRelay = document.createElement('relay')
    setDeviceParameters(digitalRelay, serialNumber, label)
    digitalRelaysParent.appendChild(digitalRelay)

    digitalRelay.ondragstart = function(event){
        event.dataTransfer.setData("Text", JSON.stringify({
            id:           id,
            numberOfPins: numberOfPins,
            serialNumber: serialNumber,
            label:        label,
            type:         'digital relay'
        }));
    }

    digitalRelay.ondragend = disallowUsedDevices

    allDevices.push({
        viewElement: digitalRelay,
        device: {
            id:           id,
            numberOfPins: numberOfPins,
            serialNumber: serialNumber,
            label:        label,
            type:         'digital relay'
        }
    })
}

function onAnalogRelay(numberOfPins, serialNumber, label, id){
    var analogRelay = document.createElement('relay')
    setDeviceParameters(analogRelay, serialNumber, label)
    analogRelaysParent.appendChild(analogRelay)

    analogRelay.ondragstart = function(event){
        event.dataTransfer.setData("Text", JSON.stringify({
            id:           id,
            numberOfPins: numberOfPins,
            serialNumber: serialNumber,
            label:        label,
            type:         'analog relay'
        }));
    }

    analogRelay.ondragend = disallowUsedDevices

    allDevices.push({
        viewElement: analogRelay,
        device: {
            id:           id,
            numberOfPins: numberOfPins,
            serialNumber: serialNumber,
            label:        label,
            type:         'analog relay'
        }
    })
}

document.querySelector('.wrapper').ondrop = function(event){
    var data = JSON.parse(event.dataTransfer.getData('text'))
    dashboard.selectedDevices.push(data)

    switch(data.type){
        case 'digital sensor':
            var digitalSensor = DigitalSensor(dashboard, data.numberOfPins, event.offsetX, event.offsetY)
            digitalSensor.device = data
            data.viewElement = digitalSensor
            break

        case 'analog sensor':
            var analogSensor = AnalogSensor(dashboard, data.numberOfPins, event.offsetX, event.offsetY)
            analogSensor.device = data
            data.viewElement = analogSensor
            break

        case 'digital relay':
            var digitalRelay = DigitalRelay(dashboard, data.numberOfPins, undefined, event.offsetX, event.offsetY)
            digitalRelay.device = data
            data.viewElement = digitalRelay
            break

        case 'analog relay':
            var analogRelay = AnalogRelay(dashboard, data.numberOfPins, undefined, event.offsetX, event.offsetY)
            analogRelay.device = data
            data.viewElement = analogRelay
            break
    }

    event.preventDefault()
    event.stopPropagation()
}


function serialize(devices, links){
    function serializeDevices(devices){
        var newDevices = []

        for(var i in devices){
            var currentDevice = devices[i]

            newDevices.push({
                id:           currentDevice.id,
                numberOfPins: currentDevice.numberOfPins,
                serialNumber: currentDevice.serialNumber,
                label:        currentDevice.label,
                type:         currentDevice.type,
                x:            currentDevice.viewElement.element.offsetLeft,
                y:            currentDevice.viewElement.element.offsetTop
            })
        }

        return newDevices
    }

    var newData = {
        devices: serializeDevices(devices),
        links: []
    }

    for(var i in links){
        var currentLink = links[i]

        newData.links.push({
            
            source: {
                device: {
                    id:           currentLink.source.element.device.id,
                    numberOfPins: currentLink.source.element.device.numberOfPins,
                    serialNumber: currentLink.source.element.device.serialNumber,
                    label:        currentLink.source.element.device.label,
                    type:         currentLink.source.element.device.type,
                    x:            currentLink.source.element.device.viewElement.element.offsetLeft,
                    y:            currentLink.source.element.device.viewElement.element.offsetTop
                },
                port:   currentLink.source.targetPort
            },

            destination: {
                device: {
                    id:           currentLink.destination.element.device.id,
                    numberOfPins: currentLink.destination.element.device.numberOfPins,
                    serialNumber: currentLink.destination.element.device.serialNumber,
                    label:        currentLink.destination.element.device.label,
                    type:         currentLink.destination.element.device.type,
                    x:            currentLink.destination.element.device.viewElement.element.offsetLeft,
                    y:            currentLink.destination.element.device.viewElement.element.offsetTop
                },
                port:   currentLink.destination.targetPort
            },
            
            script: currentLink.text
        })
    }

    //console.log(newData)
    //console.log(JSON.stringify(newData))

    return newData
}

/*
onDigitalSensor(5, '1234', 'digital sensor', '1')
onAnalogSensor(5, '1234', 'analog sensor', '2')
onDigitalRelay(5, '1234', 'digital relay', '3')
onAnalogRelay(5, '1234', 'analog relay', '4')*/

get(host + 'rest/v1/devices', function(devices){
    devices = JSON.parse(devices)

    for(var i in devices){
        var currentDevice = devices[i]

        switch(currentDevice.type){
            case 'digital sensor': onDigitalSensor(
                currentDevice.numberOfPins,
                currentDevice.serialNumber,
                currentDevice.label,
                currentDevice.id
            ); break

            case 'analog sensor': onAnalogSensor(
                currentDevice.numberOfPins,
                currentDevice.serialNumber,
                currentDevice.label,
                currentDevice.id
            ); break

            case 'digital relay': onDigitalRelay(
                currentDevice.numberOfPins,
                currentDevice.serialNumber,
                currentDevice.label,
                currentDevice.id
            ); break

            case 'analog relay': onAnalogRelay(
                currentDevice.numberOfPins,
                currentDevice.serialNumber,
                currentDevice.label,
                currentDevice.id
            ); break
        }
    }
})

function addDevice(device){
    switch(device.type){
        case 'digital sensor': onDigitalSensor(
            device.numberOfPins,
            device.serialNumber,
            device.label,
            device.id
            ); break

        case 'analog sensor': onAnalogSensor(
            device.numberOfPins,
            device.serialNumber,
            device.label,
            device.id
            ); break

        case 'digital relay': onDigitalRelay(
            device.numberOfPins,
            device.serialNumber,
            device.label,
            device.id
            ); break

        case 'analog relay': onAnalogRelay(
            device.numberOfPins,
            device.serialNumber,
            device.label,
            device.id
            ); break
    }
}


var websocket = new WebSocket(websocketAddress)

websocket.onmessage = function(event){
    //console.log(event.data)
    var data = JSON.parse(event.data)
/*
{
    message: 'new_device',
    data: {
        id:           '123',
        numberOfPins: 2,
        serialNumber: 'sn',
        label:        '',
        type:         'analog relay'
    }
}*/

    if(data.message == 'new_device'){
        addDevice(data.data)
    }

    //console.log('receive: ' + JSON.stringify(data))
}