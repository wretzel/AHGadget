from kivy.uix.widget import Widget
from kivy.graphics import Color, Rectangle, Callback
from kivy.graphics.opengl import glBlendFunc, GL_ONE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA
from kivy.animation import Animation


class SineGlowDot(Widget):
    def __init__(self, source="assets/images/glow.png", size_px=200, tint=(1,1,1), **kwargs):
        super().__init__(**kwargs)

        self.size_px = size_px
        self.source = source

        with self.canvas:
            Callback(lambda *args: glBlendFunc(GL_ONE, GL_ONE))

            self.c = Color(*tint, 1)
            self.r = Rectangle(source=self.source, size=(size_px, size_px))

            Callback(lambda *args: glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA))

        self.bind(pos=self.update_pos, size=self.update_pos)
        self.start_sine()

    def update_pos(self, *args):
        self.r.pos = (
            self.center_x - self.size_px/2,
            self.center_y - self.size_px/2
        )

    def start_sine(self):
        fade_out = Animation(a=0.4, duration=1.8)
        fade_in = Animation(a=1.0, duration=1.8)
        fade_out += fade_in
        fade_out.repeat = True
        fade_out.start(self.c)
