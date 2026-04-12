from kivy.uix.widget import Widget
from kivy.graphics import Color, Rectangle, Ellipse, InstructionGroup
from kivy.clock import Clock


class CursorEffects(Widget):
    """
    Fully optimized cursor effects:
    - One canvas
    - One update loop
    - Ring buffer for trails
    - Reused glow + dot instructions
    """

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        # -----------------------------
        # CONFIG
        # -----------------------------
        self.max_trails = 20
        self.trail_fade_speed = 3.0
        self.trail_start_size = 40

        self.glow_radius = 60
        self.dot_radius = 10

        # -----------------------------
        # CANVAS GROUPS
        # -----------------------------
        self.canvas_group = InstructionGroup()
        self.canvas.after.add(self.canvas_group)

        # Trail group (batched)
        self.trail_group = InstructionGroup()
        self.canvas_group.add(self.trail_group)

        # Glow group (batched)
        self.glow_group = InstructionGroup()
        self.canvas_group.add(self.glow_group)

        # -----------------------------
        # PREALLOCATED GLOW + DOT
        # -----------------------------
        self.glow_color = Color(0.5, 0.85, 1.0, 0)
        self.glow = Rectangle(
            source="assets/images/glow.png",
            pos=(0, 0),
            size=(0, 0)
        )

        self.dot_color = Color(0.6, 0.9, 1.0, 0)
        self.dot = Ellipse(
            pos=(0, 0),
            size=(0, 0)
        )

        self.glow_group.add(self.glow_color)
        self.glow_group.add(self.glow)
        self.glow_group.add(self.dot_color)
        self.glow_group.add(self.dot)

        # -----------------------------
        # TRAIL RING BUFFER
        # -----------------------------
        self.trails = []  # each entry: [color, rect, alpha, center]

        # -----------------------------
        # STATE
        # -----------------------------
        self.is_pressed = False
        self.alpha = 0.0

        # -----------------------------
        # UPDATE LOOP
        # -----------------------------
        Clock.schedule_interval(self.update, 1 / 60)

    # ============================================================
    # TOUCH EVENTS
    # ============================================================

    def on_touch_down(self, touch):
        self.is_pressed = True
        self.alpha = 0.5

        self._update_glow_position(touch.x, touch.y)
        self._show_glow()

        self._add_trail(touch.x, touch.y)
        return super().on_touch_down(touch)

    def on_touch_move(self, touch):
        self._update_glow_position(touch.x, touch.y)
        self._add_trail(touch.x, touch.y)
        return super().on_touch_move(touch)

    def on_touch_up(self, touch):
        self.is_pressed = False
        return super().on_touch_up(touch)

    # ============================================================
    # GLOW HANDLING
    # ============================================================

    def _update_glow_position(self, x, y):
        self.glow.pos = (x - self.glow_radius, y - self.glow_radius)
        self.glow.size = (self.glow_radius * 2, self.glow_radius * 2)

        self.dot.pos = (x - self.dot_radius, y - self.dot_radius)
        self.dot.size = (self.dot_radius * 2, self.dot_radius * 2)

    def _show_glow(self):
        self.glow_color.a = self.alpha
        self.dot_color.a = self.alpha

    def _fade_glow(self, dt):
        if not self.is_pressed and self.alpha > 0:
            self.alpha -= 2.0 * dt
            a = max(self.alpha, 0)
            self.glow_color.a = a
            self.dot_color.a = a

    # ============================================================
    # TRAIL HANDLING
    # ============================================================

    def _add_trail(self, x, y):
        # Ring buffer: reuse oldest trail
        if len(self.trails) >= self.max_trails:
            color, rect, alpha, center = self.trails.pop(0)
            self.trail_group.remove(color)
            self.trail_group.remove(rect)

        # Create new trail element
        color = Color(0.5, 0.85, 1.0, 1.0)
        rect = Rectangle(
            source="assets/images/glow.png",
            pos=(x - self.trail_start_size / 2, y - self.trail_start_size / 2),
            size=(self.trail_start_size, self.trail_start_size)
        )

        center = (x, y)

        self.trail_group.add(color)
        self.trail_group.add(rect)

        self.trails.append([color, rect, 1.0, center])

    def _update_trails(self, dt):
        for trail in list(self.trails):
            color, rect, alpha, center = trail

            alpha -= self.trail_fade_speed * dt
            if alpha <= 0:
                self.trail_group.remove(color)
                self.trail_group.remove(rect)
                self.trails.remove(trail)
                continue

            # Shrink from center
            size = self.trail_start_size * alpha
            rect.size = (size, size)
            rect.pos = (center[0] - size / 2, center[1] - size / 2)

            color.a = alpha
            trail[2] = alpha

    # ============================================================
    # MAIN UPDATE LOOP
    # ============================================================

    def update(self, dt):
        self._fade_glow(dt)
        self._update_trails(dt)
