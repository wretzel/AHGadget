from kivy.uix.screenmanager import Screen
from kivy.uix.floatlayout import FloatLayout

from .home_decor import setup_decor
from .home_buttons import setup_buttons
from system.wifi.layer3.api import start_engine

class HomeScreen(Screen):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.layout = FloatLayout()
        self.add_widget(self.layout)

        setup_decor(self)
        setup_buttons(self)

    # background + glow update functions stay here
    def _update_bg(self, *args):
        self.bg_rect.pos = self.layout.pos
        self.bg_rect.size = self.layout.size

    def _update_static_glow(self, *args):
        w = self.layout.width * 1.2
        h = self.layout.height * 0.5
        cx = 0.0
        cy = 0.6
        x = self.layout.x + (self.layout.width * cx) - (w / 2)
        y = self.layout.y + (self.layout.height * cy) - (h / 2)
        self.left_static_glow.size = (w, h)
        self.left_static_glow.pos = (x, y)

    # navigation stays here
    def go_ir(self, *args): self.manager.current = "ir"
    def go_rf(self, *args): self.manager.current = "rf"

    # UPDATED
    def go_wifi(self, *args):
        start_engine()
        self.manager.current = "wifi_loading"

    def go_ble(self, *args): self.manager.current = "ble"
    def go_usb(self, *args): self.manager.current = "usb"
    def go_system(self, *args): self.manager.current = "system"
    def go_start(self, *args): self.manager.current = "start"
