# ambient_3d_starfield.py

import random
from kivy.uix.widget import Widget
from kivy.graphics import Color, Rectangle
from kivy.clock import Clock


class Ambient3DStarfield(Widget):
    def __init__(self, count=120, **kwargs):   # fewer stars
        super().__init__(**kwargs)

        self.count = count
        self.stars = []  # [sx, sy, sz, color, rect]

        Clock.schedule_once(self._init_stars, 0)
        Clock.schedule_interval(self._update, 1/60)

    def _init_stars(self, dt):
        self.canvas.clear()
        self.stars = []

        cx = self.center_x
        cy = self.center_y

        with self.canvas:
            for _ in range(self.count):
                # random 3D position
                sx = random.uniform(-160, 160)
                sy = random.uniform(-120, 120)
                sz = random.uniform(50, 255)

                # ALWAYS WHITE
                c = Color(1, 1, 1, 1)

                # tiny star
                r = Rectangle(pos=(cx, cy), size=(2, 2))

                self.stars.append([sx, sy, sz, c, r])

    def _update(self, dt):
        if self.width == 0 or self.height == 0:
            return

        cx = self.center_x
        cy = self.center_y

        for star in self.stars:
            sx, sy, sz, c, r = star

            # move star toward camera
            sz -= 2.0

            if sz <= 1:
                # respawn far away
                star[0] = random.uniform(-160, 160)
                star[1] = random.uniform(-120, 120)
                star[2] = random.uniform(200, 255)
                continue

            # perspective projection
            scale = 256
            screen_x = (sx * scale / sz) + cx
            screen_y = (sy * scale / sz) + cy

            # if off-screen, respawn
            if (screen_x < 0 or screen_x >= self.width or
                screen_y < 0 or screen_y >= self.height):
                star[0] = random.uniform(-160, 160)
                star[1] = random.uniform(-120, 120)
                star[2] = random.uniform(200, 255)
                continue

            # ALWAYS WHITE — no brightness fade
            c.r = c.g = c.b = 1.0

            # barely grow
            size = max(1.0, 2.5 - (sz / 200.0))

            r.pos = (screen_x, screen_y)
            r.size = (size, size)

            # update depth
            star[2] = sz
