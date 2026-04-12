# start_banner.py

import random
from kivy.uix.label import Label
from kivy.graphics import Color, RoundedRectangle
from kivy.animation import Animation
from kivy.clock import Clock


def setup_banner(screen):
    """
    Sets up:
    - 'Touch to Begin' banner
    - rounded rectangle background
    - flicker animation
    - glitch animation
    """

    # ============================================================
    # BANNER (static)
    # ============================================================
    screen.banner = Label(
        text="Touch to Begin",
        font_name="RubikGlitch",
        font_size="48sp",
        color=(0, 0, 0, 1),
        size_hint=(None, None),
        pos_hint={"center_x": 0.5, "center_y": 0.25},
    )

    with screen.banner.canvas.before:
        screen.banner_bg_color = Color(1, 1.0, 1.0, 0.35)
        screen.banner_bg = RoundedRectangle(
            pos=screen.banner.pos,
            size=(0, 0),
            radius=[14]
        )

    screen.banner.bind(
        pos=screen._update_banner_bg,
        size=screen._update_banner_bg,
        texture_size=screen._update_banner_bg
    )

    screen.layout.add_widget(screen.banner)

    # ============================================================
    # ANIMATIONS
    # ============================================================
    _start_banner_animation(screen)
    _start_banner_glitch(screen)


def _start_banner_animation(screen):
    flicker = (
        Animation(color=(0, 0, 0, 1), duration=1.0) +
        Animation(color=(0, 0, 0, 0.6), duration=1.0)
    )
    flicker.repeat = True
    flicker.start(screen.banner)


def _start_banner_glitch(screen):
    def glitch(dt):
        if random.random() < 0.05:
            g = (
                Animation(color=(1, 0.2, 1, 1), duration=0.08) +
                Animation(color=(0.2, 1.0, 1.0, 1), duration=0.08)
            )
            g.start(screen.banner)

    Clock.schedule_interval(glitch, 0.2)
