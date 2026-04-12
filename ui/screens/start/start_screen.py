# start_screen.py

from kivy.uix.screenmanager import Screen
from kivy.uix.floatlayout import FloatLayout
from kivy.core.text import LabelBase
from kivy.animation import Animation

from .start_decor import setup_decor
from .start_banner import setup_banner


LabelBase.register(
    name="RubikGlitch",
    fn_regular="assets/fonts/RubikGlitch-Regular.ttf"
)


class StartScreen(Screen):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.first_touch_registered = False

        # ============================================================
        # ROOT LAYOUT (static)
        # ============================================================
        self.layout = FloatLayout()
        self.add_widget(self.layout)

        # ============================================================
        # ANIMATED CONTAINER (only this moves)
        # ============================================================
        self.animated = FloatLayout(size_hint=(1, 1), pos=(0, 0))
        self.layout.add_widget(self.animated)

        # ============================================================
        # DECOR + BANNER
        # ============================================================
        setup_decor(self)
        setup_banner(self)

        # ============================================================
        # IDLE MOTION
        # ============================================================
        self.start_idle_motion()

    # ============================================================
    # IDLE MOTION
    # ============================================================
    def start_idle_motion(self):
        anim = (
            Animation(y=10, duration=3.0, t='in_out_quad') +
            Animation(y=0, duration=3.0, t='in_out_quad')
        )
        anim.repeat = True
        anim.start(self.animated)

    # ============================================================
    # BACKGROUND UPDATE
    # ============================================================
    def _update_bg(self, *args):
        self.bg_rect.pos = self.animated.pos
        self.bg_rect.size = self.animated.size

    # ============================================================
    # PARTICLE FIELD SYNC
    # ============================================================
    def _sync_particles(self, *args):
        self.particles.pos = self.pos
        self.particles.size = self.size

    # ============================================================
    # BANNER BACKGROUND UPDATE
    # ============================================================
    def _update_banner_bg(self, *args):
        padding_x = 28
        padding_y = 16
        w, h = self.banner.texture_size

        self.banner_bg.size = (w + padding_x * 2, h + padding_y * 2)
        self.banner_bg.pos = (
            self.banner.center_x - self.banner_bg.size[0] / 2,
            self.banner.center_y - self.banner_bg.size[1] / 2
        )

    # ============================================================
    # TOUCH HANDLING
    # ============================================================
    def on_touch_down(self, touch):
        if not self.first_touch_registered:
            self.first_touch_registered = True
        return super().on_touch_down(touch)

    def on_touch_up(self, touch):
        if self.first_touch_registered:
            self.first_touch_registered = False
            self.manager.current = "home"
        return super().on_touch_up(touch)
