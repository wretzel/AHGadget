# start_decor.py

from kivy.graphics import Rectangle
from ui.widgets.particle_field import ParticleField
from ui.widgets.glow.broken_light import BrokenLight


def setup_decor(screen):
    """
    Sets up:
    - background image
    - particle field
    - corner glows
    - sync bindings
    """

    layout = screen.layout
    animated = screen.animated

    # ============================================================
    # BACKGROUND (inside animated container)
    # ============================================================
    with animated.canvas.before:
        screen.bg_rect = Rectangle(
            source="assets/backgrounds/start_bg.png",
            pos=animated.pos,
            size=animated.size
        )

    animated.bind(pos=screen._update_bg, size=screen._update_bg)

    # ============================================================
    # PARTICLE FIELD (static)
    # ============================================================
    screen.particles = ParticleField()
    layout.add_widget(screen.particles)
    screen.bind(size=screen._sync_particles, pos=screen._sync_particles)

    # ============================================================
    # CORNER GLOWS (inside animated container)
    # ============================================================
    def glow(tint, pos_hint):
        g = BrokenLight(
            tint=tint,
            size_hint=(None, None),
            size=(700, 700),
            pos_hint=pos_hint
        )
        animated.add_widget(g)
        return g

    screen.glow_tl = glow((0.2, 1.0, 0.2), {"center_x": 0.85, "center_y": 0.25})
    screen.glow_tr = glow((0.2, 0.6, 1.0), {"center_x": 0.85, "center_y": 0.78})
    screen.glow_bl = glow((0.7, 0.2, 1.0), {"center_x": 0.17, "center_y": 0.23})
    screen.glow_br = glow((0.2, 1.0, 0.2), {"center_x": 0.20, "center_y": 0.80})
