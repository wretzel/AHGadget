import random
from kivy.uix.widget import Widget
from kivy.graphics import Color, Rectangle
from kivy.animation import Animation
from kivy.clock import Clock


class BrokenLight(Widget):
    """
    A glow widget that works with FloatLayout.
    - You position it with pos_hint or pos.
    - You size it with size_hint or size.
    - The glow image stays centered inside the widget.
    """

    def __init__(
        self,
        source="assets/images/light.png",
        tint=(1, 1, 1),
        min_alpha=0.15,
        max_alpha=0.5,
        min_speed=0.05,
        max_speed=0.35,
        **kwargs
    ):
        super().__init__(**kwargs)

        self.source = source
        self.tint = tint

        self.min_alpha = min_alpha
        self.max_alpha = max_alpha
        self.min_speed = min_speed
        self.max_speed = max_speed

        with self.canvas:
            self.color = Color(*self.tint, 1)
            self.rect = Rectangle(
                source=self.source,
                pos=self.center,
                size=(0, 0)
            )

        # Keep glow centered inside widget
        self.bind(pos=self._update_rect, size=self._update_rect)

        self._flicker_step()

    def _update_rect(self, *args):
        # Center the glow inside the widget
        w, h = self.size
        self.rect.size = (w, h)
        self.rect.pos = (self.x, self.y)

    def _flicker_step(self, *args):
        target_alpha = random.uniform(self.min_alpha, self.max_alpha)
        duration = random.uniform(self.min_speed, self.max_speed)

        anim = Animation(a=target_alpha, duration=duration)
        anim.bind(on_complete=self._flicker_step)
        anim.start(self.color)
