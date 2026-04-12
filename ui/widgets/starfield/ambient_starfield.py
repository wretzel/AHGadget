# ambient_starfield.py

import random
from kivy.uix.widget import Widget
from kivy.graphics import Color, Rectangle
from kivy.clock import Clock


class AmbientStarfield(Widget):
    def __init__(self, count=40, **kwargs):
        super().__init__(**kwargs)
        self.count = count
        self.stars = []
        self.initialized = False

        Clock.schedule_once(self._init_stars, 0)
        Clock.schedule_interval(self._animate, 1/60)

    def _init_stars(self, dt):
        self.canvas.clear()
        self.stars = []

        with self.canvas:
            for _ in range(self.count):

                # ⭐ random star color
                r = random.uniform(0.4, 1.0)
                g = random.uniform(0.4, 1.0)
                b = random.uniform(0.4, 1.0)

                c = Color(r, g, b, 1.0)

                rect = Rectangle(
                    pos=(random.randint(0, int(self.width)),
                         random.randint(0, int(self.height))),
                    size=(4, 4)
                )
                self.stars.append((c, rect))

        self.initialized = True

    def on_size(self, *args):
        if self.width > 0 and self.height > 0:
            Clock.schedule_once(self._init_stars, 0)

    def _animate(self, dt):
        if not self.initialized:
            return

        for c, r in self.stars:
            x, y = r.pos
            y += 20 * dt

            if y > self.height:
                y = 0
                x = random.randint(0, int(self.width))

            r.pos = (x, y)

            # twinkle
            c.a = max(0.3, min(1.0, c.a + random.uniform(-0.05, 0.05)))
