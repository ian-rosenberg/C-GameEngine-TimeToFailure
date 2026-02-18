import cv2 as cv
import json
import sys
import numpy as np
import os

def compute_average_slope(coordinates):
    """Compute the average slope from a list of coordinate values."""
    if len(coordinates) < 2:
        return 0.0
    differences = [coordinates[i+1] - coordinates[i] for i in range(len(coordinates)-1)]
    return np.average(differences)

def scan_top_edge(img, xStart, yStart, tileWidth, tileHeight):
    """Scan from top-left to top-right of the tile.
       Returns a list of transformed y coordinates (inverted y axis)."""
    coords = []
    for x in range(xStart, xStart + tileWidth):
        found = False
        for y in range(yStart, yStart + tileHeight):
            if img[y, x][3] > 0:
                # Invert y so that the coordinate system is consistent with your original code.
                coords.append(img.shape[0] - y - 1)
                found = True
                break
        if not found:
            # Fallback if no non-transparent pixel is found
            coords.append(img.shape[0] - (yStart + tileHeight))
    return coords

def scan_right_edge(img, xStart, yStart, tileWidth, tileHeight):
    """Scan from top-right to bottom-right of the tile.
       Returns a list of x coordinates."""
    coords = []
    for y in range(yStart, yStart + tileHeight):
        found = False
        for x in range(xStart + tileWidth - 1, xStart - 1, -1):
            if img[y, x][3] > 0:
                coords.append(x)
                found = True
                break
        if not found:
            coords.append(xStart)
    return coords

def scan_bottom_edge(img, xStart, yStart, tileWidth, tileHeight):
    """Scan from bottom-left to bottom-right of the tile.
       Returns a list of transformed y coordinates."""
    coords = []
    for x in range(xStart, xStart + tileWidth):
        found = False
        for y in range(yStart + tileHeight - 1, yStart - 1, -1):
            if img[y, x][3] > 0:
                coords.append(img.shape[0] - y - 1)
                found = True
                break
        if not found:
            coords.append(img.shape[0] - yStart)
    return coords

def scan_left_edge(img, xStart, yStart, tileWidth, tileHeight):
    """Scan from top-left to bottom-left of the tile.
       Returns a list of x coordinates."""
    coords = []
    for y in range(yStart, yStart + tileHeight):
        found = False
        for x in range(xStart, xStart + tileWidth):
            if img[y, x][3] > 0:
                coords.append(x)
                found = True
                break
        if not found:
            coords.append(xStart)
    return coords

