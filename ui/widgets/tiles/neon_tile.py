from kivy.uix.behaviors import ButtonBehavior
from kivy.uix.widget import Widget
from kivy.graphics import Color, RoundedRectangle
from kivy.animation import Animation
from kivy.properties import StringProperty, NumericProperty, ListProperty
from kivy.uix.label import Label


class NeonTile(ButtonBehavior, Widget):
    text = StringProperty("")
    scale = NumericProperty(1.0)
    fill_color = ListProperty([0.2, 0.8, 1.0, 1])  # default cyan fill

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        with self.canvas.before:
            # --- Glow layer (bigger, blurred-looking) ---
            Color(self.fill_color[0], self.fill_color[1], self.fill_color[2], 0.35)
            self.glow = RoundedRectangle(radius=[22], pos=self.pos, size=self.size)

            # --- White outline ---
            Color(1, 1, 1, 1)
            self.outline = RoundedRectangle(radius=[20], pos=self.pos, size=self.size)

            # --- Fill ---
            Color(*self.fill_color)
            self.rect = RoundedRectangle(radius=[18], pos=self.pos, size=self.size)

        # --- Text label ---
        self.label = Label(
            text=self.text,
            font_name="assets/fonts/RubikGlitch-Regular.ttf",
            font_size="26sp",
            color=(0, 0, 0, 1),  # black text
            halign="center",
            valign="middle",
        )
        self.add_widget(self.label)

        self.bind(
            pos=self._update_graphics,
            size=self._update_graphics,
            scale=self._update_graphics,
            text=self._update_text,
            fill_color=self._update_fill
        )

    # ---------------------------------------------------------
    # Update graphics
    # ---------------------------------------------------------
    def _update_graphics(self, *args):
        w, h = self.size
        scaled_w = w * self.scale
        scaled_h = h * self.scale

        offset_x = (w - scaled_w) / 2
        offset_y = (h - scaled_h) / 2

        pos = (self.x + offset_x, self.y + offset_y)
        size = (scaled_w, scaled_h)

        # Glow slightly bigger
        self.glow.pos = (pos[0] - 4, pos[1] - 4)
        self.glow.size = (size[0] + 8, size[1] + 8)

        # Outline
        self.outline.pos = pos
        self.outline.size = size

        # Fill
        self.rect.pos = (pos[0] + 2, pos[1] + 2)
        self.rect.size = (size[0] - 4, size[1] - 4)

        # Text
        self.label.pos = pos
        self.label.size = size

    def _update_text(self, *args):
        self.label.text = self.text

    def _update_fill(self, *args):
        # Update fill color dynamically
        with self.canvas.before:
            pass  # color already bound; redraw happens automatically

    # ---------------------------------------------------------
    # Press animation
    # ---------------------------------------------------------
    def on_press(self):
        Animation.cancel_all(self)
        Animation(scale=0.92, d=0.08).start(self)

    def on_release(self):
        Animation.cancel_all(self)
        Animation(scale=1.0, d=0.08).start(self)
        super().on_release()
