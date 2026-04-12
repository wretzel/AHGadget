# ambient_radial_starfield.py

import random
import math
from kivy.uix.widget import Widget
from kivy.graphics import Color, Rectangle, InstructionGroup
from kivy.clock import Clock


class AmbientRadialStarfield(Widget):
    def __init__(self, spawn_rate=0.04, **kwargs):
        super().__init__(**kwargs)

        self.spawn_rate = spawn_rate
        self.stars = []  # [color, rect, x, y, vx, vy, size]

        self.group = InstructionGroup()
        self.canvas.add(self.group)

        Clock.schedule_interval(self._spawn_star, self.spawn_rate)
        Clock.schedule_interval(self._update, 1/60)

    # ------------------------------------------------------------
    # SPAWN STAR AT CENTER
    # ------------------------------------------------------------
    def _spawn_star(self, dt):
        if self.width == 0 or self.height == 0:
            return

        cx = self.center_x
        cy = self.center_y

        # random outward direction
        angle = random.uniform(0, 2 * math.pi)

        # VERY small initial speed
        speed = random.uniform(5, 15)

        vx = math.cos(angle) * speed
        vy = math.sin(angle) * speed

        size = random.uniform(2, 3)

        c = Color(1, 1, 1, 0.8)
        r = Rectangle(pos=(cx, cy), size=(size, size))

        self.group.add(c)
        self.group.add(r)

        self.stars.append([c, r, cx, cy, vx, vy, size])

    # ------------------------------------------------------------
    # UPDATE STARS (EXPONENTIAL MOTION)
    # ------------------------------------------------------------
    def _update(self, dt):
        accel = 1.04  # exponential acceleration factor

        for star in list(self.stars):
            c, r, x, y, vx, vy, size = star

            # exponential acceleration
            vx *= accel
            vy *= accel

            # move outward
            x += vx * dt
            y += vy * dt

            # tiny growth
            size += 0.1 * dt

            # update graphics
            r.pos = (x - size/2, y - size/2)
            r.size = (size, size)

            # update stored values
            star[2] = x
            star[3] = y
            star[4] = vx
            star[5] = vy
            star[6] = size

            # remove if off-screen
            if (x < 0 or x > self.width or
                y < 0 or y > self.height):
                self.group.remove(c)
                self.group.remove(r)
                self.stars.remove(star)

