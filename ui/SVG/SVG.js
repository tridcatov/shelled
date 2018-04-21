function createSVG(parent){
    var svg = document.createElementNS('http://www.w3.org/2000/svg', 'svg')

    svg.setAttributeNS(null, 'width', parent.offsetWidth + 'px')
    svg.setAttributeNS(null, 'height', parent.offsetHeight + 'px')
    parent.appendChild(svg)

    window.onresize = function(){
        svg.setAttributeNS(null, 'width', parent.offsetWidth + 'px')
        svg.setAttributeNS(null, 'height', parent.offsetHeight + 'px')
    }

    return svg
}

function updatePath(path, x1, y1, x2, y2){
    path.setAttributeNS(null, 'd', 'M' + x1 + ' ' + y1 + ',' + x2 + ' ' + y2)
    path.setAttributeNS(null, 'stroke', '#000')
    path.setAttributeNS(null, 'stroke-width', '1px')
    path.setAttributeNS(null, 'fill', '#000')
}

function createPath(svg){
    var path = document.createElementNS('http://www.w3.org/2000/svg', 'path')
    
    path.setAttributeNS(null, 'stroke', '#000')
    path.setAttributeNS(null, 'stroke-width', '1px')
    path.setAttributeNS(null, 'fill', '#000')

    path.onclick = function(event){
        alert()
    }

    svg.appendChild(path)

    return path
}

function updateArrow(arrow, x1, y1, x2, y2){
    updatePath(arrow.path1, x1, y1, x2, y2)

    var arrowVector = {
        x: -(y2 - y1),
        y: x2 - x1
    }
    var arrowVectorLength = Math.sqrt(arrowVector.x * arrowVector.x + arrowVector.y * arrowVector.y)

    arrowVector.x /= arrowVectorLength
    arrowVector.y /= arrowVectorLength

    arrowVector.x *= 10
    arrowVector.y *= 10

    var pathVector = {
        x: x2 - x1,
        y: y2 - y1
    }
    var pathVectorLength = Math.sqrt(pathVector.x * pathVector.x + pathVector.y * pathVector.y)

    pathVector.x /= pathVectorLength
    pathVector.y /= pathVectorLength

    pathVector.x *= 20
    pathVector.y *= 20

    pathVector.x = x2 + arrowVector.x - pathVector.x
    pathVector.y = y2 + arrowVector.y - pathVector.y

    if(pathVectorLength)
        updatePath(arrow.path2, x2, y2, pathVector.x, pathVector.y)
    else
        updatePath(arrow.path2, 0, 0, 0, 0)


    var arrowVector = {
        x: -(y2 - y1),
        y: x2 - x1
    }
    var arrowVectorLength = Math.sqrt(arrowVector.x * arrowVector.x + arrowVector.y * arrowVector.y)

    arrowVector.x /= arrowVectorLength
    arrowVector.y /= arrowVectorLength

    arrowVector.x *= 10
    arrowVector.y *= 10

    var pathVector = {
        x: x2 - x1,
        y: y2 - y1
    }
    var pathVectorLength = Math.sqrt(pathVector.x * pathVector.x + pathVector.y * pathVector.y)

    pathVector.x /= pathVectorLength
    pathVector.y /= pathVectorLength

    pathVector.x *= 20
    pathVector.y *= 20

    pathVector.x = x2 - arrowVector.x - pathVector.x
    pathVector.y = y2 - arrowVector.y - pathVector.y

    if(pathVectorLength)
        updatePath(arrow.path3, x2, y2, pathVector.x, pathVector.y)
    else
        updatePath(arrow.path3, 0, 0, 0, 0)
}

function createArrow(svg){
    return {
        path1: createPath(svg),
        path2: createPath(svg),
        path3: createPath(svg)
    }
}