def determine_possible_connections(top, right, bottom, left):
    """Determine allowed neighbor connections based on edge slopes.
       Returns a tuple of (layers, hillOrientation, connections).
       The connections list includes allowedLayers and offsets (x,y)."""
    connections = []
    layers = []
    hillOrientation = "None"
    
    # Process top neighbors (NW, N, NE)
    if top == 0.0:
        layers = ["ground", "wall"]
        if left == 0.0:
            connections.append({"allowedLayers": ["hill", "ground", "wall"], "x": -1, "y": -1})
        else:
            layers.append("platform")
            if left < 0.0:
                connections.append({"allowedLayers": [], "x": -1, "y": -1})
            elif left > 0.0:
                connections.append({"allowedLayers": ["hill", "ground", "wall"], "x": -1, "y": -1})
        if right == 0.0:
            connections.append({"allowedLayers": ["hill", "ground", "wall"], "x": 1, "y": -1})
        else:
            if right < 0.0:
                layers = ["platform"]
                connections.append({"allowedLayers": ["hill", "ground", "wall"], "x": 1, "y": -1})
            elif right > 0.0:
                layers = ["platform"]
                connections.append({"allowedLayers": [], "x": 1, "y": -1})
        connections.append({"allowedLayers": ["hill", "ground", "wall"], "x": 0, "y": -1})
    
    else:
        if top > 0.0:
            layers = ["hill"]
            hillOrientation = "Northeast"
            # Handle NW connection
            if left >= 0.0:
                connections.append({"allowedLayers": ["hill", "ground", "wall"], "x": -1, "y": -1})
            # Handle NE connection
            if right < 0.0:
                connections.append({"allowedLayers": ["hill", "wall"], "x": 1, "y": -1})
            elif right > 0.0:
                connections.append({"allowedLayers": [], "x": 1, "y": -1})
            elif right == 0.0:
                connections.append({"allowedLayers": ["hill", "ground", "wall"], "x": 1, "y": -1})
        elif top < 0.0:
            layers = ["hill"]
            hillOrientation = "Southeast"
            if left > 0.0:
                connections.append({"allowedLayers": ["hill"], "x": -1, "y": -1})
            elif left == 0.0:
                connections.append({"allowedLayers": ["ground", "wall"], "x": -1, "y": -1})
            if right < 0.0:
                connections.append({"allowedLayers": ["hill"], "x": 1, "y": -1})
            elif right > 0.0:
                connections.append({"allowedLayers": [], "x": 1, "y": -1})
            elif np.floor(right) == 0:
                connections.append({"allowedLayers": ["wall"], "x": 1, "y": -1})
    
    # Process bottom neighbors (SW, S, SE)
    if np.floor(bottom) == 0.0:
        if np.floor(left) == 0.0:
            connections.append({"allowedLayers": ["hill", "ground", "wall"], "x": -1, "y": 1})
        else:
            if left < 0.0:
                connections.append({"allowedLayers": ["ground"], "x": -1, "y": 1})
            elif left > 0.0:
                connections.append({"allowedLayers": ["hill", "ground"], "x": -1, "y": 1})
        if np.floor(right) == 0.0:
            connections.append({"allowedLayers": ["hill", "ground"], "x": 1, "y": 1})
        else:
            if right < 0.0:
                connections.append({"allowedLayers": ["ground"], "x": 1, "y": 1})
            elif right > 0.0:
                connections.append({"allowedLayers": ["hill"], "x": 1, "y": 1})
            elif np.floor(right) == 0.0:
                connections.append({"allowedLayers": ["ground"], "x": 1, "y": 1})
        connections.append({"allowedLayers": ["ground"], "x": 0, "y": 1})
    else:
        if bottom < 0.0:
            if left > 0.0:
                connections.append({"allowedLayers": [], "x": -1, "y": 1})
            elif np.floor(left) == 0.0:
                connections.append({"allowedLayers": ["hill", "ground"], "x": -1, "y": 1})
            if right < 0.0:
                connections.append({"allowedLayers": ["ground"], "x": 1, "y": 1})
            elif right > 0.0:
                connections.append({"allowedLayers": ["hill", "ground"], "x": 1, "y": 1})
            elif np.floor(right) == 0.0:
                connections.append({"allowedLayers": ["ground"], "x": 1, "y": 1})
        elif bottom > 0.0:
            if left > 0.0:
                connections.append({"allowedLayers": ["ground"], "x": -1, "y": 1})
            elif np.floor(left) == 0.0:
                connections.append({"allowedLayers": ["ground", "wall"], "x": -1, "y": 1})
            if right < 0.0:
                connections.append({"allowedLayers": [], "x": 1, "y": 1})
            elif right > 0.0:
                connections.append({"allowedLayers": ["hill"], "x": 1, "y": 1})
            elif np.floor(right) == 0.0:
                connections.append({"allowedLayers": ["ground", "hill"], "x": 1, "y": 1})
    
    # Process side neighbors (W, E)
    if np.floor(left) == 0.0:
        connections.append({"allowedLayers": ["ground", "hill", "wall"], "x": -1, "y": 0})
    elif left > 0.0:
        connections.append({"allowedLayers": [], "x": -1, "y": 0})
    elif left < 0.0:
        connections.append({"allowedLayers": ["ground", "hill"], "x": -1, "y": 0})
        
    if np.floor(right) == 0.0:
        connections.append({"allowedLayers": ["ground", "hill", "wall"], "x": 1, "y": 0})
    elif right > 0.0:
        connections.append({"allowedLayers": [], "x": 1, "y": 0})
    elif right < 0.0:
        connections.append({"allowedLayers": ["ground", "hill"], "x": 1, "y": 0})
    
    return layers, hillOrientation, connections

