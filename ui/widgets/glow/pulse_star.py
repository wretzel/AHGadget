from kivy.uix.widget import Widget
from kivy.graphics import Rectangle, Color, PushMatrix, PopMatrix, Rotate
from kivy.animation import Animation
from kivy.clock import Clock


class PulseGlow(Widget):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.angle = 0

        with self.canvas:
            self.push = PushMatrix()
            self.rot = Rotate(angle=self.angle, origin=self.center)

            self.glow_color = Color(1, 1, 1, 0.5)  # start at pulse peak
            self.glow_rect = Rectangle(
                source="assets/images/star.png",
                pos=self.pos,
                size=self.size
            )

            self.pop = PopMatrix()

        self.bind(pos=self._update_glow, size=self._update_glow)

        self.start_pulse()
        Clock.schedule_interval(self._rotate_step, 1/60)

    def _update_glow(self, *args):
        glow_size = min(self.width, self.height) * 1.4
        self.glow_rect.size = (glow_size, glow_size)
        self.glow_rect.pos = (
            self.center_x - glow_size / 2,
            self.center_y - glow_size / 2
        )
        self.rot.origin = self.center

    def start_pulse(self):
        # Step 1: instantly jump to 0.3
        jump = Animation(a=0.3, duration=0)

        # Step 2: fade to 0
        fade = Animation(a=0.0, duration=2.0, t='linear')

        # Chain them
        pulse = jump + fade
        pulse.repeat = True
        pulse.start(self.glow_color)

    def _rotate_step(self, dt):
        self.angle = (self.angle + 0.3) % 360
        self.rot.angle = self.angle
