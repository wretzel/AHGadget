from kivy.graphics import Color, Rectangle
from kivy.clock import Clock
from ui.widgets.starfield.ambient_3d_starfield import Ambient3DStarfield
from ui.widgets.glow.broken_light import BrokenLight
from ui.widgets.glow.pulse_star import PulseGlow


def setup_decor(screen):
    layout = screen.layout

    # Background + static glow
    with layout.canvas.before:
        screen.bg_color = Color(1, 1, 1, 1)
        screen.bg_rect = Rectangle(
            source="assets/backgrounds/home_bg.png",
            pos=layout.pos,
            size=layout.size
        )

        screen.left_static_glow_color = Color(0.2, 1.0, 0.2, 0.35)
        screen.left_static_glow = Rectangle(
            source="assets/images/glow.png",
            pos=(0, 0),
            size=(0, 0)
        )

    layout.bind(pos=screen._update_bg, size=screen._update_bg)
    layout.bind(pos=screen._update_static_glow, size=screen._update_static_glow)

    # Edge lights
    _add_edge_lights(screen)

    # Starfield
    Clock.schedule_once(lambda dt: _add_starfield(screen), 0)

    # Pulse glow
    screen.pulse = PulseGlow()
    layout.add_widget(screen.pulse)


def _add_starfield(screen):
    sf = Ambient3DStarfield()
    sf.size_hint = (1, 1)
    screen.layout.add_widget(sf, index=1)


def _add_edge_lights(screen):
    layout = screen.layout

    left_glow = BrokenLight(
        size_hint=(0.5, 0.1),
        pos_hint={"center_x": 0.0, "center_y": 0.6},
        tint=(0.2, 1, 0.2),
        min_alpha=0.10,
        max_alpha=1
    )
    layout.add_widget(left_glow, index=2)

    right_glow = BrokenLight(
        size_hint=(0.7, 0.2),
        pos_hint={"center_x": 1.0, "center_y": 0.5},
        tint=(1, 0.6, 0.2),
        min_alpha=0.10,
        max_alpha=1
    )
    layout.add_widget(right_glow, index=2)
