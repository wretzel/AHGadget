# widgets/loading/loading_widget.py

from kivy.uix.widget import Widget
from kivy.uix.image import Image
from kivy.graphics import Color, Rectangle
from kivy.clock import Clock
import math

from ui.widgets.starfield.ambient_starfield import AmbientStarfield


class LoadingWidget(Widget):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        # background
        self.bg = Image(
            source="assets/images/circuitry.png",
            allow_stretch=True,
            keep_ratio=False,
            color=(1, 1, 1, 0.2)
        )
        self.add_widget(self.bg)

        # AHG icon
        self.ahg_icon = Image(
            source="assets/images/load_icon.png",
            size_hint=(None, None),
            size=(140, 140),
            allow_stretch=True,
            keep_ratio=True
        )
        self.add_widget(self.ahg_icon)

        # starfield
        self.starfield = AmbientStarfield(count=60)
        self.add_widget(self.starfield)

        # animation state
        self.elapsed = 0
        self._glow = Clock.schedule_interval(self.animate_glow, 0.05)

    def on_size(self, *args):
        # background fills widget
        self.bg.size = self.size
        self.bg.pos = self.pos

        # icon top-left (your new choice)
        self.ahg_icon.pos = (
            30,
            self.height - self.ahg_icon.height - 30
        )

        # starfield fills widget
        self.starfield.size = self.size
        self.starfield.pos = self.pos

    def on_pos(self, *args):
        self.on_size()

    def animate_glow(self, dt):
        self.elapsed += dt
        t = self.elapsed * 1.2
        alpha = 0.4 + 0.6 * (0.5 + 0.5 * math.sin(t))
        self.ahg_icon.color = (1, 1, 1, alpha)

    def on_parent(self, widget, parent):
        # stop animation when removed
        if parent is None and hasattr(self, "_glow"):
            self._glow.cancel()
