# ambient_hyperspace.py

import random
import math
from kivy.uix.widget import Widget
from kivy.graphics import Color, Rectangle, InstructionGroup
from kivy.clock import Clock


class AmbientHyperspaceField(Widget):
    def __init__(self, spawn_rate=0.03, **kwargs):
        super().__init__(**kwargs)

        self.spawn_rate = spawn_rate
        self.stars = []  # [color, rect, x, y, vx, vy, size, growth, alpha, fade]

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
        speed = random.uniform(40, 80)  # pixels per second

        vx = math.cos(angle) * speed
        vy = math.sin(angle) * speed

        size = random.uniform(2, 4)
        growth = random.uniform(8, 16)  # px/sec
        alpha = 1.0
        fade = random.uniform(0.3, 0.6)

        c = Color(1, 1, 1, alpha)
        r = Rectangle(pos=(cx, cy), size=(size, size))

        self.group.add(c)
        self.group.add(r)

        self.stars.append([c, r, cx, cy, vx, vy, size, growth, alpha, fade])

    # ------------------------------------------------------------
    # UPDATE STARS
    # ------------------------------------------------------------
    def _update(self, dt):
        max_radius = max(self.width, self.height) * 0.75

        for star in list(self.stars):
            c, r, x, y, vx, vy, size, growth, alpha, fade = star

            # move outward
            x += vx * dt
            y += vy * dt

            # grow
            size += growth * dt

            # fade
            alpha -= fade * dt

            # update graphics
            r.pos = (x - size/2, y - size/2)
            r.size = (size, size)
            c.a = alpha

            # update stored values
            star[2] = x
            star[3] = y
            star[6] = size
            star[8] = alpha

            # remove if invisible or too far
            if alpha <= 0 or math.dist((x, y), (self.center_x, self.center_y)) > max_radius:
                self.group.remove(c)
                self.group.remove(r)
                self.stars.remove(star)
                