def get_neighbor_layers(connections):
    """Group the allowed layers from each connection by neighbor direction.
       Direction is encoded as a string (e.g., NW, N, NE, etc.)."""
    neighbor_layers = {}
    for conn in connections:
        dx = conn["x"]
        dy = conn["y"]
        direction = ""
        if dy == -1:
            direction += "N"
        elif dy == 1:
            direction += "S"
        if dx == -1:
            direction += "W"
        elif dx == 1:
            direction += "E"
        if direction == "":
            direction = "C"
        if direction in neighbor_layers:
            # Merge lists and remove duplicates
            neighbor_layers[direction] = list(set(neighbor_layers[direction] + conn["allowedLayers"]))
        else:
            neighbor_layers[direction] = conn["allowedLayers"]
    return neighbor_layers

def get_tile_coordinates(tileIndex, tileWidth, tileHeight, margin, spacing, cols):
    """Compute the x and y location of the tile given its 1-based index."""
    col_index = (tileIndex - 1) % cols
    row_index = (tileIndex - 1) // cols
    xLocation = int(tileWidth * col_index + spacing * col_index + margin)
    yLocation = int(tileHeight * row_index + spacing * row_index + margin)
    return xLocation, yLocation

# Main execution
if len(sys.argv) <= 1:
    print("Missing tilemap sprite sheet description JSON path!")
    quit()

with open(sys.argv[1], "r") as infoFile:
    ssInfo = json.load(infoFile)

img = cv.imread(ssInfo["imageForDescription"], cv.IMREAD_UNCHANGED)
margin = int(ssInfo["margin"])
spacing = int(ssInfo["spacing"])
tileHeight = int(ssInfo["tileheight"])
tileWidth = int(ssInfo["tilewidth"])
imageHeight = int(ssInfo["imageheight"])
imageWidth = int(ssInfo["imagewidth"])
orient = ssInfo["direction"]
cols = int(ssInfo["columns"])
tileCount = int(ssInfo["tilecount"])
outFileName = str(ssInfo["generationDescriptionOut"])
name = str(ssInfo["name"])

jsonList = []

print("Program start")

tileIndex = 1
while tileIndex <= tileCount:
    xStart, yStart = get_tile_coordinates(tileIndex, tileWidth, tileHeight, margin, spacing, cols)
    
    # Scan each edge to collect coordinates
    top_coords = scan_top_edge(img, xStart, yStart, tileWidth, tileHeight)
    right_coords = scan_right_edge(img, xStart, yStart, tileWidth, tileHeight)
    bottom_coords = scan_bottom_edge(img, xStart, yStart, tileWidth, tileHeight)
    left_coords = scan_left_edge(img, xStart, yStart, tileWidth, tileHeight)
    
    # Compute average slopes for each edge
    topAvgSlope = compute_average_slope(top_coords)
    rightAvgSlope = compute_average_slope(right_coords)
    bottomAvgSlope = compute_average_slope(bottom_coords)
    leftAvgSlope = compute_average_slope(left_coords)
    
    slopes = [topAvgSlope, rightAvgSlope, bottomAvgSlope, leftAvgSlope]
    
    # Determine possible connections and neighbor layers based on slopes
    layers, hillOrientation, possibleConnects = determine_possible_connections(topAvgSlope, rightAvgSlope, bottomAvgSlope, leftAvgSlope)
    neighborLayers = get_neighbor_layers(possibleConnects)
    
    tileObj = {
        "tileIndex": tileIndex,
        "xLocation": xStart,
        "yLocation": yStart,
        "slopes": slopes,
        "hillOrientation": hillOrientation,
        "layers": layers,
        "possibleConnects": possibleConnects,
        "neighborLayers": neighborLayers
    }
    
    jsonList.append(tileObj)
    tileIndex += 1

with open(outFileName, 'w', encoding='utf-8') as outfile:
    json.dump({name: jsonList}, outfile, ensure_ascii=False, indent=4, sort_keys=True)

print("Done")
