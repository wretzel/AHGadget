from kivy.uix.widget import Widget
from kivy.graphics import Color, Rectangle, InstructionGroup
from kivy.clock import Clock
import random


class ParticleField(Widget):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.particles = []  # [color, rect, alpha, size, vx, vy]
        self.group = InstructionGroup()
        self.canvas.add(self.group)

        # Slower spawn rate
        Clock.schedule_interval(self.spawn_particle, 0.08)

        # Slower update loop
        Clock.schedule_interval(self.update_particles, 1/60)

    # ------------------------------------------------------------
    # SPAWN PARTICLES
    # ------------------------------------------------------------
    def spawn_particle(self, dt):
        if random.random() > 0.55:
            return

        # Larger particles
        size = random.choice([10, 12, 14, 16, 18, 20])

        x = random.uniform(self.x, self.right)
        y = random.uniform(self.y, self.top)

        # Neon colors
        r, g, b = random.choice([
            (0.2, 1.0, 1.0),  # cyan
            (1.0, 0.2, 1.0),  # magenta
            (0.4, 0.6, 1.0),  # electric blue
        ])

        color = Color(r, g, b, 1.0)
        rect = Rectangle(pos=(x, y), size=(size, size))

        # Slower drift
        vx = random.uniform(-0.15, 0.15)
        vy = random.uniform(-0.15, 0.15)

        self.group.add(color)
        self.group.add(rect)

        self.particles.append([color, rect, 1.0, size, vx, vy])

    # ------------------------------------------------------------
    # UPDATE PARTICLES
    # ------------------------------------------------------------
    def update_particles(self, dt):
        fade_speed = 0.35  # slower fade

        for p in list(self.particles):
            color, rect, alpha, size, vx, vy = p

            alpha -= fade_speed * dt
            if alpha <= 0:
                self.group.remove(color)
                self.group.remove(rect)
                self.particles.remove(p)
                continue

            # Shrink slower
            new_size = size * (0.5 + alpha * 0.5)
            rect.size = (new_size, new_size)

            # Drift slower
            x, y = rect.pos
            rect.pos = (x + vx, y + vy)

            color.a = alpha
            p[2] = alpha
