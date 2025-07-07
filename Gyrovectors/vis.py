import pygame
import sys
from gyrovectors import *
import math
import cmath

gyr = Gyrovectorspace[complex, float](complex_mobius_addition, complex_mobius_gyr, 0 + 0j, complex_mobius_additive_inverse, complex_mobius_scaling)

a = gyr.newGyrovector(0.5 + 0j)
b = gyr.newGyrovector(0 + 0.5j)

scalars = [0.05*i for i in range(-200, 200)]
vectors = [a + (-a+b)*r for r in scalars]

# Initialize Pygame
pygame.init()

# Parameters
scale = 100  # pixels per unit
window_size = 400  # window width/height in pixels
center = (window_size // 2, window_size // 2)


# Create the window
screen = pygame.display.set_mode((window_size, window_size))
pygame.display.set_caption("Gyrolines")

clock = pygame.time.Clock()
# Coordinates to plot (you can replace this with your list)
vectors = [a + (-a+b)*r for r in scalars]


# make a hexagon
radius = math.sqrt(2)*math.sin(math.pi/12)/math.sin(3*math.pi/4)
points = [gyr.newGyrovector(radius*cmath.exp(2*math.pi*1j*i/6)) for i in range(6)]

def draw_line(a, b, color=(255, 0, 0)):
    _vectors = [a + (-a+b)*r for r in scalars]
    coordinates = [(v.value.real, v.value.imag) for v in _vectors]
    for x, y in coordinates:
        px = int(center[0] + x * scale)
        py = int(center[1] - y * scale) 
        pygame.draw.circle(screen, color, (px, py), 1)

def draw_point(a : Gyrovectorspace[complex, float].Gyrovector, color=(0, 255, 0)):
    px = int(center[0] + a.value.real * scale)
    py = int(center[1] - a.value.imag * scale)  
    pygame.draw.circle(screen, color, (px, py), 2) #type: ignore

line_segments = []
    
for idx in range(len(points)):
    a = points[idx-2]
    b = points[idx-1]
    c = points[idx]
    
    ds = -a + b
    pp =  [a + ds*n for n in range(2, 4)]
    perps = [p + (-c + b) for p in pp]
    for d, e in zip(pp, perps):
        line_segments.append((d, e, ds))

    ds = -c + b
    pp =  [c + ds*n for n in range(2, 4)]
    perps = [p + (-a + b) for p in pp]
    for d, e in zip(pp, perps):
        line_segments.append((d, e, ds))

new_line_segments = []

for a, b, ds in line_segments:
    new_ds = -a + b
    pp =  [a + new_ds*n for n in range(1, 3)]
    perps = [p + ds for p in pp]
    for a, b in zip(pp, perps):
        new_line_segments.append((a, b, new_ds))

# Main loop
running = True
while running:
    clock.tick(60)
    screen.fill((255, 255, 255))  # White background


    # Draw unit circle (scaled)
    pygame.draw.circle(screen, (0, 0, 255), center, int(1 * scale), 1)

    for i in range(len(points)):
        draw_line(points[i-1], points[i])

    for a, b, _ in line_segments:
        draw_line(a, b)

    for a, b, _ in new_line_segments:
        draw_line(a, b, color=(0, 255, 255))

    # Handle keys held down
    keys = pygame.key.get_pressed()
    ds = gyr.zero
    if keys[pygame.K_LEFT]:
        ds = gyr.newGyrovector(0.05 + 0j)
    if keys[pygame.K_RIGHT]:
        ds = gyr.newGyrovector(-0.05 + 0j)
    if keys[pygame.K_UP]:
        ds = gyr.newGyrovector(0 - 0.05j)
    if keys[pygame.K_DOWN]:
        ds = gyr.newGyrovector(0 + 0.05j)
    points = [ds + v for v in points]
    line_segments = [(ds + a, ds + b, old_ds) for a, b, old_ds in line_segments]
    new_line_segments = [(ds + a, ds + b, old_ds) for a, b, old_ds in new_line_segments]

    # Event handling
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False



    pygame.display.flip()

# Quit Pygame
pygame.quit()
sys.exit()
